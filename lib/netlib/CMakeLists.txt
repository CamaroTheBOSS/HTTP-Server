add_library (
	Netlib 
	"src/net.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.24)
  set_property(TARGET Netlib PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(Netlib PUBLIC "include")
target_link_libraries(Netlib PUBLIC Utils)

