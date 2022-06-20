
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC /usr/local/include/ )

add_library( common_lib
        ../Common/shader.cpp
        ../Common/shader.hpp
        ../Common/Scene.cpp
        ../Common/Scene.h)

target_include_directories( common_lib PUBLIC /usr/local/include/eigen3/ )

target_link_libraries( ${TARGET_NAME} ${OPENGL_opengl_LIBRARY}  ${GLEW_LIBRARY}
        ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY})

target_link_libraries( ${TARGET_NAME} common_lib )

message( "GLFW Include : " ${GLFW_INCLUDE_DIRS} )
message( "GLFW Libs : " ${GLFW_LIBRARIES} )
