cmake_minimum_required(VERSION 3.5)

project(dentrix_editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/CPM.cmake)

# Adding external deps

CPMAddPackage(
    NAME mcg
    GIT_REPOSITORY git@github.com:dentrix-dev/mcg.git
    GIT_TAG master
    OPTIONS
        "MCG_BUILD_PLAYGROUND OFF"
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

set(UI_SOURCES
    DentrixUI/mainwindow.cpp
    DentrixUI/mainwindow.h
    DentrixUI/mainwindow.ui
    DentrixUI/glwidget.cpp
    DentrixUI/glwidget.h
    DentrixUI/Brush.cpp
    DentrixUI/Brush.h
    DentrixUI/cursorFactory.cpp
    DentrixUI/cursorFactory.h
)

set(CORE_SOURCES
    DentrixCore/scene.cpp
    DentrixCore/scene.h
    DentrixCore/shader.cpp
    DentrixCore/shader.h
    DentrixCore/camera.cpp
    DentrixCore/camera.h
    DentrixCore/mesh.cpp
    DentrixCore/mesh.h
    DentrixCore/utils.cpp
    DentrixCore/utils.h
    DentrixCore/filehandler.cpp
    DentrixCore/filehandler.h
    DentrixCore/Gizmo/gizmo.cpp
    DentrixCore/Gizmo/gizmo.h
    DentrixCore/Gizmo/gizmoComponent.cpp
    DentrixCore/Gizmo/gizmoComponent.h
)

set(PROJECT_SOURCES
    main.cpp
    ${UI_SOURCES}
    ${CORE_SOURCES}
        DentrixUI/Widgets/loadingtoolbar.cpp
        DentrixUI/Widgets/loadingtoolbar.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(dentrix_editor
        MANUAL_FINALIZATION
        resources.qrc
        ${PROJECT_SOURCES}
        DentrixUI/Widgets/ToolPanelWidgets/uniformscalepanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/uniformscalepanelwidget.cpp
        DentrixUI/Widgets/ToolPanelWidgets/directionalscalepanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/directionalscalepanelwidget.cpp
        DentrixUI/Widgets/ToolPanelWidgets/deformationpanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/deformationpanelwidget.cpp
				DentrixUI/Widgets/ToolPanelWidgets/smoothpanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/smoothpanelwidget.cpp
        DentrixUI/Widgets/rightpanelstackedwidget.h DentrixUI/Widgets/rightpanelstackedwidget.cpp
        DentrixUI/Widgets/toolbarwidget.h DentrixUI/Widgets/toolbarwidget.cpp
		DentrixUI/Widgets/ToolPanelWidgets/rotatepanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/rotatepanelwidget.cpp
		DentrixUI/Widgets/ToolPanelWidgets/movetoothpanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/movetoothpanelwidget.cpp
		DentrixUI/Widgets/ToolPanelWidgets/movejawpanelwidget.h DentrixUI/Widgets/ToolPanelWidgets/movejawpanelwidget.cpp
		DentrixUI/Widgets/Dialogs/savemodeldialog.h DentrixUI/Widgets/Dialogs/savemodeldialog.cpp
		DentrixUI/Workers/jawloaderworker.h DentrixUI/Workers/jawloaderworker.cpp
		DentrixUI/Workers/archalignworker.h DentrixUI/Workers/archalignworker.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET dentrix_editor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(dentrix_editor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(dentrix_editor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(dentrix_editor
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Widgets 
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets 
        mcg
)

target_include_directories(dentrix_editor PRIVATE
    ${CMAKE_SOURCE_DIR}/DentrixUI
    ${CMAKE_SOURCE_DIR}/DentrixCore
    ${CMAKE_SOURCE_DIR}/libs
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.dentrix_editor)
endif()
set_target_properties(dentrix_editor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS dentrix_editor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(dentrix_editor)
endif()

# Copy shaders to the build directory
file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/shaders/*")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
foreach(SHADER ${SHADERS})
    configure_file(${SHADER} ${CMAKE_BINARY_DIR}/shaders COPYONLY)
endforeach()
