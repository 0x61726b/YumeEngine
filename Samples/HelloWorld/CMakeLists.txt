set(SAMPLE_TARGET "HelloWorld")

set(SOURCE_FILES
	BasicRenderWindow.cpp)

set(HEADER_FILES
	BasicRenderWindow.h)

if(OS_LINUX OR OS_MACOSX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBOOST_ALL_NO_LIB=1 -g -O0 --coverage")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_yume_sample("HelloWorld")
