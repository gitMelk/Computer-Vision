#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include "filter.cpp"

using namespace cv;
using namespace std;

void showHistogram(std::vector<cv::Mat>& hists);
void on_trackbarGauss(int, void*);
void on_trackbarMedian(int, void*);
void on_trackbarBilateral(int, void*);


int ksize = 0;
int sigma = 0;
int d = 0;
int sigmaSpace = 0;
int sigmaColor = 0; 
GaussianFilter gaussImg(Mat(),0);
MedianFilter medianImg(Mat(), 0);
BilateralFilter bilateralImg(Mat(), 0);

int main()
{
	
	Mat imageSource = imread(".\\photos\\image.jpg");
	
	// testing for other images:
	//Mat imageSource = imread(".\\photos\\overexposed.jpg");
	//Mat imageSource = imread(".\\photos\\lena.png");
	
	// part 1, Histograms

	Mat splitImage[3];
	split(imageSource, splitImage);
	Mat hist[3];
	Mat histEq[3];
	int histSize = 256;    // bin size
	const int* bins = &histSize;
	float range[] = { 0, 255 };
	const float *ranges[] = { range };
	Mat equalizedSplitImage[3];

	for (int i = 0; i < size(splitImage); i++)
	{
		calcHist(&splitImage[i], 1, 0, Mat(), hist[i], 1, bins, ranges, true, false);
		equalizeHist(splitImage[i], equalizedSplitImage[i]);
		calcHist(&equalizedSplitImage[i], 1, 0, Mat(), histEq[i], 1, bins, ranges, true, false);

	}
	
	Mat finalImage;
	merge(equalizedSplitImage, 3, finalImage);

	// show image without modifications
	vector<Mat> histForShow = { hist[0], hist[1], hist[2] };
	showHistogram(histForShow);
	namedWindow("showImage", WINDOW_NORMAL);
	imshow("showImage", imageSource);
	waitKey(0);
	// show image with equalized histograms
	vector<Mat> histForShowEq = { histEq[0], histEq[1], histEq[2] };
	showHistogram(histForShowEq);
	imshow("showImage", finalImage);
	waitKey(0);
	
	// as before, but with the Lab color space
	Mat imageColorLab;   //destination array
	cvtColor(imageSource, imageColorLab, COLOR_BGR2Lab);
	Mat splitImageLab[3];
	split(imageColorLab, splitImageLab);//split source  
	Mat equalizedSplitImageLab[3];
	Mat histEqLab[3];
	Mat finalImageLab;
	equalizedSplitImageLab[2] = splitImageLab[2];
	equalizedSplitImageLab[1] = splitImageLab[1];
	equalizeHist(splitImageLab[0], equalizedSplitImageLab[0]);
	merge(equalizedSplitImageLab, 3, finalImageLab);
	Mat finalImageLabBGR;
	cvtColor(finalImageLab, finalImageLabBGR, COLOR_Lab2BGR);
	Mat splitImageLabHisto[3];
	split(finalImageLabBGR, splitImageLabHisto);//split source 
	
	for (int i = 0; i < size(splitImage); i++)
	{
		calcHist(&splitImageLabHisto[i], 1, 0, Mat(), histEqLab[i], 1, bins, ranges, true, false);
	}
	vector<Mat> histForShowEqLab = { histEqLab[0], histEqLab[1], histEqLab[2] };
	showHistogram(histForShowEqLab);
	// Show the Lab result
	imshow("showImage", finalImageLabBGR);
	waitKey(0);
	destroyAllWindows();

	// part 2
	// Gaussian
	gaussImg = GaussianFilter(finalImageLabBGR.clone(), 2);
	namedWindow("Gauss", WINDOW_NORMAL);
	createTrackbar("kSize", "Gauss", &ksize, 100, on_trackbarGauss);
	createTrackbar("sigma", "Gauss", &sigma, 100, on_trackbarGauss);
	waitKey(0);
	destroyAllWindows();

	// Median
	ksize = 0;
	namedWindow("Median", WINDOW_NORMAL);
	medianImg = MedianFilter(finalImageLabBGR.clone(), 2);
	createTrackbar("kSizeMedian", "Median", &ksize, 100, on_trackbarMedian);
	waitKey(0);
	destroyAllWindows();
	
	// Bilateral
	Mat imageSourceMorph = finalImageLabBGR.clone();
	namedWindow("Bilateral", WINDOW_NORMAL);
	bilateralImg = BilateralFilter(imageSourceMorph, 2);
	d = 10;
	createTrackbar("sigmaColor", "Bilateral", &sigmaColor, 100, on_trackbarBilateral);
	createTrackbar("sigmaSpace", "Bilateral", &sigmaSpace, 100, on_trackbarBilateral);
	waitKey(0);
	destroyAllWindows();
	
	// part 3
	
	Mat imageSourceMorph_result;
	int iterations = 2;

	// erosion
	erode(imageSourceMorph, imageSourceMorph_result, Mat(), Point(-1, -1), iterations, BORDER_CONSTANT, 1);
	namedWindow("showImage", WINDOW_NORMAL);
	imshow("showImage", imageSourceMorph_result);
	waitKey(0);
	
	// dilatation
	dilate(imageSourceMorph, imageSourceMorph_result, Mat(), Point(-1, -1), iterations, BORDER_CONSTANT, 1);
	namedWindow("showImage", WINDOW_NORMAL);
	imshow("showImage", imageSourceMorph_result);
	waitKey(0);

	// custom filter
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(imageSourceMorph, imageSourceMorph_result, MORPH_CLOSE, element, Point(-1, -1), iterations, BORDER_CONSTANT, 1);
	namedWindow("showImage", WINDOW_NORMAL);
	imshow("showImage", imageSourceMorph_result);
	waitKey(0);

}


void showHistogram(std::vector<cv::Mat>& hists)
{
	// Min/Max computation
	double hmax[3] = { 0,0,0 };
	double min;
	cv::minMaxLoc(hists[0], &min, &hmax[0]);
	cv::minMaxLoc(hists[1], &min, &hmax[1]);
	cv::minMaxLoc(hists[2], &min, &hmax[2]);

	std::string wname[3] = { "blue", "green", "red" };
	cv::Scalar colors[3] = { cv::Scalar(255,0,0), cv::Scalar(0,255,0),
							 cv::Scalar(0,0,255) };

	std::vector<cv::Mat> canvas(hists.size());

	// Display each histogram in a canvas
	for (int i = 0, end = hists.size(); i < end; i++)
	{
		canvas[i] = cv::Mat::ones(125, hists[0].rows, CV_8UC3);

		for (int j = 0, rows = canvas[i].rows; j < hists[0].rows - 1; j++)
		{
			cv::line(
				canvas[i],
				cv::Point(j, rows),
				cv::Point(j, rows - (hists[i].at<float>(j) * rows / hmax[i])),
				hists.size() == 1 ? cv::Scalar(200, 200, 200) : colors[i],
				1, 8, 0
			);
		}

		cv::imshow(hists.size() == 1 ? "value" : wname[i], canvas[i]);
	}
}


void on_trackbarGauss(int, void*)
{
	int kvalue;
	if (ksize == 0)
	{	kvalue = 1;
	}
	else if (ksize % 2 == 0)
	{
		kvalue = ksize + 1;
	}
	else
	{
		kvalue = ksize;
	}
	setTrackbarPos("ksize", "Gauss", kvalue);
	gaussImg.doFilter(kvalue, sigma);
	imshow("Gauss", gaussImg.getResult());
}

void on_trackbarMedian(int, void*)
{
	int kvalue;
	if (ksize == 0)
	{
		kvalue = 3;
	}
	else if (ksize % 2 == 0)
	{
		kvalue = ksize + 1;
	}
	else
	{
		kvalue = ksize;
	}
	setTrackbarPos("kSizeMedian", "Median", kvalue);
	medianImg.doFilter(kvalue);
	imshow("Median", medianImg.getResult());
}

void on_trackbarBilateral(int, void*)
{
	bilateralImg.doFilter(d, sigmaColor, sigmaSpace);
	imshow("Bilateral", bilateralImg.getResult());
}
