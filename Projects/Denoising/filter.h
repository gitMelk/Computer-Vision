#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Generic class implementing a filter with the input and output image data and the parameters
class Filter{

// Methods

public:

	// constructor 
	// input_img: image to be filtered
	// filter_size : size of the kernel/window of the filter
	Filter(cv::Mat input_img, int filter_size);
	// perform filtering (in base class do nothing, to be reimplemented in the derived filters)
	void doFilter();
	// get the output of the filter
	cv::Mat getResult();

	//set the window size (square window of dimensions size x size)
	void setSize(int size);
	
	//get the Window Size
	int getSize();

// Data

protected:

	// input image
	cv::Mat input_image;

	// output image (filter result)
	cv::Mat result_image;

	// window size
	int filter_size;



};

// My Filters
class GaussianFilter : public Filter  {
public:
	// place constructor
	GaussianFilter(cv::Mat input_img, int filter_size);
	// re-implement  doFilter()
	void doFilter(int ksize, int sigmaIn);
	// additional parameter: standard deviation (sigma)
	cv::Mat getResult();
	double getSigma();
protected:
	// input image
	cv::Mat input_image;

	// output image (filter result)
	cv::Mat result_image;
	
	// window size
	int filter_size;

	double sigma;
};


class MedianFilter : public Filter {

public:
	MedianFilter(cv::Mat input_img, int filter_size);
	void doFilter(int ksize);
	cv::Mat getResult();
protected:
	// input image
	cv::Mat input_image;
	// output image (filter result)
	cv::Mat result_image;
	// window size
	int filter_size;
};

class BilateralFilter : public Filter {
	
public:
	BilateralFilter(cv::Mat input_img, int filter_size);
	void doFilter(int d, int sigmaColor, int sigmaSpace);
	cv::Mat getResult();
protected:
	// input image
	cv::Mat input_image;
	// output image (filter result)
	cv::Mat result_image;
	// window size
	int filter_size;
	int dvalue;
	int sigma_color;
	int sigma_space;
};