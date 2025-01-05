#if !defined(POINTCLOUD_H)
#define POINTCLOUD_H

#include "utils.h"
#define MAX(x,y) ((x) > (y) ? (x) : (y)) 
#define MIN(x,y) ((x) < (y) ? (x) : (y)) 
typedef struct pcd_t pcd_t;

struct pcd_t {
    double x, y, z, wd;
    pcd_t* north;
    pcd_t* south;
    pcd_t* east;
    pcd_t* west;
};

typedef struct {
    List points;
    int rows, cols; 
    double wcoef, ecoef;
} pointcloud_t; 

typedef struct clr_t {
    double range;
    int r,g,b;
    int padding;
} clr_t;

int stat1();
pointcloud_t *readPointCloudData();
void imagePointCloud();
void partitons();
int initializeWatershed();
int* findMinMax();
void watershedAddUniformWater();
void watershedStep();
double waterStep();
void imagePointCloudWater();


#endif // POINTCLOUD_H
