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

Everything that was mentioned in the handout except for Extra Credits 
1) Scene setup (8 objects)
2) Bounding Volumes
- AABB
- Bounding Sphere – Ritter’s Method
- Bounding Sphere – Modified Larsson’s Method (EPOS8, EPOS12, EPOS24)
- Bounding Sphere – PCA Method
3) Bounding Volume Hierarchy
- Top-down construction (AABB + Bounding Sphere)
- Bottom-up construction (AABB + Bounding Sphere)
4) Interactivity / Display 
- IMGUI Controls
- Wireframe for bounding volumes
- Camera Implementation

Extra credit:
NIL

d. Which part of the assignment has NOT been completed (not done, not working, 
etc.) and explanation on why those parts are not completed?
None.

e. Where the relevant source codes (both C++ and shaders, as applicable) for the 
assignment are located. Specify the file path (folder name), file name, function 
name (or line number). 

All source file are located in "1.0 - Simple Scene" folder
1. Bounding Volume related stuff are in BoundingVolume.cpp/h, 
2. Bounding Volume Hierarchy related stuff are in BVHierarchy.cpp/h
3. Camera is in Camera.cpp/h.
4. GUI Display for types being tested is done in GameObject.cpp.
5. Addition IMGUI Display for Trees is done in SimpleScene_Quad.cpp inside SimpleScene_Quad::Render().
6. Bulk of Rendering is done inside SimpleScene_Quad::Render().
7. Model loading using assimp is done in Model.cpp, in loadModel().
8. Diffuse Lighting done in shaders DiffuseShader.vert/frag

f. Which machine did you test your application on. 
Tested on: Windows 10, Intel Iris Plus Graphics, OpenGL 4.6, OpenGL Driver Intel Graphics Driver 30.0.101.1660

g. The number of hours you spent on the assignment, on a weekly basis 
18 hours per week over 3+ weeks.

h. Any other useful information pertaining to the application

Discuss the effect of choosing the heuristic in your README 
file. (For bottom-up tree creation)
Increasing the weight for nearest neighbour will create a tree with clustered nodes.
Increasing the weight for smallest combined volume will create a tree with smaller nodes
Increasing the weight for smallest relative increase volume will create a tree that are have smaller parents when
compared to the child.
Playing with the weights to find an optimal tree.

Colours for Tree Depth:
0 - Red
1 - Orange
2 - Yellow
3 - Light Blue
4 - Blue
5 - Pink
6 - Grey


