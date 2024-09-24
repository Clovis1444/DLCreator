cmake_minimum_required(VERSION 3.16)
project(DLCreatorTests LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Test Widgets)

qt_standard_project_setup()

# Include main project sources
include_directories(..)

# Test sources to link
set(TEST_SOURCES
    TestMapSaver.cpp
)

qt_add_executable(DLCreatorTests
    ${TEST_SOURCES}
    ${PROJECT_SOURCES}
)

set_target_properties(DLCreatorTests PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(DLCreatorTests PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    Qt6::Widgets
)

install(TARGETS DLCreatorTests
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET DLCreatorTests
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# [TEST_BUILD_SCRIPT]: Copy testSamples dir to the tests build dir
set(TEST_SAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/testSamples")
set(TEST_SAMPLES_DEST "${BUILD_DIR}/tests/testSamples")
add_custom_command(
    TARGET DLCreatorTests POST_BUILD
    COMMAND echo '[TEST_BUILD_SCRIPT]: Copying ${TEST_SAMPLES_DIR} to the ${TEST_SAMPLES_DEST}' &&
    rm -rf ${TEST_SAMPLES_DEST} && 
    cp -r ${TEST_SAMPLES_DIR} ${TEST_SAMPLES_DEST}
)