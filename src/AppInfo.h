#ifndef APP_INFO_H
#define APP_INFO_H

#include <QString>
#include <QStringList>

class AppInfo
{
public:
	static const char *version();
	static const char *versionString();
	static QString applicationName();
	static QString organizationName();
	static QString configVersionKey();
	static QString dataFolderKey();
	static QString vmDirectoryKey();

	static QStringList dataSearchPaths();
	static QString findBundledDataFile( const QString &fileName );
	static QString defaultLogFileName();

	static bool isLegacyAqemuConfigPath( const QString &settingsFile );
	static void offerLegacyConfigImport();
};

#endif
