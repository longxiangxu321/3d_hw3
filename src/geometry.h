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

    bool intersect(Bbox_3 l,Triangle_3 triangle) const {
        if (!CGAL::do_intersect(triangle.supporting_plane(), l)) {
            // do not intersect.
            return false;//SUPPORTING_PLANE IS THE PLANE AND WITHOUT it caculating both the boundary and is more costing
        }

        if (CGAL::do_intersect(triangle, l)) {
            return true;
        } else {
            return false;
        }

    }
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
