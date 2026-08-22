cmake_minimum_required(VERSION 3.21)

project(wx_winui_install_consumer LANGUAGES CXX)

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
        -P "${CMAKE_CURRENT_LIST_DIR}/run-smoke.cmake")
