//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include "definitions.h"



struct Face {
    std::string id;
    std::vector<unsigned long> vertices; // indices in vector of points

    Face() = default;
    Face(unsigned long v0, unsigned long v1, unsigned long v2): vertices({v0, v1, v2}) {}

    bool intersect(Bbox_3 l) const {return 0;}
};

struct Shell {
    std::string id;
    std::vector<Face> faces;

    Shell() = default;

};

struct Object {

    std::string id;
    std::vector<Shell> shells;

    Object() = default;
    Object(std::string& name):id(name) {}

    void set_id() {
        for (auto &shell: shells) {
            shell.id = id;
            for (auto &face: shell.faces) {
                face.id = id;
            }
        }
    }

};




#endif //HW3_GEOMETRY_H
