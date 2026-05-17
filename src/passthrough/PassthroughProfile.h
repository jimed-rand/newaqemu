#ifndef PASSTHROUGH_PROFILE_H
#define PASSTHROUGH_PROFILE_H

#include <QString>

class Virtual_Machine;

class PassthroughProfile
{
public:
	enum DisplayMode { DisplayVncInstall, DisplayLookingGlass, DisplayNone };

	QString gpuBdf;
	bool useLookingGlass = true;
	int kvmfrSizeMb = 32;
	DisplayMode displayMode = DisplayLookingGlass;
	QString machineType = QStringLiteral( "q35" );
	QString firmware = QStringLiteral( "uefi" );
	QString libvirtDomainName;
	bool useScreamAudio = false;
	QString screamHostIp = QStringLiteral( "192.168.122.1" );

	QString displayModeString() const;
	void setDisplayModeString( const QString &mode );

	static PassthroughProfile fromVmXml( const Virtual_Machine &vm );
	void applyToVmXml( Virtual_Machine &vm ) const;
};

#endif
