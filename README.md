# oculator

An implementation of oculesic (visual attention) behavior generation. This library is meant to incorporate a vision system as well as a gaze point selection pipeline. The architecture itself is meant to use few dependencies and incorporate a flexible design to allow for extensions. 
  
## Run Dependencies
  * [vin](https://github.com/petrogly-ph/vin)

## Running the example
Oculator contains models and DAG structure for demoing various models. Using at least vin version 1.0, you should be able to run the viewer and saliency models as is. Try the example below:

```bash
# in the oculator build directory

# Start the salency example with the default camera and a resolution of 640x480
$ vin -c run/saliency.json
```

# Contributing
Check the issue board if you'd like to hop on board! 
