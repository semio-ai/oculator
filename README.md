# oculator

An implementation of oculesic (visual attention) behavior generation. This library is meant to incorporate a vision system as well as a gaze point selection pipeline. The architecture itself is meant to use few dependencies and incorporate a flexible design to allow for extensions. 

## Build Dependencies
  * [ONNX](https://onnxruntime.ai/) (For inference)
  * [functional_dag](https://github.com/petrogly-ph/functional-dag/)
  
## Run Dependencies
  * [vin](https://github.com/petrogly-ph/vin)
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
$ vin -c ../run/saliency.json
```

# Contributing
Check the issue board if you'd like to hop on board! 
