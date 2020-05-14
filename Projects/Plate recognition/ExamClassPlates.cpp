#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <string>
#include "ExamClassPlates.h"

using namespace cv;
using namespace std;


CharFinderInPlates::CharFinderInPlates(Mat input)
{
	rawPlate = input;
	
}

void CharFinderInPlates::getPreprocessedPlates()
{
	Mat image = rawPlate;

	cvtColor(image, image, COLOR_BGR2GRAY);  // Without equalization

	image =  maximizeContrast(image);

	// Gaussian Blur
	Mat imageBLURRED;
	Size GAUSSIAN_SMOOTH_FILTER_SIZE = Size(5, 5);
	GaussianBlur(image, imageBLURRED, GAUSSIAN_SMOOTH_FILTER_SIZE, 0);

	// Binary image
	Mat imageBINARY;
	threshold(imageBLURRED, imageBINARY, 135, 255, THRESH_BINARY);

	//// Thresholding
	Mat imageTHRESH;

	adaptiveThreshold(imageBLURRED, imageTHRESH, 255.0, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 19, 9);


	preProcPlate = imageTHRESH;

	//imwrite(".\\testPlateLittle.png", preProcPlate);

}

Mat CharFinderInPlates::maximizeContrast(Mat imgGrayscale)
{

	Mat imgTopHat;
	Mat imgBlackHat;
	Mat imgGrayscalePlusTopHat;
	Mat imgGrayscalePlusTopHatMinusBlackHat;

	Mat structuringElement = cv::getStructuringElement(MORPH_RECT, cv::Size(3, 3));

	morphologyEx(imgGrayscale, imgTopHat, MORPH_TOPHAT, structuringElement);
	morphologyEx(imgGrayscale, imgBlackHat, MORPH_BLACKHAT, structuringElement);

	imgGrayscalePlusTopHat = imgGrayscale + imgTopHat;
	imgGrayscalePlusTopHatMinusBlackHat = imgGrayscalePlusTopHat - imgBlackHat;

	return(imgGrayscalePlusTopHatMinusBlackHat);
}

vector<Mat> CharFinderInPlates::getSingleChar()
{

	vector<Mat> croppedListTmp;


	Mat preProcPlateTemp = preProcPlate.clone();
	vector<Mat> singleChars;
	Mat edgesImage;
	int thresholdTrackThresh1 = 150;
	Canny(preProcPlateTemp, edgesImage, thresholdTrackThresh1, thresholdTrackThresh1 * 2.5, 3, false);

	vector<vector<Point>> contoursTmp;
	vector<Vec4i> hierarchy2;
	findContours(edgesImage, contoursTmp, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);
	Mat rawTmp = rawPlate;
	RNG rng(12345);
	vector<int> areas;
	vector<Point> xPositions;



	for (size_t i = 0; i < contoursTmp.size(); i++)
	{
		rawTmp = rawPlate.clone();

		// Calculates the bounding rect of the largest area contour
		Rect rect = boundingRect(contoursTmp[i]);
		Point pt1, pt2;
		pt1.x = rect.x;
		pt1.y = rect.y;
		pt2.x = rect.x + rect.width;
		pt2.y = rect.y + rect.height;
		// Draws the rect in the original image and show it
		rectangle(rawTmp, pt1, pt2, CV_RGB(255, 0, 0), FILLED);
		
		Mat pinkImage = rawTmp.clone();
		Scalar red(0, 0, 255);
		Mat mask2;
		inRange(pinkImage, red, red, mask2);
		
		Mat plate;
	
		plate = preProcPlate(boundingRect(mask2));
  
		if (plate.rows > plate.cols 
			&& plate.rows > (rawTmp.rows) / 2  
			&& plate.cols < rawTmp.cols / 5)
		{
			croppedListTmp.push_back(plate);
			areas.push_back(plate.cols * plate.rows);
			bool guard = true;
			
			for (int row = 0; row < pinkImage.rows; ++row)
			{
				if (guard)
				{
					for (int col = 0; col < pinkImage.cols; ++col)
					{
						Vec3b& current_color = pinkImage.at<Vec3b>(row, col);
						if (current_color[0] == 0
							&& current_color[1] == 0
							&& current_color[2] == 255)
						{
							xPositions.push_back(Point(col, i));
							guard = false;
							break;
						}
					}
				}
			}
		}		
	}

	int  meanArea = 0;

	for (int j = 0; j < croppedListTmp.size(); j++)
	{	
		meanArea = meanArea + areas[j]/ areas.size();
	}

	vector<Point> xPositionsRefined;
	vector<Mat> reifnedCharFinal;

	int position = 0;
	Mat plateTmp = croppedListTmp[0].clone();
	for (int j = 0; j < croppedListTmp.size(); j++)
	{
		plateTmp = croppedListTmp[j].clone();

		if (areas[j] > meanArea -220 && areas[j] < meanArea + 220)
		{
			//croppedListRefinedTmp.insert(croppedListRefinedTmp.end(), plate);
			xPositionsRefined.push_back(Point(xPositions[j].x, position));
			reifnedCharFinal.push_back(plateTmp);
			position++;

		}
	}

	int i, j;
	for (i = 0; i < xPositionsRefined.size() - 1; i++)
	{
		// Last i elements are already in place    
		for (j = 0; j < xPositionsRefined.size() - i - 1; j++)
		{
			if (xPositionsRefined[j].x > xPositionsRefined[j+1].x)
			{
				Point temp = xPositionsRefined[j];
				xPositionsRefined[j] = xPositionsRefined[j + 1];
				xPositionsRefined[j + 1] = temp;
			}
		}
	}

	vector<Mat> rerinedRefinedFinal;

	int guardInt = xPositionsRefined[0].x;
	rerinedRefinedFinal.push_back(reifnedCharFinal[xPositionsRefined[0].y]);

	namedWindow("test" + to_string(0), WINDOW_FREERATIO);
	imshow("test" + to_string(0), reifnedCharFinal[xPositionsRefined[0].y]);
	//imwrite(".\\temp\\" + to_string(0) + ".png", rerinedRefinedFinal[0]);

	for (int j = 1; j < xPositionsRefined.size(); j++)
	{
		guardInt = xPositionsRefined[j - 1].x;

		if(xPositionsRefined[j].x > guardInt + 4 && rerinedRefinedFinal.size() < 7)
		{
			namedWindow("test" + to_string(j), WINDOW_FREERATIO);
			imshow("test" + to_string(j), reifnedCharFinal[xPositionsRefined[j].y]);
			rerinedRefinedFinal.push_back(reifnedCharFinal[xPositionsRefined[j].y]);
			//imwrite(".\\temp\\" + to_string(rerinedRefinedFinal.size() - 1) + ".png", rerinedRefinedFinal[rerinedRefinedFinal.size()-1]);
		}
		
		
	}
	waitKey(0);
	destroyAllWindows();

	
	return rerinedRefinedFinal;
}


