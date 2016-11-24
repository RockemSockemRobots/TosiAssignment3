# Authors: Justin Siekmann, Anthony Ciancio
# NetIDs: jsiekmann, shjay
# Date: 29 November, 2016
# Assignment #3


set( CMAKE_CXX_FLAGS "-Wall -g" )

file( GLOB SRCS *.cpp *.h )

add_definitions(-std=c++11)
add_executable( hlsyn ${SRCS} )

