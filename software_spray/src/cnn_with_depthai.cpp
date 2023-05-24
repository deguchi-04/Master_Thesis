#include "../include/inference.h"

int main(int argc, char** argv) {
    std::string nnPath = "src/files/yolo8n_model_best/result640/best_openvino_2022.1_6shave.blob";
    std::vector<int> avgfps;
    cv::Mat frame, frame_HSV, frame_threshold, res;
    auto color2 = cv::Scalar(0, 255, 0);

    if (argc > 1)
        nnPath = std::string(argv[1]);

    printf("Using blob at path: %s\n", nnPath.c_str());

    dai::Pipeline pipeline;
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto detectionNetwork = pipeline.create<dai::node::YoloDetectionNetwork>();
    auto objectTracker = pipeline.create<dai::node::ObjectTracker>();
    auto xlinkOut = pipeline.create<dai::node::XLinkOut>();
    auto trackerOut = pipeline.create<dai::node::XLinkOut>();

    xlinkOut->setStreamName("preview");
    trackerOut->setStreamName("tracklets");

    camRgb->setPreviewSize(640, 640);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::BGR);
    camRgb->setFps(20);

    detectionNetwork->setConfidenceThreshold(0.5f);
    detectionNetwork->setNumClasses(1);
    detectionNetwork->setCoordinateSize(4);
    detectionNetwork->setIouThreshold(0.5f);
    detectionNetwork->setBlobPath(nnPath);
    detectionNetwork->setNumInferenceThreads(2);
    detectionNetwork->input.setBlocking(false);

    objectTracker->setDetectionLabelsToTrack({0});
    objectTracker->setTrackerType(dai::TrackerType::ZERO_TERM_COLOR_HISTOGRAM);
    objectTracker->setTrackerIdAssignmentPolicy(dai::TrackerIdAssignmentPolicy::SMALLEST_ID);

    camRgb->preview.link(detectionNetwork->input);
    objectTracker->passthroughTrackerFrame.link(xlinkOut->input);
    (fullFrameTracking ? camRgb->video : detectionNetwork->passthrough).link(objectTracker->inputTrackerFrame);
    detectionNetwork->passthrough.link(objectTracker->inputDetectionFrame);
    detectionNetwork->out.link(objectTracker->inputDetections);
    objectTracker->out.link(trackerOut->input);

    dai::Device device(pipeline);
    auto preview = device.getOutputQueue("preview", 4, false);
    auto tracklets = device.getOutputQueue("tracklets", 4, false);

    auto startTime = steady_clock::now();
    int counter = 0;
    float fps = 0;

    while (true) {
        auto imgFrame = preview->get<dai::ImgFrame>();
        auto track = tracklets->get<dai::Tracklets>();

        counter++;
        auto currentTime = steady_clock::now();
        auto elapsed = duration_cast<duration<float>>(currentTime - startTime);

        if (elapsed > seconds(1)) {
            fps = counter / elapsed.count();
            counter = 0;
            startTime = currentTime;
        }

        cv::Mat frame = imgFrame->getCvFrame();
        cv::cvtColor(frame.clone(), frame_HSV, cv::COLOR_BGR2HSV);
        cv::inRange(frame_HSV, cv::Scalar(0, 210, 210), cv::Scalar(180, 255, 255), frame_threshold);
        cv::bitwise_and(frame, frame, res, frame_threshold.clone());

        cv::Moments m = cv::moments(frame_threshold.clone(), false);
        cv::Point comLaser(m.m10 / m.m00, m.m01 / m.m00);

        cv::drawMarker(frame, comLaser, cv::Scalar(0, 255, 0), cv::MARKER_SQUARE, 20, 5);
        cv::drawMarker(frame_threshold, comLaser, cv::Scalar(0, 255, 0), cv::MARKER_SQUARE, 20, 5);
        cv::putText(frame, "Obs", cv::Point(comLaser.x - 0, comLaser.y - 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0));

        for (auto& t : track->tracklets) {
            auto roi = t.roi.denormalize(frame.cols, frame.rows);
            int x1 = roi.topLeft().x;
            int y1 = roi.topLeft().y;
            int x2 = roi.bottomRight().x;
            int y2 = roi.bottomRight().y;

            uint32_t labelIndex = t.label;
            std::string labelStr = (labelIndex < labelMap.size()) ? labelMap[labelIndex] : std::to_string(labelIndex);
            cv::putText(frame, labelStr, cv::Point(x1 + 70, y1 - 40), cv::FONT_HERSHEY_TRIPLEX, 0.4 , cv::Scalar(23, 38, 154));

            float centerX = (x2 + x1) / 2.0f;
            float centerY = (y2 + y1) / 2.0f;
            cv::Point com(centerX, centerY);
            cv::drawMarker(frame, com, cv::Scalar(0, 255, 0), cv::MARKER_CROSS, 20, 5);

            std::stringstream idStr;
            idStr << "ID: " << t.id +1 ;
            cv::putText(frame, idStr.str(), cv::Point(x1 + 10, y1 - 40), cv::FONT_HERSHEY_TRIPLEX, 0.6 , cv::Scalar(23, 38, 154));
            
            std::stringstream statusStr;
            statusStr << "Status: " << t.status;

            cv::putText(frame, "Status: " + statusStr.str(), cv::Point(x1 + 10, y1 - 20), cv::FONT_HERSHEY_TRIPLEX, 0.4 , cv::Scalar(23, 38, 154));
            cv::rectangle(frame, cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)), cv::Scalar(0, 255, 0), cv::FONT_HERSHEY_SIMPLEX);

            if (comLaser.x > 640 || comLaser.y > 640 || comLaser.x < 0 || comLaser.y < 0) {
                comLaser.x = 0;
                comLaser.y = 0;
            }
            std::cout << "Detection: " << centerX << "," << centerY << "," << comLaser.x << "," << comLaser.y << "," << t.id +1 << std::endl;
        }

        std::stringstream fpsStr;
        fpsStr << "NN fps:" << std::fixed << std::setprecision(2) << fps;
        cv::putText(frame, fpsStr.str(), cv::Point(2, imgFrame->getHeight() - 4), cv::FONT_HERSHEY_TRIPLEX, 0.6, cv::Scalar(0, 255, 0));
        avgfps.push_back(std::round(fps));

        cv::imshow("tracker", frame);
        cv::imshow("track", frame_threshold);
        cv::moveWindow("track", 1000, 100);

        int key = cv::waitKey(1);
        if (key == 'q' || key == 'Q') {
            return 0;
        }
    }
    return 0;
}
