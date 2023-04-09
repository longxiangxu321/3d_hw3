//
// Created by 55241 on 2023/4/4.
//
#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include "definitions.h"


struct Face {
    std::string id;
    std::vector<unsigned long> vertices; // indices in vector of points
//    std::vector<Point3> voxel_center;
    Face() = default;
    Face(unsigned long v0, unsigned long v1, unsigned long v2): vertices({v0, v1, v2}) {}
//    Face(std::vector<Point3>& voxel_center):voxel_center(voxel_center) {}

    bool intersect(double xmin,double ymin,double zmin,double xmax,double ymax,double zmax,double xmid,double ymid,double zmid,Triangle_3 triangle) const {
        Segment_3 l1(Point3(xmid,ymid,zmin),Point3(xmid,ymid,zmax));
        Segment_3 l2(Point3(xmin,ymid,zmid),Point3(xmax,ymid,zmid));
        Segment_3 l3(Point3(xmid,ymin,zmid),Point3(xmid,ymax,zmid));

        if (!CGAL::do_intersect(triangle, l1)&&!CGAL::do_intersect(triangle, l2)&&!CGAL::do_intersect(triangle, l3)) {
            // do not intersect.
            return false;
        }
        else {
            return true;
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
