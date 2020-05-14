#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"


class ObjectDetection{

public:

	ObjectDetection(cv::Mat, cv::Mat);
	cv::Mat getResults();

protected:
	int numPic;
	int numScene;
	cv::Mat finalImage;
	cv::Ptr<cv::ORB> detectorORB = cv::ORB::create(5000, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, 31, 20);
	
	cv::Mat descriptorObj;
	cv::Mat descriptorScenes;
	
	std::vector<cv::KeyPoint> keypointsObj;
	std::vector<cv::KeyPoint> keypointsScenes;
	std::vector<cv::DMatch> matches;

};
