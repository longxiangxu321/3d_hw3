#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include "poission_reconstruction.h"

void polyhedron_to_json(const std::string &out_json_file, const Polyhedron &building_surface, const std::vector<Polyhedron> &rooms_surfaces) {
    std::cout<<"writing to json ..." << std::endl;
    json j;
    j["type"] = "CityJSON";
    j["version"] = "1.1";
    j["transform"] = json::object();
    j["transform"]["scale"] = json::array({1, 1, 1});
    j["transform"]["translate"] = json::array({0.0, 0.0, 0.0});
    j["CityObjects"] = json::object();

    std::ofstream off_stream;
    off_stream.open(out_json_file);
    j["CityObjects"]["building-id1"]["type"] = "Building";
    j["CityObjects"]["building-id1"]["geometry"][0]["lod"] = "3.0";
    j["CityObjects"]["building-id1"]["geometry"][0]["type"] = "Solid";

    std::vector<std::tuple<double, double, double>> all_vertices;
    for (auto v = building_surface.vertices_begin(); v != building_surface.vertices_end(); ++v) {
        double x = CGAL::to_double(v->point().x());
        double y = CGAL::to_double(v->point().y());
        double z = CGAL::to_double(v->point().z());
        all_vertices.emplace_back(x, y, z);
    }

    std::vector<std::array<int, 3>> building_triangles;
    for (auto f = building_surface.facets_begin(); f != building_surface.facets_end(); ++f) {
        int id1 = std::distance(building_surface.vertices_begin(), f->halfedge()->vertex());
        int id2 = std::distance(building_surface.vertices_begin(), f->halfedge()->next()->vertex());
        int id3 = std::distance(building_surface.vertices_begin(), f->halfedge()->next()->next()->vertex());
        building_triangles.push_back(std::array<int, 3>{id1, id2, id3});
    }

    std::vector<int> vertex_offset(rooms_surfaces.size());
    int total_vertices = building_surface.size_of_vertices();
    for (int i = 0; i < rooms_surfaces.size(); ++i) {
        vertex_offset[i] = total_vertices;
        for (auto v = rooms_surfaces[i].vertices_begin(); v != rooms_surfaces[i].vertices_end(); ++v) {
            double x = CGAL::to_double(v->point().x());
            double y = CGAL::to_double(v->point().y());
            double z = CGAL::to_double(v->point().z());
            all_vertices.emplace_back(x,y,z);
        }
        total_vertices += rooms_surfaces[i].size_of_vertices();
    }



    std::vector<std::vector<std::array<int, 3>>> rooms_triangles;
    for (int i = 0; i < rooms_surfaces.size(); ++i) {
        std::vector<std::array<int, 3>> room_triangles;
        for (auto f = rooms_surfaces[i].facets_begin(); f != rooms_surfaces[i].facets_end(); ++f) {
            int id1 = std::distance(rooms_surfaces[i].vertices_begin(), f->halfedge()->vertex()) + vertex_offset[i];
            int id2 = std::distance(rooms_surfaces[i].vertices_begin(), f->halfedge()->next()->vertex()) + vertex_offset[i];
            int id3 = std::distance(rooms_surfaces[i].vertices_begin(), f->halfedge()->next()->next()->vertex()) + vertex_offset[i];
            room_triangles.push_back(std::array<int, 3>{id1, id2, id3});
        }
        rooms_triangles.emplace_back(room_triangles);
    }



    // extract vertices
    for (int i = 0; i < all_vertices.size(); i++) {
        double x = std::get<0>(all_vertices[i]);
        double y = std::get<1>(all_vertices[i]);
        double z = std::get<2>(all_vertices[i]);
        j["vertices"][i] = nlohmann::json::array({x, y, z});
    }


    // extract building faces
    for (int i = 0; i < building_triangles.size(); i++) {
        int f1, f2, f3;
        f1 = building_triangles[i][0];
        f2 = building_triangles[i][1];
        f3 = building_triangles[i][2];
        j["CityObjects"]["building-id1"]["geometry"][0]["boundaries"][0][i][0] = nlohmann::json::array({f1, f2, f3});

        }

    //extract room surfaces
    for (int i = 0; i<rooms_triangles.size();i++) {
        std::string room_id = "room-" + std::to_string(i);
        j["CityObjects"][room_id]["type"] = "BuildingPart";
        j["CityObjects"][room_id]["parents"][0] = "building-id1";
        j["CityObjects"][room_id]["geometry"][0]["lod"] = "2.2";
        j["CityObjects"][room_id]["geometry"][0]["type"] = "Solid";
        j["CityObjects"]["building-id1"]["children"][i] = room_id;
        for (int k = 0; k<rooms_triangles[i].size(); k++) {
            int f1, f2, f3;
            f1 = rooms_triangles[i][k][0];
            f2 = rooms_triangles[i][k][1];
            f3 = rooms_triangles[i][k][2];
            j["CityObjects"][room_id]["geometry"][0]["boundaries"][0][k][0] = nlohmann::json::array({f1, f2, f3});
        }
    }

    std::string json_string = j.dump(2);
    std::ofstream out_stream(out_json_file);
    std::cout<<"finished" << std::endl;
    out_stream << json_string;
    out_stream.close();

}


int main(int argc, char* argv[]) {

    std::string input_file = "../data/objs/ifc1.obj";
    bool export_building_voxel = false;
    bool export_interior_voxel = false;
    bool export_building_mesh = false;
    bool export_rooms_mesh = false;
    float resolution = 0.1;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];


        if (arg == "--input-file") {
            std::string val = argv[i + 1];
            input_file = val;
        }
        else if (arg == "--export-building-voxel") {
            export_building_voxel = true;
        }
        else if (arg == "--export-interior-voxel") {
            export_interior_voxel = true;
        }
        else if (arg == "--export-building-mesh") {
            export_building_mesh = true;
        }
        else if (arg == "--export-rooms-mesh") {
            export_rooms_mesh = true;
        }
        else if (arg == "--resolution") {
            std::string val = argv[i + 1];
            resolution = std::stof(val);
        }
        else {
            std::cout << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    // Check for incomplete arguments
    if (input_file.empty()) {
        std::cout << "Input file not specified." << std::endl;
        return 1;
    }

    size_t pos = input_file.find_last_of("/");
    std::string file_name = input_file.substr(pos + 1);
    size_t ext_pos = file_name.find_last_of(".");
    std::string fname_without_ext = file_name.substr(0, ext_pos);




    std::ifstream input_stream;
    input_stream.open(input_file);

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double min_z = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    double max_z = std::numeric_limits<double>::min();

    std::vector<Point3> points;
    std::vector<Object> objects;

    Object current_object;
    Shell current_shell;

    bool first_object_read = false;
    int index = 1;

    // 2. read data into memory
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
                current_shell.faces.push_back(Face(v0, v1, v2));
            }
            if (line[0] == 'v') {
                double x,y,z;
                std::stringstream ss(line);
                std::string temp;
                ss >> temp >> x >> y >> z;
                points.push_back(Point3(x, y, z));
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



    std::cout<<"Total number of Building objects: "<<objects.size()<<std::endl;
    std::cout<<"Total number of vertices: "<<points.size()<<std::endl;


    for (auto &object: objects) {
        object.set_id();
    }//objects:list of Object, Object.shells, list of Shell,Shell.faces,list of Face, Face, list of vertices
    //points, list of Point3


    // 3. Voxel grid creation
    auto x_num = static_cast<unsigned int>(std::ceil((max_x - min_x) / resolution)) + 2; // + 1 tp ensure voxel grid larger than building bbox
    auto y_num = static_cast<unsigned int>(std::ceil((max_y - min_y) / resolution)) + 2;
    auto z_num = static_cast<unsigned int>(std::ceil((max_z - min_z) / resolution)) + 2;
    Point3 origin(min_x - resolution, min_y - resolution, min_z - resolution);
    VoxelGrid voxel_grid(x_num, y_num, z_num, resolution, origin);

    std::cout<<"Voxel_grid_x_num: "<<x_num<<std::endl;
    std::cout<<"Voxel_grid_y_num: "<<y_num<<std::endl;
    std::cout<<"Voxel_grid_z_num: "<<z_num<<std::endl;
    std::cout<<"Voxel_grid_resolution: "<<voxel_grid.resolution<<std::endl;
    point_vector pts;



    for (auto const &object: objects) {
        for (auto const &shell: object.shells) {
            for (auto const &face: shell.faces) {
                Point3 v0 = points[face.vertices[0] - 1];
                Point3 v1 = points[face.vertices[1] - 1];
                Point3 v2 = points[face.vertices[2] - 1];
                Triangle_3 triangle(v0, v1, v2);
                voxel_grid.mark_voxels_intersecting_triangle(triangle, face.numeric_id);
                    }
                }
            }


    std::cout<<"mark building done"<<std::endl;

    // check whether building is voxelised correctly
    std::vector<unsigned int> buildings;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                if (voxel_grid(i,j,k)>0){
                    buildings.emplace_back(voxel_grid.voxel_index(i,j,k));
                }
            }
        }
    }

    voxel_grid.mark_exterior();
    voxel_grid.mark_exterior1();

    std::vector<unsigned int> exterior;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                if (voxel_grid(i,j,k) ==-1){
                    exterior.emplace_back(voxel_grid.voxel_index(i,j,k));
                }
            }
        }
    }

    std::cout<<"mark exterior done"<<std::endl;

    int label = -2;
    std::vector<unsigned int> interior;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                unsigned int idx = voxel_grid.voxel_index(i,j,k);
                if (voxel_grid(i,j,k)==0){
                    voxel_grid.mark_room(idx, label);
                    label--;
                }
                if (voxel_grid(i,j,k)<=-2){
                    interior.emplace_back(idx);
                }
                else
                    continue;
            }
        }
    }

    std::cout<<"mark interior done"<<std::endl;


    std::cout<<"exterior voxel "<< voxel_grid.ex_voxels.size() << " " << exterior.size()<<std::endl;
    std::cout<<"interior room "<< voxel_grid.in_voxels.size() << " interior voxel: " << interior.size()<<std::endl;
    std::cout<<"building voxel "<< voxel_grid.buildings.size() << " " << buildings.size() << std::endl;
    std::cout<< "total voxel " << voxel_grid.voxels.size() << " " << exterior.size() + interior.size() + buildings.size() <<std::endl;


    if (export_building_voxel) {
        std::cout<< "exporting building voxel"<<std::endl;
        const std::string buil = "../data/voxels/" + fname_without_ext + "-buildingVoxel.obj";
        voxel_grid.voxel_to_obj(voxel_grid.buildings, buil);
        std::cout<< "exporting building voxel done"<<std::endl;
    }

    if (export_interior_voxel) {
            std::cout<< "exporting interior voxel"<<std::endl;
            const std::string inside = "../data/voxels/" + fname_without_ext + "-interiorVoxel.obj";
            voxel_grid.voxel_to_obj(interior, inside);
            std::cout<< "exporting interior voxel done"<<std::endl;
    }


    std::vector<Point_with_normal> building_surface_points;
    for (auto const bu: voxel_grid.buildings)
        {
            voxel_grid.get_buildding_surface_points(bu, building_surface_points);
        }
    std::cout << "building surface number " << building_surface_points.size() << std::endl;

    std::string building_point_file = "../data/reconstructed/pointcloud/building.xyz";
    std::ofstream outfile(building_point_file);
    for (const auto& point : building_surface_points) {
        outfile << point.first.x() << " " << point.first.y() << " " << point.first.z() << " " << point.second.x() << " " << point.second.y() << " " << point.second.z() << "\n";
    }
    outfile.close();

    Polyhedron building_mesh;
    std::string building_mesh_file = "../data/reconstructed/mesh/building.off";
    reconstruction(building_point_file, building_mesh_file, building_mesh, export_building_mesh);


    std::vector<Polyhedron> rooms_meshes;

    int room_id = 1;
    for (auto const &room:voxel_grid.in_voxels) {
        if (room.size() >= 3000) {    // filter out small rooms;
            std::vector<Point_with_normal> room_points;
            std::string room_point_file = "../data/reconstructed/pointcloud/room-" + std::to_string(room_id) + ".xyz";
            for (auto const &rp: room) {
                voxel_grid.get_room_surface_points(rp, room_points);
            }
            std::ofstream outf(room_point_file);
            for (auto const &point:room_points) {
                outf << point.first.x() << " " << point.first.y() << " " << point.first.z() << " " << point.second.x() << " " << point.second.y() << " " << point.second.z() << "\n";
            }
            outf.close();
            Polyhedron room_result;
            std::string room_mesh_file = "../data/reconstructed/mesh/room-" + std::to_string(room_id) + ".off";
            reconstruction(room_point_file, room_mesh_file, room_result, export_rooms_mesh);
            rooms_meshes.emplace_back(room_result);
            room_id++;
        }
        else continue;
    }
    std::cout<<"building mesh " << 1 <<std::endl;
    std::cout<<"room meshes " << rooms_meshes.size()<<std::endl;

    const std::string output_json = "../data/cityjson/" + fname_without_ext + "-buildingmodel.json";
    polyhedron_to_json(output_json, building_mesh, rooms_meshes);
    return 0;
}
