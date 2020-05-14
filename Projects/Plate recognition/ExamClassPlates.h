#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"


class CharFinderInPlates{

public:

	CharFinderInPlates(cv::Mat);
	void getPreprocessedPlates();
	std::vector<cv::Mat> getSingleChar();
	
	std::vector<cv::Mat> charactersStored;
	std::vector<char> mapCharStored;
	cv::Mat preProcPlate;

protected:

	cv::Mat maximizeContrast(cv::Mat);
	cv::Mat rawPlate;
	

	int numPic;
	int numScene;
	


};
