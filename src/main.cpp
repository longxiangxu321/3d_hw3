#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include <sstream>
#include "json.hpp"


//const std::string input_file = "../data/objs/Ifc2x3_weld.obj";
//const std::string input_file = "../data/objs/AC90R1ifc.obj";

void mark_voxels_intersecting_triangle(const Triangle_3 &triangle, VoxelGrid &voxel_grid, const Point3 &origin, const int &label) {
    // Get the bounding box of the triangle
    Bbox_3 triangle_bbox = triangle.bbox();

    // Compute the minimum and maximum voxel coordinates that intersect the bounding box
    unsigned int min_x = static_cast<unsigned int>((triangle_bbox.xmin() - origin.x()) / voxel_grid.resolution);
    unsigned int max_x = static_cast<unsigned int>((triangle_bbox.xmax() - origin.x()) / voxel_grid.resolution);
    unsigned int min_y = static_cast<unsigned int>((triangle_bbox.ymin() - origin.y()) / voxel_grid.resolution);
    unsigned int max_y = static_cast<unsigned int>((triangle_bbox.ymax() - origin.y()) / voxel_grid.resolution);
    unsigned int min_z = static_cast<unsigned int>((triangle_bbox.zmin() - origin.z()) / voxel_grid.resolution);
    unsigned int max_z = static_cast<unsigned int>((triangle_bbox.zmax() - origin.z()) / voxel_grid.resolution);

    // Iterate over all voxels within the computed range
    for (unsigned int z = min_z; z <= max_z; ++z) {
        for (unsigned int y = min_y; y <= max_y; ++y) {
            for (unsigned int x = min_x; x <= max_x; ++x) {
                Point3 center = voxel_grid.center(x,y,z, origin);
                int value = voxel_grid(x,y,z);
                if (value!=0) continue;
                else {
                    if (intersect(center.x()-voxel_grid.resolution/2, center.y()-voxel_grid.resolution/2, center.z()-voxel_grid.resolution/2,
                               center.x()+voxel_grid.resolution/2, center.y()+voxel_grid.resolution/2, center.z()+voxel_grid.resolution/2,
                               center.x(), center.y(), center.z(), triangle))
                    voxel_grid(x,y,z) = label;
                }
            }
        }
    }
}



int main() {
    const std::string input_file = "../data/objs/ifc3.obj";

    const float resolution = 0.1;
    std::ifstream input_stream;
    input_stream.open(input_file);

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double min_z = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    double max_z = std::numeric_limits<double>::min();

    std::vector<Point3> points;
    std::vector<Object> objects;

    Object current_object;
    Shell current_shell;

    bool first_object_read = false;
    int index = 1;
    int count = 0;
    // 2. read data into memory
    if (input_stream.is_open()) {

        std::string line;
        while (std::getline(input_stream, line)) {
            if (line[0] == 'g') {//every g means a new object
                if (!first_object_read) {
                    first_object_read = true;
                } else {
                    current_object.shells.push_back(current_shell);
                    objects.push_back(current_object);
                    current_object = Object();
                    current_shell = Shell();
                }
                current_object.id = line;
                current_object.numeric_id = index;
                index++;
            }
            if (line[0] == 'u') {//new shell
                if (current_shell.faces.empty()) {
                    continue;
                } else {
                    current_object.shells.push_back(current_shell);
                    current_shell = Shell();
                }
            }
            if (line[0] == 'f') {//shell has different Faces
                unsigned long v0, v1, v2;
                std::stringstream ss(line);
                std::string temp;
                ss >> temp >> v0 >> v1 >> v2;
                current_shell.faces.push_back(Face(v0, v1, v2));
                count++;
            }
            if (line[0] == 'v') {
                double x,y,z;
                std::stringstream ss(line);
                std::string temp;
                ss >> temp >> x >> y >> z;
                points.push_back(Point3(x, y, z));
                if (x < min_x) min_x = x;
                if (y < min_y) min_y = y;
                if (z < min_z) min_z = z;
                if (x > max_x) max_x = x;
                if (y > max_y) max_y = y;
                if (z > max_z) max_z = z;
            }
            else {
                continue;
            }
        }
        if (!current_shell.faces.empty()) {
            current_object.shells.push_back(current_shell);
            objects.push_back(current_object);
        }
    }



    std::cout<<"Total number of faces "<<count<<std::endl;
    std::cout<<"Total number of vertices: "<<points.size()<<std::endl;


    for (auto &object: objects) {
        object.set_id();
    }//objects:list of Object, Object.shells, list of Shell,Shell.faces,list of Face, Face, list of vertices
    //points, list of Point3


    // 3. Voxel grid creation
    auto x_num = static_cast<unsigned int>(std::ceil((max_x - min_x) / resolution)) + 2; // + 1 tp ensure voxel grid larger than building bbox
    auto y_num = static_cast<unsigned int>(std::ceil((max_y - min_y) / resolution)) + 2;
    auto z_num = static_cast<unsigned int>(std::ceil((max_z - min_z) / resolution)) + 2;
    VoxelGrid voxel_grid(x_num, y_num, z_num, resolution);

    std::cout<<"Voxel_grid_x_num: "<<x_num<<std::endl;
    std::cout<<"Voxel_grid_y_num: "<<y_num<<std::endl;
    std::cout<<"Voxel_grid_z_num: "<<z_num<<std::endl;
    std::cout<<"Voxel_grid_resolution: "<<voxel_grid.resolution<<std::endl;
    point_vector pts;
    Point3 origin(min_x - resolution, min_y - resolution, min_z - resolution);

    int face_count = 0;
    for (auto const &object: objects) {
        for (auto const &shell: object.shells) {
            for (auto const &face: shell.faces) {
                Point3 v0 = points[face.vertices[0] - 1];
                Point3 v1 = points[face.vertices[1] - 1];
                Point3 v2 = points[face.vertices[2] - 1];
                Triangle_3 triangle(v0, v1, v2);
                face_count++;
                mark_voxels_intersecting_triangle(triangle, voxel_grid, origin, face.numeric_id);
                std::cout<<face_count<<std::endl;
                    }
                }
            }


    std::cout<<"mark building done"<<std::endl;

    // check whether building is voxelised correctly
    std::vector<unsigned int> buildings;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                if (voxel_grid(i,j,k)!=0){
                    buildings.emplace_back(voxel_grid.voxel_index(i,j,k));
                }
            }
        }
    }

    voxel_grid.mark_exterior();
    std::vector<unsigned int> exterior;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                if (voxel_grid(i,j,k)<0){
                    exterior.emplace_back(voxel_grid.voxel_index(i,j,k));
                }
            }
        }
    }

    std::cout<<"mark exterior done"<<std::endl;

    std::vector<unsigned int> interior;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                unsigned int idx = voxel_grid.voxel_index(i,j,k);
                if (voxel_grid(i,j,k)==0){
                    voxel_grid.mark_room(idx);
                }
                if (voxel_grid(i,j,k)==-2){
                    interior.emplace_back(idx);
                }
            }
        }
    }

    std::cout<<"mark interior done"<<std::endl;


    std::cout<<"exterior "<< voxel_grid.ex_voxels.size() << " " << exterior.size()<<std::endl;
    std::cout<<"interior "<< voxel_grid.in_voxels.size() << " " << interior.size()<<std::endl;
    std::cout<<"building "<< buildings.size() << std::endl;
    std::cout<< "total " << voxel_grid.voxels.size() << " " << exterior.size() + interior.size() + buildings.size() <<std::endl;


    const std::string buil = "../data/objs/ifc1_bu.obj";
    const std::string exte = "../data/objs/ifc1_ex.obj";
    const std::string inte = "../data/objs/ifc1_in.obj";

    voxel_grid.voxel_to_obj(buildings, origin, buil);
    voxel_grid.voxel_to_obj(exterior, origin, exte);
    voxel_grid.voxel_to_obj(interior, origin, inte);




    return 0;
}