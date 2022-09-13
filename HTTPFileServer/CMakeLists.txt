cmake_minimum_required(VERSION 3.9)
project(HTTPFileServer)

# Set our project to use C++ 17
set(CMAKE_CXX_STANDARD 17)

# Set our source files
set(SOURCE_FILES HTTPFileServer.cpp)
set(HTTP_SERVER_FILES include/http_server/http_server.h include/http_server/http_server.cpp)
set(DB_ADAPTER_FILES include/http_server/db_adapter/db_adapter.h include/http_server/db_adapter/db_adapter.cpp)

find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

# Create our executable file from our source files
add_executable(HTTPFileServer ${SOURCE_FILES} ${HTTP_SERVER_FILES} ${DB_ADAPTER_FILES})
target_link_libraries(HTTPFileServer PkgConfig::Pistache -lpqxx -lpq)