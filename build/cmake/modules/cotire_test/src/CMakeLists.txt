# cotire example project

add_executable(example main.cpp example.cpp log.cpp log.h example.h)

# enable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set_target_properties(example PROPERTIES COMPILE_FLAGS "-Weverything")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(example PROPERTIES COMPILE_FLAGS "-Wall -Wextra")
endif()

cotire(example)

# cotire sets the following properties
get_target_property(_unitySource example COTIRE_CXX_UNITY_SOURCE)
get_target_property(_prefixHeader example COTIRE_CXX_PREFIX_HEADER)
get_target_property(_precompiledHeader example COTIRE_CXX_PRECOMPILED_HEADER)
get_target_property(_unityTargetName example COTIRE_UNITY_TARGET_NAME)

if (_unitySource)
	message(STATUS "example unity source: ${_unitySource}")
endif()
if (_prefixHeader)
	message(STATUS "example prefix header: ${_prefixHeader}")
endif()
if (_precompiledHeader)
	message(STATUS "example precompiled header: ${_precompiledHeader}")
endif()
if (TARGET ${_unityTargetName})
	message(STATUS "example unity target: ${_unityTargetName}")
endif()
