#include "passthrough/PassthroughProfile.h"

#include "VM.h"

QString PassthroughProfile::displayModeString() const
{
	switch( displayMode )
	{
	case DisplayVncInstall: return QStringLiteral( "vnc_install" );
	case DisplayNone: return QStringLiteral( "none" );
	case DisplayLookingGlass:
	default: return QStringLiteral( "looking_glass" );
	}
}

void PassthroughProfile::setDisplayModeString( const QString &mode )
{
	if( mode == QStringLiteral( "vnc_install" ) )
		displayMode = DisplayVncInstall;
	else if( mode == QStringLiteral( "none" ) )
		displayMode = DisplayNone;
	else
		displayMode = DisplayLookingGlass;
}

PassthroughProfile PassthroughProfile::fromVmXml( const Virtual_Machine &vm )
{
	return vm.Get_Passthrough_Profile();
}

void PassthroughProfile::applyToVmXml( Virtual_Machine &vm ) const
{
	vm.Set_Passthrough_Profile( *this );
}
