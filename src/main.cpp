#include "geometry.h"
#include <iostream>
#include <sstream>
const std::string input_file = "../data/objs/Ifc2x3_weld.obj";

int main() {
    std::ifstream input_stream;
    input_stream.open(input_file);

    std::vector<Point3> points;
    std::vector<Object> objects;

    Object current_object;
    Shell current_shell;

    if (input_stream.is_open()) {
        std::string line;
        while (std::getline(input_stream, line)) {
            if (line[0] == 'g') {
                if (current_object.shells.empty()) {
                    current_object.id = line;
                } else {
                    objects.push_back(current_object);
                    current_object.id = line;
                    current_object.shells.clear();
                }
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
        std::cout<<"object "<<i<<" shell number: "<<object.shells.size() << std::endl;
        int j = 0;
        for (auto const& shell: object.shells) {
            std::cout<<"shell "<< j<< " face number: "<<shell.faces.size()<<std::endl;
            j++;
        }
        i++;
    }

    return 0;


}
