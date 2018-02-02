if (WIN32)
	cmake_minimum_required(VERSION 3.8.0)
else()
	cmake_minimum_required(VERSION 3.0.2)
endif()
project(cppevent)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++1z")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
	
	# for fix std::max conflict in windows
	add_definitions(-DNOMINMAX)
endif()

# detect c++ features
message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
	message("${i}")
endforeach()

# include cmake
include(${CMAKE_CURRENT_LIST_DIR}/cmake/muggle_utils.cmake)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# option
SET(CPP_EVENT__BUILD_SHARED ON CACHE BOOL "cpp event build shared")
SET(CPP_EVENT__BUILD_exmaple ON CACHE BOOL "cpp event build examples")
SET(CPP_EVENT__BUILD_EXTRA ON CACHE BOOL "cpp event build extra lib e.g. protobuf codec")

################################# cppevent core #################################
# libevent
set(USE_LIBEVENT_WITH_SOURCE ON CACHE BOOL "use libevent with source, if it OFF, try to find libevent binary in system")
if (USE_LIBEVENT_WITH_SOURCE)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindLibeventSrc.cmake)
	if(LibeventSrc_FOUND)
		if (WIN32)
			SET(EVENT__DISABLE_OPENSSL ON CACHE BOOL "close libevent openssl")				# libevent close openssl in windows
		endif()
		SET(EVENT__BUILD_SHARED_LIBRARIES ON CACHE BOOL "build libevent as shared")		# libevent build as static lib		
		SET(EVENT__DISABLE_SAMPLES ON CACHE BOOL "close libevent example")				# libevent close example
		SET(EVENT__DISABLE_TESTS ON CACHE BOOL "close libevent test")					# libevent close test
		SET(EVENT__DISABLE_BENCHMARK ON CACHE BOOL "close libevent benchmark")			# libevent close benchmark
		SET(EVENT__DISABLE_THREAD_SUPPORT OFF CACHE BOOL "open libevent thread")		# libevent open thread
		
		add_subdirectory(${LIBEVENT_SOURCE_DIR})
		include_directories(
			${LIBEVENT_INCLUDE_DIR}
			${libevent_BINARY_DIR}/include	# for event2/event-config.h
		)
		
		set_target_properties(event_core event_extra event
			PROPERTIES
			FOLDER "libevent"
		)
		if (EVENT__BUILD_SHARED_LIBRARIES)
			set_target_properties(event_extra_static
				PROPERTIES
				FOLDER "libevent"
			)
		endif()
	else()
		message(FATAL_ERROR "Can't find libevent source")
	endif()
else()
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindLibeventBin.cmake)
	if (LibeventBin_FOUND)
		include_directories(${LIBEVENT_INCLUDE_DIR})
	else()
		message(FATAL_ERROR "Can't find libevent binary")
	endif()
endif()

# muggleCC
set(MUGGLE_BUILD_ONLY_C OFF)
set(MUGGLE_BUILD_EXAMPLE OFF)
set(MUGGLE_BUILD_TEST OFF)
set(MUGGLE_BUILD_BENCHMARK OFF)
set(MUGGLE_BUILD_SHARED_LIB OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/muggleCC)
include_directories(
	${CMAKE_CURRENT_LIST_DIR}/thirdparty/muggleCC
)
set_target_properties(muggleCC
	PROPERTIES
	FOLDER "muggle"
)

# include directories
include_directories(
	${CMAKE_CURRENT_LIST_DIR}/src
)

# cppevent core
if (${CPP_EVENT__BUILD_SHARED})
	set(CPP_EVENT__BUILD_TYPE SHARED)
else()
	set(CPP_EVENT__BUILD_TYPE STATIC)
endif()
muggle_add_project(cppevent_core ${CMAKE_CURRENT_LIST_DIR}/src/cppevent/core ${CPP_EVENT__BUILD_TYPE})
target_link_libraries(cppevent_core
	event_core
	event_extra
)
if (USE_LIBEVENT_WITH_SOURCE)
	add_dependencies(cppevent_core
		event_core
		event_extra
	)
endif()

# add post compile event - copy libevent
if (MSVC AND USE_LIBEVENT_WITH_SOURCE)
	add_custom_target(FAKE_Copylibevent ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/bin
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/lib
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/include ${CMAKE_BINARY_DIR}/include
	)
	add_dependencies(FAKE_Copylibevent
		event
		event_core
		event_extra
	)
	set_target_properties(FAKE_Copylibevent
		PROPERTIES
		FOLDER "fake"
	)
endif()

################################# cppevent extra #################################
# protobuf
if (${CPP_EVENT__BUILD_EXTRA})
	set(USE_PROTOBUF_WITH_SOURCE ON CACHE BOOL "use protobuf with source, if it OFF, try to find protobuf binary in system")
	if (USE_PROTOBUF_WITH_SOURCE)
		include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufSrc.cmake)
		if (ProtobufSrc_FOUND)
			SET(protobuf_BUILD_TESTS OFF CACHE BOOL "protobuf test")
			SET(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "protobuf example")
			SET(protobuf_BUILD_SHARED_LIBS ON CACHE BOOL "protobuf build shared")
		
			add_subdirectory(${PROTOBUF_CMAKE_DIR})
			
			set_target_properties(
				libprotobuf libprotobuf-lite libprotoc protoc js_embed
				PROPERTIES
				FOLDER "protobuf"
			)
		endif()
	else()
		include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufBin.cmake)
		if (ProtobufBin_FOUND)
			include_directories(${PROTOBUF_INCLUDE_DIR})
			link_directories(${PROTOBUF_LIB_DIR})
		else()
			message(FATAL_ERROR "Can't find protobuf")
		endif()
	endif()
endif()

# cppevent extra
muggle_add_project(cppevent_extra ${CMAKE_CURRENT_LIST_DIR}/src/cppevent/extra ${CPP_EVENT__BUILD_TYPE})
target_link_libraries(cppevent_extra
	event_core
)
if (USE_PROTOBUF_WITH_SOURCE)
	target_link_libraries(cppevent_extra
		libprotobuf
	)
	add_dependencies(cppevent_extra
		libprotobuf
		libprotobuf-lite
		libprotoc
		protoc
		js_embed
	)
else()
	target_link_libraries(cppevent_extra
		protobuf
	)
endif()

################################# example #################################
function(add_example name folder)
	muggle_add_project(${name} ${folder} EXE)
	target_link_libraries(${name}
		cppevent_core
		cppevent_extra
		muggleCC
	)
	add_dependencies(${name}
		cppevent_core
		cppevent_extra
		muggleCC
	)
	set_target_properties(${name}
		PROPERTIES
		FOLDER "example"
	)
endfunction()

function(add_example_mul_folder name root_folder folders)
	muggle_add_project_with_folders(${name} ${root_folder} EXE "${folders}")
	target_include_directories(${name} PUBLIC
		${root_folder}
	)
	target_link_libraries(${name}
		cppevent_core
		cppevent_extra
		muggleCC
	)
	add_dependencies(${name}
		cppevent_core
		cppevent_extra
		muggleCC
	)
	set_target_properties(${name}
		PROPERTIES
		FOLDER "example"
	)
endfunction(add_example_mul_folder)

if (${CPP_EVENT__BUILD_exmaple})
	add_example(echo_server ${CMAKE_CURRENT_LIST_DIR}/example/echo_server)
	add_example(simple_client ${CMAKE_CURRENT_LIST_DIR}/example/simple_client)
	add_example(pressure_client ${CMAKE_CURRENT_LIST_DIR}/example/pressure_client)
	
	set(protobuf_server_folders "${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf/server" "${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf/gen")
	add_example_mul_folder(protobuf_server ${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf "${protobuf_server_folders}")
	
	set(protobuf_client_folders "${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf/client" "${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf/gen")
	add_example_mul_folder(protobuf_client ${CMAKE_CURRENT_LIST_DIR}/example/codec/protobuf "${protobuf_client_folders}")
endif()