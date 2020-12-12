# cotire example project

cmake_minimum_required(VERSION 2.8.12)

if (POLICY CMP0058)
	# Ninja requires custom command byproducts to be explicit
	cmake_policy(SET CMP0058 NEW)
endif()

project (CotireExample)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/..")

if (NOT CMAKE_VERSION VERSION_LESS "3.1.0")
	set (CMAKE_CXX_STANDARD "98")
	set (CMAKE_CXX_EXTENSIONS OFF)
endif()

include(cotire)

add_subdirectory(src)
