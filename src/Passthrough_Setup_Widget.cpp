#include "Passthrough_Setup_Widget.h"

#include "host/HostHwProbe.h"

#include <QClipboard>
#include <QFormLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

Passthrough_Setup_Widget::Passthrough_Setup_Widget( QWidget *parent )
	: QWidget( parent )
{
	auto *layout = new QVBoxLayout( this );

	auto *statusBox = new QGroupBox( tr( "Host passthrough readiness" ), this );
	auto *statusForm = new QFormLayout( statusBox );
	m_statusIommu = new QLabel( this );
	m_statusKvm = new QLabel( this );
	m_statusKvmfr = new QLabel( this );
	m_statusSriov = new QLabel( this );
	statusForm->addRow( tr( "IOMMU" ), m_statusIommu );
	statusForm->addRow( tr( "KVM" ), m_statusKvm );
	statusForm->addRow( tr( "kvmfr" ), m_statusKvmfr );
	statusForm->addRow( tr( "Intel SR-IOV module" ), m_statusSriov );
	layout->addWidget( statusBox );

	auto *gpuBox = new QGroupBox( tr( "GPU / VF selection" ), this );
	auto *gpuForm = new QFormLayout( gpuBox );
	m_gpuCombo = new QComboBox( gpuBox );
	m_kvmfrSize = new QSpinBox( gpuBox );
	m_kvmfrSize->setRange( 8, 256 );
	m_kvmfrSize->setValue( 32 );
	m_kvmfrSize->setSuffix( tr( " MiB" ) );
	gpuForm->addRow( tr( "PCI device" ), m_gpuCombo );
	gpuForm->addRow( tr( "kvmfr size" ), m_kvmfrSize );
	layout->addWidget( gpuBox );

	auto *btnRow = new QHBoxLayout();
	auto *refreshBtn = new QPushButton( tr( "Refresh" ), this );
	auto *copyBtn = new QPushButton( tr( "Copy diagnostics" ), this );
	auto *hostBtn = new QPushButton( tr( "Apply host templates…" ), this );
	auto *lgBtn = new QPushButton( tr( "Launch Looking Glass" ), this );
	btnRow->addWidget( refreshBtn );
	btnRow->addWidget( copyBtn );
	btnRow->addWidget( hostBtn );
	btnRow->addWidget( lgBtn );
	btnRow->addStretch();
	layout->addLayout( btnRow );

	m_diagnostics = new QPlainTextEdit( this );
	m_diagnostics->setReadOnly( true );
	layout->addWidget( m_diagnostics, 2 );

	m_guestChecklist = new QPlainTextEdit( this );
	m_guestChecklist->setReadOnly( true );
	m_guestChecklist->setPlainText(
		tr( "Windows guest checklist:\n"
			"1. Install Intel graphics driver v32+ inside the VM.\n"
			"2. Install Looking Glass host in the guest.\n"
			"3. Install Virtual Display Driver (VDD) for headless VF output.\n"
			"4. Optional: Scream audio to host bridge IP (e.g. 192.168.122.1).\n"
			"5. After LG works, set display to none and use looking-glass-client on the host.\n"
			"\n"
			"Reference: https://pen.waltuh.cyou/yonle/iommu-with-sr-iov-gpu-accelerated-windows-vm-on-a-linux-laptop-that-has-only" ) );
	layout->addWidget( m_guestChecklist, 1 );

	connect( refreshBtn, &QPushButton::clicked, this, &Passthrough_Setup_Widget::onRefresh );
	connect( copyBtn, &QPushButton::clicked, this, &Passthrough_Setup_Widget::onCopyDiagnostics );
	connect( hostBtn, &QPushButton::clicked, this, &Passthrough_Setup_Widget::onApplyHostTemplates );
	connect( lgBtn, &QPushButton::clicked, this, &Passthrough_Setup_Widget::onLaunchLookingGlass );

	refresh();
}

void Passthrough_Setup_Widget::refresh()
{
	onRefresh();
}

void Passthrough_Setup_Widget::updateReport()
{
	const HostPassthroughReport report = HostHwProbe::scan();
	m_statusIommu->setText( report.iommuEnabled ? tr( "OK" ) : tr( "Missing" ) );
	m_statusKvm->setText( report.kvmAvailable ? tr( "OK" ) : tr( "Missing" ) );
	m_statusKvmfr->setText( report.kvmfrAvailable ? tr( "OK" ) : tr( "Missing" ) );
	m_statusSriov->setText( report.i915SriovModule ? tr( "Loaded" ) : tr( "Not detected" ) );

	m_gpuCombo->clear();
	for( const PciGpuDevice &gpu : report.gpus )
	{
		const QString label = gpu.bdf + QStringLiteral( " [" ) + gpu.driver + QStringLiteral( "]" )
			+ ( gpu.isVirtualFunction ? QStringLiteral( " VF" ) : QString() );
		m_gpuCombo->addItem( label, gpu.bdf );
	}

	if( ! report.recommendedVfBdf.isEmpty() )
	{
		const int idx = m_gpuCombo->findData( report.recommendedVfBdf );
		if( idx >= 0 )
			m_gpuCombo->setCurrentIndex( idx );
	}

	m_diagnostics->setPlainText( report.diagnosticText() + QStringLiteral( "\n" )
		+ report.kernelBootParamSnippet );
}

void Passthrough_Setup_Widget::onRefresh()
{
	updateReport();
}

void Passthrough_Setup_Widget::onCopyDiagnostics()
{
	QGuiApplication::clipboard()->setText( m_diagnostics->toPlainText() );
}

void Passthrough_Setup_Widget::onApplyHostTemplates()
{
	const QString bdf = m_gpuCombo->currentData().toString();
	const int kvmfrMb = m_kvmfrSize->value();
	QProcess proc;
	proc.start( QStringLiteral( "pkexec" ),
				{ QStringLiteral( "newaqemu-host-setup" ),
				  QStringLiteral( "--gpu-bdf" ),
				  bdf,
				  QStringLiteral( "--kvmfr-mb" ),
				  QString::number( kvmfrMb ) } );
	proc.waitForFinished( 120000 );
	if( proc.exitCode() != 0 )
	{
		QMessageBox::warning( this, tr( "Host setup failed" ),
							  QString::fromUtf8( proc.readAllStandardError() ) );
		return;
	}
	QMessageBox::information( this, tr( "Host setup" ),
							  tr( "Templates were written. Reboot the host if prompted, then refresh." ) );
	refresh();
}

void Passthrough_Setup_Widget::onLaunchLookingGlass()
{
	QProcess::startDetached( QStringLiteral( "looking-glass-client" ), {} );
}
