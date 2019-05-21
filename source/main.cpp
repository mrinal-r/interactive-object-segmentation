#include "processImage.h"
#include "maskUtil.h"
extern ostringstream err;

/***********************************************************************************************************************
 Function: main
 Purpose: accepts an image and an annotated scribble image indicating an object of interest. The program outputs
          a mask for the object using seeded region growing algorithm

 **********************************************************************************************************************/
int main(int argc, char *argv[]) {
    try {

        info();

        string scribbleFile = "../../data/scribbleMasks/blackswan_38.png";
        string imageFile = "../../data/videos/blackswan/00038.jpg";

        Mat image = imread(imageFile.c_str(), CV_LOAD_IMAGE_UNCHANGED);
        Mat scribble = imread(scribbleFile.c_str(), CV_LOAD_IMAGE_UNCHANGED);
        Mat masked = processImage(image, scribble);

        if (masked.empty()){
            err << "\nMask not created";
            throw err.str();
        }

        string window = "masked";
        display(window, masked);
        waitKey(0);

        /*if (argc < 2){
            err << "No video file has been selected."  << endl;
            help();
            throw err.str();
        }

        if (argc > 2 ){
            err << "Too many arguments."  << endl;
            help();
            throw err.str();
        }*/

        //processImage(argv[1], argv[2]);

        // step 2: refactor later
        // list all videos to be processed
        // loop over each video
        // select a frame within a video, look for its scribble
        // if found, send it for segmentation else continue




    } catch (string& error) {
        cout << "\nError in main:\t" << error << endl;
        return -1;
    }
    return 0;
}

