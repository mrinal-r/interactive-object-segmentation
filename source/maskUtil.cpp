//
// Created by Mrinal Rawool on 5/20/19.
//

#include "maskUtil.h"
ostringstream err;

void display(string& windowName, Mat& image, int x, int y ){
    namedWindow(windowName);
    moveWindow(windowName, x, y);
    imshow(windowName, image);
}

void info(){
    cout << "\nATTENTION!!\nOnly included images will be processed for now"  << endl;
    cout << "\nExpected usage will be mask <image> <scribble>" << endl;
}

/*void info(){
    err << "\nATTENTION!!\nOnly included images will be processed for now"  << endl;
    err << "\nExpected usage will be mask <image> <scribble>" << endl;
}*/
