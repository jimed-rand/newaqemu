#include "host/HostHwProbe.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>

#ifdef Q_OS_LINUX

static QString readOneLine( const QString &path )
{
	QFile f( path );
	if( ! f.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();
	return QString::fromUtf8( f.readLine() ).trimmed();
}

static bool cmdlineHas( const QString &token )
{
	const QString cmd = readOneLine( QStringLiteral( "/proc/cmdline" ) );
	return cmd.contains( token, Qt::CaseInsensitive );
}

static QString pciClass( const QString &sysfsDir )
{
	return readOneLine( sysfsDir + QStringLiteral( "/class" ) );
}

static QString pciDriver( const QString &sysfsDir )
{
	const QFileInfo link( sysfsDir + QStringLiteral( "/driver" ) );
	if( ! link.exists() )
		return QString();
	return link.symLinkTarget().section( '/', -1 );
}

static int pciIommuGroup( const QString &sysfsDir )
{
	const QFileInfo link( sysfsDir + QStringLiteral( "/iommu_group" ) );
	if( ! link.exists() )
		return -1;
	return link.symLinkTarget().section( '/', -1 ).toInt();
}

static void appendGpuFromSysfs( HostPassthroughReport &report, const QString &bdf )
{
	const QString sysfs = QStringLiteral( "/sys/bus/pci/devices/" ) + bdf + QLatin1Char( '/' );
	if( ! pciClass( sysfs ).startsWith( QStringLiteral( "0x03" ) ) )
		return;

	PciGpuDevice gpu;
	gpu.bdf = bdf;
	gpu.vendorId = readOneLine( sysfs + QStringLiteral( "vendor" ) );
	gpu.deviceId = readOneLine( sysfs + QStringLiteral( "device" ) );
	gpu.driver = pciDriver( sysfs );
	gpu.iommuGroup = pciIommuGroup( sysfs );
	gpu.vfioBound = ( gpu.driver == QStringLiteral( "vfio-pci" ) );

	const QString total = readOneLine( sysfs + QStringLiteral( "sriov_totalvfs" ) );
	const QString num = readOneLine( sysfs + QStringLiteral( "sriov_numvfs" ) );
	gpu.sriovTotalVfs = total.toInt();
	gpu.sriovNumVfs = num.toInt();
	gpu.sriovCapable = gpu.sriovTotalVfs > 0;

	const QString physfn = readOneLine( sysfs + QStringLiteral( "physfn" ) );
	gpu.isVirtualFunction = ! physfn.isEmpty();

	if( gpu.isVirtualFunction )
		gpu.recommendedUse = QStringLiteral( "guest_vf" );
	else if( gpu.sriovCapable )
		gpu.recommendedUse = QStringLiteral( "host_pf" );

	report.gpus.append( gpu );
}

HostPassthroughReport HostHwProbe::scan()
{
	HostPassthroughReport report;

	report.iommuEnabled = QDir( QStringLiteral( "/sys/class/iommu" ) ).exists()
		&& ( cmdlineHas( QStringLiteral( "intel_iommu=on" ) )
			 || cmdlineHas( QStringLiteral( "amd_iommu=on" ) )
			 || cmdlineHas( QStringLiteral( "iommu=pt" ) )
			 || cmdlineHas( QStringLiteral( "iommu=on" ) ) );

	report.kvmAvailable = QFile::exists( QStringLiteral( "/dev/kvm" ) );
	report.kvmfrAvailable = QFile::exists( QStringLiteral( "/dev/kvmfr0" ) );

	QProcess lsmod;
	lsmod.start( QStringLiteral( "lsmod" ) );
	lsmod.waitForFinished( 3000 );
	const QString mods = QString::fromUtf8( lsmod.readAll() );
	report.i915SriovModule = mods.contains( QStringLiteral( "i915_sriov" ) )
		|| mods.contains( QStringLiteral( "i915_sriov_dkms" ) )
		|| mods.contains( QStringLiteral( "intel_sriov_compat" ) );

	QDir pci( QStringLiteral( "/sys/bus/pci/devices" ) );
	for( const QString &entry : pci.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
		appendGpuFromSysfs( report, entry );

	for( const PciGpuDevice &gpu : report.gpus )
	{
		if( gpu.isVirtualFunction && gpu.vfioBound )
		{
			report.recommendedVfBdf = gpu.bdf;
			break;
		}
	}

	if( report.recommendedVfBdf.isEmpty() )
	{
		for( const PciGpuDevice &gpu : report.gpus )
		{
			if( gpu.isVirtualFunction )
			{
				report.recommendedVfBdf = gpu.bdf;
				break;
			}
		}
	}

	if( ! report.gpus.isEmpty() )
	{
		const QString devId = report.gpus.first().deviceId.remove( QStringLiteral( "0x" ) );
		report.kernelBootParamSnippet = QStringLiteral(
			"intel_iommu=on i915.enable_guc=3 i915.max_vfs=1 module_blacklist=xe" );
		report.cmdlineHint = report.kernelBootParamSnippet
			+ QStringLiteral( "\n# device id from lspci: " ) + devId;
	}

	return report;
}

#else

HostPassthroughReport HostHwProbe::scan()
{
	HostPassthroughReport report;
	report.cmdlineHint = QStringLiteral( "GPU passthrough host setup is Linux-only." );
	return report;
}

#endif

QString HostPassthroughReport::diagnosticText() const
{
	QString out;
	out += QStringLiteral( "IOMMU: " ) + QString( iommuEnabled ? "yes" : "no" ) + QLatin1Char( '\n' );
	out += QStringLiteral( "KVM: " ) + QString( kvmAvailable ? "yes" : "no" ) + QLatin1Char( '\n' );
	out += QStringLiteral( "kvmfr0: " ) + QString( kvmfrAvailable ? "yes" : "no" ) + QLatin1Char( '\n' );
	out += QStringLiteral( "i915 SR-IOV module: " ) + QString( i915SriovModule ? "yes" : "no" ) + QLatin1Char( '\n' );
	out += QStringLiteral( "Recommended VF: " ) + recommendedVfBdf + QLatin1Char( '\n' );
	out += QStringLiteral( "Boot params: " ) + kernelBootParamSnippet + QLatin1Char( '\n' );
	for( const PciGpuDevice &gpu : gpus )
	{
		out += gpu.bdf + QStringLiteral( " driver=" ) + gpu.driver
			+ QStringLiteral( " iommu_group=" ) + QString::number( gpu.iommuGroup )
			+ QStringLiteral( " vfio=" ) + QString( gpu.vfioBound ? "yes" : "no" )
			+ QStringLiteral( " sriov_vfs=" ) + QString::number( gpu.sriovNumVfs )
			+ QLatin1Char( '\n' );
	}
	return out;
}
