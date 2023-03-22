cmake_minimum_required(VERSION 3.10)
project(nix-build-results-project VERSION 0.0.1)
add_executable(nix-build-results main.cpp)
# Specify SYSTEM that nix should be configured for
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_ARCH)
set(BUILD_ARCH "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_ARCH}")
target_compile_definitions(nix-build-results PRIVATE SYSTEM="${BUILD_ARCH}")
target_link_libraries(nix-build-results PRIVATE nixcmd nixmain nixstore nixexpr)
install(TARGETS nix-build-results DESTINATION bin)
