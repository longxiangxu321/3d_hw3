# Processing a BIM model using voxels

The code included processes IFC models. It extracts outer building envelope and room envelops and store a building in *.json* file following [cityjson](https://www.cityjson.org/) specification.

The code has these functionalities:

- Read OBJ(for .obj with weld-vertices) file into memory, in the form of objects, shells, faces, and points
- Create a voxel grid that bounds the building
- Voxelize the building
- Mark exterior voxels and room voxels
- Extract outter building envelope and room envelops

# Team Member

- *Bingshiuan Tsai*
- *Qiuxian Wei*
- *Longxiang Xu*



# Requirements

The code has been tested on WSL2.

Required package:

- CGAL



# Folder structure

```
hw3
  data
    ifcs
      IfcOpenHouse_IFC2x3.ifc
      ...
    objs
        ifc1.obj
        ...
    reconstructed
      mesh
        building.off
        ...
      pointcloud
        building.xyz
        ...
      voxels
        ifc_bu.obj
        ...
  src
    definitions.h
    geometry.h
    main.cpp
    poission_reconstruction.h
    voxelgrid.h
  CMakeLists.txt
  README.md
```



# Run

To compile and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./hw02 myfile.city.json







```
.\IfcConvert.exe .\hw3\data\ifcs\model.ifc .\hw3\data\objs\ifc2x3.obj -j 11 --no-normals --weld-vertices --orient-shells --exclude+=entities IfcOpeningElement IfcBeam IfcColumns--use-element-names
```
