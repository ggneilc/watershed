#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pointcloud.h"
#include "bmp.h"


pointcloud_t *readPointCloudData(FILE *stream);
void imagePointCloud(pointcloud_t *pc, int width, char* filename);
void partitons(clr_t* range, double start, double end);
int initializeWatershed(pointcloud_t *);
int* findMinMax(pointcloud_t *);
void watershedAddUniformWater(pointcloud_t *, double);
void watershedStep(pointcloud_t *);
double waterStep(double,double,double,double,double);
void imagePointCloudWater(pointcloud_t *, double, char *);
pcd_t **allocateArray(int rows, int cols);

/**
 * Sort through pc.points and assign each NWSE value
 * Already initialized water to 0 in readPointCloudData()
 */
int initializeWatershed(pointcloud_t *pc){
    /* Coords [0 1] = x */
    /* Coords [2 3] = y */
    /* Coords [4 5] = z */
    int* coords = findMinMax(pc);

    // Calculate Step Size
    double x_step = (coords[1] - coords[0]) / (double)(pc->cols - 1);
    double y_step = (coords[3] - coords[2]) / (double)(pc->cols - 1);

    pcd_t **grid = allocateArray(pc->cols, pc->cols);

    // Calculate new index for direction array
    for (int i=0; i < pc->points.size; i++){
        pcd_t *point = &((pcd_t*)pc->points.data)[i];       
        int new_x = (int)((point->x - coords[0]) / x_step);
        int new_y = (int)((point->y - coords[2]) / y_step);

        grid[new_x][new_y] = *point;
        // Update pointers to neighbor cells
        point->east = (new_x-1 < 0) ? NULL : &(grid[new_x-1][new_y]);
        point->west = (new_x+1 >= pc->cols) ? NULL : &(grid[new_x+1][new_y]);
        point->south = (new_y-1 < 0) ? NULL : &(grid[new_x][new_y-1]);
        point->north = (new_y+1 >= pc->cols) ? NULL : &(grid[new_x][new_y+1]);
    }
    return 0;
}

/**
 * Initialize water in each cell to amt
 */
void watershedAddUniformWater(pointcloud_t* pc, double amt){
    for (int i=0; i < pc->points.size; i++){
        pcd_t *point = &((pcd_t*)pc->points.data)[i];       
        point->wd = amt;
    }
//    for (int i =0; i < pc->points.size; i++){
//        printf("water content %f", ((pcd_t*)pc->points.data)[i].wd);
//    }
}

/**
 * Update each points current water based on neighbors
 */
void watershedStep(pointcloud_t* pc){

    // Create temporary array to store new water depths
    double* new_water_depths = (double*)malloc(pc->points.size * sizeof(double));

    for (int i=0; i < pc->points.size; i++){
        pcd_t *point = &((pcd_t*)pc->points.data)[i];       
        double eastw, westw, northw, southw = 0;
        eastw = (point->east == NULL) ? waterStep(point->z, point->wd, 0,0,pc->wcoef) : waterStep(point->z,point->wd,point->east->z,point->east->wd, pc->wcoef);
        westw = (point->west == NULL) ? waterStep(point->z, point->wd, 0,0,pc->wcoef) : waterStep(point->z,point->wd,point->west->z,point->west->wd, pc->wcoef);
        northw = (point->north == NULL) ? waterStep(point->z, point->wd, 0,0,pc->wcoef) : waterStep(point->z,point->wd,point->north->z,point->north->wd, pc->wcoef);
        southw = (point->south == NULL) ? waterStep(point->z, point->wd, 0,0,pc->wcoef) : waterStep(point->z,point->wd,point->south->z,point->south->wd, pc->wcoef);

        // Calculate new water depth
        double water_change = (eastw + westw + northw + southw);
        new_water_depths[i] = (water_change-point->wd) * pc->ecoef;
   }

    // Update all points with new water depths
    for (int i = 0; i < pc->points.size; i++) {
        pcd_t *point = &((pcd_t*)pc->points.data)[i];
        if (new_water_depths[i] < 0) new_water_depths[i] = 0;
        point->wd = new_water_depths[i];
    }


    free(new_water_depths);
}

/**
 * Determine water update for a single neighbor
 */
double waterStep(double cur_t, double cur_wd, double other_t, double other_wd, double wcoef){
    return ((other_t + other_wd) - (cur_t + cur_wd)) * wcoef;
}

/**
 * Convert RGB values to hex color code
 * Increases blue component based on water content
 */
unsigned int rgbToHexWater(int baseColor, double waterDepth, double maxwd) {
    // Keep the base grayscale value for red and green
    int r = baseColor;
    int g = baseColor;
    
    // Calculate blue component: interpolate between baseColor and 255 based on water depth
    double waterRatio = waterDepth / maxwd;
    if (waterRatio > 1.0) waterRatio = 1.0;
    if (waterRatio < 0.0) waterRatio = 0.0;
    
    int b = baseColor + (int)((255 - baseColor) * waterRatio);
    
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

/**
 * Create heightmap with water depth visualization
 * pc: point cloud data
 * maxwd: maximum water depth for scaling
 */
void imagePointCloudWater(pointcloud_t *pc, double maxwd, char* filename) {
    int *coords = findMinMax(pc);
    clr_t* p = calloc(256, sizeof(clr_t));
    if (p != NULL) 
        partitons(p, coords[4], coords[5]);
    
    Bitmap *b = bm_create(pc->cols, pc->cols);

    for (int i = 0; i < pc->points.size; i++) {
        pcd_t coord = ((pcd_t*)pc->points.data)[i];
        
        // Map x and y to pixel coordinates 
        int pixel_x = (int)((coord.x - coords[0]) / (coords[1] - coords[0]) * (pc->cols - 1));
        int pixel_y = (int)((coord.y - coords[2]) / (coords[3] - coords[2]) * (pc->cols - 1));
        pixel_y = pc->cols - 1 - pixel_y;

        // Find the closest range for height color
        int closest_j = 0;
        double min_diff = fabs(p[0].range - coord.z);
        for (int j = 1; j < 256; j++) {
            double diff = fabs(p[j].range - coord.z);
            if (diff < min_diff) {
                min_diff = diff;
                closest_j = j;
            }
        }

        // Get base grayscale color
        int baseColor = p[closest_j].r; // Since r=g=b in grayscale

        // Convert to hex color with water depth represented by blue component
        unsigned int hexValue = rgbToHexWater(baseColor, coord.wd, maxwd);

        bm_set_color(b, hexValue);
        bm_putpixel(b, pixel_x, pixel_y);
    }

    bm_save(b, filename);
    printf("saved water bitmap\n");
    bm_free(b);
    free(p);
}


pointcloud_t pc;

/**
 * Read from a FILE stream in format x y z
 * Create pcd_t for x,y,z
 * Store in list* 
 */
pointcloud_t *readPointCloudData(FILE *stream){
    double x,y,z;

    List l;
    listInit(&l, sizeof(pcd_t));
    
    while (fscanf(stream, "%lf %lf %lf", &x,&y,&z) != EOF){
        pcd_t datum;
        datum.x = x;
        datum.y = y;
        datum.z = z;
        datum.wd = 0;

        datum.east = datum.west = datum.north = datum.south = NULL;

        listAddEnd(&l,&datum);
    }
    pc.points = l;
    return &pc;
}

/**
 * convert RGB to AARRGGBB hex
 */ 
unsigned int rgbToHex(int r, int g, int b) {
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

/**
 * Determine min & max z of all points in list
 * create 256 equal partitions between min and max
 * assign lowest height to black (0,0,0)
 * assign highest height to white (255,255,255)
 * Heights inbetween should be properly distributed:
 *  i.e. the middle most height should be (128,128,128)
 */
void imagePointCloud(pointcloud_t *pc, int width, char* filename){
    int *coords = findMinMax(pc);

    clr_t* p = calloc(256 , sizeof(clr_t));
    if (p != NULL)
    partitons(p, coords[4], coords[5]);

    Bitmap *b = bm_create(width,width);

    // Map the values to color coordinates in their range
    for (int i=0; i < pc->points.size; i++){
        pcd_t coord = ((pcd_t*)pc->points.data)[i];

        // Map x and y to pixel coordinates

        int pixel_x = (int)((coord.x - coords[0]) / (coords[1] - coords[0]) * (width - 1));
        int pixel_y = (int)((coord.y - coords[2]) / (coords[3] - coords[2]) * (width - 1));
        
        // Invert y-coordinate because bitmap's origin is top-left
        pixel_y = width - 1 - pixel_y;

        // Find the closest range
        int closest_j = 0;
        double min_diff = fabs(p[0].range - coord.z);
        for (int j=1; j<256; j++){
            double diff = fabs(p[j].range - coord.z);
            if (diff < min_diff) {
                min_diff = diff;
                closest_j = j;
            }
        }
        
        unsigned int hexValue = rgbToHex(p[closest_j].r,p[closest_j].g,p[closest_j].b);
        bm_set_color(b,hexValue);
        bm_putpixel(b,pixel_x,pixel_y);
    }
    bm_save(b, filename);
    printf("saved bitmap\n");
    bm_free(b);
    free(p);
}

/**
 * Create 256 equal partitions between 2 points
 */
void partitons(clr_t* range, double start, double end){
    // 255 steps for 256 partitions    
    double step = (end - start) / 255.0;  

    for (int i = 0; i < 256; i++) {
        clr_t datum;
        datum.range = start + i * step;
        datum.r=i; datum.g =i; datum.b =i;
        range[i] = datum;
    }
}

/**
 * Find the min and max coordinates in the pc list
 * return an array that contains the minmax coords
 * [0 1] minx maxx
 * [2 3] miny maxy
 * [4 5] minz maxz
 */
int* findMinMax(pointcloud_t *pc){
    int* coords = (int*)malloc(6 * sizeof(int));
    double min_x = INFINITY, max_x = -INFINITY;
    double min_y = INFINITY, max_y = -INFINITY;
    double min_z = INFINITY, max_z = -INFINITY;


    // Find min and max
    for (int i=0; i < pc->points.size; i++){
        pcd_t coord = ((pcd_t*)pc->points.data)[i];
        min_x = MIN(min_x, coord.x);
        max_x = MAX(max_x, coord.x);
        min_y = MIN(min_y, coord.y);
        max_y = MAX(max_y, coord.y);
        min_z = MIN(min_z, coord.z);
        max_z = MAX(max_z, coord.z);
    }

    coords[0] = min_x;
    coords[1] = max_x;
    coords[2] = min_y;
    coords[3] = max_y;
    coords[4] = min_z;
    coords[5] = max_z;

    return coords;
}

pcd_t **allocateArray(int rows, int cols){
    // Initialize rows
    pcd_t **array = (pcd_t **)calloc(rows,  sizeof(pcd_t *));
    if (array == NULL){
        fprintf(stderr, "Memory allocation failed for rows.\n");
        return NULL;
    }

    // Initialize row arrays (columns)
    for (int i = 0; i < rows; i++){
        array[i] = (pcd_t *)calloc(cols,  sizeof(pcd_t));

        if (array[i] == NULL){
            fprintf(stderr, "Memory allocation failed for cols at row %d", i);
            //free previous rows
            for (int j = 0; j < i; j++){
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    return array;
}