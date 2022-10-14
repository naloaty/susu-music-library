cmake_minimum_required(VERSION 3.5)

project(MusicLibrary VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_RESOURCES
    resources/resources.qrc
)

set(PROJECT_HEADERS
    src/database.h

    src/model/disc.h
    src/model/song.h
    src/model/band.h
    src/model/singer.h
    src/model/discsong.h
    src/model/author.h

    src/model/variantmaptablemodel.h
    src/model/simplecolumn.h
    src/model/lookupcolumn.h
    src/model/discsongcheckcolumn.h

    src/view/disc_editor.h
    src/view/song_editor.h
    src/view/singer_editor.h
    src/view/main_window.h
    src/view/band_editor.h
    src/view/help_note.h
)

set(PROJECT_SOURCES
    src/main.cpp
    src/database.cpp

    src/model/variantmaptablemodel.cpp
    src/model/simplecolumn.cpp
    src/model/lookupcolumn.cpp
    src/model/discsongcheckcolumn.cpp

    src/view/disc_editor.cpp
    src/view/song_editor.cpp
    src/view/singer_editor.cpp
    src/view/main_window.cpp
    src/view/band_editor.cpp
    src/view/help_note.cpp
)

set(PROJECT_GUI
    src/view/disc_editor.ui
    src/view/song_editor.ui
    src/view/singer_editor.ui
    src/view/main_window.ui
    src/view/band_editor.ui
    src/view/help_note.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MusicLibrary
        MANUAL_FINALIZATION
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        ${PROJECT_GUI}
        ${PROJECT_RESOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MusicLibrary APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MusicLibrary SHARED
            ${PROJECT_HEADERS}
            ${PROJECT_SOURCES}
            ${PROJECT_GUI}
            ${PROJECT_RESOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MusicLibrary
            ${PROJECT_HEADERS}
            ${PROJECT_SOURCES}
            ${PROJECT_GUI}
            ${PROJECT_RESOURCES}
        )
    endif()
endif()

target_link_libraries(MusicLibrary PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(MusicLibrary PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MusicLibrary)
endif()
