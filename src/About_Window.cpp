/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
** Copyright (C) 2016 Tobias Gläßer (Qt5 port)
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

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <fstream>

#include "Utils.h"
#include "AppInfo.h"
#include "About_Window.h"

About_Window::About_Window( QWidget *parent ): QDialog( parent )
{
	ui.setupUi( this );
	
	// Minimum Size
	resize( width(), minimumSizeHint().height() );

	QString aboutHtml = ui.About_Text->text();
	aboutHtml.replace(
		QStringLiteral( ">1.0.0<" ),
		QStringLiteral( ">" ) + QString::fromUtf8( AppInfo::version() ) + QStringLiteral( "<" ) );
	ui.About_Text->setText( aboutHtml );

	// Thanks HTML Text
	ui.Edit_Thanks_To_Text->setHtml( tr(
	"<b>Developers:</b>\n"
	"<br>Andrey Rijov (aka RDron) - Original Author (Up until version 0.8.2)\n"
	"<br>Tobias Gläßer - Maintainer/Developer (Version 0.9.0 and up)\n"
	"<br>James Ed Randson - newaqemu fork (Qt6, GPU passthrough)\n"

	"<br><br><b>Contributors:</b>\n"
    "<br>Georg Schlisio - Arch Linux Packaging and Testing\n"
	"<br>Eli Carter - Sound card support patches\n"
	"<br>Max Brazhnikov - FreeBSD Port\n"
	"<br>Boris Savelev - ALTLinux Package\n"
	"<br>Denis Smirnov (aka reMiND) - Arch Linux Package and Other Help\n"
	"<br>Ignace Mouzannar - Debian Package\n"
	"<br>Michael Brandstetter - Improvement Scripts\n"
	"<br>Robert Warnke - German AQEMU Documentation Translation\n"
	"<br>Michael Schmöller (aka schmoemi) - German Translation and Correcting English translation\n"
	"<br>Guillem Rieu - Patch for Network Redirections\n"
	"<br>Karol Krenski - IP Address RegExp Fix\n"
	"<br>Alexander Romanov (aka romale) - Testing, New Ideas and Other Help\n"
	"<br>Timothy Redaelli - Patch for CMake\n"
	"<br>Pavel Serebryakov (aka Kronoph) - Correcting English translation\n"
	
	"<br><br><b>Special thanks:</b>\n"
	"<br>Sergey Sinitsa (aka sin)\n"
	"<br>Grigory Pulyaev (aka Rodegast)\n"
	"<br>Alexander Saifulin (aka Ne01eX)\n"
	"<br>Mihail Parshin (aka Skeeper)\n"
	"<br>Garret Acott (aka gacott)\n"
	"<br>Damir Vafin (aka Denver-22)\n"
	
	"<br><br><b>Thanks all www.nclug.ru group for testing and suggestions for improvement.</b>\n"
	
	"<br><br><b>AQEMU used code from QtEMU and Krdc thanks all developers:</b>\n"
	"<br>Tim Jansen (tim@tjansen.de)\n"
	"<br>Urs Wolfer (uwolfer@kde.org)\n"
	"<br>Ben Klopfenstein (benklop@gmail.com)\n"
	
	"<br><br><b>Icons:</b>\n"
	"<br>Oxygen - Icon Theme From Oxygen Team\n") );
	
	linksFilePath = AppInfo::findBundledDataFile( QStringLiteral( "docs/links.html" ) );
	Show_Links_File();

    ui.Tabs->setCurrentIndex(0);
}

void About_Window::Show_Links_File()
{
	const QString show_url = AppInfo::findBundledDataFile( QStringLiteral( "docs/links.html" ) );

	// #region agent log
	{
		std::ofstream log( "/home/jimedrand/Git/newaqemu/.cursor/debug-406f4b.log", std::ios::app );
		log << "{\"sessionId\":\"406f4b\",\"hypothesisId\":\"B\",\"location\":\"About_Window.cpp:Show_Links_File\","
		    << "\"message\":\"resolve links.html\","
		    << "\"data\":{\"path\":\"" << show_url.toStdString() << "\","
		    << "\"exists\":" << ( show_url.isEmpty() ? "false" : "true" ) << "},"
		    << "\"timestamp\":" << QDateTime::currentMSecsSinceEpoch() << "}\n";
	}
	// #endregion

	if( show_url.isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot Find AQEMU Links File!") );
		return;
	}

	linksFilePath = show_url;

	QFile links_file( show_url );

	if( ! links_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQError( "void About_Window::Show_Links_File()",
				 "Cannot Open \"docs/links.html\" File!" );
		return;
	}
	else
	{
		QTextStream out( &links_file );
		QString all = out.readAll();
		ui.Links_View->setHtml( all );
	}
}

