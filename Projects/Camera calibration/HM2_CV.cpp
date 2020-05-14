#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
// #include <opencv2/core/core.hpp>
#define NUM_OF_PICS 5 // number of pics in the folder

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	vector<Mat> images(NUM_OF_PICS);
	vector<Mat> gray_images(NUM_OF_PICS);
	string test;
	
	/* This lines of code are needed for testing with another set of images
	Size patternsize(12, 8); //interior number of corners
	Size winSize(12 * 2 + 1, 8 * 2 + 1);
	*/
	
	// Initialization of the checkerboards details 
	Size patternsize(6, 5); 
	Size winSize(6 * 2 + 1, 5 * 2 + 1);
	vector<vector<Point2f>> corners(NUM_OF_PICS); 
	vector<vector<Point3f>> objectPoints(NUM_OF_PICS);

	for (int i = 0; i < images.size(); i++)
	{
		// load the photos
		images[i] = imread(".\\photos\\IAS_LAB\\00" + to_string(i + 1) + "_color.png");

		cvtColor(images[i], gray_images[i], COLOR_BGR2GRAY);
		bool patternfound = findChessboardCorners(images[i], patternsize, corners[i], CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

		// only if the pattern has been found i can use the following functions
		if (patternfound)
		{
			// conernerSubPix requires b&w images
			cornerSubPix(gray_images[i], corners[i], winSize, Size(-1, -1),	TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(images[i], patternsize, Mat(corners[i]), patternfound);
			
			/* Draw checkerboard - DEBUG
			//namedWindow("Image " + to_string(i+1), WINDOW_NORMAL);
			//imshow("Image " + to_string(i+1), images[i]);
			//waitKey(0);
			*/
			
			// simple check 
			cout << "Found checkerboard in img" << i + 1 << " \n";
		}
		for (int k = 0; k < patternsize.height; k++)
			for (int j = 0; j < patternsize.width; j++)	
			{
				objectPoints[i].push_back({ (float)j, (float)k, 0.0f });
			}
	}

	
	Mat cameraMatrix = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;
	vector<double> stdDeviationsIntrinsics;
	vector<double> stdDeviationsExtrinsics;
	vector<double>  perViewErrors;
	TermCriteria criteria;
	int flags = 0;
	
	// Compute the mean error
	double error = calibrateCamera(objectPoints, corners, images[0].size(), cameraMatrix, distCoeffs, rvecs, tvecs,
		stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors, flags, criteria);
	
	// Find min and man error
	int maxElementIndex = max_element(perViewErrors.begin(), perViewErrors.end()) - perViewErrors.begin();
	int minElementIndex = min_element(perViewErrors.begin(), perViewErrors.end()) - perViewErrors.begin();

	cout << "The mean error is " << error << "\n";
	cout << "The max error is given by img" << maxElementIndex + 1 << " and its value is: " << perViewErrors[maxElementIndex] <<"\n";
	cout << "The min error is given by img" << minElementIndex + 1 << " and its value is: " << perViewErrors[minElementIndex] << "\n";
	
	Mat R = Mat::eye(3, 3, CV_32FC1);

	// Optionsal: try to get the undistorted image
	Mat newCameraMatrix = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, images[0].size(),	0.35, images[0].size(), 0, true);
	Mat map1;
	Mat map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), newCameraMatrix, images[0].size(), CV_32FC1, map1, map2);
	
	Mat src = imread(".\\photos\\IAS_LAB\\test.png");
	Mat dst;
	remap(src, dst, map1, map2, INTER_LINEAR, BORDER_CONSTANT, Scalar());
	
	// Final print of the results
	namedWindow("Original", WINDOW_NORMAL);
	imshow("Original", src);
	waitKey(0);
	namedWindow("Undistorted", WINDOW_NORMAL);
	imshow("Undistorted", dst);
	waitKey(0);
}