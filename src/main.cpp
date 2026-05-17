/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
** Copyright (C) 2016 Tobias Gläßer
**
** This file is part of AQEMU.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QTranslator>
#include <QFileDialog>
#include <QDir>
#include <QFile>

// For Check User UID
#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#endif
#include <iostream>

#include "Utils.h"
#include "AppInfo.h"
#include "Main_Window.h"
#include "First_Start_Wizard.h"

#include "docopt/docopt.h"

static const char USAGE[] =
R"(Usage: newaqemu [options]

Frontend for QEMU on modern Linux.

  Options:
      -h --help          Show this screen.
      --version          Show version.

  Qt Options:
      --style THEME      Set theme/style.
)";
/* mockup

      aqemu start <VIRTUAL_MACHINE>
      aqemu pause <VIRTUAL_MACHINE>
      aqemu stop  <VIRTUAL_MACHINE>
      aqemu (-h | --help)
      aqemu --version

*/


int main( int argc, char *argv[] )
{
    QString version = QString::fromUtf8( AppInfo::versionString() );
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,               // show help if requested
                          qPrintable( version ) );  // version string

    /*for(auto const& arg : args) {
        std::cout << arg.first <<  arg.second << std::endl;
    }*/


	// Set QSettings Data
	QCoreApplication::setOrganizationName( AppInfo::organizationName() );
	QCoreApplication::setApplicationName( AppInfo::applicationName() );
	#ifdef Q_OS_WIN32
	QSettings::setDefaultFormat( QSettings::IniFormat );
	#endif
	QSettings settings;
	AppInfo::offerLegacyConfigImport();
	
	// Use Log?
	if( settings.value( "Log/Save_In_File", "yes" ).toString() == "yes" )
	{
		settings.setValue( "Log/Save_In_File", "yes" );
		AQUse_Log( true );
		
		if( settings.value("Log/Log_Path", "").toString().isEmpty() )
		{
			QFileInfo logFileDir( settings.fileName() );
			QString logDirPath = logFileDir.absolutePath();
			
			// Dir for log file exists?
			if( ! QFile::exists(logDirPath) )
			{
				QDir dir;
				if( ! dir.mkpath(logDirPath) )
					AQGraphic_Warning( QObject::tr("Error"),
									   QObject::tr("Cannot create directory for log file! Path: %1").arg(logDirPath) );
			}
			
			settings.setValue( "Log/Log_Path", QDir::toNativeSeparators( logDirPath + "/" + AppInfo::defaultLogFileName() ) );
		}
		else
		{
			// Log Size
			if( QFile::exists(settings.value("Log/Log_Path", "").toString()) )
			{
				QFileInfo log_info( settings.value("Log/Log_Path", "").toString() );
				
				// Log > 1MB
				if( log_info.size() > (1 * 1024 * 1024) )
				{
					// FIXME Delete Half Log Size
					QFile::remove( settings.value("Log/Log_Path", "").toString() );
				}
			}
		}
	}
	else AQUse_Log( false );
	
	// Log File Name
	AQLog_Path( settings.value("Log/Log_Path", "").toString() );
	
	// Log Filter
	#ifdef Q_OS_WIN32
	AQUse_Debug_Output( settings.value("Log/Print_In_STDOUT", "no").toString() == "yes",
						settings.value("Log/Save_Debug", "no").toString() == "yes",
						settings.value("Log/Save_Warning", "yes").toString() == "yes",
						settings.value("Log/Save_Error", "yes").toString() == "yes" );
	#else
	AQUse_Debug_Output( settings.value("Log/Print_In_STDOUT", "yes").toString() == "yes",
						settings.value("Log/Save_Debug", "no").toString() == "yes",
						settings.value("Log/Save_Warning", "yes").toString() == "yes",
						settings.value("Log/Save_Error", "yes").toString() == "yes" );
	#endif
	
	// Create QApplication
	QApplication app( argc, argv );
	
	Set_Show_Error_Window( true );
	
	// Init emulators settings "data base"
	System_Info::Update_VM_Computers_List();
	
	// Check For First Start in root Mode
	#ifdef Q_OS_LINUX
	if( settings.value("First_Start", "yes").toString() == "yes" ) // This is a first start AQEMU on this computer?
	{
		uid_t user_uid = getuid();
		
		if( user_uid == 0 )
		{
			int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
											 QObject::tr("This is a first AQEMU start and program running in root mode.\n"
														 "In some Linux distributions you may have problems with configuration saving.\n"
														 "Close AQEMU?"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
			
			if( ret == QMessageBox::Yes ) return 0;
		}
	}
	#endif
	
	// This is an Upgrade of AQEMU? Find Previous Config...
	//FIXME if( QFile::exists(QDir::homePath() + "/.config/aqemu/AQEMU.conf") )
	if( QFile::exists(settings.fileName()) )
	{
		QString conf_ver = settings.value( AppInfo::configVersionKey(), AppInfo::version() ).toString();
		if( conf_ver.isEmpty() )
			conf_ver = settings.value( "AQEMU_Config_Version", AppInfo::version() ).toString();
		
		if( conf_ver == "0.5" )
		{
			AQDebug( "int main( int argc, char *argv[] )",
					 "AQEMU Config Version: 0.5\nRun Firt Start Wizard" );
			
			settings.setValue( "First_Start", "yes" );
			settings.setValue( AppInfo::configVersionKey(), AppInfo::version() );
		}
		else if( conf_ver == "0.7.2" || conf_ver == "0.7.3" || conf_ver == "0.8" )
		{
			AQDebug( "int main( int argc, char *argv[] )",
					 "AQEMU Config Version: 0.7.X\nStart Emulators Search..." );
			
			QMessageBox::information( NULL, QObject::tr("AQEMU emulators search"),
									  QObject::tr("AQEMU will search for emulators after uptating. Please wait."),
									  QMessageBox::Ok );
			
			First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
			
			if( first_start_win->Find_Emulators() )
				AQDebug( "int main( int argc, char *argv[] )",
						 "Find Emulators and Save Settings Complete" );
			else
				AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
								 QObject::tr("Cannot Find any Emulators installed in your OS! You should choose them In Advanced Settings!"), false );
			
			delete first_start_win;
			
			settings.setValue( AppInfo::configVersionKey(), AppInfo::version() );
		}
		else if( conf_ver == AppInfo::version() )
		{
			AQDebug( "int main( int argc, char *argv[] )",
					 QString( "newaqemu config version: %1" ).arg( AppInfo::version() ) );
		}
		else
		{
			// Remove Old Config!
			//FIXME if( QFile::copy(QDir::homePath() + "/.config/aqemu/AQEMU.conf",
			//	QDir::homePath() + "/.config/aqemu/AQEMU.conf.bak") )
			if( QFile::copy(settings.fileName(), settings.fileName() + ".bak") )
			{
				AQWarning( "int main( int argc, char *argv[] )",
						   "AQEMU Configuration File No Version 0.5. File Saved: AQEMU.conf.bak" );
				
				settings.clear();
				settings.sync();
				
				settings.setValue( AppInfo::configVersionKey(), AppInfo::version() );
			}
			else AQError( "int main( int argc, char *argv[] )", "Cannot Save Old Version AQEMU Configuration File!" );
		}
	}
	else
	{
		// Config File Not Found. This is the First Install
		settings.setValue( AppInfo::configVersionKey(), AppInfo::version() );
	}
	
	const auto dataFolderValue = [&settings]() {
		return settings.value( AppInfo::dataFolderKey(),
			settings.value( "AQEMU_Data_Folder", "" ).toString() ).toString();
	};

	// Find Data Folder
	if( settings.value( AppInfo::dataFolderKey(), settings.value( "AQEMU_Data_Folder", "" ).toString() ).toString().isEmpty() )
	{
		#ifdef Q_OS_WIN32
		if( QDir(QDir::currentPath() + "\\os_icons").exists() &&
			QDir(QDir::currentPath() + "\\os_templates").exists() )
		{
			settings.setValue( "AQEMU_Data_Folder", QDir::toNativeSeparators(QDir::currentPath()) );
			AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: " + QDir::currentPath() );
		}
		else
		{
			AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
							 QObject::tr("Cannot Find AQEMU Data!"), false );
		}
		#else
		QStringList dataDirs = AppInfo::dataSearchPaths();
		
		// Find data dir
		for( int dx = 0; dx < dataDirs.count(); ++dx )
		{
			QDir dataDir( dataDirs[dx] );
			
			if( dataDir.exists("./os_icons") &&
				dataDir.exists("./os_templates") &&
				dataDir.entryList(QStringList("*.rcc"), QDir::Files).isEmpty() == false )
			{
				settings.setValue( AppInfo::dataFolderKey(), dataDirs[dx] );
				break;
			}
		}
		
		// Found?
		if( settings.value( AppInfo::dataFolderKey(), settings.value( "AQEMU_Data_Folder", "" ).toString() ).toString().isEmpty() )
		{
			QMessageBox::information( NULL, QObject::tr("Error!"),
									  QObject::tr("Cannot locate newaqemu data folder.\n"
												  "Select the folder in the next dialog."),
									  QMessageBox::Ok );
			
			QString pickedDataDir = QFileDialog::getExistingDirectory( NULL, QObject::tr("Select newaqemu data folder:"),
																	  "/", QFileDialog::ShowDirsOnly );
			
			if( pickedDataDir.isEmpty() )
			{
				QMessageBox::critical( NULL, QObject::tr("Error!"),
									   QObject::tr("newaqemu requires a data folder to run.") );
				return -1;
			}
			else
			{
				if( ! pickedDataDir.endsWith("/") ) pickedDataDir += "/";
				settings.setValue( AppInfo::dataFolderKey(), pickedDataDir );
			}
		}
		#endif
	}
	
	// Load Images
	QString iconsThemeFile = "";
	
	iconsThemeFile = QDir::toNativeSeparators( dataFolderValue() + "/icons.rcc" );
		
	if( ! QResource::registerResource(iconsThemeFile) )
	{
		AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
						 QObject::tr("Cannot load newaqemu icon theme.\nFile \"%1\" not found.").arg(iconsThemeFile), false );
	}
	
	// This is a first start AQEMU on this computer?
	if( settings.value("First_Start", "yes").toString() == "yes" )
	{
		First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
		
		if( first_start_win->exec() == QDialog::Accepted ) AQDebug( "int main( int argc, char *argv[] )", "Fisrt Start Wizard Complete" );
		else AQWarning( "int main( int argc, char *argv[] )", "Fisrt Start Wizard Canceled!" );
		
		delete first_start_win;
	}
	
	// Load Language
	QTranslator appTranslator;
	
	if( settings.value("Language", "").toString() != "en" )
	{
		appTranslator.load( dataFolderValue() + settings.value("Language", "").toString() + ".qm",
							app.applicationDirPath() );
			
		app.installTranslator( &appTranslator );
	}
	
	// VM Directory Exists?
	QDir vm_dir;
	if( ! vm_dir.exists(settings.value("VM_Directory", "").toString()) )
	{
		int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
										 QObject::tr("AQEMU VM Folder doesn't Exists! Create It?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( ret == QMessageBox::Yes )
		{
			#ifdef Q_OS_WIN32
			vm_dir.mkpath( QDir::toNativeSeparators(QDir::homePath() + "/AQEMU_VM/") );
			#else
			vm_dir.mkpath( QDir::homePath() + "/.local/share/newaqemu/vms" );
			#endif
		}
	}
	
	// Check QEMU and KVM Versions
	Update_Emulators_List(); // FIXME
	
	// Show main window
	Main_Window Window;
	Window.show();
	
    app.setWindowIcon(QIcon(":/aqemu.png"));
    QApplication::setApplicationDisplayName( AppInfo::applicationName() );

	return app.exec();
}
