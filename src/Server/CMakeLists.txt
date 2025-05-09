add_executable (
	Server 
	"src/main.cpp"
	"src/HTTPServer.cpp"
)

target_include_directories(Server PRIVATE "include")
target_link_libraries(Server PRIVATE Netlib Threadlib)

if (CMAKE_VERSION VERSION_GREATER 3.24)
  set_property(TARGET Server PROPERTY CXX_STANDARD 20)
endif()

