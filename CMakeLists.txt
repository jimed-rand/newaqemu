# newaqemu CMake Project

CMAKE_MINIMUM_REQUIRED( VERSION 3.16 FATAL_ERROR )
PROJECT( newaqemu VERSION 1.0.0 LANGUAGES CXX )

OPTION( DEBUG "Build with debug information" )
OPTION( WITHOUT_EMBEDDED_DISPLAY "Build without support Embedded VNC Display" )
OPTION( WITH_SPICE_DISPLAY "Build SPICE display helper (uses external remote-viewer)" ON )
OPTION( UPDATE_TRANSLATIONS "Update source translation files" )
OPTION( INSTALL_MAN "Install MAN page file" "ON" )
OPTION( WITH_LIBVIRT "Build libvirt passthrough backend helpers" ON )

IF( DEBUG )
	SET( CMAKE_VERBOSE_MAKEFILE ON )
	ADD_DEFINITIONS( -Wall -g )
ENDIF()

SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )

SET( newaqemu_headers
	src/AppInfo.h
	src/IP_Edit.h
	src/Dialog_Button_Box.h
	src/docopt/docopt.h
	src/Settings_Widget.h
	src/QDom.h
	src/tinyxml2.h
	src/RegExpCompat.h
	src/QtCompat.h
	src/About_Window.h
	src/Add_New_Device_Window.h
	src/Add_Port_Window.h
	src/Add_USB_Port_Window.h
	src/Advanced_Settings_Window.h
	src/Boot_Device_Window.h
	src/Convert_HDD_Image_Window.h
	src/Copy_VM_Window.h
	src/Create_HDD_Image_Window.h
	src/Create_Template_Window.h
	src/Delete_VM_Files_Window.h
	src/Device_Manager_Widget.h
	src/Edit_Emulator_Version_Window.h
	src/Emulator_Control_Window.h
	src/Error_Log_Window.h
	src/First_Start_Wizard.h
	src/Folder_Sharing_Widget.h
	src/HDD_Image_Info.h
	src/Main_Window.h
	src/Monitor_Window.h
	src/Network_Widget.h
	src/Old_Network_Widget.h
	src/Ports_Tab_Widget.h
	src/Properties_Window.h
	src/Emulator_Options_Window.h
	src/Select_Icon_Window.h
	src/SMP_Settings_Window.h
	src/Snapshot_Properties_Window.h
	src/Snapshots_Window.h
	src/SPICE_Settings_Widget.h
	src/System_Info.h
	src/Utils.h
	src/VM.h
	src/VM_Devices.h
	src/VM_Wizard_Window.h
	src/VNC_Password_Window.h
	src/host/HostInfo.h
	src/host/HostHwProbe.h
	src/passthrough/PassthroughProfile.h
	src/libvirt/LibvirtRunner.h
	src/libvirt/DomainXmlBuilder.h
	src/Passthrough_Setup_Widget.h
	src/UiTooltips.h
)

SET( newaqemu_sources
	src/AppInfo.cpp
	src/IP_Edit.cpp
	src/Dialog_Button_Box.cpp
	src/docopt/docopt.cpp
	src/Settings_Widget.cpp
	src/QDom.cpp
	src/tinyxml2.cpp
	src/RegExpCompat.cpp
	src/About_Window.cpp
	src/Add_New_Device_Window.cpp
	src/Add_Port_Window.cpp
	src/Add_USB_Port_Window.cpp
	src/Advanced_Settings_Window.cpp
	src/Boot_Device_Window.cpp
	src/Convert_HDD_Image_Window.cpp
	src/Copy_VM_Window.cpp
	src/Create_HDD_Image_Window.cpp
	src/Create_Template_Window.cpp
	src/Delete_VM_Files_Window.cpp
	src/Device_Manager_Widget.cpp
	src/Edit_Emulator_Version_Window.cpp
	src/Emulator_Control_Window.cpp
	src/Error_Log_Window.cpp
	src/First_Start_Wizard.cpp
	src/Folder_Sharing_Widget.cpp
	src/HDD_Image_Info.cpp
	src/main.cpp
	src/Main_Window.cpp
	src/Monitor_Window.cpp
	src/Network_Widget.cpp
	src/Old_Network_Widget.cpp
	src/Ports_Tab_Widget.cpp
	src/Properties_Window.cpp
	src/Emulator_Options_Window.cpp
	src/Select_Icon_Window.cpp
	src/SMP_Settings_Window.cpp
	src/Snapshot_Properties_Window.cpp
	src/Snapshots_Window.cpp
	src/SPICE_Settings_Widget.cpp
	src/System_Info.cpp
	src/Utils.cpp
	src/VM.cpp
	src/VM_Devices.cpp
	src/VM_Wizard_Window.cpp
	src/VNC_Password_Window.cpp
	src/host/HostInfo.cpp
	src/host/HostHwProbe.cpp
	src/passthrough/PassthroughProfile.cpp
	src/libvirt/LibvirtRunner.cpp
	src/libvirt/DomainXmlBuilder.cpp
	src/Passthrough_Setup_Widget.cpp
	src/UiTooltips.cpp
)

SET( newaqemu_ui
	src/About_Window.ui
	src/Add_New_Device_Window.ui
	src/Add_Port_Window.ui
	src/Add_USB_Port_Window.ui
	src/Advanced_Settings_Window.ui
	src/Boot_Device_Window.ui
	src/Convert_HDD_Image_Window.ui
	src/Copy_VM_Window.ui
	src/Create_HDD_Image_Window.ui
	src/Create_Template_Window.ui
	src/Delete_VM_Files_Window.ui
	src/Device_Manager_Widget.ui
	src/Edit_Emulator_Version_Window.ui
	src/Emulator_Control_Window.ui
	src/Error_Log_Window.ui
	src/First_Start_Wizard.ui
	src/Folder_Sharing_Widget.ui
	src/Main_Window.ui
	src/Monitor_Window.ui
	src/Network_Widget.ui
	src/Old_Network_Widget.ui
	src/Ports_Tab_Widget.ui
	src/Properties_Window.ui
	src/Emulator_Options_Window.ui
	src/Select_Icon_Window.ui
	src/Settings_Window.ui
	src/SMP_Settings_Window.ui
	src/Snapshot_Properties_Window.ui
	src/Snapshots_Window.ui
	src/SPICE_Settings_Widget.ui
	src/VM_Wizard_Window.ui
	src/VNC_Password_Window.ui
)

SET( newaqemu_qrc
	resources/shared_images.qrc
	resources/icons.qrc
)

SET( newaqemu_man_page resources/docs/newaqemu.1 )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

FIND_PACKAGE( Qt6 REQUIRED COMPONENTS Core Widgets Network Test PrintSupport )

IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	FIND_PACKAGE( LibVNCServer REQUIRED )
	SET( newaqemu_headers ${newaqemu_headers}
		src/Embedded_Display/Machine_View.h
		src/Embedded_Display/remoteview.h
		src/Embedded_Display/vncclientthread.h
		src/Embedded_Display/vncview.h )
	SET( newaqemu_sources ${newaqemu_sources}
		src/Embedded_Display/Machine_View.cpp
		src/Embedded_Display/remoteview.cpp
		src/Embedded_Display/vncclientthread.cpp
		src/Embedded_Display/vncview.cpp )
	IF( WITH_SPICE_DISPLAY )
		SET( newaqemu_headers ${newaqemu_headers} src/Embedded_Display/SpiceView.h )
		SET( newaqemu_sources ${newaqemu_sources} src/Embedded_Display/SpiceView.cpp )
		ADD_DEFINITIONS( -DSPICE_DISPLAY )
	ENDIF()
	ADD_DEFINITIONS( -DVNC_DISPLAY -DQTONLY )
ENDIF()

SET( translations
	resources/translations/German.ts
	resources/translations/Russian.ts
	resources/translations/Ukrainian.ts
	resources/translations/French.ts
	resources/translations/English.ts )

IF( UPDATE_TRANSLATIONS )
	qt6_create_translation( qm_files ${CMAKE_CURRENT_SOURCE_DIR} ${newaqemu_sources} ${newaqemu_ui} ${translations} )
ELSE()
	qt6_add_translation( qm_files ${translations} )
ENDIF()

ADD_EXECUTABLE( newaqemu
	${newaqemu_sources}
	${newaqemu_ui}
	${newaqemu_qrc}
	${qm_files}
)

TARGET_INCLUDE_DIRECTORIES( newaqemu PUBLIC src/ )
TARGET_COMPILE_DEFINITIONS( newaqemu PRIVATE NEWAQEMU_VERSION="${PROJECT_VERSION}" )

TARGET_LINK_LIBRARIES( newaqemu
	Qt6::Core
	Qt6::Widgets
	Qt6::Network
	Qt6::Test
	Qt6::PrintSupport
)

IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	TARGET_LINK_LIBRARIES( newaqemu ${LIBVNCCLIENT_LIBRARIES} )
ENDIF()

IF( WITH_LIBVIRT )
	TARGET_COMPILE_DEFINITIONS( newaqemu PRIVATE WITH_LIBVIRT=1 )
ENDIF()

IF( WIN32 )
	TARGET_LINK_LIBRARIES( newaqemu setupapi cfgmgr32 )
ENDIF()

INSTALL( TARGETS newaqemu DESTINATION bin )
INSTALL( FILES ${qm_files} DESTINATION share/newaqemu )
INSTALL( PROGRAMS scripts/newaqemu-host-setup DESTINATION bin )

FILE( GLOB os_icons "${CMAKE_CURRENT_SOURCE_DIR}/resources/os_icons/*.*" )
INSTALL( FILES ${os_icons} DESTINATION share/newaqemu/os_icons )

FILE( GLOB os_templates "${CMAKE_CURRENT_SOURCE_DIR}/resources/os_templates/*.*")
INSTALL( FILES ${os_templates} DESTINATION share/newaqemu/os_templates )

FILE( GLOB links "${CMAKE_CURRENT_SOURCE_DIR}/resources/docs/links.html" )
INSTALL( FILES ${links} DESTINATION share/newaqemu/docs )

FILE( GLOB pixmaps "${CMAKE_CURRENT_SOURCE_DIR}/menu_data/*.png" )
INSTALL( FILES ${pixmaps} DESTINATION share/pixmaps )

INSTALL( FILES menu_data/newaqemu.desktop DESTINATION share/applications )

MACRO( create_docs _sources )
	FOREACH( curFile ${ARGN} )
		GET_FILENAME_COMPONENT( _in ${curFile} ABSOLUTE )
		GET_FILENAME_COMPONENT( _basename ${curFile} NAME_WE )
		SET( _out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.bz2 )
		ADD_CUSTOM_COMMAND( OUTPUT ${_out} COMMAND bzip2 ARGS -zkf9 ${_in} DEPENDS ${_in} )
		SET( ${_sources} ${${_sources}} ${_out} )
	ENDFOREACH()
ENDMACRO()

CREATE_DOCS( docs_bz2 AUTHORS CHANGELOG README TODO )
ADD_CUSTOM_TARGET( docs ALL DEPENDS ${docs_bz2} )
INSTALL( FILES ${docs_bz2} DESTINATION share/doc/newaqemu )

MACRO( pack_icons _sources )
	FOREACH( curFile ${ARGN} )
		GET_FILENAME_COMPONENT( _in ${curFile} ABSOLUTE )
		GET_FILENAME_COMPONENT( _basename ${curFile} NAME_WE )
		SET( _out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.rcc )
		ADD_CUSTOM_COMMAND( OUTPUT ${_out} COMMAND rcc ARGS ${_in} -binary -o ${_out} DEPENDS ${_in} )
		SET( ${_sources} ${${_sources}} ${_out} )
	ENDFOREACH()
ENDMACRO()

PACK_ICONS( gui_icons resources/shared_images.qrc resources/icons.qrc )
ADD_CUSTOM_TARGET( icons ALL DEPENDS ${gui_icons} )
INSTALL( FILES ${gui_icons} DESTINATION share/newaqemu )

IF( INSTALL_MAN )
	INSTALL( FILES ${CMAKE_SOURCE_DIR}/${newaqemu_man_page} DESTINATION share/man/man1 )
ENDIF()
