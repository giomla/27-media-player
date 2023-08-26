cmake_minimum_required(VERSION 3.16)
project(videoPlayer VERSION 1.0.0 LANGUAGES CXX)


include_directories(headers)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

endif()

##THIS PATH IS AN EXAMPLE, YOU PUT THE DIRECTORY WHERE YOU ACCTUALLY HAVE QT INSTALLED
##THE VERSION THAT QMAKE AND CMAKE USE CAN BE DIFFERENT
list(APPEND CMAKE_PREFIX_PATH "/home/mladen/Qt/5.15.1/gcc_64")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)
find_package(Qt5 COMPONENTS MultimediaWidgets REQUIRED)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(SOURCES
    src/videoplayer.cpp
    src/subtitles.cpp
    src/rightClickMenu.cpp
    src/main.cpp
    src/commands.cpp
    src/annotation.cpp
    src/playlist.cpp
)

set(MOC_HEADERS
    headers/videoplayer.h
    headers/subtitles.h
    headers/rightClickMenu.h
    headers/commands.h
    headers/annotation.h
    headers/playlist.h
)

QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::MultimediaWidgets
    Qt5::Multimedia
    )
