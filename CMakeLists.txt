cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(simplejson-cpp
        VERSION 0.1
        DESCRIPTION "Header-only C++ library to easily build JSON"
        HOMEPAGE_URL "https://github.com/WiFiBeat/simplejson-cpp"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(simplejson-cpp INTERFACE)
target_include_directories(simplejson-cpp INTERFACE .)