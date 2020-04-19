#############################################################################
# Name:        build/cmake/lib/png.cmake
# Purpose:     Use external or internal libpng
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LIBPNG STREQUAL "builtin")
    if(NOT MSVC)
        set(PNG_EXTRA_SOURCES
            src/png/mips/filter_msa_intrinsics.c
            src/png/mips/mips_init.c
            src/png/powerpc/filter_vsx_intrinsics.c
            src/png/powerpc/powerpc_init.c
        )
    endif()
    wx_add_builtin_library(wxpng
            src/png/png.c
            src/png/pngerror.c
            src/png/pngget.c
            src/png/pngmem.c
            src/png/pngpread.c
            src/png/pngread.c
            src/png/pngrio.c
            src/png/pngrtran.c
            src/png/pngrutil.c
            src/png/pngset.c
            src/png/pngtrans.c
            src/png/pngwio.c
            src/png/pngwrite.c
            src/png/pngwtran.c
            src/png/pngwutil.c
            src/png/arm/arm_init.c
            src/png/arm/filter_neon_intrinsics.c
            src/png/arm/palette_neon_intrinsics.c
            src/png/intel/intel_init.c
            src/png/intel/filter_sse2_intrinsics.c
            ${PNG_EXTRA_SOURCES}
    )
    if(WIN32)
        # define this to get rid of a warning about using POSIX lfind():
        # confusingly enough, we do define lfind as _lfind for MSVC but
        # doing this results in a just more confusing warning, see:
        # http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=101278
        target_compile_definitions(wxpng PRIVATE _CRT_NONSTDC_NO_WARNINGS)
    endif()
    target_compile_definitions(wxpng PRIVATE PNG_INTEL_SSE)
    target_include_directories(wxpng PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(wxpng PRIVATE ${ZLIB_LIBRARIES})
    set(PNG_LIBRARIES wxpng)
    set(PNG_INCLUDE_DIRS ${wxSOURCE_DIR}/src/png)
elseif(wxUSE_LIBPNG)
    find_package(PNG REQUIRED)
endif()
