a. How to use parts of your user interface that is NOT specified in the assignment 
description:

Camera controls:
WASD to move around
Hold down Left Mouse Button and move mouse to look around
Mouse scroll to zoom
Hitting the ESC key will terminate the program.

After moving primitive objects in the world or making changes to the tree, remember to 
click on the "Update" button provided.

b. Any assumption that you make on how to use the application that, if violated, might 
cause the application to fail:

- Tree creation only works when all the primitive objects have the same type of Bounding Volumes
- Trees construction using bounding spheres assumes Ritter's spheres only
- Minimising the application, as I did not handle the event.

c. Which part of the assignment has been completed? 


Camera Implementation

Extra credit:
NIL

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
20 hours per week.

h. Any other useful information pertaining to the application


