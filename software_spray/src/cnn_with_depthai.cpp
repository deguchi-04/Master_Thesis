// #include <iostream>

// // Includes common necessary includes for development using depthai library
// #include "depthai/depthai.hpp"

// // MobilenetSSD label texts
// static const std::vector<std::string> labelMap = {"grape_bunch"};

// int main(int argc, char** argv) {
//     using namespace std;
//     // Default blob path provided by Hunter private data download
//     // Applicable for easier example usage only
//     std::string nnPath("/home/thaidy/Documents/resultv5/best_openvino_2022.1_6shave.blob");

//     // If path to blob specified, use that
//     if(argc > 1) {
//         nnPath = std::string(argv[1]);
//     }

//     // Print which blob we are using
//     printf("Using blob at path: %s\n", nnPath.c_str());

//     // Create pipeline
//     dai::Pipeline pipeline;

//     // Define sources and outputs
//     auto camRgb = pipeline.create<dai::node::ColorCamera>();
//     auto videoEncoder = pipeline.create<dai::node::VideoEncoder>();
//     auto nn = pipeline.create<dai::node::YoloDetectionNetwork>();

//     auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
//     auto videoOut = pipeline.create<dai::node::XLinkOut>();
//     auto nnOut = pipeline.create<dai::node::XLinkOut>();

//     xoutRgb->setStreamName("rgb");
//     videoOut->setStreamName("h265");
//     nnOut->setStreamName("nn");

//     // Properties
//     camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
//     camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
//     camRgb->setPreviewSize(512,512 );
//     camRgb->setInterleaved(false);
//     pipeline.setXLinkChunkSize(0);

//     videoEncoder->setDefaultProfilePreset(30, dai::VideoEncoderProperties::Profile::H265_MAIN);

//     nn->setConfidenceThreshold(0.5f);
//     nn->setNumClasses(1);
//     nn->setCoordinateSize(4);
    // nn->setAnchors({ 11.703125,
    //             13.109375,
    //             21.25,
    //             29.125,
    //             34.625,
    //             39.875,
    //             30.078125,
    //             67.3125,
    //             47.84375,
    //             63.75,
    //             45.59375,
    //             100.1875,
    //             66.125,
    //             83.8125,
    //             81.0625,
    //             129.25,
    //             162.125,
    //             269.75});
    // nn->setAnchorMasks({{"side64", {0, 1, 2}}, {"side32", {3, 4, 5}}, {"side16",{6,7,8}}});
//     nn->setIouThreshold(0.5f);
//     nn->setBlobPath(nnPath);
//     nn->setNumInferenceThreads(2);
//     nn->input.setBlocking(false);
//     nn->setNumInferenceThreads(2); // By default 2 threads are used
//     nn->setNumNCEPerInferenceThread(1); //By default, 1 NCE is used per thread
    
//     // Linking
//     camRgb->video.link(videoEncoder->input);
//     camRgb->preview.link(xoutRgb->input);
//     camRgb->preview.link(nn->input);
//     videoEncoder->bitstream.link(videoOut->input);
//     nn->out.link(nnOut->input);

//     // Connect to device and start pipeline
//     dai::Device device(pipeline);

//     // Queues
//     int queueSize = 8;
//     auto qRgb = device.getOutputQueue("rgb", queueSize);
//     auto qDet = device.getOutputQueue("nn", queueSize);
//     auto qRgbEnc = device.getOutputQueue("h265", 30, true);

//     nn->input.setQueueSize(1);

//     cv::Mat frame;
//     std::vector<dai::ImgDetection> detections;

//     // Add bounding boxes and text to the frame and show it to the user
//     auto displayFrame = [](std::string name, cv::Mat frame, std::vector<dai::ImgDetection>& detections) {
//         auto color = cv::Scalar(255, 0, 0);
//         // nn data, being the bounding box locations, are in <0..1> range - they need to be normalized with frame width/height
//         for(auto& detection : detections) {
//             int x1 = detection.xmin * frame.cols;
//             int y1 = detection.ymin * frame.rows;
//             int x2 = detection.xmax * frame.cols;
//             int y2 = detection.ymax * frame.rows;

//             uint32_t labelIndex = detection.label;
//             std::string labelStr = to_string(labelIndex);
//             if(labelIndex < labelMap.size()) {
//                 labelStr = labelMap[labelIndex];
//             }
//             cv::putText(frame, labelStr, cv::Point(x1 + 10, y1 + 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, color);
//             std::stringstream confStr;
//             confStr << std::fixed << std::setprecision(2) << detection.confidence * 100;
//             cv::putText(frame, confStr.str(), cv::Point(x1 + 10, y1 + 40), cv::FONT_HERSHEY_TRIPLEX, 0.5, color);
//             cv::rectangle(frame, cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)), color, cv::FONT_HERSHEY_SIMPLEX);
//         }
//         // Show the frame
//         cv::imshow(name, frame);
//     };

//     auto videoFile = std::ofstream("video.h264", std::ios::binary);

//     while(true) {
//         auto inRgb = qRgb->tryGet<dai::ImgFrame>();
//         auto inDet = qDet->tryGet<dai::ImgDetections>();

//         auto out = qRgbEnc->get<dai::ImgFrame>();
//         videoFile.write((char*)out->getData().data(), out->getData().size());

//         if(inRgb) {
//             frame = inRgb->getCvFrame();
//         }

//         if(inDet) {
//             detections = inDet->detections;
//         }

//         if(!frame.empty()) {
//             displayFrame("rgb", frame, detections);
//         }

//         int key = cv::waitKey(1);
//         if(key == 'q' || key == 'Q') {
//             break;
//         }
//     }
//     cout << "To view the encoded data, convert the stream file (.h265) into a video file (.mp4), using a command below:" << endl;
//     cout << "ffmpeg -framerate 30 -i video.h264 -c copy video.mp4" << endl;
//     return 0;
// }

#include <chrono>
#include <iostream>

// Includes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"

static const std::vector<std::string> labelMap = {"grape_bunch"};

static std::atomic<bool> syncNN{true};

template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

int main(int argc, char** argv) {
    using namespace std;
    using namespace std::chrono;
    
    std::string nnPath("/home/thaidy/Documents/yolo8n_Run_aug/result_nano8_512/best_openvino_2022.1_6shave.blob");

    // If path to blob specified, use that
    if(argc > 1) {
        nnPath = std::string(argv[1]);
    }

    // Print which blob we are using
    printf("Using blob at path: %s\n", nnPath.c_str());

    // Create pipeline
    dai::Pipeline pipeline;

    // Define sources and outputs
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto detectionNetwork = pipeline.create<dai::node::YoloDetectionNetwork>();
    auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
    auto nnOut = pipeline.create<dai::node::XLinkOut>();
    


    xoutRgb->setStreamName("rgb");
    nnOut->setStreamName("detections");

    // Properties
    camRgb->setPreviewSize(512, 512);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::BGR);
    camRgb->setFps(40);

    // detectionNetwork->setAnchors({ 11.703125,
    //             13.109375,
    //             21.25,
    //             29.125,
    //             34.625,
    //             39.875,
    //             30.078125,
    //             67.3125,
    //             47.84375,
    //             63.75,
    //             45.59375,
    //             100.1875,
    //             66.125,
    //             83.8125,
    //             81.0625,
    //             129.25,
    //             162.125,
    //             269.75});
    // detectionNetwork->setAnchorMasks({{"side64", {0, 1, 2}}, {"side32", {3, 4, 5}}, {"side16",{6,7,8}}});
    
    

    // Network specific settings
    detectionNetwork->setConfidenceThreshold(0.2f);
    detectionNetwork->setNumClasses(1);
    detectionNetwork->setCoordinateSize(4);
    detectionNetwork->setIouThreshold(0.2f);
    detectionNetwork->setBlobPath(nnPath);
    detectionNetwork->setNumInferenceThreads(2);
    detectionNetwork->input.setBlocking(false);
    
    vector<int> avgfps;
    vector<int> avgtime;
    // Linking
    camRgb->preview.link(detectionNetwork->input);
    if(syncNN) {
        detectionNetwork->passthrough.link(xoutRgb->input);
    } else {
        camRgb->preview.link(xoutRgb->input);
    }

    detectionNetwork->out.link(nnOut->input);

    // Connect to device and start pipeline
    dai::Device device(pipeline, dai::UsbSpeed::SUPER);
    
    // Output queues will be used to get the rgb frames and nn data from the outputs defined above
    auto qRgb = device.getOutputQueue("rgb", 4, false);
    auto qDet = device.getOutputQueue("detections", 4, false);

    cv::Mat frame;
    std::vector<dai::ImgDetection> detections;
    auto startTime = steady_clock::now();
    int counter = 0;
    float fps = 0;
    auto color2 = cv::Scalar(255, 255, 255);

    // Add bounding boxes and text to the frame and show it to the user
    auto displayFrame = [](std::string name, cv::Mat frame, std::vector<dai::ImgDetection>& detections) {
        auto color = cv::Scalar(255, 0, 0);
        // nn data, being the bounding box locations, are in <0..1> range - they need to be normalized with frame width/height
        for(auto& detection : detections) {
            int x1 = detection.xmin * frame.cols;
            int y1 = detection.ymin * frame.rows;
            int x2 = detection.xmax * frame.cols;
            int y2 = detection.ymax * frame.rows;

            uint32_t labelIndex = detection.label;
            std::string labelStr = to_string(labelIndex);
            if(labelIndex < labelMap.size()) {
                labelStr = labelMap[labelIndex];
            }
            cv::putText(frame, labelStr, cv::Point(x1 + 10, y1 + 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, 255);
            std::stringstream confStr;
            confStr << std::fixed << std::setprecision(2) << detection.confidence * 100;
            cv::putText(frame, confStr.str(), cv::Point(x1 + 10, y1 + 40), cv::FONT_HERSHEY_TRIPLEX, 0.5, 255);
            cv::rectangle(frame, cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)), color, cv::FONT_HERSHEY_SIMPLEX);
        }
        // Show the frame
        cv::imshow(name, frame);
    };

    while(true) {
        std::shared_ptr<dai::ImgFrame> inRgb;
        std::shared_ptr<dai::ImgDetections> inDet;

        if(syncNN) {
            inRgb = qRgb->get<dai::ImgFrame>();
            inDet = qDet->get<dai::ImgDetections>();
        } else {
            inRgb = qRgb->tryGet<dai::ImgFrame>();
            inDet = qDet->tryGet<dai::ImgDetections>();
        }

        counter++;
        auto currentTime = steady_clock::now();
        auto elapsed = duration_cast<duration<float>>(currentTime - startTime);
        if(elapsed > seconds(1)) {
            fps = counter / elapsed.count();
            counter = 0;
            startTime = currentTime;
        }

        if(inRgb) {
            frame = inRgb->getCvFrame();
            std::stringstream fpsStr;
            fpsStr << "NN fps: " << std::fixed << std::setprecision(2) << fps;
            cv::putText(frame, fpsStr.str(), cv::Point(2, inRgb->getHeight() - 4), cv::FONT_HERSHEY_TRIPLEX, 0.4, color2);
            avgfps.push_back(round(fps));
            avgtime.push_back(1/fps);
        }

        if(inDet) {
            detections = inDet->detections;
        }

        if(!frame.empty()) {
            displayFrame("rgb", frame, detections);
        }

        int key = cv::waitKey(1);
        if(key == 'q' || key == 'Q') {
             cout << "AVG FPS: " << getAverage(avgfps) << endl;
            cout << "AVG TIME: " << getAverage(avgtime) << endl;
            return 0;
        }
    }
   
    return 0;
}