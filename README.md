# traceview2d
A simple tracer and viewer for 2d vector field

Instruction for LIC: Press key 's' to (re)compute LIC image at current zoom level.

Instruction for interactive streamline: Click on any location on plane to draw a streamline starting from that location.

For recentering about any point, use "Shift+mouse click"

To clear currently rendered streamlines, press 'c'.

To store current view parameters into "%timestamp%.txt" file, press 'p'.

To store a snapshop of the render window "%timestamp%.png", press 'i'.

Command line arguments:
./main filepath offset seed_x seed_y nmax step lic_size lic_nmax rate [-v view_filename] 

offset - value of z coordinate to slice the 3D data set

seed_x - x coordinate of seed point

seed_y - y coordinate of seed point

nmax - maximum number of advection steps

step - size of each advection step

lic_size - length of side of LIC window

lic_nmax - maximum number of advection steps used for computing LIC

rate - controls resolution of LIC images and is inversely related to resolution of LIC image

Optional argument:
-v view_filename: 


Example command line arguments:
./main /path/to/nek5000.nc 22 75.5 75.3 100 0.25 150.0 50 1