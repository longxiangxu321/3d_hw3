//
// Created by 55241 on 2023/4/12.
//

#include <iostream>
#include <sstream>
#include "definitions.h"
#include "geometry.h"
#include "voxelgrid.h"

inline bool intersect(const double xmin, const double ymin, const double zmin, const double xmax, const double ymax, const double zmax,
                      const double xmid, const double ymid, const double zmid, Triangle_3 triangle) {
//    Inputs are voxel center and bounding box coordinates
    Segment_3 l1(Point3(xmin,ymid,zmid),Point3(xmax,ymid,zmid)); // x-axis
    Segment_3 l2(Point3(xmid,ymin,zmid),Point3(xmid,ymax,zmid)); // y-axis
    Segment_3 l3(Point3(xmid,ymid,zmin),Point3(xmid,ymid,zmax)); // z-axis

    if (!CGAL::do_intersect(triangle, l1) && !CGAL::do_intersect(triangle, l2) &&
        !CGAL::do_intersect(triangle, l3)) {
        // do not intersect.
        return false;
    }
    else {
        return true;
    }
}


std::vector<Point3> read_geometry(const std::string &input_file, std::vector<Point3> &points, std::vector<Object> &objects) {
    bool first_object_read = false;
    int index = 1;
    Object current_object;
    Shell current_shell;
    std::ifstream input_stream;
    input_stream.open(input_file);
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double min_z = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    double max_z = std::numeric_limits<double>::min();

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
                current_shell.faces.emplace_back(v0, v1, v2);
            }
            if (line[0] == 'v') {
                double x,y,z;
                std::stringstream ss(line);
                std::string temp;
                ss >> temp >> x >> y >> z;
                points.emplace_back(x, y, z);
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
    Point3 minp(min_x, min_y, min_z);
    Point3 maxp(max_x, max_y, max_z);
    std::vector<Point3> result;
    result.push_back(minp);
    result.push_back(maxp);
    return result;
}



int main() {
    const std::string input_file = "../data/objs/ifc4.obj";
//    const std::string output_file = "../data/objs/Ifc2voxel.obj";
    const float resolution = 0.1;

    std::vector<Point3> points;
    std::vector<Object> objects;

    std::vector<Point3> o_m = read_geometry(input_file, points, objects);
    Point3 origin = o_m[0];
    Point3 maxp = o_m[1];

    std::cout << "Total number of Building objects: " << objects.size() << std::endl;
    std::cout << "Total number of vertices: " << points.size() << std::endl;
    Point3 origin1 = Point3(origin.x()-resolution, origin.y()-resolution, origin.z() - resolution);

    unsigned int x_num = (maxp.x() - origin1.x()) / resolution + 1;
    unsigned int y_num = (maxp.y() - origin1.y()) / resolution + 1;
    unsigned int z_num = (maxp.z() - origin1.z()) / resolution + 1;
    std::cout << "x,y,z numbers" << x_num << " " << y_num << " " << z_num<< std::endl;


    Voxel_grid voxel_grid(origin1, resolution, x_num, y_num,z_num);
    const std::string output_file = "../data/objs/ifc4_voxel.obj";
//    voxel_grid.voxel2obj(output_file);

    std::vector<Point3> pts;
    //
    for (unsigned int i=0; i<x_num; i++) {
        for (unsigned int j=0; j<y_num; j++) {
            for (unsigned int k=0; k<z_num; k++) {//WQX EDITED
                Point3 vc = voxel_grid(i,j,k).center;
                pts.push_back(vc);
            }
        }
    }

    double max_span = std::max({voxel_grid.length, voxel_grid.width, voxel_grid.height}); // the length of the longest dimension

    pts.push_back(voxel_grid.origin_corner);
    pts.push_back(Point3(voxel_grid.origin_corner.x() + max_span, voxel_grid.origin_corner.y() + max_span, voxel_grid.origin_corner.z() + max_span));

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
                    for (auto jt = it->begin(); jt != it->end(); jt++) {  // only one point in each node
                        // each octree node intersects with triangle, its size is same as the voxel grid
                        double xmin = jt->x() - resolution / 2;
                        double ymin = jt->y() - resolution / 2;
                        double zmin = jt->z() - resolution / 2;
                        double xmax = jt->x() + resolution / 2;
                        double ymax = jt->y() + resolution / 2;
                        double zmax = jt->z() + resolution / 2;
                        double xmid = jt->x();
                        double ymid = jt->y();
                        double zmid = jt->z();


                        if (intersect(xmin, ymin, zmin, xmax, ymax, zmax, xmid, ymid, zmid, triangle)) {
                            unsigned int voxel_x = std::ceil((xmid - origin1.x() - resolution / 2) / resolution);
                            unsigned int voxel_y = std::ceil((ymid - origin1.y() - resolution / 2) / resolution);
                            unsigned int voxel_z = std::ceil((zmid - origin1.z() - resolution / 2) / resolution);
                            if (voxel_grid(voxel_x, voxel_y, voxel_z).marked) {continue;}
                            else {
                                voxel_grid(voxel_x, voxel_y, voxel_z).value = face.numeric_id;
                                voxel_grid(voxel_x, voxel_y, voxel_z).marked = true;
                            }
                        }
                    }
                }
            }
        }
    }

    voxel_grid.voxel2obj(output_file);

    return 0;

}