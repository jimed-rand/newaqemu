#include "AppInfo.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#ifndef NEWAQEMU_VERSION
#define NEWAQEMU_VERSION "1.0.0"
#endif

static const char kVersion[] = NEWAQEMU_VERSION;

const char *AppInfo::version()
{
	return kVersion;
}

const char *AppInfo::versionString()
{
	return "newaqemu " kVersion;
}

QString AppInfo::applicationName()
{
	return QStringLiteral( "newaqemu" );
}

QString AppInfo::organizationName()
{
	return QStringLiteral( "newaqemu" );
}

QString AppInfo::configVersionKey()
{
	return QStringLiteral( "newaqemu_config_version" );
}

QString AppInfo::dataFolderKey()
{
	return QStringLiteral( "newaqemu_data_folder" );
}

QString AppInfo::vmDirectoryKey()
{
	return QStringLiteral( "VM_Directory" );
}

QStringList AppInfo::dataSearchPaths()
{
	return QStringList()
		<< QStringLiteral( "/usr/share/newaqemu/" )
		<< QStringLiteral( "/usr/local/share/newaqemu/" )
		<< QStringLiteral( "/usr/share/aqemu/" )
		<< QStringLiteral( "/usr/share/apps/aqemu/" )
		<< QStringLiteral( "/usr/local/share/aqemu/" );
}

QString AppInfo::defaultLogFileName()
{
	return QStringLiteral( "newaqemu.log" );
}

bool AppInfo::isLegacyAqemuConfigPath( const QString &settingsFile )
{
	return settingsFile.contains( QStringLiteral( "/.config/aqemu/" ), Qt::CaseInsensitive )
		|| settingsFile.contains( QStringLiteral( "AQEMU.conf" ), Qt::CaseInsensitive );
}

void AppInfo::offerLegacyConfigImport()
{
	const QString legacyDir = QStandardPaths::writableLocation( QStandardPaths::ConfigLocation )
		+ QStringLiteral( "/aqemu/" );
	const QString legacyConf = legacyDir + QStringLiteral( "AQEMU.conf" );
	const QString newDir = QStandardPaths::writableLocation( QStandardPaths::ConfigLocation )
		+ QStringLiteral( "/newaqemu/" );
	const QString newConf = newDir + QStringLiteral( "newaqemu.conf" );

	if( ! QFile::exists( legacyConf ) || QFile::exists( newConf ) )
		return;

	const int ret = QMessageBox::question(
		nullptr,
		QStringLiteral( "Import AQEMU settings?" ),
		QStringLiteral( "Found AQEMU configuration at:\n%1\n\nImport settings into newaqemu?" ).arg( legacyConf ),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::Yes );

	if( ret != QMessageBox::Yes )
		return;

	QDir().mkpath( newDir );
	QSettings legacy( legacyConf, QSettings::IniFormat );
	QSettings fresh( newConf, QSettings::IniFormat );

	const QStringList keys = legacy.allKeys();
	for( const QString &key : keys )
	{
		QString mapped = key;
		mapped.replace( QStringLiteral( "AQEMU_" ), QStringLiteral( "newaqemu_" ) );
		mapped.replace( QStringLiteral( "aqemu_" ), QStringLiteral( "newaqemu_" ) );
		fresh.setValue( mapped, legacy.value( key ) );
	}

	if( legacy.contains( QStringLiteral( "AQEMU_Config_Version" ) ) )
		fresh.setValue( configVersionKey(), legacy.value( QStringLiteral( "AQEMU_Config_Version" ) ) );

	if( legacy.contains( QStringLiteral( "AQEMU_Data_Folder" ) ) )
		fresh.setValue( dataFolderKey(), legacy.value( QStringLiteral( "AQEMU_Data_Folder" ) ) );

	fresh.sync();
}
