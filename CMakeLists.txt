cmake_minimum_required(VERSION 3.0)
project(ReactiveTelegram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CONANBUILDINFOCMAKE_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")

if(EXISTS "${CONANBUILDINFOCMAKE_FILE}")
    include("${CONANBUILDINFOCMAKE_FILE}")
    conan_basic_setup(TARGETS)
else()
    message(FATAL_ERROR "${CONANBUILDINFOCMAKE_FILE} file not found. Make sure you have run conan install .. --build=missing from the build directory")
endif()

add_library(urp INTERFACE)
target_include_directories(urp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(urp INTERFACE CONAN_PKG::boost_signals2)

add_executable(bot bot.cpp)
target_link_libraries(bot PRIVATE urp CONAN_PKG::tgbot_cpp CONAN_PKG::fmt CONAN_PKG::range-v3)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    configure_file(
        ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py.in
        ${CMAKE_SOURCE_DIR}/.ycm_extra_conf.py
    )
endif()
