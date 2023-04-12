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
    const float resolution = 0.5;

    std::vector<Point3> points;
    std::vector<Object> objects;

    std::vector<Point3> o_m = read_geometry(input_file, points, objects);
    Point3 origin = o_m[0];
    Point3 maxp = o_m[1];

    std::cout << "Total number of Building objects: " << objects.size() << std::endl;
    std::cout << "Total number of vertices: " << points.size() << std::endl;

    unsigned int x_num = (maxp.x() - origin.x()) / resolution + 1;
    unsigned int y_num = (maxp.y() - origin.y()) / resolution + 1;
    unsigned int z_num = (maxp.z() - origin.z()) / resolution + 1;
    std::cout << "x,y,z numbers" << x_num << " " << y_num << " " << z_num<< std::endl;


    Voxel_grid voxel_grid(origin, resolution, x_num, y_num,z_num);
    const std::string output_file = "../data/objs/ifc4_voxel.obj";
    voxel_grid.voxel2obj(output_file);


    return 0;

}