#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include <sstream>
const std::string input_file = "../data/objs/Ifc2x3.obj";
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
                    objects.push_back(current_object);
                    current_object.shells.clear();
                }
                current_object.id = line;
            }
            if (line[0] == 'u') {
                if (current_shell.faces.empty()) {
                    continue;
                } else {
                    current_object.shells.push_back(current_shell);
                    current_shell.faces.clear();
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
            if (line.empty() || line[0] == '#') {  // Skip empty lines and lines starting with '#'
                continue;
            }
        }
        if (!current_object.shells.empty()) {
            objects.push_back(current_object);
        }
    }



    std::cout<<objects.size()<<std::endl;
    std::cout<<points.size()<<std::endl;
    int i = 0;
    for (auto const& object: objects) {
        std::cout<<object.id<< std::endl;
    }
//        std::cout<<"object "<< i <<" "<<object.id<<" shell number: "<<object.shells.size() << std::endl;
//        int j = 0;
//        for (auto const& shell: object.shells) {
//            std::cout<<"shell "<< j<< " face number: "<<shell.faces.size()<<std::endl;
//            j++;
//        }
//        i++;}

    auto x_num = static_cast<unsigned int>(std::ceil((max_x - min_x) / resolution));
    auto y_num = static_cast<unsigned int>(std::ceil((max_y - min_y) / resolution));
    auto z_num = static_cast<unsigned int>(std::ceil((max_z - min_z) / resolution));
    VoxelGrid voxel_grid(x_num, y_num, z_num);



    return 0;


}
