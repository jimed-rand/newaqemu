#include "host/HostInfo.h"

#include "System_Info.h"

void HostInfo::getFreeMemorySize( int &allRamMb, int &freeRamMb )
{
	System_Info::Get_Free_Memory_Size( allRamMb, freeRamMb );
}

QStringList HostInfo::hostFloppyList()
{
	return System_Info::Get_Host_FDD_List();
}

QStringList HostInfo::hostCdromList()
{
	return System_Info::Get_Host_CDROM_List();
}

bool HostInfo::updateHostUsb()
{
	return System_Info::Update_Host_USB();
}
