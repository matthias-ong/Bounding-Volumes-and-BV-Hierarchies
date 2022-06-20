cmake_minimum_required( VERSION 3.9 )
project( Simple_Scene )

include( ../Common/build_files/Common_packages.cmake )

set( TARGET_NAME SimpleScene_OGL4 )

add_executable( ${TARGET_NAME}
        ../Common/shader.cpp
        ../Common/shader.hpp
        SimpleScene_Quad.cpp
        SimpleScene_Quad.h
        GLApplication.cpp )

# Set target properties
include( ../Common/build_files/Common_target.cmake )