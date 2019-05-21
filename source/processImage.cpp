//
// Created by Mrinal Rawool on 5/19/19.
//

#include "processImage.h"

Mat processImage(Mat image, Mat scribble){

    if(image.size()!=scribble.size()){
        // resize
    }

    if(image.empty() || scribble.empty()){
        cerr << "\nprocessImage: Please supply an image and a scribble";
        return Mat();
    }

    Mat objects_segmented = segment(image, scribble);

    return objects_segmented;

}