cmake_minimum_required(VERSION 3.5)

project(DLCreator VERSION 0.0.1 LANGUAGES CXX)

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
        resources/resources.qrc
        src/main.cpp
        src/mainWindow.cpp
        src/mainWindow.h
        src/mainWindow.ui
        src/tool.h
        src/history.h
        src/widgets/toolWidget/collapsibleSection.h
        src/widgets/toolWidget/toolButton.h
        src/widgets/toolWidget/toolWidget.h
        src/widgets/tabWidget/tabWidget.h
        src/widgets/tabWidget/tabScrollArea/tabScrollArea.h
        src/widgets/tabWidget/tabScrollArea/tabButton.h
        src/widgets/tabWidget/stackedContentWidget/stackedContentWidget.h
        src/widgets/tabWidget/stackedContentWidget/contentWidget/cellCollection.h
        src/widgets/tabWidget/stackedContentWidget/contentWidget/contentWidget.h
        src/widgets/tabWidget/stackedContentWidget/contentWidget/expandButton.h
        src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cell.h
        src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cell.cpp
        src/widgets/tabWidget/stackedContentWidget/contentWidget/cell/cellAction.h
        src/widgets/tabWidget/stackedHistoryWidget/stackedHistoryWidget.h
        src/widgets/tabWidget/stackedHistoryWidget/historyWidget/historyWidget.h
        src/widgets/tabWidget/stackedHistoryWidget/historyWidget/cellActionWidget.h
        src/widgets/components/nameFrame.h
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

target_link_libraries(DLCreator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Add project dir to the include path
target_include_directories(DLCreator PRIVATE "${CMAKE_SOURCE_DIR}")

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
