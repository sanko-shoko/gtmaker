# gtmaker
image annotation tool

[binary file (windows, mac)](https://github.com/sanko-shoko/gtmaker/releases)

[demo (youtube)](https://www.youtube.com/watch?time_continue=106&v=a3ub4SHJEEs)

## how to use

### open image directory
select [file -> open image dir]

### edit rectangle
check [edit menu-> rectangle]

left click on image and drag.

![fig1](https://github.com/sanko-shoko/gtmaker/blob/master/screenshot/fig1.png)

### edit contour
check [edit menu -> contour] (after editing rectangle)

left click on image for the points.

click on the start point to create a loop.


![fig2](https://github.com/sanko-shoko/gtmaker/blob/master/screenshot/fig2.png)

### adjust image
shift   : space key + left click + drag

rescale : space key + mouse scroll

### save & load
save : click [save]

load : click left string of [save], and select directory


## compile
required
- [OpenCV](https://opencv.org/)
- [GLFW](http://www.glfw.org/)
- [simplesp](https://github.com/sanko-shoko/simplesp)

