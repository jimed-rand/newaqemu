#ifndef UI_TOOLTIPS_H
#define UI_TOOLTIPS_H

class Main_Window;
class Advanced_Settings_Window;
class Emulator_Control_Window;
class VM_Wizard_Window;
class Network_Widget;
class SPICE_Settings_Widget;
class Passthrough_Setup_Widget;

namespace UiTooltips
{
void Apply_Main_Window( Main_Window *mw );
void Apply_Advanced_Settings( Advanced_Settings_Window *win );
void Apply_Emulator_Control( Emulator_Control_Window *win );
void Apply_VM_Wizard( VM_Wizard_Window *win );
void Apply_Network_Widget( Network_Widget *win );
void Apply_SPICE_Widget( SPICE_Settings_Widget *win );
void Apply_Passthrough_Widget( Passthrough_Setup_Widget *win );
}

#endif
