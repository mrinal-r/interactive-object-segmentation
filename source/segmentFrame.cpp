//
// Created by Mrinal Rawool on 4/29/19.
//

#include "segmentFrame.h"

Mat segment(Mat& src, Mat& scribbleMask){

    // ********** Step 1: Preprocessing ********** //
    // -- Using median blur to smoothen the image -- //
    cout << "\nImage size" << src.rows << "  " << src.cols;
    Mat blurImage;
    medianBlur(src, blurImage,3);

    // -- Changing to CIELab space for better segmentation
    Mat cie;
    cvtColor(blurImage, cie, CV_BGR2Lab);

    // ********** Step 3: Superpixels ********** //

    Mat labels;
    int numSuperpixels;
    tie(labels, numSuperpixels) = slic_segmentation(cie);

    // -- build a map of neighbors -- //
    set < pair <int, int> > neighboring_labels;
    neighboring_labels = build_neighbors(labels);

    // ********** Step 4: Begin calculation to get the average pixel per label ********** //

    // -- creating a 2D bucket for storing x,y co-ord of superpixel centroid indexed by the number of superpixels -- //
    std::vector<cv::Vec2f> means(numSuperpixels);
    means = get_sp_centroids(labels, numSuperpixels);

    // -- create segmented image by replacing every pixel with the pixel value of the centroid of the superpixel -- //
    // -- to which the pixel belongs. -- //
    Mat segmented = Mat::zeros(src.size(), CV_8UC3);
    segmented = create_superpixel_img(labels, src, numSuperpixels);
    imshow("segmented", segmented);
    imwrite("segmented.jpg", segmented);

    // alternatively done using grabCut by Thejanie
    Mat masked = Mat::zeros(src.rows, src.cols,CV_8UC3);
    masked = create_image_mask(segmented,cie, labels,scribbleMask,means, neighboring_labels);
    cout << "\nHere we go "<< masked.rows << "  " << masked.cols;
    imwrite("masked.jpg", masked);
    return masked;

}

set < pair <int, int> > build_neighbors(Mat& labels){

    set < pair <int, int> > neighbor_labels;

    for(int row = 1; row < (labels.rows-1); row++){
        for(int col = 1; col < (labels.cols-1); col++){

            for(int i=-1; i<2; i++){
                for(int j=-1; j<2; j++){
                    if(labels.at<int>((row+i),(col+j)) != labels.at<int>(row,col)){
                        neighbor_labels.insert(make_pair(labels.at<int>(row,col),labels.at<int>((row+i),(col+j))));
                    }
                }
            }
        }
    }

    return neighbor_labels;
}

// checks for similarity of labels. based on reg growing algo
int check_similarity(int q_label, Scalar mean, Scalar stddev, vector<cv::Vec2f> means, Mat& segmented){

    double gaussianTau = pow(2,2);

    Vec3b q;

    int q_label_x = (int)ceil(means[q_label][1]);
    int q_label_y = (int)ceil(means[q_label][0]);

    q = segmented.at<Vec3b>(q_label_y,q_label_x);

    double similarity_index_b = pow(((q[0]-mean[0])/stddev[0]),2);
    double similarity_index_g = pow(((q[1]-mean[1])/stddev[1]),2);
    double similarity_index_r = pow(((q[2]-mean[2])/stddev[2]),2);

    if (((similarity_index_b - gaussianTau)<0) &&
        ((similarity_index_g - gaussianTau)<0) &&
        ((similarity_index_r - gaussianTau)<0)){
        return 0;
    }else{
        return 1;
    }
}

tuple <Mat , int > slic_segmentation(Mat& src){
    int numIterations = 20; // adjust
    int minElementSize = 20;  // adjust 80
    int regionSize = 10; // 30
    float smoothnessFactor = 10.0;  // 10.0

    Ptr<SuperpixelSLIC> slic = createSuperpixelSLIC(src,SLICO,regionSize,smoothnessFactor); // adjusting last 3 params

    slic->iterate(numIterations);

    if (minElementSize>0)
        slic->enforceLabelConnectivity(minElementSize);

    // -- if you need the mask superimposed on the orig image -- //
    Mat mask, superimposed;
    slic->getLabelContourMask(mask, true);
    superimposed.setTo(Scalar(0, 0, 255), mask);
    imwrite("label_mask.jpg", mask);

    // -- get # superpixels -- //
    int numSuperpixels = slic->getNumberOfSuperpixels();

    // -- get labels -- //
    Mat labels;
    slic->getLabels(labels);

    return make_tuple(labels, numSuperpixels);
}

vector< cv::Vec2f > get_sp_centroids(Mat& labels, int numSuperpixels){

    std::vector<cv::Vec2f> means(numSuperpixels);
    for (int k = 0; k < numSuperpixels; k++)
    {
        means[k] = cv::Vec2f(0, 0);
    }

    // -- keep a count of # pixels per superpixel label -- //
    std::vector<int> counts(numSuperpixels, 0); // This will help us take the avg

    // -- Sum y and x coordinates for each superpixel: -- //
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            means[labels.at<int>(i, j)][0] += i; // for computing mean i (i.e. row or y axis)
            means[labels.at<int>(i, j)][1] += j; // for computing the mean j (i.e. column or x axis)

            counts[labels.at<int>(i, j)]++;
        }
    }

    // -- Obtain average row and column index -- //
    for (int k = 0; k < numSuperpixels; ++k) {
        means[k] /= counts[k]; // at this point vector means contains the centroid for each superpixel
    }

    return means;
}

Mat create_superpixel_img(Mat& labels, Mat& src, int numSuperpixels){

    Mat segmented = Mat::zeros(src.size(), CV_8UC3);

    // -- get average color -- //
    unsigned int labelColor[numSuperpixels][4];
    for(int i = 0; i< numSuperpixels; i++){
        labelColor[i][0] = {0};
        labelColor[i][1] = {0};
        labelColor[i][2] = {0};
        labelColor[i][3] = {0};
    }

    for(int row =0; row < labels.rows; row++){
        for(int col = 0; col < labels.cols; col++){
            Vec3b p = src.at<Vec3b>(row, col);
            int label = labels.at<int>(row, col);
            labelColor[label][0] += 1;
            labelColor[label][1] += p[0]; // b
            labelColor[label][2] += p[1]; // g
            labelColor[label][3] += p[2]; // r
        }
    }

    // to get avg
    for(int i = 0; i< numSuperpixels; i++){
        labelColor[i][1] = labelColor[i][1]/labelColor[i][0];
        labelColor[i][2] = labelColor[i][2]/labelColor[i][0];
        labelColor[i][3] = labelColor[i][3]/labelColor[i][0];
    }

    for(int row= 0; row < src.rows; row++){
        for(int col=0; col< src.cols; col++){

            int label = labels.at<int>(row, col);
            Vec3b avgPix = Vec3b(labelColor[label][1],labelColor[label][2],labelColor[label][3]);

            segmented.at<Vec3b>(row, col)= avgPix;
        }
    }

    return segmented;
}

Mat create_image_mask(Mat& seg, Mat& cie, Mat& labels, Mat& scribbles,vector<cv::Vec2f> means, set<pair<int, int>> neighbor_labels){
    Mat masked = Mat::zeros(labels.size(), CV_8UC3);

    // get number of objects with pixel level
    // we need a vector of int and the size will be # objects to be masked
    set <int, greater <int> > imageObjects;
    imageObjects = get_img_objects(scribbles);
    cout << "\nNumber of objects are "<< imageObjects.size();

    // creating a cie version of segmented image to be used for similarity checking -- //
    Mat cie_seg;
    cvtColor(seg, cie_seg, CV_BGR2Lab);

    vector<Mat> masks;

    // loop over contents of imageObjects and create a mask for each object
    set <int, greater <int> > :: iterator itr;
    for (itr = imageObjects.begin(); itr != imageObjects.end(); ++itr)
    {
        int objectPixVal = *itr;
        cout << "\nCurrently handling" << objectPixVal;

        // function that returns a binary mask for object
        Mat binary_mask = Mat::zeros(labels.size(), CV_8UC1);
        binary_mask = get_object_mask(objectPixVal, labels, cie_seg, cie, scribbles, means, neighbor_labels);


        masks.emplace_back(binary_mask.clone());
    }

    Mat maskArr[3];
    // initialize.. to save the prog from failing if num obj < 3
    for(int k=0; k<3; k++){
        maskArr[k] = Mat::zeros(labels.size(), CV_8UC1);
    }
    int k = 0;
    for (auto i = masks.begin(); i != masks.end(); ++i) {
        maskArr[k] = *i;
        k+=1;
    }

    merge(maskArr, 3, masked);

    return masked;
}

// to get list of object intensities
set <int, greater <int> > get_img_objects(Mat& scribbles){

    set <int, greater <int> > imageObjects;

    for (int i = 0; i < scribbles.rows; i++) {
        for (int j = 0; j < scribbles.cols; j++) {
            int pixel = (int)scribbles.at<uchar>(i, j);
            if (pixel !=0){
                imageObjects.insert(pixel);
            }
        }
    }

    return imageObjects;
}

// returns individual obejct mask
Mat get_object_mask(int objectPixVal, Mat& labels, Mat& cie_seg, Mat& cie, Mat& scribbleMask, vector<cv::Vec2f> means, set<pair<int, int>> neighbor_labels){
    Mat binary_mask = Mat::zeros(labels.size(), CV_8UC1);

    // create containers for frontier and visited pixels (This later helps us in masking)
    vector <Point> object;  // frontier
    set <int, greater<int> > mask_labels_image;  // visited labels
    unsigned long int num_pix_obj = 0;

    // initialize the object frontier with scribbles pixels
    for(int row = 0; row < scribbleMask.rows; row++){
        for(int col = 0; col < scribbleMask.cols; col++){
            if((int)scribbleMask.at<uchar>(row,col) == objectPixVal){
                object.push_back(Point(col,row));
            }
        }
    }
    num_pix_obj = object.size();
    cout << "\nNum pix is " << num_pix_obj;

    // find mean and std dev of the scribbles - to be used for check similarity
    int row = 0;
    Mat object_pix = Mat::zeros(num_pix_obj, 1, CV_8UC3);
    for (auto i = object.begin(); i != object.end(); ++i)
    {
        Point p = *i;
        object_pix.at<Vec3b>(row,1) = cie.at<Vec3b>(p.y, p.x);
        row++;
    }
    Scalar mean, stddev;
    meanStdDev(object_pix,mean, stddev);
    cout << "\nObject Channel 0: Mean is "<< mean[0] << " stddev is " << stddev[0];
    cout << "\nObject Channel 1: Mean is "<< mean[1] << " stddev is " << stddev[1];
    cout << "\nObject Channel 2: Mean is "<< mean[2] << " stddev is " << stddev[2];

    // process the frontier to create a list of pixels belonging to the mask
    // the initial values in the frontier are pixels. later, we only insert label centroids
    // to speed up the process
    cout << "\nObject size before loop processing starts is "<< object.size();
    for (auto curr_frontier_pix = object.begin(); curr_frontier_pix != object.end(); ++curr_frontier_pix)
    {
        Point p = *curr_frontier_pix; // we have the seed x, y

        // I don't know why the bug but this is my quick albeit an inelegant fix
        // The bug happens only for dog. It gets hold of some nasty x,y values from the object1 vector
        if(((p.y>=labels.rows) || (p.y <= 0)) && ((p.x>=labels.cols) || (p.x <= 0))){
            break;
        }

        int superpix = labels.at<int>(p.y, p.x); //and superpix label
        mask_labels_image.insert(superpix); // the current frontier most certainly belongs to mask

        // get the neighbour, check similarity
        for(auto it= neighbor_labels.begin(); it!= neighbor_labels.end(); ++it)
        {
            // since we treat (p,q) and (q,p) seperate, we need the if else
            if (it->first == superpix){
                int result = check_similarity(it->second,mean, stddev, means, cie_seg);
                if (result == 0){
                    mask_labels_image.insert(it->second);
                    // I have to push the centroid; using floow to ensure that mask is compact
                    int new_x = (int)floor(means[it->second][1]);
                    int new_y = (int)floor(means[it->second][0]);
                    // first check if the label exists
                    if (std::find(object.begin(), object.end(),Point(new_x, new_y))==object.end()){
                        object.emplace_back(Point(new_x, new_y));
                    }
                }
            }else if(it->second == superpix){
                int result = check_similarity(it->first,mean, stddev, means, cie_seg);
                if (result == 0){
                    mask_labels_image.insert(it->first);
                    // I have to push the centroid
                    int new_x = (int)floor(means[it->first][1]);
                    int new_y = (int)floor(means[it->first][0]);
                    // first check if the label exists
                    if (std::find(object.begin(), object.end(),Point(new_x, new_y))==object.end()){
                        object.emplace_back(Point(new_x, new_y));
                    }
                }
            } // end of if else if
        }  // end of neighbor per frontier pixel
    }  // end of frontier

    // should be made better
    // -- masking -- //
    //Vec3b m(0,0,255);
    for(auto it=mask_labels_image.begin(); it!=mask_labels_image.end(); ++it){
        // get x,y from labels whose labels are in the mask
        for(int row= 0; row < labels.rows; row++){
            for(int col=0; col < labels.cols; col++) {

                if (labels.at<int>(row, col) == *it) {
                    binary_mask.at<uchar>(row, col) = (uchar)objectPixVal;
                }
            }
        }
        //imshow("maskOutput", binary_mask);
        //waitKey(30);
    }
    return binary_mask;
}