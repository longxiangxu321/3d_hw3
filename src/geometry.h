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

inline bool intersect(const double xmin, const double ymin, const double zmin, const double xmax, const double ymax, const double zmax,
                      const double xmid, const double ymid, const double zmid, Triangle_3 triangle) {
//    Inputs are voxel center and bounding box coordinates
    Segment_3 l1(Point3(xmin,ymid,zmid),Point3(xmax,ymid,zmid)); // x-axis
    Segment_3 l2(Point3(xmid,ymin,zmid),Point3(xmid,ymax,zmid)); // y-axis
    Segment_3 l3(Point3(xmid,ymid,zmin),Point3(xmid,ymid,zmax)); // z-axis

    if (!CGAL::do_intersect(triangle, l1) && !CGAL::do_intersect(triangle, l2) &&
        !CGAL::do_intersect(triangle, l3)) {
        // do not intersect.
        return false;//SUPPORTING_PLANE IS THE PLANE AND WITHOUT it caculating both the boundary and is more costing
    }
    else {
        return true;
    }
}




#endif //HW3_GEOMETRY_H