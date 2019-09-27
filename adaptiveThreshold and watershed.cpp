
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\opencv.hpp>
#include<iostream>
#include <windows.h>


using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	
	//step1输入图像  
	Mat srcImage = imread("3.png");
	//imshow("原始图", srcImage);
  
  
  //开始计时 
	DWORD start_time = GetTickCount();    
	Mat dstImage;
	blur(srcImage, dstImage, Size(3, 3));//(均值滤波操作，参数：输入图像，输出图像，表示内核大小）进行滤波为了保证后面二值化时的效果更好
	//boxFilter(srcImage, dstImage, -1, Size(3, 3));//(方框滤波操作，参数：输入图像，输出图像，输出图像的深度，表示内核大小）
	//GaussianBlur(srcImage, dstImage, Size(5, 5), 3, 3);//(高斯滤波操作，参数：输入图像，输出图像，内核大小，sigmax，sigmay）
	//中值滤波花费的时间是均值滤波的5倍以上，不考虑
  //imshow("滤波图", dstImage);
	
  
  //step2 灰度图 二值化
	Mat grayImage, binaryImage;
	cvtColor(dstImage, grayImage, COLOR_BGR2GRAY);//灰度图
	adaptiveThreshold(grayImage, binaryImage, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 255,75);
	//imshow("二值化图", binaryImage);
	
  
  //step3距离变换
	Mat dist;
	distanceTransform(binaryImage, dist, DistanceTypes::DIST_L2, 3, CV_32F);
	//normalize(dist, dist, 0, 1, NORM_MINMAX);//归一化处理
	//imshow("距离变换效果图", dist);
	//threshold(dist, dist, 0, 255, THRESH_BINARY);//再次二值化
	//imshow("距离变换效果图", dist);
	
  
  //sep4寻找种子
	Mat dist_m;
	binaryImage.convertTo(dist_m, CV_8UC1);				
	//imshow("种子图", dist_m);	//dist_m的像素值十分的小，扩大1000倍，才看出来轮廓						

	
  //step5生成Marker
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;
  //只显示外轮廓
	findContours(dist_m, contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat markers = Mat::zeros(srcImage.size(), CV_32SC1);
	for (size_t i = 0; i < contours.size(); i++) 
  {
		drawContours(markers, contours,i, Scalar::all(i+1));
	}
	circle(markers, Point(7, 7), 30, Scalar(255), -1);	
	
	
  //step6分水岭变换
	Mat k = getStructuringElement(MORPH_ERODE, Size(3, 3), Point(-1, -1));
	morphologyEx(srcImage, srcImage, MORPH_ERODE, k);// 腐蚀，去粘连部位的干扰
  watershed(srcImage, markers);
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark, Mat());

	// 颜色随机
	vector<Vec3b> colorTab;
	for (size_t i = 0; i < contours.size(); i++) 
  {
		int r = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int b = theRNG().uniform(0, 255);
		colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	
  
  //step7输出图片
  //颜色填充与最终显示
	Mat dst = Mat::zeros(markers.size(), CV_8UC3); 
	int index = 0;
	for (int row = 0; row < markers.rows; row++) 
  {
		for (int col = 0; col < markers.cols; col++) 
    {
			index = markers.at<int>(row, col);
			if (index > 0 && index <= contours.size()) 
      {
				dst.at<Vec3b>(row, col) = colorTab[index - 1];
			}
			else 
      {
				dst.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
			}
		}
	}
    
  DWORD end_time = GetTickCount();       //结束计时
	cout << "The run time is:" << (end_time - end_time5) << "ms!" << endl; 
	imshow("最终结果", dst);
  cout<< "数量"<< contours.size() << endl;
	waitKey(0);
	return 0;


	
}

