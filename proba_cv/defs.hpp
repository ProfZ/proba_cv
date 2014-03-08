#ifndef DEFS_HPP
#define DEFS_HPP

#include <opencv2\opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2\gpu\gpu.hpp>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define MIN_GRATE 5 //pixels
#define ERODE_THRESH 100 //minimalna povrsina koja se prihvata
#define MARKER_NUM 3 //broj markera, privremeno
#define RECT_ALLOC_UNIT MARKER_NUM*5*2 //jedinica zauzeca memorije za rect-ove
#define EXPAND 10 //Prosirivanje ROI-a
#define RANGE_THRESH 10//threshold za boje

#define xCoord(a) a-4
#define yCoord(a) a-3
#define height(a) a-2
#define width(a) a-1

#define FOCUS 30.0f
#define REF_R 50.0f

namespace tracking{
	//standardni rect
	typedef cv::Rect_<int> Rect;
	enum Sablon {
		_TOP_BOTTOM_S = 0x1,
		_TOP_BOTTOM_D = 0x2,
		_HEAD_TOP_BOTTOM_D = 0x4,
		_HEAD_TOP_BOTTOM_D_BODY = 0x8
	};

	extern cv::Mat frame;
	extern unsigned char lB,lG,lR,uB,uG,uR;
	extern Sablon sablon;

	extern float cart_x0,cart_y0;
	extern std::chrono::steady_clock::duration timeSample;
}

#endif