#ifndef SPICE_VIEW_H
#define SPICE_VIEW_H

#include <QWidget>
#include <QProcess>

class QLabel;
class QPushButton;
class QVBoxLayout;

class SpiceView : public QWidget
{
	Q_OBJECT

public:
	explicit SpiceView( QWidget *parent = nullptr );
	~SpiceView() override;

	void connectToHost( const QString &host, int port );
	void disconnectSession();
	void launchViewer();

signals:
	void Connected();

private slots:
	void onOpenExternalViewer();
	void onViewerFinished( int exitCode, QProcess::ExitStatus status );

private:
	void setupUi();

	QLabel *Status_Label = nullptr;
	QPushButton *Open_Button = nullptr;
	QVBoxLayout *Layout = nullptr;
	QProcess *Viewer_Process = nullptr;
	QString Host;
	int Port = 0;
};

#endif
