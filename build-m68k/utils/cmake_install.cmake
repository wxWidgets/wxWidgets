# Install script for directory: /Users/andyvand/Downloads/wxWidgets/build/cmake/utils

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/retro68/m68k-apple-macos")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/retro68/bin/m68k-apple-macos-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/andyvand/Downloads/wxWidgets/build-m68k/lib/wxrc-3.3")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/wxrc-3.3" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/wxrc-3.3")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/retro68/bin/m68k-apple-macos-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/wxrc-3.3")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
            set(SYMLINK_SRC "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/wxrc-3.3")
            set(SYMLINK_DST "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/wxrc")
            message(STATUS "Installing: ${SYMLINK_DST}")

            if(CMAKE_VERSION GREATER_EQUAL "3.17")
                execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -f "${SYMLINK_DST}")
            else()
                execute_process(COMMAND "${CMAKE_COMMAND}" -E remove -f "${SYMLINK_DST}")
            endif()

            set(SYMLINK_RES 1)
            set(wxBUILD_INSTALL_USE_SYMLINK ON)
            if(wxBUILD_INSTALL_USE_SYMLINK)
                if(SYMLINK_RES)
                    execute_process(COMMAND ln -s --relative "${SYMLINK_SRC}" "${SYMLINK_DST}" RESULT_VARIABLE SYMLINK_RES OUTPUT_QUIET ERROR_QUIET)
                endif()
                if(SYMLINK_RES)
                    execute_process(COMMAND ln -s "${SYMLINK_SRC}" "${SYMLINK_DST}" RESULT_VARIABLE SYMLINK_RES OUTPUT_QUIET ERROR_QUIET)
                endif()
                if(SYMLINK_RES)
                    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink "${SYMLINK_SRC}" "${SYMLINK_DST}" RESULT_VARIABLE SYMLINK_RES)
                endif()
            endif()
            if(SYMLINK_RES)
                execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SYMLINK_SRC}" "${SYMLINK_DST}" RESULT_VARIABLE SYMLINK_RES)
            endif()
        
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/andyvand/Downloads/wxWidgets/build-m68k/utils/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
