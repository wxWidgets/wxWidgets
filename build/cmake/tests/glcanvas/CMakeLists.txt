#############################################################################
# Purpose:     Opt-in wxGTK/Wayland OpenGL regression test
# Licence:     wxWindows licence
#############################################################################

option(wxTEST_WAYLAND "Register the headless Wayland OpenGL regression test" OFF)

wx_add_test(test_glcanvas CONSOLE_GUI NO_CTEST
    test.cpp
    asserthelper.cpp
    testableframe.cpp
    graphics/glcanvas.cpp
    LIBRARIES wxgl
)
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_glcanvas wxnet)
endif()
if(NOT wxTEST_WAYLAND)
    set_target_properties(test_glcanvas PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

if(wxTEST_WAYLAND)
    if(CMAKE_VERSION VERSION_LESS 3.18)
        message(FATAL_ERROR "wxTEST_WAYLAND requires CMake 3.18 or later")
    endif()
    find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
    find_program(wxTEST_SWAY_EXECUTABLE sway REQUIRED)
    find_program(wxTEST_SWAYMSG_EXECUTABLE swaymsg REQUIRED)
    add_test(NAME test_glcanvas_wayland
        COMMAND ${Python3_EXECUTABLE}
            ${wxSOURCE_DIR}/tests/graphics/wayland/run.py
            --test $<TARGET_FILE:test_glcanvas>
            --sway ${wxTEST_SWAY_EXECUTABLE}
            --swaymsg ${wxTEST_SWAYMSG_EXECUTABLE}
            --results ${wxBINARY_DIR}/wayland-results
    )
    set_tests_properties(test_glcanvas_wayland PROPERTIES TIMEOUT 240)
endif()
