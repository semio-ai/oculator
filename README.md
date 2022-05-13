# oculator

An implementation of oculesic (visual attention) behavior generation. This library is meant to incorporate a vision system as well as a gaze point selection pipeline. The architecture itself is meant to use few dependencies and incorporate a flexible design to allow for extensions. 

## Dependencies
  * Qt (For visualization)
  * OpenIL (For image loading)
  * Torch (For inference)
  * GStreamer (For reading video files, RTSP, and webcam)

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


# TODOs 
QT has a framework for video reading. Use it?
