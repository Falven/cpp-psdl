include_directories(${psdl_SOURCE_DIR}/include)
set(
	psdl_SRCS
	main.cpp
	psclient.cpp
	pscourse.cpp
	pslesson.cpp
	psmodule.cpp
)
add_executable(psdl ${psdl_SRCS})

find_library(POCO_FOUNDATION PocoFoundation)
find_library(CURL curl)
find_library(GUMBO_QUERY gumbo_query)

target_link_libraries(
	psdl
	${POCO_FOUNDATION}
	${CURL}
	${GUMBO_QUERY}
)

set_property(TARGET psdl PROPERTY CXX_STANDARD 14)
set_property(TARGET psdl PROPERTY CXX_STANDARD_REQUIRED ON)