#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

void get_first_class_points(Mat&, Mat&);
void get_second_class_points(Mat&, Mat&);
void create_central_difference_image(Mat&, int, Mat&);
bool feature_point_or_not(Mat&, int, int);
bool compare_sign(int, int);

void main() {
	//Test
	double a = 5;
	double b = 5 * 1.15 + 0.5;
	cout << (int) b << endl;
	VideoCapture capture;
	//Mat img;

	Mat img = imread("E:\\objects.jpg");
	cvtColor(img, img, COLOR_BGR2GRAY);
	Mat first_class_feature(img.rows,img.cols,CV_8UC1);
	Mat second_class_feature(img.rows, img.cols, CV_8UC1);

	get_first_class_points(img, first_class_feature);
	get_second_class_points(img, second_class_feature);

	imshow("Test", img);
	imshow("First Class Feature Points", first_class_feature);
	imshow("Second Class Feature Points", second_class_feature);

	int c = waitKey(0);
	if ((char)c == 27) { return; }  

	/*capture.open(0);
	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return; }

	while (capture.read(img))
	{
		if (img.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		cvtColor(img, img, COLOR_BGR2GRAY);
		Mat first_class_feature(img.rows, img.cols, CV_8UC1);
		Mat second_class_feature(img.rows, img.cols, CV_8UC1);

		get_first_class_points(img, first_class_feature);
		get_second_class_points(img, second_class_feature);

		imshow("Test", img);
		imshow("First Class Feature Points", first_class_feature);

		int c = waitKey(1000);
		if ((char)c == 27) { break; } // escape

	} */
}

void get_first_class_points(Mat& m, Mat& dest) {
	int width = m.cols;
	int height = m.rows;

	for (int x = 1; x < width-1; x++) {
		for (int y = 1; y < height-1; y++) {
			int central_intensity = m.at<uchar>(Point(x,y));
			uchar val = 0;

			int i1 = m.at<uchar>(Point(x - 1, y - 1)) - central_intensity;
			int i9 = m.at<uchar>(Point(x + 1, y + 1)) - central_intensity;

			int i2 = m.at<uchar>(Point(x, y - 1)) - central_intensity;
			int i8 = m.at<uchar>(Point(x, y + 1)) - central_intensity;

			int i3 = m.at<uchar>(Point(x + 1, y - 1)) - central_intensity;
			int i7 = m.at<uchar>(Point(x - 1, y + 1)) - central_intensity;

			int i4 = m.at<uchar>(Point(x - 1, y)) - central_intensity;
			int i6 = m.at<uchar>(Point(x + 1, y)) - central_intensity;

			if (i1 * i9 <= 0 || i2 * i8 <= 0 || i3 * i7 <= 0 || i4 * i6 <= 0)
				val = 0;
			else {
				val = 255;
			} 

			dest.at<uchar>(y, x) = val;
		}
	}
}

void get_second_class_points(Mat& m, Mat& dest) {
	Mat central_difference_image_0(m.rows, m.cols, CV_8UC1);
	Mat central_difference_image_1(m.rows, m.cols, CV_8UC1);
	Mat central_difference_image_2(m.rows, m.cols, CV_8UC1);
	Mat central_difference_image_3(m.rows, m.cols, CV_8UC1);

	create_central_difference_image(central_difference_image_0, 0, m);
	create_central_difference_image(central_difference_image_1, 1, m);
	create_central_difference_image(central_difference_image_2, 2, m);
	create_central_difference_image(central_difference_image_3, 3, m);

	for (int x = 1; x < m.cols - 1; x++) {
		for (int y = 1; y < m.rows - 1; y++) {
			uchar val = 0;
			if (feature_point_or_not(central_difference_image_0, x, y)
				&& feature_point_or_not(central_difference_image_1, x, y)
				&& feature_point_or_not(central_difference_image_2, x, y)
				&& feature_point_or_not(central_difference_image_3, x, y)) {
				
				val = 255;
				dest.at<uchar>(y, x) = val;
			}
			else {
				val = 0;
				dest.at<uchar>(y, x) = val;
			}
		}
	}
}

void create_central_difference_image(Mat& dest, int offset, Mat& m) {
	int width = m.cols;
	int height = m.rows;

	for (int x = 1; x < width - 1; x++) {
		for (int y = 1; y < height - 1; y++) {
			int central_intensity = m.at<uchar>(Point(x, y));
			uchar val = 0;

			int i1 = m.at<uchar>(Point(x - 1, y - 1)) - central_intensity;
			int i9 = m.at<uchar>(Point(x + 1, y + 1)) - central_intensity;

			int i2 = m.at<uchar>(Point(x, y - 1)) - central_intensity;
			int i8 = m.at<uchar>(Point(x, y + 1)) - central_intensity;

			int i3 = m.at<uchar>(Point(x + 1, y - 1)) - central_intensity;
			int i7 = m.at<uchar>(Point(x - 1, y + 1)) - central_intensity;

			int i4 = m.at<uchar>(Point(x - 1, y)) - central_intensity;
			int i6 = m.at<uchar>(Point(x + 1, y)) - central_intensity;

			if (offset == 0)
				val = i6 - i4;
			else if (offset == 1)
				val = i3 - i7;
			else if (offset == 2)
				val = i2 - i8;
			else if (offset == 3)
				val = i1 - i9;

			dest.at<uchar>(y, x) = val;
		}
	}
}

bool feature_point_or_not(Mat& m, int x, int y) {
	int width = m.cols;
	int height = m.rows;

	int central_intensity = m.at<uchar>(Point(x, y));

	int i1 = m.at<uchar>(Point(x - 1, y - 1)) - central_intensity;
	int i9 = m.at<uchar>(Point(x + 1, y + 1)) - central_intensity;

	int i2 = m.at<uchar>(Point(x, y - 1)) - central_intensity;
	int i8 = m.at<uchar>(Point(x, y + 1)) - central_intensity;

	int i3 = m.at<uchar>(Point(x + 1, y - 1)) - central_intensity;
	int i7 = m.at<uchar>(Point(x - 1, y + 1)) - central_intensity;

	int i4 = m.at<uchar>(Point(x - 1, y)) - central_intensity;
	int i6 = m.at<uchar>(Point(x + 1, y)) - central_intensity;

	if (i1 * i9 <= 0 || i2 * i8 <= 0 || i3 * i7 <= 0 || i4 * i6 <= 0)
		return false;
	else {
		return true; } 
}

bool compare_sign(int x, int y) {
	return (x >= 0) ^ (y < 0);
}
