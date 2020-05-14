#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"


class PlateDetection{

public:

	PlateDetection(cv::Mat);
	cv::Mat getResultsPreprocessed();
	cv::Mat getResultsPreprocessedCanny();
	void retriveMy2Plates();
	
	cv::Mat rawImage;

	void preprocessing();

	std::vector<std::vector<cv::Point> > contours;
	cv::Mat drawingCountors;
	cv::Mat plate;
	cv::Mat drawing;


protected:

	int numPic;
	int numScene;


	cv::Mat tryGiveMeAPlate(cv::Mat, std::vector<cv::Point>);
	cv::Mat getMask(cv::Mat original);
	cv::Mat getPlate(cv::Mat mask2, cv::Mat rawImg);

	
	cv::Mat preprocessedCanny;
	cv::Mat preprocessed;

	cv::Mat finalImage;


};
