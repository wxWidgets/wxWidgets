cmake_minimum_required(VERSION 3.21)

project(wx_winui_install_consumer LANGUAGES CXX)

set(WX_WINUI_RUNTIME_FAULT_MODE "" CACHE STRING
    "Expected runtime test-fault support in the installed wx library: ON or OFF")
set_property(CACHE WX_WINUI_RUNTIME_FAULT_MODE PROPERTY STRINGS ON OFF)
if(NOT WX_WINUI_RUNTIME_FAULT_MODE STREQUAL "ON" AND
        NOT WX_WINUI_RUNTIME_FAULT_MODE STREQUAL "OFF")
    message(FATAL_ERROR
        "Set WX_WINUI_RUNTIME_FAULT_MODE explicitly to ON for a "
        "wxBUILD_TESTS=ALL library, or OFF for a shipping library")
endif()

find_package(wxWidgets CONFIG REQUIRED COMPONENTS core)
if(NOT COMMAND wx_winui3_deploy_runtime)
    message(FATAL_ERROR
        "The installed wxWinUI package did not provide "
        "wx_winui3_deploy_runtime()")
endif()

add_executable(wx_winui_install_consumer main.cpp)
target_compile_features(wx_winui_install_consumer PRIVATE cxx_std_17)
target_link_libraries(wx_winui_install_consumer PRIVATE wx::core)

# Installed consumers opt in explicitly; in-tree wx targets call the same
# helper automatically from the wx CMake functions.
wx_winui3_deploy_runtime(wx_winui_install_consumer)

enable_testing()
add_test(NAME wx_winui_install_consumer_smoke
    COMMAND "${CMAKE_COMMAND}"
        "-DWX_WINUI_TEST_EXE=$<TARGET_FILE:wx_winui_install_consumer>"
        "-DWX_WINUI_WX_DLL_DIR=$<TARGET_FILE_DIR:wx::core>"
        "-DWX_WINUI_RUNTIME_FAULT_MODE=${WX_WINUI_RUNTIME_FAULT_MODE}"
        -P "${CMAKE_CURRENT_LIST_DIR}/run-smoke.cmake")
