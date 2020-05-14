#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "ObjDect.h"

using namespace cv;
using namespace std;


ObjectDetection::ObjectDetection(Mat singleImagesColors, Mat scenesImagesColors)
{
	Mat singleImages;
	Mat scenesImages;

 	cvtColor(singleImagesColors, singleImages, COLOR_BGR2GRAY);
	cvtColor(scenesImagesColors, scenesImages, COLOR_BGR2GRAY);
	
	Mat descriptorObjTmp;
	vector<KeyPoint> keypointsObjTmp;
	Mat descriptorScenesTmp;
	detectorORB->detectAndCompute(scenesImages, Mat(), keypointsScenes, descriptorScenes);
	detectorORB->detectAndCompute(singleImages, Mat(), keypointsObj, descriptorObj);
		
	Ptr<BFMatcher> matcherBF = BFMatcher::create(NORM_HAMMING, false);
	matcherBF->match(descriptorObj, descriptorScenes, matches, Mat());
	
	// ##### OPTIMIZATION #########
	// Sort matches by distance
	sort(matches.begin(), matches.end());
	// Remove matches 
	const int numGoodMatches = matches.size() * 0.10f;
	matches.erase(matches.begin() + numGoodMatches, matches.end());

	vector<Point2f> pointsTrain;
	vector<Point2f> pointsQuery;

	for (size_t j = 0; j < matches.size(); j++)
	{
		vector<DMatch> matchesTmp = matches;
		vector<KeyPoint> keypointsObjTmp = keypointsObj;
		vector<KeyPoint> keypointsSceneTmp = keypointsScenes;
		pointsTrain.push_back(keypointsObjTmp[matchesTmp[j].queryIdx].pt);
		pointsQuery.push_back(keypointsSceneTmp[matchesTmp[j].trainIdx].pt);
	}
					
	Mat H = findHomography(pointsTrain, pointsQuery, RANSAC);
	Mat imgMatches;

	
	drawMatches(singleImages, keypointsObj, scenesImages, keypointsScenes, matches, imgMatches);
	vector<Point2f> cornersObj(4);
	cornersObj[0] = Point2f(0, 0);
	cornersObj[1] = Point2f((float)singleImages.cols, 0);
	cornersObj[2] = Point2f((float)singleImages.cols, (float)singleImages.rows);
	cornersObj[3] = Point2f(0, (float)singleImages.rows);
	vector<Point2f> sceneCorners(4);
	perspectiveTransform(cornersObj, sceneCorners, H);

	int rowsMax = scenesImagesColors.rows;
	if (scenesImagesColors.rows < singleImagesColors.rows)
	{
		rowsMax = singleImagesColors.rows;
	}

	Mat finalImageTmp = Mat::zeros(rowsMax, scenesImagesColors.cols + singleImagesColors.cols, singleImagesColors.type());
	singleImagesColors.copyTo(finalImageTmp(Rect(0, 0, singleImagesColors.cols, singleImagesColors.rows)));
	scenesImagesColors.copyTo(finalImageTmp(Rect(singleImagesColors.cols, 0, scenesImages.cols, scenesImagesColors.rows)));

	line(finalImageTmp, sceneCorners[0] + Point2f((float)singleImages.cols, 0),	sceneCorners[1] + Point2f((float)singleImages.cols, 0), Scalar(0, 255, 0), 4);
	line(finalImageTmp, sceneCorners[1] + Point2f((float)singleImages.cols, 0),	sceneCorners[2] + Point2f((float)singleImages.cols, 0), Scalar(0, 255, 0), 4);
	line(finalImageTmp, sceneCorners[2] + Point2f((float)singleImages.cols, 0),	sceneCorners[3] + Point2f((float)singleImages.cols, 0), Scalar(0, 255, 0), 4);
	line(finalImageTmp, sceneCorners[3] + Point2f((float)singleImages.cols, 0),	sceneCorners[0] + Point2f((float)singleImages.cols, 0), Scalar(0, 255, 0), 4);
	finalImage = finalImageTmp;
}


Mat ObjectDetection::getResults()
{
	return finalImage;
}
