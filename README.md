# Bounding Volumes (BV) and BV Hierachies

This is a study on Spatial Data Structures. The content is an application of the concepts taught in <i>[Real-Time Collision Detection by Christer Ericson](https://www.amazon.com/Real-Time-Collision-Detection-Interactive-Technology/dp/1558607323)</i> textbook, specifically on the application of the common bounding volumes and bounding volume hierarchies. This application allows you to test different Bounding Volumes and different splits for BV Hierchies.

Bounding Volume Hierarchies is a method to improve efficiency when dealing with large sets of objects or scenes. Bounding volume hierarchies organize objects hierarchically using bounding volumes, such as axis-aligned bounding boxes (AABBs) or spheres. Instead of dividing the space like Spatial Partitioning, BVHs build a tree where each node represents a bounding volume containing a subset of objects. Whether to use spatial partitioning or BV Hierarchies depend on different cases. Sometimes in scenarios where object distribution is uniform, object sizes vary significantly, or where complex spatial relationships need to be captured efficiently, spatial partitioning methods may offer better performance and memory utilization.

## Screenshots

### Bounding Volumes
![Bounding Volume Screenshot](https://github.com/matthias-ong/Bounding-Volumes-and-BV-Hierarchies/blob/main/screenshots/bounding_volumes.png)

### Bounding Volume Hierachies
![Bounding Volume Hierachies Screenshot](https://github.com/matthias-ong/Bounding-Volumes-and-BV-Hierarchies/blob/main/screenshots/bv_hierachies.png)
 
## Setup

As I don't plan on keeping it up to date, I included all the necessary dependencies, the file is still relatively small enough.
Tested on: Windows 10, Intel Iris Plus Graphics, OpenGL 4.6, OpenGL Driver Intel Graphics Driver 30.0.101.1660
Refer to `README.txt` for more information.
