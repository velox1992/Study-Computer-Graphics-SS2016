# P5 - Project Augmented Reality Application

A augmented reality application with a combination of C++, OpenGL, OpenCV and the ArUco library.

## What
In an endless loop the programs gets a webcam image. With OpenCV and the ArUco it's possible to detect the markers and calculate a projective transformation matrix.
Within the render module several models are loaded and dependendt on the transformation these models are placed on the markers.
In addition to that I implemented a litter shader to visualize a blend effect for the models.

## Example

![Alt text](ARDemoWuerfel.gif)
![Alt text](ArDemoObjects.gif)

### OpenCV/ArUco and OpenGL image

![Alt text](ArExample.JPG?raw=true)

