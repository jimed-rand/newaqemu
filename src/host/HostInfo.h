#ifndef HOST_INFO_H
#define HOST_INFO_H

#include <QStringList>

class HostInfo
{
public:
	static void getFreeMemorySize( int &allRamMb, int &freeRamMb );
	static QStringList hostFloppyList();
	static QStringList hostCdromList();
	static bool updateHostUsb();
};

#endif
