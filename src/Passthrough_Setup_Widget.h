#ifndef PASSTHROUGH_SETUP_WIDGET_H
#define PASSTHROUGH_SETUP_WIDGET_H

#include <QWidget>

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

class Passthrough_Setup_Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Passthrough_Setup_Widget( QWidget *parent = nullptr );
	void refresh();

private slots:
	void onRefresh();
	void onCopyDiagnostics();
	void onApplyHostTemplates();
	void onLaunchLookingGlass();

private:
	void updateReport();

	QLabel *m_statusIommu = nullptr;
	QLabel *m_statusKvm = nullptr;
	QLabel *m_statusKvmfr = nullptr;
	QLabel *m_statusSriov = nullptr;
	QComboBox *m_gpuCombo = nullptr;
	QSpinBox *m_kvmfrSize = nullptr;
	QPlainTextEdit *m_diagnostics = nullptr;
	QPlainTextEdit *m_guestChecklist = nullptr;
};

#endif
