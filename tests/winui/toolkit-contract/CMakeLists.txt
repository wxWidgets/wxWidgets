cmake_minimum_required(VERSION 3.21)
project(wx_toolkit_contract LANGUAGES CXX)

if(NOT WX_EXPECT_TOOLKIT MATCHES "^(msw|winui)$")
    message(FATAL_ERROR "Pass -DWX_EXPECT_TOOLKIT=msw or winui")
endif()
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core)

# Deliberately compile separate translation units, without wxprec.h or a source
# tree include path. This is an installed-header contract, not a GUI test.
add_library(wx_toolkit_contract OBJECT
    dispatch.cpp radiobox.cpp spinctrl.cpp spinbutt.cpp)
target_link_libraries(wx_toolkit_contract PRIVATE wx::core)
target_compile_features(wx_toolkit_contract PRIVATE cxx_std_17)
target_compile_definitions(wx_toolkit_contract PRIVATE
    WX_EXPECT_WINUI=$<STREQUAL:${WX_EXPECT_TOOLKIT},winui>)

enable_testing()
add_test(NAME wx_installed_toolkit_compile_contract
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
        --config $<CONFIG> --target wx_toolkit_contract)
set_tests_properties(wx_installed_toolkit_compile_contract PROPERTIES
    TIMEOUT 120 LABELS "compile-only")
