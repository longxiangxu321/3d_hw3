#include "geometry.h"
#include "voxelgrid.h"
#include <iostream>
#include "poission_reconstruction.h"




int main() {
    const std::string input_file = "../data/objs/ifc1.obj";

    const float resolution = 0.1;
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
    std::vector<unsigned int> exterior;
    for(int i = 0; i<x_num; i++){
        for(int j = 0; j<y_num; j++){
            for(int k = 0; k< z_num; k++){
                if (voxel_grid(i,j,k)<0){
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
            }
        }
    }

    std::cout<<"mark interior done"<<std::endl;


    std::cout<<"exterior "<< voxel_grid.ex_voxels.size() << " " << exterior.size()<<std::endl;
    std::cout<<"interior "<< voxel_grid.in_voxels.size() << " " << interior.size()<<std::endl;
    std::cout<<"building "<< voxel_grid.buildings.size() << " " << buildings.size() << std::endl;
    std::cout<< "total " << voxel_grid.voxels.size() << " " << exterior.size() + interior.size() + buildings.size() <<std::endl;

//    for (auto const &room:voxel_grid.in_voxels){
//        std::cout << "room has " << room.size() <<std::endl;
//    }

    const std::string buil = "../data/objs/ifc1_bu.obj";
    const std::string exte = "../data/objs/ifc3_ex.obj";
    const std::string inte = "../data/objs/ifc3_in.obj";

    std::vector<Point_with_normal> building_surface_points;
    for (auto const bu: voxel_grid.buildings)
        {
            voxel_grid.get_buildding_surface_points(bu, building_surface_points);
        }
    std::cout << "building surface number " << building_surface_points.size() << std::endl;

    std::string building1 = "../data/objs/output.xyz";
    std::ofstream outfile(building1);
    for (const auto& point : building_surface_points) {
        outfile << point.first.x() << " " << point.first.y() << " " << point.first.z() << " " << point.second.x() << " " << point.second.y() << " " << point.second.z() << "\n";
    }
    outfile.close();

    int result = reconstruction(building1);
//    std::cout<<result<<std::endl;


//    std::vector<Point_with_normal> points1;
//    FT sm_angle = 20.0; // Min triangle angle in degrees.
//    FT sm_radius = 30; // Max triangle size w.r.t. point set average spacing.
//    FT sm_distance = 0.375; // Surface Approximation error w.r.t. point set average spacing.
//
//    if(!CGAL::IO::read_points(CGAL::data_file_path("../data/objs/output.xyz"), std::back_inserter(points1),
//                              CGAL::parameters::point_map(Point_map())
//                                      .normal_map (Normal_map())))
//    {
//        std::cerr << "Error: cannot read file input file!" << std::endl;
//        return EXIT_FAILURE;
//    }
//    // Creates implicit function from the read points using the default solver.
//    // Note: this method requires an iterator over points
//    // + property maps to access each point's position and normal.
//    Poisson_reconstruction_function function(points1.begin(), points1.end(), Point_map(), Normal_map());
//    // Computes the Poisson indicator function f()
//    // at each vertex of the triangulation.
//    if ( ! function.compute_implicit_function() )
//        return EXIT_FAILURE;
//    // Computes average spacing
//    FT average_spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>
//            (points1, 6 /* knn = 1 ring */,
//             CGAL::parameters::point_map (Point_map()));
//    // Gets one point inside the implicit surface
//    // and computes implicit function bounding sphere radius.
//    Point3 inner_point = function.get_inner_point();
//    Sphere bsphere = function.bounding_sphere();
//    FT radius = std::sqrt(bsphere.squared_radius());
//    // Defines the implicit surface: requires defining a
//    // conservative bounding sphere centered at inner point.
//    FT sm_sphere_radius = 5.0 * radius;
//    FT sm_dichotomy_error = sm_distance*average_spacing/1000.0; // Dichotomy error must be << sm_distance
//    Surface_3 surface(function,
//                      Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
//                      sm_dichotomy_error/sm_sphere_radius);
//    // Defines surface mesh generation criteria
//    CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
//                                                        sm_radius*average_spacing,  // Max triangle size
//                                                        sm_distance*average_spacing); // Approximation error
//    // Generates surface mesh with manifold option
//    STr tr; // 3D Delaunay triangulation for surface mesh generation
//    C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
//    CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
//                            surface,                              // implicit surface
//                            criteria,                             // meshing criteria
//                            CGAL::Manifold_with_boundary_tag());  // require manifold mesh
//    if(tr.number_of_vertices() == 0)
//        return EXIT_FAILURE;
//    // saves reconstructed surface mesh
//    std::ofstream out("../data/objs/output.off");
//    Polyhedron output_mesh;
//    CGAL::facets_in_complex_2_to_triangle_mesh(c2t3, output_mesh);
//    out << output_mesh;

//    voxel_grid.voxel_to_obj(exterior, origin, exte);
//    voxel_grid.voxel_to_obj(interior, origin, inte);


    return 0;
}
