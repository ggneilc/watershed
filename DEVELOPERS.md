# Assignment 1 Part A

This program contains a `main` and `stat1` method.

`main`:
- Just used to call `stat1()`

`stat1`:
- Reads from `stdin`, expects the format of stdin to be `%f %f %f` (three floats delimited by spaces)
- averages the total value of the 3rd float
- Finds the min and max with macros defined in `pointcloud.h`
- stores the coords (the first and second float) in an array for min and max respectively. 


# Assignment 1 Part B

This program contains `utils.c` and `pointcloud.c`.

`util.c`:
- Macro for getting the index of a double dimension array
- Generic ArrayList data structure to act as a buffer
    - `listInit(List, elmt)` initialize the array, allocating size equal to that of `elmt`: `listInit(&l, sizeof(int))`
    - `listAddEnd(list, elmt)` appends an element `elmt` to the end of the list, expanding the array if necessary
    - `listGet(list, index)` returns a pointer to the data at the given index.

`pointcloud.c`:
- Macros for determining MIN and MAX of 2 variables
- `pcd_t` struct for storing pointcloud `x y z`
- `clr_t` struct for storing rgb values for bitmap coloring
- `readPointCloudData()` which reads a file in the format of `cols\n x y z ` and stores each line as a `pcd_t` struct and puts them into the ArrayList
- `imagePointCloud()` which reads from that list and maps each `pcd_t` to a coordinate in the bitmap
- `partitions()` helper function to create equal 256 partitions between the min and max z value for equal coloring.

# Assignment 1 Part C

This program added many functions to handle the `pointcloud_t pc` initialization, orienting their direction, and handling the water simulation.

- `pointcloud_t *readPointCloudData();`: Initializes the pointcloud structure from the given datafile.
- `int initializeWatershed();`: Orients each terrain cell to where it can identify its NSWE neighbors.
- `int* findMinMax();`: Helper function to find the min/max of `x y z` coordinates.
- `void watershedAddUniformWater();`: Initialize a starting amount of water into each cell.
- `void watershedStep();`: Calculates how much water runs off / into each cell based on its neighbors.
- `double waterStep();`: Helper function to calculate the water runoff between individual neighboring cells.
- `void imagePointCloudWater();`: Generates our grayscale image and applies a blue shading to it, determined by the amount of water in the current cell and the `maxwd` parameter which defines how much water is in a cell before it is completely shaded blue.