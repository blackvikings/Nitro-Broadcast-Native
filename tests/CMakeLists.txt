find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(nitro_tests
    test_main.cpp
    test_scenes.cpp
    test_settings.cpp
    test_mixer.cpp
    test_wasapi_live.cpp
    test_hardening.cpp
)

target_link_libraries(nitro_tests
    PRIVATE
        nitro_core
        Qt6::Test
)

set_target_properties(nitro_tests PROPERTIES AUTOMOC ON)

add_test(NAME nitro_tests COMMAND nitro_tests)
