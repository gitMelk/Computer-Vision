#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ObjDect.h"
#include <string>

using namespace cv;
using namespace std;

vector<Mat> getImage(vector<Mat>, vector<Mat>);
void showImage(vector<Mat>, int);

int main()
{
	
	vector<Mat> objCereal;
	vector<Mat> sceneCereal;
	vector<Mat> objRandom;
	vector<Mat> sceneRandom;
	vector<Mat> objCars;
	vector<Mat> sceneCars;
	vector<Mat> objBears;
	vector<Mat> sceneBears;

	// load all the images
	for (int i = 0; i < 3; i++)
	{
		objRandom.push_back(imread(".\\Dataset 1\\obj" + to_string(i + 1) + ".jpg"));
		sceneRandom.push_back(imread(".\\Dataset 1\\scene" + to_string(i + 1) + ".jpg"));
		objCereal.push_back(imread(".\\Dataset 2\\obj" + to_string(i + 1) + ".png"));
		objCars.push_back(imread(".\\Dataset 3\\obj" + to_string(i + 1) + ".png"));
		objBears.push_back(imread(".\\Dataset 4\\obj" + to_string(i + 1) + ".png"));
		sceneBears.push_back(imread(".\\Dataset 4\\scene" + to_string(i + 1) + ".png"));
		if (i == 2)
		{
			objBears.push_back(imread(".\\Dataset 4\\obj" + to_string(i + 2) + ".png"));
			sceneBears.push_back(imread(".\\Dataset 4\\scene" + to_string(i + 2) + ".png"));
			sceneRandom.push_back(imread(".\\Dataset 1\\scene" + to_string(i + 2) + ".jpg"));
			sceneRandom.push_back(imread(".\\Dataset 1\\scene" + to_string(i + 3) + ".jpg"));
			sceneCereal.push_back(imread(".\\Dataset 2\\scene" + to_string(i - 1) + ".png"));
			sceneCars.push_back(imread(".\\Dataset 3\\scene" + to_string(i - 1) + ".png"));
			sceneCars.push_back(imread(".\\Dataset 3\\scene" + to_string(i) + ".png"));
		}
	}

	// create a vector with the results to show
	vector<vector<Mat>> resultsAll;
	resultsAll.push_back(getImage(objRandom, sceneRandom));
	resultsAll.push_back(getImage(objCereal, sceneCereal));
	resultsAll.push_back(getImage(objCars, sceneCars));
	resultsAll.push_back(getImage(objBears, sceneBears));

	//show the images
	for (int i = 0; i < resultsAll.size(); i++)
	{
		showImage(resultsAll[i], i);
	}

	return 0;
}

// for every object, check every scene
vector<Mat> getImage(vector<Mat> objs, vector<Mat> scenes)
{
	vector<Mat> resultsTMP;
	do {
		Mat sceneTmp = scenes[scenes.size() - 1].clone();
		scenes.pop_back();
		for (int i = 0; i < objs.size(); i++)
		{
			ObjectDetection homeWork(objs[i], sceneTmp);
			resultsTMP.push_back(homeWork.getResults());
		}

	} while (scenes.size() != 0);

	return resultsTMP;
}

// simple show/save image function
void showImage(vector<Mat> results, int index)
{
	index++;
	int k = 0;
	while (results.size() != 0)
	{
		// show the images
		namedWindow("showImage", WINDOW_NORMAL);
		imshow("showImage", results[results.size() - 1]);
		waitKey(0);
		// save the images
		// imwrite(".\\resultsTest\\result" + to_string(k + index * 1000) + ".png", results[results.size()-1]);
		k++;

		results.pop_back();
	}
}