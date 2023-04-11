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

    bool get_neighbour(int x,int y,int z,VoxelGrid &voxel_grid){

        int i = x - 1;
        int j = y - 1;
        int k = z - 1;
                if(i>=0){
                    if(voxel_grid(i,y,z) == -1) { return true; }
                }
                if(j>=0){
                    if(voxel_grid(x,j,z) == -1) {return true;}
                }
                if(k>=0){
                    if(voxel_grid(x,y,k) == -1) { return true; }
                }
                return false;
 }


    void mark_exterior(VoxelGrid &voxel_grid) {
        voxel_grid(0,0,0) = -1;
        ex_voxels.emplace_back(0);
        for(int i = 0;i<max_z;i++){
            for(int j = 0;j<max_y;j++){
                for(int k = 0;k<max_x;k++){
                    if(i==0&&j==0&&k==0){
                        continue;
                    }
                    else{
                        if(voxel_grid(k,j,i)==0) {
                            if (get_neighbour(k, j, i, voxel_grid)) {
                                voxel_grid(k, j, i) = -1;
                                ex_voxels.emplace_back(0);
                            }
                            else{
                                voxel_grid(k, j, i) = -2;
                                in_voxels.emplace_back(0);
                            }
                        }
                        else{
                            continue;
                        }
                    }
                }
            }
        }

    }

    void mark_room() {

    }

};


#endif //HW3_VOXELGRID_H
