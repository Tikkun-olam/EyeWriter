/**
* Program to detect pupil, based on
* http://www.codeproject.com/Articles/137623/Pupil-or-Eyeball-Detection-and-Extraction-by-C-fro
* with some improvements.
*/
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <ncurses.h>

extern "C"{
  #include <xdo.h>
}
using namespace std;
using namespace cv;

int was  = 0;
int count2 = 0;
int isClosed=0;
clock_t timeClosed = 0;
int isOn = 0;
int main(int argc, char** argv)

{
// Load image

  VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;
    namedWindow("edges",1);
    namedWindow("original",1);
    
    for(; ;) {
      cv::Mat tmp;
      cap >> tmp;
      cv::Mat src = tmp(Rect(153,70,(530-153 ),(270-70)));
      
      if (src.empty())
      return -1;

// Invert the source image and convert to grayscale
cv::Mat gray;
cv::cvtColor(~src, gray, CV_BGR2GRAY);
cv::imshow("negative", ~src);
//cv::waitKey(0);
// Convert to binary image by thresholding it
cv::threshold(gray, gray, 170, 255, cv::THRESH_BINARY);
cv::imshow("threshold", gray);
//cv::waitKey(0);
// Find all contours
 int erosion_size = 4; 
Mat element = getStructuringElement(cv::MORPH_CROSS,
              cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
              cv::Point(erosion_size, erosion_size) );
//erode(gray,gray,element);
//erode(gray,gray,element);
erode(gray,gray,element);

dilate(gray,gray,element);
dilate(gray,gray,element);
dilate(gray,gray,element);
cv::imshow("erode",gray);
vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    findContours( gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    /// Draw contours
  /*  Mat drawing = Mat::zeros( gray.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }    
 
    imshow( "Result window", drawing );
    waitKey(0);    */                       
    

//std::vector<std::vector<cv::Point> > contours;
//cv::findContours(gray.clone(), contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

    
// Fill holes in each contour
  Scalar color( rand()&255, rand()&255, rand()&255 );
//cv::drawContours(gray, contours, -1, CV_RGB(255,255,255), -1);
//cv::imshow("contour", gray);

//cv::waitKey(0);
//cout << "Contours: " << contours.size() << endl;



 Mat drawing = Mat::zeros( gray.size(), CV_8UC3 );


/*
  for( int i = 0; i< contours.size(); i++ )
     {
       if( contours[i].size() > 5 ) {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       // contour
       drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       // ellipse
       ellipse( drawing, fitEllipse( Mat(contours[i]) ), color, 2, 8 );
       }
       // rotated rectangle
 //      Point2f rect_points[4]; minRect[i].points( rect_points );

     }*/
imshow("new",drawing);
  //   waitKey(0);
int x=0,y;
int maxArea = 0;
int maxAreaIndex = -1;
int maxCenterX=0;
for (int i = 0; i < contours.size(); i++)
{
    double area = cv::contourArea(contours[i]);
   if( contours[i].size() > 5 ) {
      RotatedRect m = fitEllipse( Mat(contours[i]) );
     // if(m.center.x > 50 && ( m.center.y < 160 && m.center.y > 40)) {
      
	//if (area >= 5 && m.center.x > x)
	  if (area >= 5 && area > maxArea)
	  
	{
	  x = m.center.x;
	  y = m.center.y;
	maxAreaIndex = i;
	maxArea = area;
      }
      
      }
  // }
}

if(maxAreaIndex != -1) {
    double area = cv::contourArea(contours[maxAreaIndex]);
    cv::Rect rect = cv::boundingRect(contours[maxAreaIndex]);
    int radius = rect.width/2;
  //  cout << "Printing a circle " << endl;
    cv::circle(src, cv::Point(rect.x + radius, rect.y + radius), radius, CV_RGB(255,0,0), 2);
    if(isClosed) {
      isClosed = 0;
      printf("Been closed for: %lf  - %ld   - %ld   - %ld\n",(clock()-timeClosed)/(double)CLOCKS_PER_SEC*10 ,timeClosed,clock(),CLOCKS_PER_SEC);
      if((clock()-timeClosed)/(double)CLOCKS_PER_SEC*10.0 >=3 && (clock()-timeClosed)/(double)CLOCKS_PER_SEC*10.0  < 15) {
	isOn = !isOn;
	//cout << "Weehoooooooooooo" << endl;
	//cout << '\a';
	beep();
      }
      
      if(isOn && (clock()-timeClosed)/(double)CLOCKS_PER_SEC*10.0 >=0.5 && (clock()-timeClosed)/(double)CLOCKS_PER_SEC*10.0  < 1.5) {
	printf("Sending return\n\n");
	//isOn = !isOn;
	xdo_t *xdo = xdo_new(NULL);
	  //char u[2] = "A"
	  //xdo_keysequence(xdo, CURRENTWINDOW, "D", 0);
	  xdo_keysequence_down(xdo, CURRENTWINDOW,
			"KP_Enter", 0);
	  xdo_keysequence_up(xdo, CURRENTWINDOW,
			"KP_Enter", 0);
      }
      
     // KP_Enter
    }
    if(isOn && was == 0) {
      
      if(x > 250) {
	  xdo_t *xdo = xdo_new(NULL);
	  //char u[2] = "A"
	  //xdo_keysequence(xdo, CURRENTWINDOW, "D", 0);
	  xdo_keysequence_down(xdo, CURRENTWINDOW,
			"Left", 0);
	  xdo_keysequence_up(xdo, CURRENTWINDOW,
			"Left", 0);
	  was = 1;
      }
      else if(x <115) {
	  //char u[2] ="D"
	  xdo_t *xdo = xdo_new(NULL);
	  //char u[2] = "A"
	  //xdo_keysequence(xdo, CURRENTWINDOW, "D", 0);
	  xdo_keysequence_down(xdo, CURRENTWINDOW,
			"Right", 0);
	  xdo_keysequence_up(xdo, CURRENTWINDOW,
			"Right", 0);
	  was = 1;
      } else if(y < 18) {
	  //char u[2] ="D"
	  xdo_t *xdo = xdo_new(NULL);
	  //char u[2] = "A"
	  //xdo_keysequence(xdo, CURRENTWINDOW, "D", 0);
	  xdo_keysequence_down(xdo, CURRENTWINDOW,
			"Up", 0);
	  xdo_keysequence_up(xdo, CURRENTWINDOW,
			"Up", 0);
	  was = 1;
      } else if(y >100) {
	  //char u[2] ="D"
	  xdo_t *xdo = xdo_new(NULL);
	  //char u[2] = "A"
	  //xdo_keysequence(xdo, CURRENTWINDOW, "D", 0);
	  xdo_keysequence_down(xdo, CURRENTWINDOW,
			"Down", 0);
	  xdo_keysequence_up(xdo, CURRENTWINDOW,
			"Down", 0);
	  was = 1;
      } 
    } else {
      
      count2++;
      if(count2> 5) {
	count2 =0;
	was = 0;
      }
    }
    
} else {
  if(!isClosed) {
    printf("NOW CLOSING\n\n");
    isClosed = 1;
    timeClosed = clock();
  } 
    
  
}
cv::imshow("image", src);
cv::waitKey(10);
    }
return 0;
}

