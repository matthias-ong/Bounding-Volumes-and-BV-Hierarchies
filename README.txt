a. How to use parts of your user interface that is NOT specified in the assignment 
description:

Camera controls:
WASD to move around
Hold down Left Mouse Button and move mouse to look around
Mouse scroll to zoom
Hitting the ESC key will terminate the program.

b. Any assumption that you make on how to use the application that, if violated, might 
cause the application to fail:

- There should be only 2 objects in the scene, not more or less or it may cause the application to fail.
- Setting the scale/radius to negative.
- Minimising the application.

c. Which part of the assignment has been completed? 

Collision:
• Sphere Vs Sphere
• AABB Vs Sphere
• Sphere Vs AABB
• AABB	Vs AABB
• Point	Vs Sphere
• Point	Vs AABB
• Point	Vs Triangle
• Point	Vs Plane
• Ray Vs Plane
• Ray Vs AABB
• Ray Vs Sphere
• Ray Vs Triangle
• Plane	Vs AABB
• Plane	Vs Sphere

Camera Implementation

Extra credit:
Displays the primitives being tested and also display details on GUI. (ImGui integration).
Minimap implementation.

d. Which part of the assignment has NOT been completed (not done, not working, 
etc.) and explanation on why those parts are not completed?
None.

e. Where the relevant source codes (both C++ and shaders, as applicable) for the 
assignment are located. Specify the file path (folder name), file name, function 
name (or line number). 

1. Collision test functions are in Collision.cpp, 
2. Colliders are in Collision.h
3. Camera is in Camera.cpp/h.
4. GUI Display for types being tested is done in GameObject.cpp.
5. Model loading using assimp is done in Model.cpp, in loadModel().
6. Rendering is done inside SimpleScene_Quad::Render().

f. Which machine did you test your application on. 
Tested on: Windows 10, Intel Iris Plus Graphics, OpenGL 4.6, OpenGL Driver Intel Graphics Driver 30.0.101.1660

g. The number of hours you spent on the assignment, on a weekly basis 
15 hours per week.

h. Any other useful information pertaining to the application


