# interactive-object-segmentation
Purpose: Segment an object based on annotated scribbles provided by the user

Sample images used from [DAVIS Interactive Challenge 2019](https://davischallenge.org/challenge2018/interactive.html "DAVIS 2019")

### To test
Place the image to be segmented in the `data/videos` folder and the corresponding scribble annotation image in the `data/scribbleMasks` folder.

### To run
Clone the repository

```
cd source/build
cmake ..
make
./mask
```

### Sample run
##### Sample Input
![alt text](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/sample/00038.jpg "Input Image")

##### Scribble Mask indicating Object of Interest
![alt text](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/sample/blackswan_38.png "Scribble Masks")

##### Binary Image of Superpixel boundaries
![alt text](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/sample/label_mask.jpg "Labels")

##### Superpixels
![alt text](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/sample/segmented.jpg "Superpixels")

##### Segmented Mask
![alt text](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/sample/masked.jpg "Mask")

This project was implemented for academic purposes. Please refer the [project report](https://github.com/mrinal-r/interactive-object-segmentation/blob/master/project-report.pdf "Project Report") to know details about the implementation.



