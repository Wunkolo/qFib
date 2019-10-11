cmake_minimum_required( VERSION 3.2.2 )
project( qFib )

### Standard
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

### Optimizations
if( MSVC )
	add_compile_options( /W3 )
elseif( CMAKE_COMPILER_IS_GNUCXX )
	add_compile_options( -march=native )
	add_compile_options( -Wall )
	add_compile_options( -Wextra )
	# Force colored diagnostic messages in Ninja's output
	if( CMAKE_GENERATOR STREQUAL "Ninja" )
	    add_compile_options( -fdiagnostics-color=always )
	endif()
endif()

## GLM
set( GLM_TEST_ENABLE OFF CACHE BOOL "Build GLM Unit Tests")
add_subdirectory( extern/glm )

add_executable(
	qFib
	tests/bench.cpp
)
target_link_libraries(
	qFib
	PRIVATE
	glm
)

add_executable(
	fastGen
	tests/fastgen.cpp
)
target_link_libraries(
	fastGen
	PRIVATE
	glm
)

add_executable(
	terms
	tests/terms.cpp
)
