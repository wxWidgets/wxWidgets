if(wxBUILD_LOCALES STREQUAL "AUTO")
    find_package(Gettext QUIET)
elseif(wxBUILD_LOCALES)
    find_package(Gettext REQUIRED)
endif()

mark_as_advanced(GETTEXT_MSGMERGE_EXECUTABLE)
mark_as_advanced(GETTEXT_MSGFMT_EXECUTABLE)

if(NOT GETTEXT_FOUND OR wxBUILD_LOCALES STREQUAL "OFF")
    return()
endif()

# list and process the po files
file(GLOB _po_files "${wxSOURCE_DIR}/locale/*.po")
foreach(_po_file ${_po_files})
    get_filename_component(name "${_po_file}" NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" lang ${name})

    gettext_process_po_files(${lang} ALL PO_FILES "${_po_file}")

    wx_install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/${lang}.gmo"
               DESTINATION "share/locale/${lang}/LC_MESSAGES"
               RENAME      "wxstd-${wxMAJOR_VERSION}.${wxMINOR_VERSION}.mo"
    )
endforeach()

# put all pofiles targets in a group to not clutter visual studio
set(base_name "pofiles")
set(target_index 0)
set(target_name ${base_name})
while(TARGET ${target_name})
    set_target_properties(${target_name} PROPERTIES FOLDER "Locales")
    math(EXPR target_index "${target_index}+1")
    set(target_name "${base_name}_${target_index}")
endwhile()
