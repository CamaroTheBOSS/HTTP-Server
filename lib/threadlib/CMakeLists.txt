add_library (
	Threadlib INTERFACE 
)

target_include_directories(Threadlib INTERFACE "include")

if (CMAKE_VERSION VERSION_GREATER 3.24)
  set_property(TARGET Threadlib PROPERTY CXX_STANDARD 20)
endif()