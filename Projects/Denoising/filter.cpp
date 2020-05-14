#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "filter.h"

//using namespace cv;

	// constructor
	Filter::Filter(cv::Mat input_img, int size) {

		input_image = input_img;
		if (size % 2 == 0)
			size++;
		filter_size = size;
	}

	// for base class do nothing (in derived classes it performs the corresponding filter)
	void Filter::doFilter() {

		// it just returns a copy of the input image
		result_image = input_image.clone();

	}

	// get output of the filter
	cv::Mat Filter::getResult() {

		return result_image;
	}

	//set window size (it needs to be odd)
	void Filter::setSize(int size) {

		if (size % 2 == 0)
			size++;
		filter_size = size;
	}

	//get window size 
	int Filter::getSize() {

		return filter_size;
	}



	// Write your code to implement the Gaussian, median and bilateral filters

	GaussianFilter::GaussianFilter(cv::Mat input_img, int size) : Filter(input_img, size)
	{
		input_image = input_img;
		if (size % 2 == 0)
			size++;
		filter_size = size;
	}

	
	// re-implement  doFilter()
	void GaussianFilter::doFilter(int ksize, int sigmaIn)
	{	//max sigma = 1
		sigma = static_cast<double>(sigmaIn);
		GaussianBlur(input_image, result_image, cv::Size(ksize, ksize), sigma, sigma, 4 );

	}
	cv::Mat GaussianFilter::getResult() 
	{
		return result_image;
	}
	double GaussianFilter::getSigma()
	{
		return sigma;
	}

	MedianFilter::MedianFilter(cv::Mat input_img, int size) : Filter(input_img, size)
	{
		input_image = input_img;
		if (size % 2 == 0)
			size++;
		filter_size = size;
	}
	// re-implement  doFilter()
	void MedianFilter::doFilter(int ksize)
	{	
		medianBlur(input_image, result_image, ksize);
	}
	cv::Mat MedianFilter::getResult()
	{
		return result_image;
	}


	BilateralFilter::BilateralFilter(cv::Mat input_img, int size) : Filter(input_img, size)
	{
		input_image = input_img;
		if (size % 2 == 0)
			size++;
		filter_size = size;
	}
	// re-implement  doFilter()
	void BilateralFilter::doFilter(int d, int sigmaColor, int sigmaSpace)
	{
		bilateralFilter(input_image, result_image, d, static_cast<double>(sigmaColor),
			static_cast<double>(sigmaSpace), 4);
	}
	cv::Mat BilateralFilter::getResult()
	{
		return result_image;
	}
	