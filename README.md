# Processing a BIM model using voxels

The code included processes IFC models. It extracts outer building envelope and room envelops and store a building in *.json* file following [cityjson](https://www.cityjson.org/) specification.

The code has these functionalities:

- Read OBJ(for `.obj` with weld-vertices) file into memory, in the form of objects, shells, faces, and points
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

Required software:

- IfcConvert

Required package:

- CGAL



# Folder structure

```
.
└── hw3/
    ├── data/
    │   ├── ifcs/
    │   │   ├── IfcOpenHouse_IFC2x3.ifc
    │   │   └── ...
    │   ├── objs/
    │   │   ├── ifc1.obj
    │   │   └── ...
    │   └── reconstructed/
    │       ├── mesh/
    │       │   ├── building.off
    │       │   └── ...
    │       ├── pointcloud/
    │       │   ├── building.xyz
    │       │   └── ...
    │       └── voxels/
    │           ├── ifc_bu.obj
    │           └── ...
    ├── src/
    │   ├── definitions.h
    │   ├── geometry.h
    │   ├── json.hpp
    │   ├── main.cpp
    │   ├── poission_reconstruction.h
    │   └── voxelgrid.h
    ├── CMakeLists.txt
    └── README.md
```



# Run

## IFC to OBJ

First, use ifcConvert to convert `.ifc` into `.obj` file with following command: change `model.ifc` and `ifc1.obj` into the name you want

```
.\IfcConvert.exe .\hw3\data\ifcs\model.ifc .\hw3\data\objs\ifc1.obj -j 11 --no-normals --weld-vertices --orient-shells --exclude+=entities IfcOpeningElement IfcBeam IfcColumns IfcObject --use-element-names
```

Note:

- `--weld-vertices` is necessary, without it, obj file will not be correctly read
- `-j 11` can be changed according to your cpu cores
- details of other options can be find with `.\IfcConvert.exe --help`

## OBJ, voxelisation, and to cityjson

To compile and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./hw3



./hw3 provide following options:

- `--input-file`: This option allows you to specify the path to the input file in OBJ format. For example, `--input-file ../data/objs/ifc1.obj` sets the input file to be `ifc1.obj` located in the `../data/objs` directory.
- `--export-building-voxel`: This option allows you to specify whether the program should export building voxels. By default, this option is set to `false`, which means that the program will not export building voxels. If you set this option to `true`, the program will export building voxels.
- `--export-interior-voxel`: This option allows you to specify whether the program should export interior voxels. By default, this option is set to `false`, which means that the program will not export interior voxels. If you set this option to `true`, the program will export interior voxels.
- `--export-building-mesh`: This option allows you to specify whether the program should export building meshes. By default, this option is set to `false`, which means that the program will not export building meshes. If you set this option to `true`, the program will export building meshes.
- `--export-rooms-mesh`: This option allows you to specify whether the program should export room meshes. By default, this option is set to `false`, which means that the program will not export room meshes. If you set this option to `true`, the program will export room meshes.
- `--resolution`: This option allows you to specify the resolution of the voxels used to generate the voxel representation of the building. By default, this value is set to `0.1`. You can change this value to any other float value, for example, `--resolution 0.05`.





