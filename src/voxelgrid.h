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

    bool get_neighbour(unsigned int x,unsigned int y,unsigned int z,const VoxelGrid &voxel_grid)const{

        unsigned int i = x + 1;
        unsigned int j = y + 1;
        unsigned int k = z + 1;
        //check its former three connected neighbour,if one of them is exterior,return true
                if(i<=max_x-1){
                    if(voxel_grid(i,y,z) == -1) { return true;}
                }
                if(j<=max_y-1){
                    if(voxel_grid(x,j,z) == -1) {return true;}
                }
                if(k<=max_z-1){
                    if(voxel_grid(x,y,k) == -1) { return true; }
                }
                return false;
 }


    void mark_exterior(VoxelGrid &voxel_grid) {
        for(unsigned int i =voxel_grid.max_x-1;i>0;i--){
            for(unsigned int j = voxel_grid.max_y-1;j>0;j--){
                for(unsigned int k = voxel_grid.max_z-1;k>0;k--){
                    if(i==max_x-1&&j==max_y-1&&k==max_z-1){
                        voxel_grid(max_x-1,max_y-1,max_z-1) = -1;
                        ex_voxels.emplace_back(0);
                    }
                    else{
                        if(voxel_grid(i,j,k)==0) {//make sure this is not building voxel
                            if (get_neighbour(i, j, k, voxel_grid)) {
                                voxel_grid(i, j, k) = -1;//if one of the neigbour is exterior,it's connected and is exterior too
                                ex_voxels.emplace_back(0);
                            }
                            else{
                                in_voxels.emplace_back(0);// this is interior, the value will be changed in mark_room
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
