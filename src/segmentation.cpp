#include "../include/camera.h"

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H - 1, low_H);
    setTrackbarPos("Low H", track, low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H + 1);
    setTrackbarPos("High H", track, high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S - 1, low_S);
    setTrackbarPos("Low S", track, low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S + 1);
    setTrackbarPos("High S", track, high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V - 1, low_V);
    setTrackbarPos("Low V", track, low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V + 1);
    setTrackbarPos("High V", track, high_V);
}

int main(int, char **)
{
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 2;        // 0 = open default camera
    int apiID = cv::CAP_ANY; // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID, apiID);
    // check if we succeeded
    if (!cap.isOpened())
    {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    namedWindow(window_capture_name);
    namedWindow(window_detection_name);
    namedWindow(track);
    // Trackbars to set thresholds for HSV values
    createTrackbar("Low H", track, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", track, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", track, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", track, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", track, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", track, &high_V, max_value, on_high_V_thresh_trackbar);
    Mat frame, frame_HSV, frame_threshold, res, prin;
    cv::Scalar color = cv::Scalar(0, 255, 0);
    cv::Scalar color2 = cv::Scalar(255, 0, 0);
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
         << "Press any key to terminate" << endl;
    while(cap.isOpened())
    {
        // wait for a new frame from camera and store it into 'frame'
        cap >> frame;
    
        // check if we succeeded
        if (frame.empty())
        {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images

        // Convert from BGR to HSV colorspace
        cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
        cvtColor(frame, frame, COLOR_BGR2RGB);
        // Detect the object based on HSV Range Values
        inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
        
        bitwise_and(frame, frame, res, frame_threshold);


        cv::Moments m = moments(frame_threshold, false);
        cv::Point com(m.m10 / m.m00, m.m01 / m.m00);
        cv::drawMarker(frame, com, color, cv::MARKER_CROSS, 20, 5);
        cv::drawMarker(frame_threshold, com, color, cv::MARKER_CROSS, 20, 5);
        cv::putText(frame, "Obs", cv::Point(com.x - 0, com.y - 50), cv::FONT_HERSHEY_DUPLEX, 1, color);

        cv::Point tracked;
        cv::drawMarker(frame, tracked, color2, cv::MARKER_TRIANGLE_DOWN, 20, 5);
        cv::putText(frame, "Tracked", cv::Point(tracked.x - 100, tracked.y - 100), cv::FONT_HERSHEY_DUPLEX, 1, color2);

        // Show the frames
        imshow(window_capture_name, frame);
        imshow(window_detection_name, frame_threshold);

        moveWindow(window_capture_name, 10, 10);
        moveWindow(window_detection_name, 800, 10);
        resizeWindow(window_capture_name, 10, 10);
        resizeWindow(window_detection_name, 10, 10);
        char key = (char)waitKey(30);
        if (key == 'q' || key == 27)
        {
            break;
        }
    }

    return 0;
}