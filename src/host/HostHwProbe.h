#ifndef HOST_HW_PROBE_H
#define HOST_HW_PROBE_H

#include <QString>
#include <QVector>

struct PciGpuDevice
{
	QString bdf;
	QString vendorId;
	QString deviceId;
	QString driver;
	int iommuGroup = -1;
	bool sriovCapable = false;
	int sriovTotalVfs = 0;
	int sriovNumVfs = 0;
	bool vfioBound = false;
	bool isVirtualFunction = false;
	QString recommendedUse;
};

struct HostPassthroughReport
{
	bool iommuEnabled = false;
	bool kvmAvailable = false;
	bool kvmfrAvailable = false;
	bool i915SriovModule = false;
	QString cmdlineHint;
	QString kernelBootParamSnippet;
	QVector<PciGpuDevice> gpus;
	QString recommendedVfBdf;
	QString diagnosticText() const;
};

class HostHwProbe
{
public:
	static HostPassthroughReport scan();
};

#endif
