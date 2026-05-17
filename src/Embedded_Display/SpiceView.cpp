#include "SpiceView.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>

SpiceView::SpiceView( QWidget *parent )
	: QWidget( parent )
{
	setupUi();
}

SpiceView::~SpiceView()
{
	disconnectSession();
}

void SpiceView::setupUi()
{
	Layout = new QVBoxLayout( this );
	Status_Label = new QLabel( tr( "SPICE display: not connected" ), this );
	Open_Button = new QPushButton( tr( "Open SPICE viewer" ), this );
	Layout->addWidget( Status_Label );
	Layout->addWidget( Open_Button );
	Layout->addStretch();
	connect( Open_Button, &QPushButton::clicked, this, &SpiceView::onOpenExternalViewer );
}

void SpiceView::connectToHost( const QString &host, int port )
{
	Host = host;
	Port = port;
	Status_Label->setText( tr( "SPICE endpoint: %1:%2" ).arg( host ).arg( port ) );
}

void SpiceView::disconnectSession()
{
	if( Viewer_Process )
	{
		Viewer_Process->terminate();
		Viewer_Process->waitForFinished( 2000 );
		delete Viewer_Process;
		Viewer_Process = nullptr;
	}
}

void SpiceView::launchViewer()
{
	onOpenExternalViewer();
}

void SpiceView::onOpenExternalViewer()
{
	if( Host.isEmpty() || Port <= 0 )
	{
		QMessageBox::warning( this, tr( "SPICE" ), tr( "Invalid SPICE host or port." ) );
		return;
	}

	disconnectSession();

	const QString uri = QString( "spice://%1:%2" ).arg( Host ).arg( Port );
	Viewer_Process = new QProcess( this );
	connect( Viewer_Process, QOverload<int, QProcess::ExitStatus>::of( &QProcess::finished ),
	         this, &SpiceView::onViewerFinished );

	QStringList candidates = { "remote-viewer", "virt-viewer" };
	for( const QString &bin : candidates )
	{
		Viewer_Process->start( bin, { uri } );
		if( Viewer_Process->waitForStarted( 3000 ) )
		{
			Status_Label->setText( tr( "SPICE viewer running (%1)" ).arg( bin ) );
			emit Connected();
			return;
		}
	}

	delete Viewer_Process;
	Viewer_Process = nullptr;
	QMessageBox::warning( this, tr( "SPICE" ),
	                      tr( "Could not start remote-viewer or virt-viewer.\n"
	                          "Install virt-viewer and ensure it is on PATH." ) );
}

void SpiceView::onViewerFinished( int exitCode, QProcess::ExitStatus status )
{
	Q_UNUSED( exitCode );
	Q_UNUSED( status );
	Status_Label->setText( tr( "SPICE viewer closed" ) );
}
