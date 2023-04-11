//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include "definitions.h"


struct Face {
    std::string id;
    unsigned int numeric_id;
    std::vector<unsigned long> vertices; // indices in vector of points


    Face() = default;
    Face(unsigned long v0, unsigned long v1, unsigned long v2): vertices({v0, v1, v2}) {}




};

struct Shell {
    std::string id;
    unsigned int numeric_id;
    std::vector<Face> faces;

    Shell() = default;

};

struct Object {

    std::string id;
    unsigned int numeric_id;
    std::vector<Shell> shells;

    Object() = default;
    Object(std::string& name):id(name) {}

    void set_id() {
        for (auto &shell: shells) {
            shell.id = id;
            shell.numeric_id = numeric_id;
            for (auto &face: shell.faces) {
                face.id = id;
                face.numeric_id = numeric_id;
            }
        }
    }

};




#endif //HW3_GEOMETRY_H