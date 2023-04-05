#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include <sstream>
const std::string input_file = "../data/objs/ifc2x3.obj";
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

    if (input_stream.is_open()) {
        std::string line;
        while (std::getline(input_stream, line)) {
            if (line[0] == 'g') {
                if (!first_object_read) {
                    first_object_read = true;
                } else {
                    current_object.shells.push_back(current_shell);
                    objects.push_back(current_object);
                    current_object = Object();
                    current_shell = Shell();
                }
                current_object.id = line;
            }
            if (line[0] == 'u') {
                if (current_shell.faces.empty()) {
                    continue;
                } else {
                    current_object.shells.push_back(current_shell);
                    current_shell = Shell();
                }
            }
            if (line[0] == 'f') {
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



    std::cout<<objects.size()<<std::endl;
    std::cout<<points.size()<<std::endl;

    for (auto const& object: objects) {
        std::cout<<"object "<<" "<<object.id<<std::endl;
        std::cout<< " shell number: "<<object.shells.size() << std::endl;
        for (auto const& shell: object.shells) {
            std::cout << "face number: " << shell.faces.size() << std::endl;
        }}

    for (auto &object: objects) {
        object.set_id();
    }


    auto x_num = static_cast<unsigned int>(std::ceil((max_x - min_x) / resolution));
    auto y_num = static_cast<unsigned int>(std::ceil((max_y - min_y) / resolution));
    auto z_num = static_cast<unsigned int>(std::ceil((max_z - min_z) / resolution));
    VoxelGrid voxel_grid(x_num, y_num, z_num, resolution);

    std::cout<<x_num<<std::endl;
    std::cout<<y_num<<std::endl;
    std::cout<<z_num<<std::endl;
    std::cout<<voxel_grid.resolution<<std::endl;
    point_vector pts;

    for (unsigned int i=0; i<x_num; i++) {
        for (unsigned int j=0; j<y_num; j++) {
            for (unsigned int k=0; k<z_num; k++) {
                Point3 vc = voxel_grid.center(i,j,k);
                pts.push_back(vc);
            }
        }
    }

    Octree voxel_tree(pts);
    int max_depth = std::ceil(std::log(points.size())/ std::log(8));
    voxel_tree.refine(max_depth,1);



    for (auto const &object: objects) {
        for (auto const &shell: object.shells) {
            for (auto const &face: shell.faces) {
                Point3 v0 = points[face.vertices[0]];
                Point3 v1 = points[face.vertices[1]];
                Point3 v2 = points[face.vertices[2]];
                Triangle_3 triangle(v0, v1, v2);
                output_nodes o1;
                auto output_iterator = std::back_inserter(o1);
                voxel_tree.intersected_nodes(triangle, output_iterator);

                for (auto it = o1.begin(); it != o1.end(); ++it) {
                    for (auto jt = it->begin(); jt != it->end(); jt++) {  // only one point in each node
                        double xmin = jt->x() - resolution;
                        double ymin = jt->y() - resolution;
                        double zmin = jt->z() - resolution;
                        double xmax = jt->x() + resolution;
                        double ymax = jt->y() + resolution;
                        double zmax = jt->z() + resolution;
                        if (face.intersect(Bbox_3(xmin, ymin, zmin, xmax, ymax, zmax))) {
                            continue;  // apply precise line triangle test here
                        }
                    }
                }
            }
        }
    }

    return 0;


}
