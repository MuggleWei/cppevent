set(TCMALLOC_PATH CACHE PATH "The directory of protobuf")
set(TCMALLOC_SEARCH_PATH /usr/local ${TCMALLOC_PATH})

FIND_LIBRARY(TCMALLOC_LIBS
	NAMES tcmalloc
	PATH_SUFFIXES lib
	PATHS ${TCMALLOC_SEARCH_PATH})
if (TCMALLOC_LIBS)
	get_filename_component(TCMALLOC_LIB_DIR ${TCMALLOC_LIBS} DIRECTORY)
	message(${TCMALLOC_LIBS})
	message(${TCMALLOC_LIB_DIR})
endif()
	
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TCMallocBin DEFAULT_MSG
	TCMALLOC_LIBS
)
