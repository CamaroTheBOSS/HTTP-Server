add_library (
	Utils INTERFACE 
)

target_include_directories(Utils INTERFACE "include")

if (CMAKE_VERSION VERSION_GREATER 3.24)
  set_property(TARGET Utils PROPERTY CXX_STANDARD 20)
endif()