cmake_minimum_required(VERSION 3.22)
project(Proxima C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(CURL REQUIRED libcurl)

find_package(cJSON REQUIRED)
set(CJSON_LIB cjson)


include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

include_directories(${CURL_INCLUDE_DIRS})

add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(Proxima src/main.c src/cairo_function.c src/cta_api.c src/key.h)
target_link_libraries(Proxima ${GTK4_LIBRARIES})
target_link_libraries(Proxima ${CURL_LIBRARIES})
target_link_libraries(Proxima ${CJSON_LIBRARIES})