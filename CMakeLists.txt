﻿cmake_minimum_required (VERSION 3.24)

# Cmake MSVC hot reload policy
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# HTTP Server Project
project (
	"HTTP"
	VERSION 0.0.1
	LANGUAGES CXX
)

add_subdirectory("lib")
add_subdirectory("static")
add_subdirectory ("src/Server")

