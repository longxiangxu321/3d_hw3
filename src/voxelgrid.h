//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_VOXELGRID_H
#define HW3_VOXELGRID_H

#include "definitions.h"
#include "geometry.h"


struct VoxelGrid {
//    std::vector<unsigned int> voxels;
    std::vector<std::string> voxels;
    unsigned int max_x, max_y, max_z;
    float resolution;

    VoxelGrid(unsigned int x, unsigned int y, unsigned int z, const float &reso) {
        max_x = x;
        max_y = y;
        max_z = z;
        resolution = reso;
        unsigned int total_voxels = x*y*z;
        voxels.reserve(total_voxels);
        for (unsigned int i = 0; i < total_voxels; ++i) voxels.push_back(" ");
    }

    // overloaded function, enabling different types input arguments to call one function of the same name
    std::string &operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) {
        assert(x >= 0 && x < max_x);
        assert(y >= 0 && y < max_y);
        assert(z >= 0 && z < max_z);
        return voxels[x + y*max_x + z*max_x*max_y];
    }

    std::string operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) const {
        assert(x >= 0 && x < max_x);
        assert(y >= 0 && y < max_y);
        assert(z >= 0 && z < max_z);
        return voxels[x + y*max_x + z*max_x*max_y];
    }

    Point3 center(const unsigned int &x, const unsigned int &y, const unsigned int &z, const Point3& corner) const {
        double xc = (x+0.5) * resolution + corner.x();
        double yc = (y+0.5) * resolution + corner.y();
        double zc = (z+0.5) * resolution + corner.z();
        Point3 cp(xc,yc,zc);
        return cp;
    }
    void set_id(VoxelGrid grid, double xmid, double ymid, double zmid, const Point3& corner, Face face) {
        unsigned int vx = std::ceil((xmid - corner.x()) / resolution) - 0.5;
        unsigned int vy = std::ceil((ymid - corner.y()) / resolution) - 0.5;
        unsigned int vz = std::ceil((zmid - corner.z()) / resolution) - 0.5;
        // mark the voxel with the face id which is inherited from the object id
        grid(vx, vy, vz) = face.id;
    }
};


#endif //HW3_VOXELGRID_H
