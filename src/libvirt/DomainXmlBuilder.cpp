#include "libvirt/DomainXmlBuilder.h"

#include "VM.h"
#include "passthrough/PassthroughProfile.h"

QString DomainXmlBuilder::build( const Virtual_Machine &vm )
{
	const PassthroughProfile profile = vm.Get_Passthrough_Profile();
	const QString domainName = profile.libvirtDomainName.isEmpty()
		? QStringLiteral( "newaqemu-" ) + vm.Get_UID()
		: profile.libvirtDomainName;

	const qint64 memKb = static_cast<qint64>( vm.Get_Memory_Size() ) * 1024;
	const qint64 kvmfrBytes = static_cast<qint64>( profile.kvmfrSizeMb ) * 1024 * 1024;

	QString videoXml;
	if( profile.displayMode == PassthroughProfile::DisplayLookingGlass
		|| profile.displayMode == PassthroughProfile::DisplayNone )
		videoXml = QStringLiteral( "    <video>\n      <model type='none'/>\n    </video>\n" );
	else
		videoXml = QStringLiteral( "    <video>\n      <model type='vga' vram='16384'/>\n    </video>\n" );

	QString lgXml;
	if( profile.useLookingGlass )
	{
		lgXml = QString(
			"  <qemu:commandline xmlns:qemu='http://libvirt.org/schemas/domain/qemu/1.0'>\n"
			"    <qemu:arg value='-object'/>\n"
			"    <qemu:arg value='memory-backend-file,id=looking-glass,mem-path=/dev/kvmfr0,size=%1,share=on'/>\n"
			"    <qemu:arg value='-device'/>\n"
			"    <qemu:arg value='ivshmem-plain,memdev=looking-glass,id=lg'/>\n"
			"  </qemu:commandline>\n" ).arg( kvmfrBytes );
	}

	const QString diskPath = vm.Get_Primary_Disk_Path();

	return QString(
		"<?xml version='1.0' encoding='UTF-8'?>\n"
		"<domain type='kvm' xmlns:qemu='http://libvirt.org/schemas/domain/qemu/1.0'>\n"
		"  <name>%1</name>\n"
		"  <memory unit='KiB'>%2</memory>\n"
		"  <vcpu placement='static'>%3</vcpu>\n"
		"  <os>\n"
		"    <type arch='x86_64' machine='%4'>hvm</type>\n"
		"    <boot dev='hd'/>\n"
		"  </os>\n"
		"  <features>\n"
		"    <acpi/>\n"
		"  </features>\n"
		"  <cpu mode='host-passthrough'/>\n"
		"  <devices>\n"
		"    <emulator>/usr/bin/qemu-system-x86_64</emulator>\n"
		"    <disk type='file' device='disk'>\n"
		"      <driver name='qemu' type='qcow2'/>\n"
		"      <source file='%5'/>\n"
		"      <target dev='vda' bus='virtio'/>\n"
		"    </disk>\n"
		"    <interface type='network'>\n"
		"      <source network='default'/>\n"
		"      <model type='virtio'/>\n"
		"    </interface>\n"
		"    <input type='tablet' bus='virtio'/>\n"
		"    <input type='keyboard' bus='virtio'/>\n"
		"%6"
		"    <hostdev mode='subsystem' type='pci' managed='yes'>\n"
		"      <source>\n"
		"        <address domain='0x0000' bus='0x%7' slot='0x%8' function='0x%9'/>\n"
		"      </source>\n"
		"    </hostdev>\n"
		"  </devices>\n"
		"%10"
		"</domain>\n" )
		.arg( domainName )
		.arg( memKb )
		.arg( vm.Get_SMP().SMP_Count > 0 ? vm.Get_SMP().SMP_Count : 2 )
		.arg( profile.machineType )
		.arg( diskPath )
		.arg( videoXml )
		.arg( profile.gpuBdf.section( ':', 1, 1 ) )
		.arg( profile.gpuBdf.section( ':', 2, 2 ).section( '.', 0, 0 ) )
		.arg( profile.gpuBdf.section( '.', 1, 1 ) )
		.arg( lgXml );
}
