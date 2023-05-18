#include "../include/inference.h"
#include <unordered_map>

int main(int argc, char** argv)
{
    std::string nnPath = "src/files/yolo8n_model_best/result640/best_openvino_2022.1_6shave.blob";
    std::vector<int> avgfps;
    std::vector<int> avgtime;
    cv::Mat frame;
    cv::Mat frame_HSV, frame_threshold, res;
    std::vector<dai::ImgDetection> detections;
    auto startTime = std::chrono::steady_clock::now();
    int counter = 0;
    float fps = 0;
    auto color2 = cv::Scalar(0, 255, 0);

    if (argc > 1)
        nnPath = std::string(argv[1]);

    printf("Using blob at path: %s\n", nnPath.c_str());

    dai::Pipeline pipeline;
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto detectionNetwork = pipeline.create<dai::node::YoloDetectionNetwork>();
    auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
    auto nnOut = pipeline.create<dai::node::XLinkOut>();

    xoutRgb->setStreamName("rgb");
    nnOut->setStreamName("detections");

    camRgb->setPreviewSize(640, 640);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::BGR);
    camRgb->setFps(30);

    detectionNetwork->setConfidenceThreshold(0.5f);
    detectionNetwork->setNumClasses(1);
    detectionNetwork->setCoordinateSize(4);
    detectionNetwork->setIouThreshold(0.5f);
    detectionNetwork->setBlobPath(nnPath);
    detectionNetwork->setNumInferenceThreads(2);
    detectionNetwork->input.setBlocking(false);

    camRgb->preview.link(detectionNetwork->input);
    (syncNN ? detectionNetwork->passthrough : camRgb->preview).link(xoutRgb->input);
    detectionNetwork->out.link(nnOut->input);

    dai::Device device(pipeline, dai::UsbSpeed::SUPER);
    auto qRgb = device.getOutputQueue("rgb", 4, false);
    auto qDet = device.getOutputQueue("detections", 4, false);

    std::unordered_map<int, int> idMap; // Map to store unique IDs for each grape
    
    auto displayFrame = [](const std::string& name, const cv::Mat& frame, const std::vector<dai::ImgDetection>& detections, std::unordered_map<int, int>& idMap, const cv::Mat& frame_HSV, const cv::Mat& frame_threshold) {
        auto color = cv::Scalar(0, 255, 0);
        cv::Mat res;
    for (size_t i = 0; i < detections.size(); i++) {
        const auto& detection = detections[i];
        const int x1 = detection.xmin * frame.cols;
        const int y1 = detection.ymin * frame.rows;
        const int x2 = detection.xmax * frame.cols;
        const int y2 = detection.ymax * frame.rows;

        const uint32_t labelIndex = detection.label;
        const std::string labelStr = (labelIndex < labelMap.size()) ? labelMap[labelIndex] : std::to_string(labelIndex);
        const float centerX = (x2 + x1) / 2.0f;
        const float centerY = (y2 + y1) / 2.0f;
        cv::Point com(centerX, centerY);
        cv::drawMarker(frame, com, color, cv::MARKER_CROSS, 20, 5);
        

        std::stringstream idStr;
        int id_number = i + 1;
        idStr << "ID: " << i + 1;  // Assign unique ID based on detection index
        cv::putText(frame, idStr.str(), cv::Point(x1 + 10, y1 - 40), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(23, 38, 154));

        cv::putText(frame, labelStr, cv::Point(x1 + 60, y1 - 40), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(23, 38, 154));
        std::stringstream confStr;
        confStr << std::fixed << std::setprecision(2) << detection.confidence * 100;
        cv::putText(frame, confStr.str(), cv::Point(x1 + 10, y1 - 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar(23, 38, 154));
        cv::rectangle(frame, cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)), color, cv::FONT_HERSHEY_SIMPLEX);

        cv::bitwise_and(frame, frame, res, frame_threshold.clone());

        cv::Moments m = cv::moments(frame_threshold.clone(), false);
        cv::Point comLaser(m.m10 / m.m00, m.m01 / m.m00);

        cv::drawMarker(frame, comLaser, color, cv::MARKER_SQUARE, 20, 5);
        cv::drawMarker(frame_threshold, comLaser, color, cv::MARKER_SQUARE, 20, 5);

        cv::putText(frame, "Obs", cv::Point(comLaser.x - 0, comLaser.y - 50), cv::FONT_HERSHEY_DUPLEX, 1, color);

        if(comLaser.x > 640 || comLaser.y > 640 || comLaser.x < 0 || comLaser.y < 0){
            comLaser.x = 0;
            comLaser.y = 0;
        }
        std::cout << "Detection: " << centerX << "," << centerY << "," << comLaser.x << "," << comLaser.y << "," << id_number << std::endl;
    }

    cv::imshow(name, frame);
    cv::imshow("track", frame_threshold);
    cv::moveWindow("track", 1000, 100);
};

    while (true) {
        const auto inRgb = syncNN ? qRgb->get<dai::ImgFrame>() : qRgb->tryGet<dai::ImgFrame>();
        const auto inDet = syncNN ? qDet->get<dai::ImgDetections>() : qDet->tryGet<dai::ImgDetections>();

        counter++;
        const auto currentTime = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - startTime);
        if (elapsed > std::chrono::seconds(1)) {
            fps = counter / elapsed.count();
            counter = 0;
            startTime = currentTime;
        }

        if (inRgb) {
            frame = inRgb->getCvFrame();
            cv::cvtColor(frame.clone(), frame_HSV, cv::COLOR_BGR2HSV);
            cv::inRange(frame_HSV, cv::Scalar(0, 210, 210), cv::Scalar(180, 255, 255), frame_threshold);
            
            std::stringstream fpsStr;
            fpsStr << "NN fps: " << std::fixed << std::setprecision(2) << fps;
            cv::putText(frame, fpsStr.str(), cv::Point(2, inRgb->getHeight() - 4), cv::FONT_HERSHEY_TRIPLEX, 0.6, color2);
            avgfps.push_back(std::round(fps));
        }

        if (inDet) {
            detections = inDet->detections;
        }

        if (!frame.empty()) {
            
            displayFrame("rgb", frame, detections, idMap, frame_HSV, frame_threshold);
        }

        const int key = cv::waitKey(1);
        if (key == 'q' || key == 'Q') {
            return 0;
        }
    }

    return 0;
}
