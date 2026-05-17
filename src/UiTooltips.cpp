#include "UiTooltips.h"

#include "Main_Window.h"
#include "Advanced_Settings_Window.h"
#include "Emulator_Control_Window.h"
#include "VM_Wizard_Window.h"
#include "Network_Widget.h"
#include "SPICE_Settings_Widget.h"
#include "Passthrough_Setup_Widget.h"

namespace UiTooltips {

void Apply_Main_Window( Main_Window *mw )
{
	if( ! mw ) return;
	mw->ui.actionPower_On->setToolTip( QObject::tr( "Start the selected virtual machine (QEMU/KVM)." ) );
	mw->ui.actionPower_Off->setToolTip( QObject::tr( "Stop the running virtual machine." ) );
	mw->ui.actionPause->setToolTip( QObject::tr( "Pause or resume the guest CPU." ) );
	mw->ui.actionReset->setToolTip( QObject::tr( "Reset the guest (system_reset monitor command)." ) );
	mw->ui.actionSave->setToolTip( QObject::tr( "Save VM state to a snapshot tag." ) );
	mw->ui.CH_sb16->setToolTip( QObject::tr( "Emulate Sound Blaster 16 (-soundhw sb16 or HDA stack)." ) );
	mw->ui.CH_HDA->setToolTip( QObject::tr( "Intel HD Audio (recommended for modern guests)." ) );
	mw->ui.CH_AC97->setToolTip( QObject::tr( "AC97 audio device." ) );
	mw->ui.CH_es1370->setToolTip( QObject::tr( "Ensoniq ES1370 PCI audio." ) );
}

void Apply_Advanced_Settings( Advanced_Settings_Window *win )
{
	if( ! win ) return;
	win->ui.CH_Minimize_To_Tray->setToolTip(
		QObject::tr( "Hide the main window to the system tray when minimized." ) );
	win->ui.CH_Close_To_Tray->setToolTip(
		QObject::tr( "Keep newaqemu running in the tray when the main window is closed." ) );
	win->ui.CH_Remember_Emulator_Geometry->setToolTip(
		QObject::tr( "Restore emulator control window size and position between sessions." ) );
	win->ui.CH_Audio_Default->setToolTip(
		QObject::tr( "Select host audio backend passed to QEMU (-audiodev) instead of the default." ) );
	win->ui.CB_Host_Sound_System->setToolTip(
		QObject::tr( "Host output driver: pipewire, pa, alsa, sdl, etc. (probed from QEMU)." ) );
	win->ui.CH_Use_New_Device_Changer->setToolTip(
		QObject::tr( "Use the monitor-based removable media menu in Emulator Control." ) );
}

void Apply_Emulator_Control( Emulator_Control_Window *win )
{
	if( ! win ) return;
	win->ui.actionGrab_Mouse->setToolTip(
		QObject::tr( "Capture mouse input for the embedded display (VNC)." ) );
	win->ui.actionUSB_Update_Device_List->setToolTip(
		QObject::tr( "Refresh host USB devices available for hot-plug." ) );
	win->ui.actionUSB_Disconnect_All_Devices->setToolTip(
		QObject::tr( "Disconnect all USB devices from the running VM." ) );
	win->ui.actionBy_Bus_Address->setToolTip(
		QObject::tr( "Disconnect by bus.address (monitor: usb_del)." ) );
}

void Apply_VM_Wizard( VM_Wizard_Window *win )
{
	if( ! win ) return;
	win->setToolTip( QObject::tr( "Create a new virtual machine configuration." ) );
}

void Apply_Network_Widget( Network_Widget *win )
{
	if( ! win ) return;
	win->setToolTip( QObject::tr( "Configure virtual network cards and port redirections for the VM." ) );
}

void Apply_SPICE_Widget( SPICE_Settings_Widget *win )
{
	if( ! win ) return;
	win->setToolTip( QObject::tr( "SPICE display and compression settings for the VM." ) );
}

void Apply_Passthrough_Widget( Passthrough_Setup_Widget *win )
{
	if( ! win ) return;
	win->setToolTip( QObject::tr( "Linux-only GPU passthrough host readiness and templates." ) );
}

}
