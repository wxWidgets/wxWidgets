find_package(WebP CONFIG)

if(NOT WebP_FOUND)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(WebP libwebp libwebpdemux)
    endif()
endif()
