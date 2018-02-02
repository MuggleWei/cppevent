set(PROTOBUF_PATH CACHE PATH "The directory of libevent")
set(PROTOBUF_SEARCH_PATH /usr/local ${PROTOBUF_PATH})

FIND_PATH(PROTOBUF_INCLUDE_DIR
	NAMES message.h
	PATH_SUFFIXES include/google/protobuf
	PATHS ${PROTOBUF_SEARCH_PATH})
	
FIND_LIBRARY(PROTOBUF_LIBS
	NAMES protobuf protobuf-lite protoc
	PATH_SUFFIXES lib
	PATHS ${PROTOBUF_SEARCH_PATH})
if (PROTOBUF_LIBS)
	get_filename_component(PROTOBUF_LIB_DIR ${PROTOBUF_LIBS} DIRECTORY)
endif()
	
FIND_LIBRARY(PROTOBUF_BIN
	NAMES protoc
	PATH_SUFFIXES bin
	PATHS ${PROTOBUF_SEARCH_PATH})
if (PROTOBUF_BIN)
	get_filename_component(PROTOBUF_BIN_DIR ${PROTOBUF_BIN} DIRECTORY)
endif()

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PROTOBUF_BIN DEFAULT_MSG
    PROTOBUF_INCLUDE_DIR
	PROTOBUF_LIB_DIR
	PROTOBUF_BIN_DIR)