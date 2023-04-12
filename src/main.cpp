#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include <sstream>
//const std::string input_file = "../data/objs/Ifc2x3_weld.obj";
//const std::string input_file = "../data/objs/AC90R1ifc.obj";
const std::string input_file = "../data/objs/Smiley_West.obj";
const float resolution = 0.1;


int main() {
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



    std::cout<<"Total number of Building objects: "<<objects.size()<<std::endl;
    std::cout<<"Total number of vertices: "<<points.size()<<std::endl;

//    for (auto const& object: objects) {
//        std::cout<<"object "<<" "<<object.id<<std::endl;
//        std::cout<< " shell number: "<<object.shells.size() << std::endl;
//        for (auto const& shell: object.shells) {
//            std::cout << "face number: " << shell.faces.size() << std::endl;
//        }}

    for (auto &object: objects) {
        object.set_id();
    }//objects:list of Object, Object.shells, list of Shell,Shell.faces,list of Face, Face, list of vertices
    //points, list of Point3


    // 3. Voxel grid creation
    auto x_num = static_cast<unsigned int>(std::ceil((max_x - min_x) / resolution)) + 1; // + 1 tp ensure voxel grid larger than building bbox
    auto y_num = static_cast<unsigned int>(std::ceil((max_y - min_y) / resolution)) + 1;
    auto z_num = static_cast<unsigned int>(std::ceil((max_z - min_z) / resolution)) + 1;
    VoxelGrid voxel_grid(x_num, y_num, z_num, resolution);

    std::cout<<"Voxel_grid_x_num: "<<x_num<<std::endl;
    std::cout<<"Voxel_grid_y_num: "<<y_num<<std::endl;
    std::cout<<"Voxel_grid_z_num: "<<z_num<<std::endl;
    std::cout<<"Voxel_grid_resolution: "<<voxel_grid.resolution<<std::endl;
    point_vector pts;
    Point3 origin(min_x, min_y, min_z);


    // 4. Voxelisation of triangles

    // 4.1 create octree to speed up search
    for (unsigned int i=0; i<x_num; i++) {
        for (unsigned int j=0; j<y_num; j++) {
            for (unsigned int k=0; k<z_num; k++) {//WQX EDITED
                Point3 vc = voxel_grid.center(i,j,k, origin);
                pts.push_back(vc);
            }
        }
    }

    double max_span = std::max({x_num, y_num, z_num}) * resolution; // the length of the longest dimension

    pts.push_back(Point3(min_x, min_y, min_z));
    pts.push_back(Point3(min_x + max_span, min_y + max_span, min_z + max_span));

    Octree voxel_tree(pts, CGAL::Identity_property_map<Point3>(), 1.0);
    auto rnode = voxel_tree.root();


    auto bo1 = voxel_tree.bbox(rnode);

    //   ensure octree root node aligned with the voxel grid
    assert(bo1.x_span() == bo1.y_span() == bo1.z_span() && bo1.x_span() == max_span);
    assert(bo1.xmin() == min_x && bo1.ymin() == min_y && bo1.zmin() == min_z);

    int max_depth = std::ceil(std::log(points.size())/ std::log(8));
    voxel_tree.refine(100,1);


    // 4.2 intersection test
    for (auto const &object: objects) {
        for (auto const &shell: object.shells) {
            for (auto const &face: shell.faces) {
                Point3 v0 = points[face.vertices[0]];
                Point3 v1 = points[face.vertices[1]];
                Point3 v2 = points[face.vertices[2]];
                Triangle_3 triangle(v0, v1, v2);
                output_nodes o1;//std::vector<Octree::Node> output_nodes;
                auto output_iterator = std::back_inserter(o1);
                voxel_tree.intersected_nodes(triangle, output_iterator);

                for (auto it = o1.begin(); it != o1.end(); ++it) {
                    for (auto jt = it->begin(); jt!=it->end(); jt++) {  // only one point in each node
                        // each octree node intersects with triangle, its size is same as the voxel grid
                        double xmin = jt -> x() - resolution/2;
                        double ymin = jt -> y() - resolution/2;
                        double zmin = jt -> z() - resolution/2;
                        double xmax = jt -> x() + resolution/2;
                        double ymax = jt -> y() + resolution/2;
                        double zmax = jt -> z() + resolution/2;
                        double xmid = jt -> x();
                        double ymid = jt -> y();
                        double zmid = jt -> z();


                        if (intersect(xmin, ymin, zmin, xmax, ymax, zmax, xmid, ymid, zmid, triangle)) {
                            unsigned int voxel_x = std::ceil((xmid - origin.x() - resolution/2) / resolution);
                            unsigned int voxel_y = std::ceil((ymid - origin.y() - resolution/2) / resolution);
                            unsigned int voxel_z = std::ceil((zmid - origin.z() - resolution/2) / resolution);
                            voxel_grid(voxel_x, voxel_y, voxel_z) = face.numeric_id;
                        }
                    }
                }
            }
        }


    }

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

//    for (auto const& item:voxel_grid.voxels) {
//        std::vector<unsigned int> coord = voxel_grid.voxel_coordinates(item);
//        if (coord[0] < 0 || coord[0] >x_num) std::cout<<"found one" <<std::endl;
//        if (coord[1] < 0 || coord[1] >y_num) std::cout<<"found one" <<std::endl;
//        if (coord[2] < 0 || coord[2] >z_num) std::cout<<"found one" <<std::endl;
//    }


//    voxel_grid.mark_exterior(voxel_grid);
//    std::cout << "All num: "<< voxel_grid.voxels.size() <<std::endl;
//    std::cout << "exterior_num: "<< voxel_grid.ex_voxels.size() <<std::endl;
//    std::cout << "interior_num: "<< voxel_grid.in_voxels.size() <<std::endl;
//    std::cout << "building_num: "<< voxel_grid.buildings.size() <<std::endl;

    voxel_grid.voxel_to_obj(buildings, origin, "building.obj");
//    voxel_grid.voxel_to_obj(voxel_grid.ex_voxels, origin, "exterior.obj");
//    voxel_grid.voxel_to_obj(voxel_grid.in_voxels, origin, "interior.obj");
    return 0;
}
