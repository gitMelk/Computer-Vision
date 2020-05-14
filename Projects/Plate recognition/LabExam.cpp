#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ExamClass.h"
#include "ExamClassPlates.h"
#include <string>
#include <opencv2/ml/ml.hpp>

#define CARNUM  7

using namespace cv;
using namespace std;

void printChar(vector<Mat> vectorOfChar);

int main()
{
	vector<PlateDetection> datasetCars;
	vector<CharFinderInPlates> plates;
	//load the dataset
	for (int i = 0; i < CARNUM; i++)
	{
		datasetCars.push_back(imread(".\\images_T1\\" + to_string(i + 1) + ".jpg"));
	}
	
	//preprocessing
	for (int i = 0; i < CARNUM; i++)
	{
		datasetCars[i].preprocessing();
	}
	//main loop	
	for (int i = 0; i < CARNUM; i++)
	{
		// find the plate
		datasetCars[i].retriveMy2Plates();
		
		Mat rawImage = datasetCars[i].plate;
				
		plates.push_back(CharFinderInPlates(rawImage));
		// preprocess plate
		plates[i].getPreprocessedPlates();
		// try to get single chat
		vector<Mat> possibleCharVec = plates[i].getSingleChar();
		//find the char
		printChar(possibleCharVec);

	}

	return 0;

}

void printChar(vector<Mat> vectorOfChar)
{
	// load the classifications file
	FileStorage fsClassifications("classifications.xml", FileStorage::READ);       

	// vector containing the classification number
	Mat matClassificationInts;     
	fsClassifications["classifications"] >> matClassificationInts;     
	// close the file
	fsClassifications.release();                                      
	
	// load the training images file
	FileStorage fsTrainingImages("images.xml", FileStorage::READ);          
	// multiple images into a single vector
	Mat matTrainingImagesAsFlattenedFloats;         
	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;     
	fsTrainingImages.release();                                                

	 // instantiate the KNN object
	Ptr<ml::KNearest>  kNearest(ml::KNearest::create());           
	kNearest->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);


	string myPLate;
	int resized_width = 20;
	int resized_height = 30;

	for (int i = 0; i < vectorOfChar.size(); i++) {
		Mat charsInPlateResized;
		resize(vectorOfChar[i], charsInPlateResized, Size(resized_width, resized_height));

		Mat charsInPlateFloat;
		// convert Mat to float, required for find_nearest
		charsInPlateResized.convertTo(charsInPlateFloat, CV_32FC1);             
		Mat charsInPlateFlattenedFloat = charsInPlateFloat.reshape(1, 1);
		Mat matCurrentChar(0, 0, CV_32F);

		// finally, use the classifier 
		kNearest->findNearest(charsInPlateFlattenedFloat, 1, matCurrentChar); 

		//concat the chat in a single string
		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
		myPLate = myPLate + char(int(fltCurrentChar));
	}
	cout << "\n\n" << "numbers read = " << myPLate << "\n\n";

}