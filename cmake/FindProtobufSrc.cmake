if (NOT PROTOBUF_SEARCH_PATH)
	SET(PROTOBUF_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/../thirdparty/google/protobuf")
endif()

FIND_PATH(PROTOBUF_CMAKE_DIR
    NAMES CMakeLists.txt
	PATH_SUFFIXES cmake
    PATHS ${PROTOBUF_SEARCH_PATH})
	
FIND_PATH(PROTOBUF_SOURCE_DIR
	NAMES message.cc descriptor.cc
	PATH_SUFFIXES src/google/protobuf
	PATHS ${PROTOBUF_SEARCH_PATH})
	
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ProtobufSrc DEFAULT_MSG
    PROTOBUF_SOURCE_DIR
    PROTOBUF_CMAKE_DIR)