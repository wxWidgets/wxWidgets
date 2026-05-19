set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard version")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require the specified C++ standard")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler-specific C++ extensions")

# Enable metafile support
set(wxUSE_METAFILE ON CACHE BOOL "Enable metafile support" FORCE)
set(wxUSE_WIN_METAFILES_ALWAYS ON CACHE BOOL "Enable Windows metafiles support" FORCE)

# Keep enhanced metafiles enabled (default)
set(wxUSE_ENH_METAFILE ON CACHE BOOL "Enable enhanced metafiles" FORCE)
