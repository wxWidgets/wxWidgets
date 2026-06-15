cmake_minimum_required(VERSION 3.16)
project(wxWebViewExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base webview)
include(${wxWidgets_USE_FILE})

add_executable(wxWebViewExample
    runscript.cpp
)

target_link_libraries(wxWebViewExample ${wxWidgets_LIBRARIES})

add_custom_command(TARGET wxWebViewExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/runscript.html
        $<TARGET_FILE_DIR:wxWebViewExample>/runscript.html
)

add_custom_command(TARGET wxWebViewExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/runscript.js
        $<TARGET_FILE_DIR:wxWebViewExample>/runscript.js
)