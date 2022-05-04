if(wxUSE_NANOSVG STREQUAL "builtin")
    wx_add_builtin_library(wxnanosvg
        3rdparty/nanosvg/src/nanosvg.h
    )

    target_compile_definitions(wxnanosvg PRIVATE NANOSVG_IMPLEMENTATION NANOSVGRAST_IMPLEMENTATION)

    set(NANOSVG_LIBRARIES wxnanosvg)
    set(NANOSVG_INCLUDE_DIRS ${wxSOURCE_DIR}/3rdparty/nanosvg/src)
    set_target_properties(wxnanosvg PROPERTIES LINKER_LANGUAGE C)
elseif(wxUSE_NANOSVG)
    find_package(NanoSVG REQUIRED)
    set(NANOSVG_LIBRARIES NanoSVG::nanosvgrast)
    get_target_property(_nanosvg_incl NanoSVG::nanosvg INTERFACE_INCLUDE_DIRECTORIES)
    set(NANOSVG_INCLUDE_DIRS ${_nanosvg_incl})
endif()
