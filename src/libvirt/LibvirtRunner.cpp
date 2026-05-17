#include "libvirt/LibvirtRunner.h"

#include "libvirt/DomainXmlBuilder.h"
#include "VM.h"

#include <QFile>
#include <QProcess>

static QString g_lastError;

QString LibvirtRunner::lastError()
{
	return g_lastError;
}

bool LibvirtRunner::available()
{
	QProcess which;
	which.start( QStringLiteral( "virsh" ), { QStringLiteral( "--version" ) } );
	which.waitForFinished( 3000 );
	return which.exitCode() == 0;
}

static bool runVirsh( const QStringList &args, QByteArray *out = nullptr )
{
	QProcess proc;
	proc.start( QStringLiteral( "virsh" ), args );
	proc.waitForFinished( 15000 );
	if( out )
		*out = proc.readAllStandardOutput();
	if( proc.exitCode() != 0 )
	{
		g_lastError = QString::fromUtf8( proc.readAllStandardError() );
		if( g_lastError.isEmpty() )
			g_lastError = QStringLiteral( "virsh failed" );
		return false;
	}
	return true;
}

bool LibvirtRunner::defineDomain( const Virtual_Machine &vm )
{
	const QString xml = DomainXmlBuilder::build( vm );
	const QString path = vm.Get_VM_XML_File_Path() + QStringLiteral( ".libvirt.xml" );
	QFile f( path );
	if( ! f.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		g_lastError = QStringLiteral( "Cannot write libvirt XML: " ) + path;
		return false;
	}
	f.write( xml.toUtf8() );
	f.close();

	const QString domain = vm.Get_Passthrough_Profile().libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: vm.Get_Passthrough_Profile().libvirtDomainName;

	if( ! runVirsh( { QStringLiteral( "define" ), path } ) )
		return false;

	Q_UNUSED( domain );
	return true;
}

bool LibvirtRunner::start( Virtual_Machine &vm )
{
	if( ! defineDomain( vm ) )
		return false;

	const QString domain = vm.Get_Passthrough_Profile().libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: vm.Get_Passthrough_Profile().libvirtDomainName;

	return runVirsh( { QStringLiteral( "start" ), domain } );
}

bool LibvirtRunner::stop( Virtual_Machine &vm )
{
	const QString domain = vm.Get_Passthrough_Profile().libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: vm.Get_Passthrough_Profile().libvirtDomainName;
	return runVirsh( { QStringLiteral( "destroy" ), domain } );
}

bool LibvirtRunner::pause( Virtual_Machine &vm )
{
	const QString domain = vm.Get_Passthrough_Profile().libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: vm.Get_Passthrough_Profile().libvirtDomainName;
	return runVirsh( { QStringLiteral( "suspend" ), domain } );
}

bool LibvirtRunner::resume( Virtual_Machine &vm )
{
	const QString domain = vm.Get_Passthrough_Profile().libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: vm.Get_Passthrough_Profile().libvirtDomainName;
	return runVirsh( { QStringLiteral( "resume" ), domain } );
}
