#include "main.h"
#include <opencv2\opencv.hpp>
#include "opencv2/core/core.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <opencv2/features2d/features2d.hpp>  
#include "opencv2/nonfree/nonfree.hpp"    
#include "opencv2/legacy/legacy.hpp"   
#include "highgui/highgui.hpp"    
#include <math.h>

using namespace cv;
using namespace std;

main::main()
{
}


main::~main()
{
}

void drawArrow(cv::Mat& img, cv::Point pStart, cv::Point pEnd, int len, int alpha, cv::Scalar& color, int thickness, int lineType)
{
	if (pStart == pEnd)
	{
		//ֻ��һ����
		circle(img, pStart, 1, color);
		return;
	}

	const double PI = 3.1415926;
	Point arrow;
	//���� �� �ǣ���򵥵�һ�����������ͼʾ���Ѿ�չʾ���ؼ����� atan2 ��������������棩   
	double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));

	line(img, pStart, pEnd, color, thickness, lineType);

	//������Ǳߵ���һ�˵Ķ˵�λ�ã�����Ļ��������Ҫ����ͷ��ָ��Ҳ����pStart��pEnd��λ�ã� 
	arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);

	arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);

	line(img, pEnd, arrow, color, thickness, lineType);

	arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);

	arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);

	line(img, pEnd, arrow, color, thickness, lineType);
}


//��������
float GetCenterOfMass(Mat m)
{
	float m_00 = 0, m_01 = 0, m_10 = 0;

	for (int x = 0; x < m.rows; x++)
	{
		for (int y = 0; y < m.cols; y++)
		{
			m_00 += m.at<uchar>(x, y);
			m_01 += (float)y * m.at<uchar>(x, y);
			m_10 += (float)x * m.at<uchar>(x, y);
		}
	}

	float x_c = m_10 / m_00;
	float y_c = m_01 / m_00;

	return fastAtan2(m_01, m_10);

	//return Point2f(x_c, y_c);
}

//����Ҷ�ֱ��ͼ
Mat getHistograph(const Mat grayImage)
{
	//������ֱ��ͼ��ͨ����Ŀ����0��ʼ����  
	int channels[] = { 0 };
	//����ֱ��ͼ����ÿһά�ϵĴ�С������Ҷ�ͼֱ��ͼ�ĺ�������ͼ��ĻҶ�ֵ����һά��bin�ĸ���  
	//���ֱ��ͼͼ�������bin����Ϊx��������bin����Ϊy����channels[]={1,2}��ֱ��ͼӦ��Ϊ��ά�ģ�Z����ÿ��bin��ͳ�Ƶ���Ŀ  
	const int histSize[] = { 256 };
	//ÿһάbin�ı仯��Χ  
	float range[] = { 0,256 };

	//����bin�ı仯��Χ��������channelsӦ�ø�channelsһ��  
	const float* ranges[] = { range };

	//����ֱ��ͼ�����������ֱ��ͼ����  
	Mat hist;
	//opencv�м���ֱ��ͼ�ĺ�����hist��СΪ256*1��ÿ�д洢��ͳ�Ƶĸ��ж�Ӧ�ĻҶ�ֵ�ĸ���  
	calcHist(&grayImage, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	//�ҳ�ֱ��ͼͳ�Ƶĸ��������ֵ��������Ϊֱ��ͼ������ĸ�  
	double maxValue = 0;
	//�Ҿ����������Сֵ����Ӧ�����ĺ���  
	minMaxLoc(hist, 0, &maxValue, 0, 0);
	//���ֵȡ��  
	int rows = cvRound(maxValue);
	//����ֱ��ͼͼ��ֱ��ͼ������ĸ���Ϊ����������Ϊ256(�Ҷ�ֵ�ĸ���)  
	//��Ϊ��ֱ��ͼ��ͼ�������Ժڰ���ɫΪ���֣���ɫΪֱ��ͼ��ͼ��  
	Mat histImage = Mat::zeros(rows, 256, CV_8UC1);

	//ֱ��ͼͼ���ʾ  
	for (int i = 0; i<256; i++)
	{
		//ȡÿ��bin����Ŀ  
		int temp = (int)(hist.at<float>(i, 0));
		//���bin��ĿΪ0����˵��ͼ����û�иûҶ�ֵ��������Ϊ��ɫ  
		//���ͼ�����иûҶ�ֵ���򽫸��ж�Ӧ������������Ϊ��ɫ  
		if (temp)
		{
			//����ͼ�������������Ͻ�Ϊԭ�㣬����Ҫ���б任��ʹֱ��ͼͼ�������½�Ϊ����ԭ��  
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 255;
		}
	}
	//����ֱ��ͼͼ���и߿��ܸܺߣ���˽���ͼ�����Ҫ���ж�Ӧ��������ʹֱ��ͼͼ���ֱ��  
	Mat resizeImage;
	resize(histImage, resizeImage, Size(256, 256));
	return resizeImage;
}


void main()
{
	double start = static_cast<double>(getTickCount());
	Mat src1, gray1, src2, gray2;
	src1 = imread("img_00000.bmp");
	src2 = imread("img_00001.bmp");


	//src1 = imread("Farm_REF.bmp");
	//src2 = imread("Farm_IR.bmp");

	//Mat image = imread("2.png");

	//vector<KeyPoint> keypoints;

	//SiftFeatureDetector surf;
	//surf.detect(image, keypoints);

	//drawKeypoints(image, keypoints, image, Scalar(255, 0, 0),
	//	DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//for (int i = 0; i < keypoints.size(); i++)
	//{
	//	Point start = Point(keypoints[i].pt.x, keypoints[i].pt.y);
	//	Point end;

	//	float angle = keypoints[i].angle;


	//	//int th = 3;
	//	//Rect rect(start.x - th, start.y - th, 2 * th, 2 * th);
	//	//Mat roi = image(rect);
	//	//float angle = GetCenterOfMass(roi);

	//	int len = 20;
	//	int end_x = cos(angle) * len;
	//	int end_y = sin(angle) * len;
	//	end = Point(end_x+ start.x, end_y+ start.y);
	//	drawArrow(image, start, end, 5, 30, Scalar(255 , 0 , 0), 1, 1);
	//}

	//imshow("c",image);
	//imwrite("fang.png", image);

	cvtColor(src1, gray1, CV_BGR2GRAY);
	cvtColor(src2, gray2, CV_BGR2GRAY);

	//Mat hist1 = getHistograph(gray1);
	//Mat hist2 = getHistograph(gray2);

	//imshow("hist1", hist1);
	//imshow("hist2", hist2);

	//imwrite("hist1.png", hist1);
	//imwrite("hist2.png", hist2);

	morphologyEx(gray1, gray1, MORPH_GRADIENT, Mat());
	morphologyEx(gray2, gray2, MORPH_GRADIENT, Mat());

	//imshow("mor1", gray1);
	//imshow("mor2", gray2);
	//imwrite("mor1.png", gray1);
	//imwrite("mor2.png", gray2);

	//hist1 = getHistograph(gray1);
	//hist2 = getHistograph(gray2);

	//imshow("hist3", hist1);
	//imshow("hist4", hist2);
	//imwrite("hist3.png", hist1);
	//imwrite("hist4.png", hist2);

	vector<KeyPoint> keypoints1, keypoints2;
	Mat image1_descriptors, image2_descriptors;

	SurfFeatureDetector detector;
	//BriskFeatureDetector detector;
	//SurfFeatureDetector descriptor;
	BriefDescriptorExtractor descriptor;

	//ORB detector;     //����ORB�㷨��ȡ������  
	detector.detect(gray1, keypoints1);
	detector.detect(gray2, keypoints2);

	//for (int i = 0; i < keypoints1.size(); i++)
	//{
	//	Point start = Point(keypoints1[i].pt.x, keypoints1[i].pt.y);
	//	int th = 5;
	//	Rect rect(start.x - th, start.y - th, 2 * th, 2 * th);
	//	Mat roi = gray1(rect);
	//	float angle = GetCenterOfMass(roi);
	//	keypoints1[i].angle = angle;
	//}

	//for (int i = 0; i < keypoints2.size(); i++)
	//{
	//	Point start = Point(keypoints2[i].pt.x, keypoints2[i].pt.y);
	//	int th = 5;
	//	Rect rect(start.x - th, start.y - th, 2 * th, 2 * th);
	//	Mat roi = gray2(rect);
	//	float angle = GetCenterOfMass(roi);
	//	keypoints2[i].angle = angle;
	//}

	descriptor.compute(gray1, keypoints1, image1_descriptors);
	descriptor.compute(gray2, keypoints2, image2_descriptors);

	BFMatcher matcher(NORM_HAMMING, true); //����������Ϊ���ƶȶ���  
	//BruteForceMatcher<L2<float>> matcher;

	vector<DMatch> matches;
	matcher.match(image1_descriptors, image2_descriptors, matches);

	sort(matches.begin(), matches.end());

	Mat match_img;
	//drawMatches(src1, keypoints1, src2, keypoints2, matches, match_img);
	//imshow("�˳���ƥ��ǰ", match_img);

	//����ƥ������  
	vector<int> queryIdxs(matches.size()), trainIdxs(matches.size());
	for (size_t i = 0; i < matches.size(); i++)
	{
		queryIdxs[i] = matches[i].queryIdx;
		trainIdxs[i] = matches[i].trainIdx;
	}

	Mat H12;   //�任����  

	vector<Point2f> points1; KeyPoint::convert(keypoints1, points1, queryIdxs);
	vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, trainIdxs);
	int ransacReprojThreshold = 5;  //�ܾ���ֵ  
	H12 = findHomography(Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold);
	vector<char> matchesMask(matches.size(), 0);
	Mat points1t;
	perspectiveTransform(Mat(points1), points1t, H12);

	int mask_sum = 0;

	for (size_t i1 = 0; i1 < points1.size(); i1++)  //���桮�ڵ㡯  
	{
		if (norm(points2[i1] - points1t.at<Point2f>((int)i1, 0)) <= ransacReprojThreshold) //���ڵ������  
		{
			matchesMask[i1] = 1;
			mask_sum++;
		}
	}


	Mat Mat_img;
	drawMatches(src1, keypoints1, src2, keypoints2, matches, Mat_img, Scalar(0, 0, 255), Scalar::all(-1), matchesMask);

	imshow("ransacɸѡ��", Mat_img);

	imwrite("result.png", Mat_img);

	double time = ((double)getTickCount() - start) / getTickFrequency();
	cout << "����ʱ��Ϊ��" << time << "��" << endl;

	cout << "ͼ1�ҵ������㣺" << keypoints1.size() << endl;
	cout << "ͼ2�ҵ������㣺" << keypoints2.size() << endl;
	cout << "һ���ҵ�ƥ���ԣ�" << matches.size() << endl;
	cout << "��ȷƥ���ԣ�" << mask_sum << endl;

	waitKey(0);

}