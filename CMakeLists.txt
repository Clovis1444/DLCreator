cmake_minimum_required(VERSION 3.5)

project(DLCreator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Weffc++ -Wno-sign-compare -Wconversion -Wsign-conversion -pedantic-errors")

# Set your QT directory here
set(CMAKE_PREFIX_PATH $ENV{QT_PATH})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        # ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc      
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mainWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mainWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tool.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/history.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mapSaver/mapSaver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mapSaver/mapSaver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mapSaver/mapSaverJsons.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/toolWidget/collapsibleSection.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/toolWidget/toolButton.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/toolWidget/toolWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/tabWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/tabScrollArea/tabScrollArea.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/tabScrollArea/tabButton.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/stackedContentWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cellCollection.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/contentWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/expandButton.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cell.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cell.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellAction.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedHistoryWidget/stackedHistoryWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedHistoryWidget/historyWidget/historyWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedHistoryWidget/historyWidget/cellActionWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/components/nameFrame.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/gridManager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellItem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellItem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/tabWidget/stackedContentWidget/contentWidget/cellItem/cellAction.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DLCreator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DLCreator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DLCreator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DLCreator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Add main.cpp
target_sources(DLCreator PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(DLCreator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Add project dir to the include path
target_include_directories(DLCreator PRIVATE "${CMAKE_SOURCE_DIR}")

# [BUILD_SCRIPT]: Copy resources dir to build dir
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCES_DEST "${BUILD_DIR}/resources")
add_custom_command(
    TARGET DLCreator POST_BUILD
    COMMAND echo '[BUILD_SCRIPT]: Copying ${RESOURCES_DIR} to the ${RESOURCES_DEST}' &&
    rm -rf ${RESOURCES_DEST} && 
    cp -r ${RESOURCES_DIR} ${RESOURCES_DEST}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DLCreator)
endif()
set_target_properties(DLCreator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DLCreator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DLCreator)
endif()


# Add a test directory
add_subdirectory(tests)
