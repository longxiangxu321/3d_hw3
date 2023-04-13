//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include "definitions.h"
#include "json.hpp"


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
std::vector<Point3> get_coordinates(const json& j, bool translate) {
    std::vector<Point3> lspts;
    std::vector<std::vector<int>> lvertices = j["vertices"];
    if (translate) {
        for (auto& vi : lvertices) {
            double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
            double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
            double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
            lspts.push_back(Point3(x, y, z));
        }
    } else {
        //-- do not translate, useful to keep the values low for downstream processing of data
        for (auto& vi : lvertices) {
            double x = (vi[0] * j["transform"]["scale"][0].get<double>());
            double y = (vi[1] * j["transform"]["scale"][1].get<double>());
            double z = (vi[2] * j["transform"]["scale"][2].get<double>());
            lspts.push_back(Point3(x, y, z));
        }
    }
    return lspts;
}

void write2city(std::string filename,const std::string& object_id){
    json j;
    j["type"] = "CityJSON";
    j["version"] = "1.1";
    j["transform"] = json::object();
    j["transform"]["scale"] = json::array({1.0, 1.0, 1.0});
    j["transform"]["translate"] = json::array({0.0, 0.0, 0.0});
    j["CityObjects"][object_id] = json::object();
    j["CityObjects"][object_id]["type"] = "Building";
    j["CityObjects"][object_id]["children"] = json::array();
    j["vertices"] = json::array();



    std::string json_string = j.dump(2);
    std::ofstream out_stream("mybuilding.json");
    out_stream << json_string;
    out_stream.close();
}



#endif //HW3_GEOMETRY_H