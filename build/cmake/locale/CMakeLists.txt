if(wxBUILD_LOCALES)
    if(wxBUILD_LOCALES STREQUAL "ON")
        find_package(Gettext)
    else() # wxBUILD_LOCALES is AUTO, don't give any messages if Gettext is not available
        find_package(Gettext QUIET)
    endif()
    if(NOT GETTEXT_FOUND)
        if(wxBUILD_LOCALES STREQUAL "ON")
            message(FATAL_ERROR "Translations can't be built without gettext: either install it or don't set wxBUILD_LOCALES to ON")
        endif()
        wx_option_force_value(wxBUILD_LOCALES OFF)
    endif()
endif()

mark_as_advanced(GETTEXT_MSGMERGE_EXECUTABLE GETTEXT_MSGFMT_EXECUTABLE)

if(NOT wxBUILD_LOCALES)
    return()
endif()

# list and process the po files
file(GLOB _po_files "${wxSOURCE_DIR}/locale/*.po")
set(_gmo_files)
foreach(_po_file ${_po_files})
    get_filename_component(name "${_po_file}" NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" lang ${name})
    set(_gmo_file "${CMAKE_CURRENT_BINARY_DIR}/${lang}.gmo")

    add_custom_command(OUTPUT "${_gmo_file}"
        COMMAND ${CMAKE_COMMAND}
                -DMSGFMT=${GETTEXT_MSGFMT_EXECUTABLE}
                -DPO=${_po_file} -DGMO=${_gmo_file}
                -P ${wxSOURCE_DIR}/build/cmake/modules/msgfmt.cmake
        DEPENDS "${_po_file}"
        COMMENT "Generating ${lang}.gmo"
        VERBATIM)
    list(APPEND _gmo_files "${_gmo_file}")

    wx_install(FILES       "${_gmo_file}"
               DESTINATION "share/locale/${lang}/LC_MESSAGES"
               RENAME      "wxstd-${wxMAJOR_VERSION}.${wxMINOR_VERSION}.mo"
    )
endforeach()

add_custom_target(wxstd_translations ALL DEPENDS ${_gmo_files})

# put all pofiles targets in a group to not clutter visual studio
set(base_name "pofiles")
set(target_index 0)
set(target_name ${base_name})
while(TARGET ${target_name})
    set_target_properties(${target_name} PROPERTIES FOLDER "Locales")
    math(EXPR target_index "${target_index}+1")
    set(target_name "${base_name}_${target_index}")
endwhile()
