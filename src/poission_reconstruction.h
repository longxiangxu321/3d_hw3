//
// Created by 55241 on 2023/4/13.
//

#ifndef MAIN_CPP_POISSION_RECONSTRUCTION_H
#define MAIN_CPP_POISSION_RECONSTRUCTION_H
#include "definitions.h"
#include <iostream>

int reconstruction(const std::string &point_file, const std::string &mesh_file, Polyhedron &out_mesh, bool const &export_mesh) {
    std::vector<Point_with_normal> points1;
    FT sm_angle = 25.0; // Min triangle angle in degrees.
    FT sm_radius = 20; // Max triangle size w.r.t. point set average spacing.
    FT sm_distance = 0.175; // Surface Approximation error w.r.t. point set average spacing.

    if(!CGAL::IO::read_points(CGAL::data_file_path(point_file), std::back_inserter(points1),
                              CGAL::parameters::point_map(Point_map())
                                      .normal_map (Normal_map())))
    {
        std::cerr << "Error: cannot read file input file!" << std::endl;
        return EXIT_FAILURE;
    }

    // Creates implicit function from the read points using the default solver.
    // Note: this method requires an iterator over points
    // + property maps to access each point's position and normal.
    Poisson_reconstruction_function function(points1.begin(), points1.end(), Point_map(), Normal_map());
    // Computes the Poisson indicator function f()
    // at each vertex of the triangulation.
    if ( ! function.compute_implicit_function() )
        return EXIT_FAILURE;
    // Computes average spacing
    FT average_spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>
            (points1, 6 /* knn = 1 ring */,
             CGAL::parameters::point_map (Point_map()));
    // Gets one point inside the implicit surface
    // and computes implicit function bounding sphere radius.
    Point3 inner_point = function.get_inner_point();
    Sphere bsphere = function.bounding_sphere();
    FT radius = std::sqrt(bsphere.squared_radius());
    // Defines the implicit surface: requires defining a
    // conservative bounding sphere centered at inner point.
    FT sm_sphere_radius = 5.0 * radius;
    FT sm_dichotomy_error = sm_distance*average_spacing/1000.0; // Dichotomy error must be << sm_distance
    Surface_3 surface(function,
                      Sphere(inner_point,sm_sphere_radius*sm_sphere_radius),
                      sm_dichotomy_error/sm_sphere_radius);
    // Defines surface mesh generation criteria
    CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
                                                        sm_radius*average_spacing,  // Max triangle size
                                                        sm_distance*average_spacing); // Approximation error
    // Generates surface mesh with manifold option
    STr tr; // 3D Delaunay triangulation for surface mesh generation
    C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
    CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
                            surface,                              // implicit surface
                            criteria,                             // meshing criteria
                            CGAL::Manifold_with_boundary_tag());  // require manifold mesh
    if(tr.number_of_vertices() == 0)
        return EXIT_FAILURE;
    // saves reconstructed surface mesh

    Polyhedron output_mesh;
    CGAL::facets_in_complex_2_to_triangle_mesh(c2t3, output_mesh);
    if (export_mesh) {
        std::ofstream out(mesh_file);
        out << output_mesh;
        std::cout << "exporting mesh"<<std::endl;}
    out_mesh = output_mesh;
    return 0;
}

#endif //MAIN_CPP_POISSION_RECONSTRUCTION_H
