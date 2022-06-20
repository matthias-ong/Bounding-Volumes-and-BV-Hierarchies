rem Setup
@echo off
set dependencies_path=./extern

GOTO comment
rem GLFW
set glfw_path=%dependencies_path%/glfw/
rmdir "%glfw_path%" /S /Q
git clone https://github.com/glfw/glfw.git %glfw_path%
cmake -S "%glfw_path%" -B "%glfw_path%\build" -D BUILD_SHARED_LIBS=ON -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF
cmake --build "%glfw_path%\build"
mkdir "%glfw_path%\bin\"
move "%glfw_path%\build\src\Debug\glfw3.dll" "%glfw_path%\bin\glfw3.dll"
move "%glfw_path%\build\src\Debug\glfw3dll.lib" "%glfw_path%\bin\glfw3.lib"

rem TODO: Release mode for GLFW

rem GLM
set glm_path=%dependencies_path%/glm/
rmdir %glm_path% /S /Q
git clone https://github.com/g-truc/glm.git %glm_path%

rem assimp
set assimp_path=%dependencies_path%/assimp/
rmdir "%assimp_path%" /S /Q
git clone https://github.com/assimp/assimp.git %assimp_path%
cd %assimp_path%
call BUILDBINARIES_EXAMPLE.bat

:comment
rem IMGUI
set imgui_path=%dependencies_path%/imgui/
rmdir "%imgui_path%" /S /Q
git clone https://github.com/ocornut/imgui.git %imgui_path%
pause