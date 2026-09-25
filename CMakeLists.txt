cmake_minimum_required(VERSION 3.21)

project(NitroBroadcast
    VERSION 0.1.0
    DESCRIPTION "Nitro Broadcast — native desktop broadcasting studio"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(NITRO_BUILD_TESTS "Build unit tests" ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Test
)

qt_standard_project_setup(REQUIRES 6.5)

# Silence Qt 6.8 qmldir-extra-dir policy noise; we ship a flat module import.
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

add_subdirectory(src)

set(NITRO_QML_FILES
    qml/Main.qml
    qml/Theme/Theme.qml
    qml/components/NitroButton.qml
    qml/components/NitroIconButton.qml
    qml/components/NitroIcon.qml
    qml/components/NitroPanel.qml
    qml/components/NitroMeter.qml
    qml/components/NitroStatusBadge.qml
    qml/components/NitroSceneItem.qml
    qml/components/NitroSourceItem.qml
    qml/components/NitroSlider.qml
    qml/studio/TopBar.qml
    qml/studio/ScenesPanel.qml
    qml/studio/SourcesPanel.qml
    qml/studio/PreviewProgram.qml
    qml/studio/MixerPanel.qml
    qml/studio/StatusBar.qml
    qml/dialogs/SettingsDialog.qml
)

set_source_files_properties(qml/Main.qml PROPERTIES QT_RESOURCE_ALIAS "Main.qml")
set_source_files_properties(qml/Theme/Theme.qml
    PROPERTIES QT_RESOURCE_ALIAS "Theme/Theme.qml" QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(qml/components/NitroButton.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroButton.qml")
set_source_files_properties(qml/components/NitroIconButton.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroIconButton.qml")
set_source_files_properties(qml/components/NitroIcon.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroIcon.qml")
set_source_files_properties(qml/components/NitroPanel.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroPanel.qml")
set_source_files_properties(qml/components/NitroMeter.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroMeter.qml")
set_source_files_properties(qml/components/NitroStatusBadge.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroStatusBadge.qml")
set_source_files_properties(qml/components/NitroSceneItem.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroSceneItem.qml")
set_source_files_properties(qml/components/NitroSourceItem.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroSourceItem.qml")
set_source_files_properties(qml/components/NitroSlider.qml PROPERTIES QT_RESOURCE_ALIAS "components/NitroSlider.qml")
set_source_files_properties(qml/studio/TopBar.qml PROPERTIES QT_RESOURCE_ALIAS "studio/TopBar.qml")
set_source_files_properties(qml/studio/ScenesPanel.qml PROPERTIES QT_RESOURCE_ALIAS "studio/ScenesPanel.qml")
set_source_files_properties(qml/studio/SourcesPanel.qml PROPERTIES QT_RESOURCE_ALIAS "studio/SourcesPanel.qml")
set_source_files_properties(qml/studio/PreviewProgram.qml PROPERTIES QT_RESOURCE_ALIAS "studio/PreviewProgram.qml")
set_source_files_properties(qml/studio/MixerPanel.qml PROPERTIES QT_RESOURCE_ALIAS "studio/MixerPanel.qml")
set_source_files_properties(qml/studio/StatusBar.qml PROPERTIES QT_RESOURCE_ALIAS "studio/StatusBar.qml")
set_source_files_properties(qml/dialogs/SettingsDialog.qml PROPERTIES QT_RESOURCE_ALIAS "dialogs/SettingsDialog.qml")
set_source_files_properties(resources/themes/colors.json PROPERTIES QT_RESOURCE_ALIAS "themes/colors.json")

qt_add_executable(NitroBroadcast
    src/main.cpp
)

qt_add_qml_module(NitroBroadcast
    URI NitroBroadcast
    VERSION 1.0
    QML_FILES ${NITRO_QML_FILES}
    RESOURCES
        resources/themes/colors.json
)

target_link_libraries(NitroBroadcast
    PRIVATE
        nitro_core
        Qt6::QuickControls2
)

set_target_properties(NitroBroadcast PROPERTIES
    WIN32_EXECUTABLE FALSE
)

if(NITRO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
