#ifndef TEST_H
#define TEST_H

//include of OpenCV libraries
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

//basic C++ includes
#include <iostream>
#include <stdio.h>

//DepthAi includes (for the native camera functions)
#include <depthai/depthai.hpp> //This include works but appear here to be wrong

//namespace declarations
using namespace cv;
using namespace std;


//constant values declaration
const int max_value_H = 360 / 2;
const int max_value = 255;

const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
const String track = "track";

int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

std::vector<double> vec;
#endif