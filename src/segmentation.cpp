#include "../include/camera.h"

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H - 1, low_H);
    setTrackbarPos("Low H", "track", low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H + 1);
    setTrackbarPos("High H", "track", high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S - 1, low_S);
    setTrackbarPos("Low S", "track", low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S + 1);
    setTrackbarPos("High S", "track", high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V - 1, low_V);
    setTrackbarPos("Low V", "track", low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V + 1);
    setTrackbarPos("High V", "track", high_V);
}



int main() {
    // Create pipeline
    dai::Pipeline pipeline;

    // Define source and output
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto xoutRgb = pipeline.create<dai::node::XLinkOut>();

    xoutRgb->setStreamName("rgb");

    // Properties
    camRgb->setPreviewSize(700, 700);
    camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);

    // Linking
    camRgb->preview.link(xoutRgb->input);

    // Connect to device and start pipeline
    dai::Device device(pipeline, dai::UsbSpeed::SUPER);

    cout << "Connected cameras: ";
    for(const auto& cam : device.getConnectedCameras()) {
        cout << cam << " ";
    }
    cout << endl;

    // Print USB speed
    cout << "Usb speed: " << device.getUsbSpeed() << endl;

    // Bootloader version
    if(device.getBootloaderVersion()) {
        cout << "Bootloader version: " << device.getBootloaderVersion()->toString() << endl;
    }

    // Output queue will be used to get the rgb frames from the output defined above
    auto qRgb = device.getOutputQueue("rgb", 4, false);
    
    // Trackbars to set thresholds for HSV values
    namedWindow("track");
    createTrackbar("Low H", "track", &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", "track",&high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", "track", &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", "track",&high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", "track", &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", "track", &high_V, max_value, on_high_V_thresh_trackbar);

    Mat frame, frame_HSV, frame_threshold, res;
    cv::Scalar color = cv::Scalar(0, 255, 0);

    while(true) {
        auto inRgb = qRgb->get<dai::ImgFrame>();
        frame = inRgb->getCvFrame();

        // Convert from BGR to HSV colorspace
        cvtColor(frame.clone(), frame_HSV, COLOR_BGR2HSV);
        // Detect the object based on HSV Range Values
        inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
        
        bitwise_and(frame, frame, res, frame_threshold.clone());


        cv::Moments m = moments(frame_threshold.clone(), false);
        cv::Point com(m.m10 / m.m00, m.m01 / m.m00);

        cv::drawMarker(frame, com, color, cv::MARKER_CROSS, 20, 5);
        cv::drawMarker(frame_threshold, com, color, cv::MARKER_CROSS, 20, 5);

        cv::putText(frame, "Obs", cv::Point(com.x - 0, com.y - 50), cv::FONT_HERSHEY_DUPLEX, 1, color);

    
        // Show the frames
        // Retrieve 'bgr' (opencv format) frame
        imshow("rgb", frame);
        imshow("track", frame_threshold);

        moveWindow("track", 300, 100);
        moveWindow("rgb", 1000, 100);
        resizeWindow("rgb", 700, 700);
        resizeWindow("track", 700, 700);
        
        int key = cv::waitKey(1);
        if(key == 27 ) {
            break;
        }
    }
    return 0;
}