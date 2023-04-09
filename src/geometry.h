//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include "definitions.h"
#include "voxelgrid.h"


struct Face {
    std::string id;
    std::vector<unsigned long> vertices; // indices in vector of points


    Face() = default;
    Face(unsigned long v0, unsigned long v1, unsigned long v2): vertices({v0, v1, v2}) {}

    bool intersect(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax,
                   double xmid, double ymid, double zmid, Triangle_3 triangle) const {
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

    std::vector<Point3> voxelisation(double xmid, double ymid, double zmid, const Point3& corner,
                              float resolution, VoxelGrid voxel_grid) const{
        // if voxel intersects with the triangle, push back voxel center point
        std::vector<Point3>        voxel_center; // intersected voxel center points
        voxel_center.push_back(Point3(xmid, ymid, zmid));
        // convert back to voxel coordinates
        unsigned int vx = std::ceil((xmid - corner.x()) / resolution) - 0.5;
        unsigned int vy = std::ceil((ymid - corner.y()) / resolution) - 0.5;
        unsigned int vz = std::ceil((zmid - corner.z()) / resolution) - 0.5;
        // mark the voxel with the face id which is inherited from the object id
        voxel_grid.operator()(vx, vy, vz) = id;
        return voxel_center;
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