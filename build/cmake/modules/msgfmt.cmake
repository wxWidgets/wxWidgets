# This is used from build/cmake/locale/CMakeLists.txt to generate .gmo files
# from .po files using msgfmt in a more flexible way than done by the standard
# gettext_process_po_files.
foreach(attempt RANGE 1 5)
    execute_process(
        COMMAND "${MSGFMT}" -o "${GMO}" "${PO}"
        RESULT_VARIABLE res
        ERROR_VARIABLE err
    )

    if(res EQUAL 0)
        # Don't swallow msgfmt's own warnings about the .po file.
        if(err)
            message("${err}")
        endif()
        return()
    endif()

    message(STATUS "msgfmt failed for ${PO} (attempt ${attempt}): ${err}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep ${attempt})
endforeach()

message(FATAL_ERROR "msgfmt failed for ${PO} after 5 attempts: ${err}")
