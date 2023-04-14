//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_DEFINITIONS_H
#define HW3_DEFINITIONS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/optimal_bounding_box.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Triangulation_3.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/Octree.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/Triangulation_vertex_base_with_id_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/IO/facets_in_complex_2_to_triangle_mesh.h>
#include <CGAL/poisson_surface_reconstruction.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/iterator.h>

#include <CGAL/pca_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/property_map.h>
#include "json.hpp"
// Define a vector to store the output

using json = nlohmann::json;

//-- for mark_domain()
struct FaceInfo2
{
    FaceInfo2() {}
    int nesting_level;
    bool in_domain() {
        return nesting_level % 2 == 1;
    }
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_2                  Point2;
typedef K::Point_3                  Point3;
typedef CGAL::Polygon_2<K>          Polygon2;
typedef K::Plane_3                  Plane;
typedef K::Vector_3                 Vector3;
typedef K::Triangle_3               Triangle_3;
typedef K::Sphere_3                 Sphere;
typedef K::FT                       FT;


typedef CGAL::Bbox_3                Bbox_3;
typedef CGAL::Segment_3<K>          Segment_3;

typedef CGAL::Triangulation_vertex_base_with_id_2 <K>             Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K>   Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>       Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_intersections_tag                             Itag;

typedef std::vector<Point3> point_vector;
typedef std::vector<Bbox_3> boxes;
typedef CGAL::Octree<K, point_vector> Octree;


typedef std::pair<Point3, Vector3> Point_with_normal;
typedef CGAL::First_of_pair_property_map<Point_with_normal> Point_map;
typedef CGAL::Second_of_pair_property_map<Point_with_normal> Normal_map;
typedef std::vector<Point_with_normal> PointList;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef CGAL::Poisson_reconstruction_function<K> Poisson_reconstruction_function;
typedef CGAL::Surface_mesh_default_triangulation_3 STr;
typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
typedef CGAL::Implicit_surface_3<K, Poisson_reconstruction_function> Surface_3;

typedef CGAL::Parallel_if_available_tag Concurrency_tag;
#endif //HW3_DEFINITIONS_H