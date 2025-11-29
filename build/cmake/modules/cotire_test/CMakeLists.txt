# cotire example project

cmake_minimum_required(VERSION 3.5...4.1)

if (POLICY CMP0058)
	# Ninja requires custom command byproducts to be explicit
	cmake_policy(SET CMP0058 NEW)
endif()

project (CotireExample)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/..")
set (CMAKE_CXX_STANDARD "98")
set (CMAKE_CXX_EXTENSIONS OFF)

include(cotire)

add_subdirectory(src)
