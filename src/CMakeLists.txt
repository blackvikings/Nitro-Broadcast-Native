# C++ sources as a static library (no QML here)
add_library(nitro_core STATIC
    app/NitroApplication.cpp
    core/NitroEngine.cpp
    core/NitroState.cpp
    config/NitroSettings.cpp
    diagnostics/NitroLogger.cpp
    scenes/Scene.cpp
    scenes/SceneManager.cpp
    sources/SourceType.cpp
    sources/SourceTransform.cpp
    sources/SourceBase.cpp
    sources/SourceFactory.cpp
    sources/ConcreteSources.cpp
    sources/WinCaptureEnum.cpp
    sources/SourceListModel.cpp
    sources/SourcePropertiesController.cpp
    audio/AudioMixer.cpp
    audio/AudioClock.cpp
    audio/AudioRingBuffer.cpp
    audio/AudioMeter.cpp
    audio/AudioDeviceManager.cpp
    audio/WasapiCapture.cpp
    audio/AudioEngine.cpp
    # Headers with Q_OBJECT must be listed for AUTOMOC
    ${CMAKE_SOURCE_DIR}/include/nitro/app/NitroApplication.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/core/NitroState.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/config/NitroSettings.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/scenes/SceneManager.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/sources/SourceListModel.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/sources/SourceFactory.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/sources/SourcePropertiesController.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/sources/CaptureDeviceHelper.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/audio/AudioMixer.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/audio/AudioDeviceManager.hpp
    ${CMAKE_SOURCE_DIR}/include/nitro/audio/AudioEngine.hpp
)

target_include_directories(nitro_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(nitro_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
)

if(WIN32)
    target_link_libraries(nitro_core PUBLIC Ole32 Avrt Dwmapi)
    target_compile_definitions(nitro_core PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

set_target_properties(nitro_core PROPERTIES AUTOMOC ON)
