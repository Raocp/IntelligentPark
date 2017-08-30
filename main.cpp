#include "easylogging++.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;

INITIALIZE_EASYLOGGINGPP

int main()
{
	double time = 0;
	double timesum = 0;
	int testlength = 1;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nPartA;
	LARGE_INTEGER nPartB;
	LARGE_INTEGER nPartC;
	LARGE_INTEGER nEndTime;
	QueryPerformanceFrequency(&nFreq);
	//****初始化log配置文件
	el::Configurations conf("myconfig.conf");
	el::Loggers::reconfigureLogger("default", conf);
	el::Loggers::reconfigureAllLoggers(conf);

	IplImage* src;
	IplImage* binaryimage;
	IplImage* grayimage;
	IplImage* dstTri;  //Store the triangle
	
	IplImage* dst0 ;
	IplImage* dst1 ;
	IplImage* color_dst ;
	

	LOG(INFO) << "My first info log using default logger";

	//IplImage* src; //= cvLoadImage("	IplImage* src = cvLoadImage("D:\\Workspace\\opencv_test\\TestPic\\-37.jpg");");
	for (int i = 0; i < testlength; i++)
	{
		time = 0;
		switch (2)
		{
		case 0:
			src = cvLoadImage("D:\\Workspace\\opencv_test\\TestPic\\-37.jpg");
			//src = cvLoadImage("D:\\Workspace\\opencv_test\\StainedTestPic\\stained103.jpg");
			break;
		case 1:
			src = cvLoadImage("D:\\Workspace\\opencv_test\\TestPic\\5.jpg");
			//src = cvLoadImage("D:\\Workspace\\opencv_test\\StainedTestPic\\stained5.jpg");
			break;
		case 2:
			src = cvLoadImage("D:\\Workspace\\opencv_test\\TestPic\\103.jpg");
			//src = cvLoadImage("D:\\Workspace\\opencv_test\\StainedTestPic\\stained98.jpg");
			break;
		default:
			break;
		}
		/*预处理*/
		QueryPerformanceCounter(&nBeginTime);//开始计时  
		binaryimage = cvCreateImage(cvGetSize(src), 8, 1);
		grayimage = cvCreateImage(cvGetSize(src), 8, 1);
		dstTri = cvCreateImage(cvGetSize(src), 8, 3);
		dst0 = cvCreateImage(cvGetSize(dstTri), 8, 3);
		dst1 = cvCreateImage(cvGetSize(dstTri), 8, 1);
		color_dst = cvCreateImage(cvGetSize(dstTri), 8, 1);
		cvZero(dstTri);
		cvZero(color_dst);

		//IplImage* binaryimage = cvCreateImage(cvGetSize(src), 8, 1);
		//IplImage* grayimage = cvCreateImage(cvGetSize(src), 8, 1);
		cvZero(binaryimage);
		cvZero(grayimage);
		cvCvtColor(src, grayimage, CV_BGR2GRAY);
		cvThreshold(grayimage, binaryimage, 240, 255, CV_THRESH_BINARY_INV);
		/*cvSaveImage("D:\\Workspace\\opencv_test\\TestPic\\三角形binary.jpg", binaryimage);*/

		QueryPerformanceCounter(&nPartA);//二值化部分计时



		//IplImage* dstTri = cvCreateImage(cvGetSize(src), 8, 3);  //Store the triangle
		//cvZero(dstTri);

		CvMemStorage* storage_cont = cvCreateMemStorage(0);
		CvSeq* first_contour = NULL;
		cvFindContours(binaryimage, storage_cont, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		int count = 0;
		/*cvSaveImage("D:\\Workspace\\opencv_test\\TestPic\\三角形gray.jpg", grayimage);*/

		/*画出图像轮廓*/
		for (; first_contour != 0; first_contour = first_contour->h_next)
		{
			if (first_contour->total > 100)
			{
				cout << count + 1 << " th 条轮廓有 " << first_contour->total << " 个点。" << endl;
				count++;
				CvScalar color = CV_RGB(255, 255, 255);
				for (int i = 0; i < first_contour->total; i++)
				{
					CvPoint* cor0 = (CvPoint*)cvGetSeqElem(first_contour, i - 1);
					CvPoint* cor1 = (CvPoint*)cvGetSeqElem(first_contour, i);
					cvLine(dstTri, *cor0, *cor1, color, 1, 8, 0);

					//cout << "(" << cor1->x << ","<<cor1->y<<")" << endl;
				}
			}
		}
		//cvSaveImage("D:\\Workspace\\opencv_test\\TestPic\\三角形轮廓图2.jpg", dstTri);  //dstTri代替下面的img

		QueryPerformanceCounter(&nPartB);//canny部分计时


		//IplImage* img = cvLoadImage("D:\\Workspace\\opencv_test\\tri.jpg");
		//IplImage* img = cvLoadImage("D:\\Workspace\\opencv_test\\TestPic\\三角形轮廓图2.jpg");

		//IplImage* dst0 = cvCreateImage(cvGetSize(dstTri), 8, 3);
		//IplImage* dst1 = cvCreateImage(cvGetSize(dstTri), 8, 1);
		//IplImage* color_dst = cvCreateImage(cvGetSize(dstTri), 8, 1);
		//cvZero(color_dst);

		cvThreshold(dstTri, dst0, 100, 255, CV_THRESH_BINARY);
		cvSaveImage("D:\\Workspace\\opencv_test\\TestPic\\output0.jpg", dst0);
		//cvCanny(dst0, dst1, 50, 200, 3);

		cvCvtColor(dst0, dst1, CV_BGR2GRAY);

		//cvSaveImage("D:\\Workspace\\opencv_test\\output1.jpg", dst1);

		CvSeq* lines = 0;
		CvMemStorage* storage_line = cvCreateMemStorage(0);
		//lines = cvHoughLines2(dst1, storage_line, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 200, 0, 0);

		lines = cvHoughLines2(dst1, storage_line, CV_HOUGH_STANDARD, 1, CV_PI / 180, 12
			0, 0, 0);    //150为识别阀值

		//if (lines->total == 3)
		//{
		cout << "检测到" << lines->total << "条线" << endl;
		//}
		//else
		//{ 
		//	cout << "WARNING: error in detecting triangle!" << endl;
		//}

		QueryPerformanceCounter(&nPartC);//canny部分计时


		double k[3];     //直线斜率
		double y0[3];    //直线y轴截距
		double pt[3][2]; //三个顶点坐标
		double dist[3];  //顶点到中点距离
		double angle = 0;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				pt[i][j] = 0;
			}
		}

		/*画出三条边*/
		for (int i = 0; i < MIN(lines->total, 3); i++)
		{
			float* line = (float*)cvGetSeqElem(lines, i);
			float rho = line[0];
			float theta = line[1];
			CvPoint pt1, pt2;
			double a = cos(theta), b = sin(theta);
			k[i] = -a / b; y0[i] = rho / b;
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			cvLine(dst0, pt1, pt2, CV_RGB(255, 255, 255), 3, CV_AA, 0);
		}
		for (int i = 0; i < 2; i++)
		{
			for (int j = i + 1; j < 3; j++)
			{
				pt[i + j - 1][0] = (y0[j] - y0[i]) / (k[i] - k[j]);
				pt[i + j - 1][1] = k[i] * pt[i + j - 1][0] + y0[i];
			}
		}
		CvPoint midPt;
		CvPoint pekPt;
		int num_pekPt;
		double temp = 0;
		midPt.x = (pt[0][0] + pt[1][0] + pt[2][0]) / 3;
		midPt.y = (pt[0][1] + pt[1][1] + pt[2][1]) / 3;
		for (int i = 0; i < 3; i++)
		{
			dist[i] = sqrt((pt[i][0] - midPt.x)*(pt[i][0] - midPt.x) + (pt[i][1] - midPt.y)*(pt[i][1] - midPt.y));
			/*cout << "第 " <<i+1 << " 个顶点坐标为 （ "<<pt[i][0] << ","<<pt[i][1]<<" )" << endl;*/
		}

		/*找到等腰三角形顶点的编号*/
		for (int i = 0; i < 3; i++)
		{
			if (dist[i]>temp)
			{
				temp = dist[i];
				num_pekPt = i;
			}
		}
		pekPt.x = pt[num_pekPt][0];
		pekPt.y = pt[num_pekPt][1];

		/*cvLine(dst0, pekPt, midPt, Scalar(100, 100, 100), 3, CV_AA, 0);*/
		/*cvCircle(dst0, midPt, 2, Scalar(255, 255, 255));*/

		double steep = (pekPt.y - midPt.y)*1.0 / (pekPt.x - midPt.x);
		angle = atan(steep);
		angle = angle * 180 / CV_PI;
		cout << "偏转的角度为 " << angle << " 度" << endl;

		QueryPerformanceCounter(&nEndTime);//停止计时 
		time = (double)(nPartA.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s 
		LOG(INFO) << "No : " << 1 << " 二值化 times spend:  " << time * 1000;

		time = (double)(nPartB.QuadPart - nPartA.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s 
		LOG(INFO) << "No : " << 2 << " 查找轮廓 times spend:  " << time * 1000;

		time = (double)(nPartC.QuadPart - nPartB.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s 
		LOG(INFO) << "No : " << 3 << " 图像转换 times spend:  " << time * 1000;

		time = (double)(nEndTime.QuadPart - nPartC.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s 
		LOG(INFO) << "No : " << 4 << " 拟合 times spend:  " << time * 1000;

		time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s  
		LOG(INFO) << "Total times spend:  " << time * 1000;
		timesum = time + timesum;
	}
	LOG(INFO) << "average : " << timesum * 1000 / testlength;

	cvSaveImage("D:\\Workspace\\opencv_test\\TestPic\\output2.jpg", dst0);
	return 1;
}