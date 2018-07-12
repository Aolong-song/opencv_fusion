#include<opencv2\opencv.hpp>
using namespace cv;







int main()
{
	// 1.	ͼƬ��ȡ
	Mat src = imread("123.jpg");
	Mat dst = imread("timg.jpg");
	if (src.empty() || dst.empty())
	{
		return 0;
	}

	Mat src_mask = 255 * Mat::ones(src.rows, src.cols, src.depth());

	Point center(dst.cols / 2, dst.rows / 2);

	Mat normal_clone;
	Mat mixed_clone;

	// 2.	��ȡͼ����ݶȳ�
	{
		Mat img = src;
		Mat gx = dst;
		Mat gy = dst;
		Mat kernel = Mat::zeros(1, 3, CV_8S);
		kernel.at<char>(0, 2) = 1;
		kernel.at<char>(0, 1) = -1;

		if (img.channels() == 3)
		{
			filter2D(img, gx, CV_32F, kernel);
		}
		else if (img.channels() == 1)
		{
			Mat tmp[3];
			for (int chan = 0; chan < 3; ++chan)
			{
				filter2D(img, tmp[chan], CV_32F, kernel);
			}
			merge(tmp, 3, gx);
		}
		////Mat kernel = Mat::zeros(3, 1, CV_8S);
		//kernel.at<char>(2, 0) = 1;
		//kernel.at<char>(1, 0) = -1;

		//if (img.channels() == 3)
		//{
		//	filter2D(img, gy, CV_32F, kernel);
		//}
		//else if (img.channels() == 1)
		//{
		//	Mat tmp[3];
		//	for (int chan = 0; chan < 3; ++chan)
		//	{
		//		filter2D(img, tmp[chan], CV_32F, kernel);
		//	}
		//	merge(tmp, 3, gy);
		//}
	}

	// 3.	����ں�ͼ���ɢ��
	{
		Mat img = src;
		Mat laplacianX = dst;
		Mat kernel = Mat::zeros(1, 3, CV_8S);
		kernel.at<char>(0, 0) = -1;
		kernel.at<char>(0, 1) = 1;
		filter2D(img, laplacianX, CV_32F, kernel);
	}

	// 4.	�����ؽ�
	{
		{
		const Mat src = src;
		const Mat dest = src;
		const Mat mask = dest;
		//_blend.create(dest.size(), CV_8UC3);//�ںϺ�ͼƬ�Ĵ�С�϶�������ͼ��һ��  
		Mat blend = dest;

		int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
		int h = mask.size().height;
		int w = mask.size().width;

		Mat gray = Mat(mask.size(), CV_8UC1);
		Mat dst_mask = Mat::zeros(dest.size(), CV_8UC1);//����ͼ���mask  
		Mat cs_mask = Mat::zeros(src.size(), CV_8UC3);
		Mat cd_mask = Mat::zeros(dest.size(), CV_8UC3);

		if (mask.channels() == 3)//���������mask�ǲ�ɫͼ ��Ҫת���ɵ�ͨ���Ҷ�ͼ  
			cvtColor(mask, gray, COLOR_BGR2GRAY);
		else
			gray = mask;
		//�������mask����С���Σ�Ҳ���ǰ���ֻ�ܰ�����������С���ο����������λ��src�ģ����滹��һ����Ӧ�ľ���λ��dst  
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{

				if (gray.at<uchar>(i, j) == 255)
				{
					minx = std::min(minx, i);
					maxx = std::max(maxx, i);
					miny = std::min(miny, j);
					maxy = std::max(maxy, j);
				}
			}
		}
		int lenx = maxx - minx;//������εĿ�  
		int leny = maxy - miny;//������εĸ�  


		Mat patch = Mat::zeros(Size(leny, lenx), CV_8UC3);//��������ľ������򣬴���һ����С��ͬ����  

		Point p;
		p.x = 0;
		p.y = 0;
		int minxd = p.y - lenx / 2;//����dst�ľ���  
		int maxxd = p.y + lenx / 2;
		int minyd = p.x - leny / 2;
		int maxyd = p.x + leny / 2;

		//CV_Assert(minxd >= 0 && minyd >= 0 && maxxd <= dest.rows && maxyd <= dest.cols);

		Rect roi_d(minyd, minxd, leny, lenx);//dst ��Ȥ����ľ���  
		Rect roi_s(miny, minx, leny, lenx);//src ��Ȥ�������  

		//Mat destinationROI = dst_mask(roi_d);
		Mat sourceROI = cs_mask(roi_s);

		//gray(roi_s).copyTo(destinationROI);//  
		//src(roi_s).copyTo(sourceROI, gray(roi_s));
		//src(roi_s).copyTo(patch, gray(roi_s));//patchΪ  

		//destinationROI = cd_mask(roi_d);
		//cs_mask(roi_s).copyTo(destinationROI);//cs_maskΪ��ǰ��ͼƬ�ľ�������ͼ�� ת��������ͼƬ�����е�ͼƬ  
	}
		seamlessClone(src, dst, src_mask, center, normal_clone, NORMAL_CLONE);
		seamlessClone(src, dst, src_mask, center, mixed_clone, MIXED_CLONE);
	}

	// 5.	ͼ���ں�Ч��
	imshow("source", src);
	imshow("destination", dst);
	imshow("seamless", mixed_clone);
	waitKey(0);
	return 1;
}