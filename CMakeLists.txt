cmake_minimum_required(VERSION 3.10.2)
project(videoPlayer VERSION 1.0.0 LANGUAGES CXX)

include_directories(headers)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

set(SOURCES
    src/videoplayer.cpp
    src/subtitles.cpp
    src/rightClickMenu.cpp
    src/main.cpp
    src/commands.cpp
    src/annotation.cpp
    src/playlist.cpp
)
set(HEADERS
    headers/videoplayer.h
    headers/subtitles.h
    headers/rightClickMenu.h
    headers/commands.h
    headers/annotation.h
    headers/playlist.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
qt5_use_modules(${PROJECT_NAME} Core Gui Widgets MultimediaWidgets Multimedia)
