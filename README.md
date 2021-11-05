# oculator

An implementation of oculesic behavior (visual attention) generation.

## Dependencies
  * OpenCV 4

## Building

```bash
# in the oculator root directory
mkdir build
cd build
cmake ..
make -j4
```

## Running the example

```bash
# in the oculator build directory

# Start the salency example with the default camera and a resolution of 640x480
./examples/saliency/saliency --device=0 --resolution_x=640 --resolution_y=480
```
