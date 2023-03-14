#include "../include/cnn.h"


using namespace std;
using namespace cv;

template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin()+1, v.end(), 0.0) / v.size();
}

int main(int argc, char **argv)
{
    std::string projectBasePath = "/home/thaidy/Documents/Master_Thesis/src/files/";
    
    bool runOnGPU = false;

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

    //
    // Pass in either:
    //
    // "/source/models/yolov8s.onnx"
    // or
    // "/source/models/yolov5s.onnx"
    //
    // To run Inference with yolov8/yolov5 (ONNX)
    //
    
    Inference inf("/home/thaidy/Documents/Master_Thesis/src/files/best.onnx", cv::Size(640, 640),
                  projectBasePath + "classes.txt", runOnGPU);

    std::vector<std::string> imageNames;
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/files/uva.png");
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/files/uva2.png");
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/files/uva22.jpg");
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/files/uva33.jpg");
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/files/uva244.jpg");
    VideoCapture cap(0);
    vector<int> avgfps;
    vector<float> avgtime;
     int i =0;
    Mat frame;
    while(true) {
        auto inRgb = qRgb->get<dai::ImgFrame>();
        frame = inRgb->getCvFrame();
        //cv::Mat frame = cv::imread(imageNames[i]);
        //cap.read(frame);
        auto start = getTickCount();
        // Inference starts here...
        std::vector<Detection> output = inf.runInference(frame);

        int detections = output.size();
        std::cout << "Number of detections:" << detections << std::endl;

        for (int i = 0; i < detections; ++i)
        {
            Detection detection = output[i];

            cv::Rect box = detection.box;
            cv::Scalar color = detection.color;

            // Detection box
            cv::rectangle(frame, box, color, 2);

            // Detection box text
            std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
            cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
            cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

            cv::rectangle(frame, textBox, color, cv::FILLED);
            cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
        }
        // Inference ends here...
        auto end = getTickCount();
        // This is only for preview purposes
        float scale = 0.8;
        auto total = (end-start)/( getTickFrequency());
        
        cv::putText(frame, "FPS: " + std::to_string(int(round(int(1/total)))), cv::Point(50,50), FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,255,0),2,false);
        cout << "time: " << total << endl;
        avgfps.push_back(round(int(1/total)));
        avgtime.push_back(total);
        cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
        cv::imwrite("/home/thaidy/result/result" + to_string(i) + ".png", frame);
        cv::imshow("Inference", frame);
        
        i++;
        //waitKey(1000);
        if (waitKey(5) >= 0)
             break;
        
    }
    cout << "AVG FPS: " << getAverage(avgfps) << endl;
    cout << "AVG TIME: " << getAverage(avgtime) << endl;
}

