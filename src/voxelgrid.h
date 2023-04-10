//
// Created by 55241 on 2023/4/4.
//

#ifndef HW3_VOXELGRID_H
#define HW3_VOXELGRID_H
#include "definitions.h"

struct VoxelGrid {

    std::vector<int> voxels;
    std::vector<int> ex_voxels;
    std::vector<int> in_voxels;
    std::vector<std::string>  building;
    unsigned int max_x, max_y, max_z;
    float resolution;


    VoxelGrid(unsigned int x, unsigned int y, unsigned int z, const float &reso) {
        max_x = x;
        max_y = y;
        max_z = z;
        resolution = reso;
        unsigned int total_voxels = x*y*z;
        voxels.reserve(total_voxels);
        for (unsigned int i = 0; i < total_voxels; ++i) voxels.push_back(0);
    }

    // overloaded function, enabling different types input arguments to call one function of the same name
    int &operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) {
        assert(x >= 0 && x < max_x);
        assert(y >= 0 && y < max_y);
        assert(z >= 0 && z < max_z);
        return voxels[x + y*max_x + z*max_x*max_y];
    }

    int operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) const {
        assert(x >= 0 && x < max_x);
        assert(y >= 0 && y < max_y);
        assert(z >= 0 && z < max_z);
        return voxels[x + y*max_x + z*max_x*max_y];
    }

    Point3 center(const unsigned int &x, const unsigned int &y, const unsigned int &z, const Point3& origin) const {
        double xc = (x+0.5) * resolution + origin.x();
        double yc = (y+0.5) * resolution + origin.y();
        double zc = (z+0.5) * resolution + origin.z();
        Point3 cp(xc,yc,zc);
        return cp;
    }


    void mark_exterior(){
        // start from the origin - 6 connectivity - exterior marked as -1
        bool interior_check = false; // exterior: false, interior: true
        for (unsigned int i=0; i<max_x; i++) {
            for (unsigned int j = 0; j < max_y; j++) {
                for (unsigned int k = 0; k < max_z; k++) {
                    if (voxels[i + j*max_x + k*max_x*max_y] == 0 && !interior_check){ // not building
                        voxels[i + j*max_x + k*max_x*max_y] = -1; // exterior
                        ex_voxels.push_back(-1);
                    }
                    else if (voxels[i + j*max_x + k*max_x*max_y] != 0 && !interior_check){ //touch building
                        interior_check = true; // flipped
                        building.push_back("b");
//                        continue;
                    }
                    else if (voxels[i + j*max_x + k*max_x*max_y] != 0 && interior_check){ //touch building again
                        interior_check = false; // flipped again
                        building.push_back("b");
//                        continue;
                    }
                    else if (voxels[i + j*max_x + k*max_x*max_y] == 0 && interior_check){
                        voxels[i + j*max_x + k*max_x*max_y] = -2; // interior marked as -2
                        in_voxels.push_back(-2);
                    }
                }
            }
        }
//        std::cout << ex_voxels.size() << std::endl;
    }

    void mark_room() {

    }

};


#endif //HW3_VOXELGRID_H