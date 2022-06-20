set( CMAKE_CXX_STANDARD 11 )
set( OpenGL_GL_PREFERENCE GLVND)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package( glfw3 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )

