# - Find Webkit-3.0
# Find the Webkit-3.0 includes and library
#
#  WEBKIT_INCLUDE_DIR - Where to find webkit include sub-directory.
#  WEBKIT_LIBRARIES   - List of libraries when using Webkit-3.0.
#  WEBKIT_FOUND       - True if Webkit-3.0 found.

SET( WEBKIT_VERSION "1.0")

IF (WEBKIT_INCLUDE_DIR)
    # Already in cache, be silent.
    SET(WEBKIT_FIND_QUIETLY TRUE)
ENDIF (WEBKIT_INCLUDE_DIR)

FIND_PATH(WEBKIT_INCLUDE_DIR webkit/webkit.h
    PATH_SUFFIXES "webkitgtk-${WEBKIT_VERSION}"
)

SET(WEBKIT_NAMES "webkitgtk-${WEBKIT_VERSION}")
FIND_LIBRARY(WEBKIT_LIBRARY
    NAMES ${WEBKIT_NAMES}
)

# Handle the QUIETLY and REQUIRED arguments and set WEBKIT_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    WEBKIT DEFAULT_MSG
    WEBKIT_LIBRARY WEBKIT_INCLUDE_DIR
)

IF(WEBKIT_FOUND)
    SET( WEBKIT_LIBRARIES ${WEBKIT_LIBRARY} )
ELSE(WEBKIT_FOUND)
    SET( WEBKIT_LIBRARIES )
ENDIF(WEBKIT_FOUND)

MARK_AS_ADVANCED( WEBKIT_LIBRARY WEBKIT_INCLUDE_DIR )
