## FindMSPACK.cmake
##
## Copyright (C) 2016 Christian Schenk
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

find_path(MSPACK_INCLUDE_DIR
  NAMES
    mspack.h
)

find_library(MSPACK_LIBRARY
  NAMES
    mspack
)

find_package_handle_standard_args(MSPACK DEFAULT_MSG MSPACK_LIBRARY MSPACK_INCLUDE_DIR)

if(MSPACK_FOUND)
  set(MSPACK_INCLUDE_DIRS ${MSPACK_INCLUDE_DIR})
  set(MSPACK_LIBRARIES ${MSPACK_LIBRARY})
else()
  set(MSPACK_INCLUDE_DIRS)
  set(MSPACK_LIBRARIES)
endif()

mark_as_advanced(MSPACK_LIBRARY MSPACK_INCLUDE_DIR)
