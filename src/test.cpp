
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;
    namedWindow("edges",1);
    namedWindow("original",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_BGR2GRAY);
       // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
       // Canny(edges, edges, 0, 30, 3);
    //   setImageROI(frame, cvRect(165,24,245,176));
    //  cv::Mat subImg = frame(cv::Range(165,24), cv::Range(245, 176));
      cv::Mat subImg = edges(Rect(165,25,245,176));
      
	//cvResetImageROI(frame);
	//img = cvCloneImage(tmp);
        Mat test;
        threshold( subImg, test, 40, 255,0 );
        imshow("edges", test);
	imshow("original",subImg);
	imwrite("test.jpg",subImg);
	
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}