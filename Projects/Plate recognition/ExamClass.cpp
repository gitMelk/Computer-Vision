#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <string>
#include "ExamClass.h"

using namespace cv;
using namespace std;

PlateDetection::PlateDetection(Mat input)
{
	rawImage = input;
	
}

void PlateDetection::preprocessing()
{
	Mat finalImageTmp;
	//GaussianBlur(rawImage, finalImageTmp, cv::Size(3, 3), 1, 1, 4);
	bilateralFilter(rawImage, finalImageTmp, 10, static_cast<double>(3),
		static_cast<double>(3), 4);

	// Lab color space
	Mat imageColorLab;   //destination array
	cvtColor(finalImageTmp, imageColorLab, COLOR_BGR2Lab);
	Mat splitImageLab[3];
	split(imageColorLab, splitImageLab); //split source  
	Mat equalizedSplitImageLab[3];
	Mat histEqLab[3];
	Mat finalImageLab;
	equalizedSplitImageLab[2] = splitImageLab[2];
	equalizedSplitImageLab[1] = splitImageLab[1];
	equalizeHist(splitImageLab[0], equalizedSplitImageLab[0]);
	merge(equalizedSplitImageLab, 3, finalImageLab);
	Mat finalImageLabBGR;
	cvtColor(finalImageLab, finalImageLabBGR, COLOR_Lab2BGR);
	
	preprocessed = finalImageLabBGR;


	Mat canny_output;
	Mat src_gray;
	// random number generator, to have nice colors
	RNG rng(12345);
	blur(preprocessed, src_gray, Size(3, 3));
	int treshddd = 191;
	Canny(src_gray, canny_output, treshddd, treshddd * 2);

	
	vector<Vec4i> hierarchy;
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t k = 0; k < contours.size(); k++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contours, (int)k, color, 2, LINE_8, hierarchy, 2);
	}

	erode(drawing, drawing, Mat(), Point(-1, -1), 1, BORDER_CONSTANT, 1);
		
}


Mat PlateDetection::getResultsPreprocessedCanny()
{
	return preprocessedCanny;
}


Mat PlateDetection::getResultsPreprocessed()
{
	return preprocessed;
}


void PlateDetection::retriveMy2Plates()
{
	
	Mat original = rawImage;
	Mat plateComplex;
	Mat plateSimple;
	vector<int> guard;
	int imax = 0;
	int imaxcontour = -1;


	for (int i = 0; i < contours.size(); i++) {
		int itmp = contourArea(contours[i]);
		if (imaxcontour < itmp)
		{
			if (!(find(guard.begin(), guard.end(), i) != guard.end()))
			{
				imax = i;
				imaxcontour = itmp;
			}
		}
	}

	plateSimple = tryGiveMeAPlate(original, contours[imax]);


	vector<vector<Point>> contoursAgain;
	Mat src = drawing;
	Mat src_hsv;
	Mat bin;
	Mat src_h;

	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	vector<Mat> rgb_planes;
	split(src_hsv, rgb_planes);
	src_h = rgb_planes[0]; 

	threshold(src_h, bin, 100, 255, THRESH_OTSU);

	findContours(bin, contoursAgain, RETR_LIST, CHAIN_APPROX_SIMPLE);
	
	for (int i = 0; i < contoursAgain.size(); i++) {
		int itmp = contourArea(contoursAgain[i]);
		if (imaxcontour < itmp)
		{
			if (!(find(guard.begin(), guard.end(), i) != guard.end()))
			{
				imax = i;
				imaxcontour = itmp;
			}
		}
	}

	plateComplex = tryGiveMeAPlate(original, contoursAgain[imax]);
	guard.push_back(imax);

	if (plateSimple.cols > plateComplex.cols)
	{
		plate = plateSimple;
	}
	else
	{
		plate = plateComplex;
	}
		
	
	if (plate.cols < 110)
	{
		int method = INTER_LANCZOS4;
		resize(plate, plate, Size(plate.cols*3, plate.rows*3), 0, 0, method);
	}
	

}


Mat PlateDetection::tryGiveMeAPlate(Mat original, vector<Point> checkTheseContours)
{

	Mat rawImg = original;
	bool response = false;
	vector<vector<Point> > controusTmp;
	controusTmp.push_back(checkTheseContours);

	cvtColor(original, original, COLOR_BGR2GRAY);
	cvtColor(original, original, COLOR_GRAY2BGR);
	drawContours(original, controusTmp, 0, Scalar(0, 0, 255), FILLED);

	Mat mask = getMask(original);
	Mat plate = getPlate(mask, rawImg);


	return plate;
}

Mat PlateDetection::getMask(Mat original)
{
	Mat pinkImage = original.clone();
	Scalar red(0, 0, 255);
	Mat mask2;
	inRange(pinkImage, red, red, mask2);

	erode(mask2, mask2, Mat(), Point(-1, -1), 3, BORDER_CONSTANT, 1);
	
	
	return mask2;
}

Mat PlateDetection::getPlate(Mat mask2, Mat rawImg)
{
	Mat colorRaw = rawImg;
	//Mat temp;
	//Mat plate;

	//cvtColor(rawImg, rawImg, COLOR_BGR2GRAY);
	//bitwise_and(rawImg, mask2, temp);

	plate = colorRaw(boundingRect(mask2));

	return plate;
}
