set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE Client.cpp SocketClient.cpp Messages.cpp RemoteObject.cpp msgpack_show.cpp interfaces/ITwibMetaInterface.cpp interfaces/ITwibDeviceInterface.cpp interfaces/ITwibPipeReader.cpp interfaces/ITwibPipeWriter.cpp interfaces/ITwibProcessMonitor.cpp interfaces/ITwibDebugger.cpp interfaces/ITwibFilesystemAccessor.cpp interfaces/ITwibFileAccessor.cpp interfaces/ITwibDirectoryAccessor.cpp)

if(TWIB_NAMED_PIPE_FRONTEND_ENABLED)
	set(SOURCE ${SOURCE} NamedPipeClient.cpp)
endif()

if(TWIB_GDB_ENABLED)
	set(SOURCE ${SOURCE} GdbConnection.cpp GdbStub.cpp)
endif()

add_library(twib-tool ${SOURCE})
target_link_libraries(twib-tool PUBLIC twib-platform twib-common)

include_directories(msgpack11 INTERFACE)
target_link_libraries(twib-tool PUBLIC msgpack11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(twib-tool PRIVATE Threads::Threads)

if (WIN32)
	target_link_libraries(twib-tool PRIVATE wsock32 ws2_32)
endif()

add_executable(twib Twib.cpp)
target_link_libraries(twib PRIVATE twib-tool)
include_directories(CLI11 INTERFACE)
target_link_libraries(twib PRIVATE CLI11)

install(TARGETS twib RUNTIME DESTINATION bin)

if (TWIB_PYBIND11)
	find_package(pybind11 REQUIRED)
	pybind11_add_module(pytwib pybind11/main.cpp)
	target_link_libraries(pytwib PRIVATE twib-tool)
endif()
