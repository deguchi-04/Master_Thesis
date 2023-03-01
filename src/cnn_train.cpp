#include <iostream>
#include <vector>
#include <getopt.h>

#include <opencv2/opencv.hpp>

#include "../include/cnn.h"

using namespace std;
using namespace cv;


Inference::Inference(const std::string &onnxModelPath, const cv::Size2f &modelInputShape, const std::string &classesTxtFile, const bool &runWithCuda)
{
    modelPath = onnxModelPath;
    modelShape = modelInputShape;
    classesPath = classesTxtFile;
    cudaEnabled = runWithCuda;

    loadOnnxNetwork();
    loadClassesFromFile();
}

std::vector<Detection> Inference::runInference(const cv::Mat &input)
{
    cv::Mat modelInput = input;
    if (letterBoxForSquare && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    bool yolov8 = false;
    // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
    // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
    if (dimensions > rows) // Check if the shape[2] is more than shape[1] (yolov8)
    {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }
    float *data = (float *)outputs[0].data;

    float x_factor = modelInput.cols / modelShape.width;
    float y_factor = modelInput.rows / modelShape.height;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        if (yolov8)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > modelScoreThreshold)
            {
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        else // yolov5
        {
            float confidence = data[4];

            if (confidence >= modelConfidenseThreshold)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > modelScoreThreshold)
                {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    std::vector<Detection> detections{};
    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(100, 255);
        result.color = cv::Scalar(dis(gen),
                                  dis(gen),
                                  dis(gen));

        result.className = classes[result.class_id];
        result.box = boxes[idx];

        detections.push_back(result);
    }

    return detections;
}

void Inference::loadClassesFromFile()
{
    std::ifstream inputFile(classesPath);
    if (inputFile.is_open())
    {
        std::string classLine;
        while (std::getline(inputFile, classLine))
            classes.push_back(classLine);
        inputFile.close();
    }
}

void Inference::loadOnnxNetwork()
{
    net = cv::dnn::readNetFromONNX(modelPath);
    if (cudaEnabled)
    {
        std::cout << "\nRunning on CUDA" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        std::cout << "\nRunning on CPU" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

cv::Mat Inference::formatToSquare(const cv::Mat &source)
{
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}

int main(int argc, char **argv)
{
    std::string projectBasePath = "/home/thaidy/Documents/Master_Thesis/src/files/";
    
    bool runOnGPU = true;

    //
    // Pass in either:
    //
    // "/source/models/yolov8s.onnx"
    // or
    // "/source/models/yolov5s.onnx"
    //
    // To run Inference with yolov8/yolov5 (ONNX)
    //
    
    Inference inf(projectBasePath + "last.onnx", cv::Size(640, 640),
                  projectBasePath + "classes.txt", runOnGPU);

    std::vector<std::string> imageNames;
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/uva.png");
    imageNames.push_back("/home/thaidy/Documents/Master_Thesis/src/uva2.png");

    for (int i = 0; i < imageNames.size(); ++i)
    {
        cv::Mat frame = cv::imread(imageNames[i]);

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

        // This is only for preview purposes
        float scale = 0.8;
        cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
        cv::imshow("Inference", frame);

        cv::waitKey(-1);
    }
}

// #include <iostream>
// #include <fstream>
// #include <opencv2/opencv.hpp>
// #include <opencv2/dnn.hpp>
// #include <opencv2/dnn/all_layers.hpp>

// using namespace cv;
// using namespace dnn;
// using namespace std;

// const float INPUT_WIDTH = 640.0;
// const float INPUT_HEIGHT = 640.0;
// const float SCORE_THRESHOLD = 0.5;
// const float NMS_THRESHOLD = 0.45;
// const float CONFIDENCE_THRESHOLD = 0.45;


// const float FONT_SCALE = 0.7;
// const int FONT_FACE = FONT_HERSHEY_SIMPLEX;
// const int THICKNESS = 1;

// Scalar BLACK = Scalar(0,0,0);
// Scalar BLUE = Scalar(255,178,50);
// Scalar YELLOW = Scalar(0,255,255);
// Scalar RED = Scalar(0,0,255);

// const string caffeConfigFile = "/home/thaidy/Documents/Master_Thesis/src/files/last.onnx";
// const double inScaleFactor = 1.0;
// const float confidenceThreshold = 0.7;
// const Scalar meanVal(184.0, 177.0,123.0);

// void detectFaceOpenCVDNN(Net net, Mat &frameOpenCVDNN){
//   int frameHeight = frameOpenCVDNN.rows;
//   int frameWidht = frameOpenCVDNN.cols;

//   Mat inputBlob;
//   inputBlob = blobFromImage(frameOpenCVDNN, inScaleFactor, Size(INPUT_WIDTH,INPUT_HEIGHT), meanVal, false,false);
  

//   net.setInput(inputBlob,"data");
//   Mat detection = net.forward("detection_out");

//   Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

//   for(int i = 0; i < detectionMat.rows; i++){
//     float confidence = detectionMat.at<float>(i,2);
//     if(confidence > confidenceThreshold){
//       int x1 = static_cast<int>(detection.at<float>(i,3)*frameWidht);
//       int y1 = static_cast<int>(detection.at<float>(i,4)*frameHeight);
//       int x2 = static_cast<int>(detection.at<float>(i,5)*frameWidht);
//       int y2 = static_cast<int>(detection.at<float>(i,6)*frameHeight);

//       rectangle(frameOpenCVDNN, cv::Point(x1,y1), cv::Point(x2,y2), cv::Scalar(0,255,0),2,4);
//     }
//   }
// }

// void draw_label(Mat &input_image, string label, int left, int top){
//   int baseLine;
//   Size label_size = getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
//   top = max(top, label_size.height);

//   Point tlc = Point(left, top);
//   Point brc = Point(left+label_size.width, top+label_size.height+baseLine);
//   rectangle(input_image, tlc, brc, BLACK, FILLED);
//   putText(input_image, label, Point(left,top+label_size.height), FONT_FACE, FONT_SCALE,YELLOW,THICKNESS);

// }

// vector<Mat> pre_process(Mat &input_image, Net &net){
//   Mat blob;
//   blobFromImage(input_image, blob, 1./255, Size(INPUT_WIDTH,INPUT_HEIGHT), Scalar(), true, false);
//   net.setInput(blob);
//   vector<Mat> outputs;
//   net.forward(outputs, net.getUnconnectedOutLayersNames());
//   return outputs;
// }

// Mat post_process(Mat &input_image, vector<Mat> &outputs, const vector<string> &class_names){
//   vector<int> class_ids;
//   vector<float> confidence;
//   vector<Rect> boxes;

//   float x_factor = input_image.cols/INPUT_WIDTH;
//   float y_factor = input_image.rows/INPUT_HEIGHT;

//   float *data = (float*)outputs[0].data;

//   const int dimensions = 85;
// }
// // int main(int, char**){

// //   string file_path = "../src/files/";
// //   vector<string> class_names;
// //   ifstream ifs(string(file_path + "classes.txt").c_str());
// //   string line;

// //   while(getline(ifs, line))
// //   {
// //     cout << line << endl;
// //     class_names.push_back(line);
// //   }

// //   auto net = readNetFromONNX(file_path + "last.onnx");

// //   VideoCapture cap(0);

// //   // net.setPreferableBackend(DNN_TARGET_CUDA_FP16);
// //   // net.setPreferableTarget(DNN_TARGET_CUDA_FP16);

// //   float min_confidence_score = 0.5;

// //   while (cap.isOpened())
// //   {
// //     Mat image;
// //     bool isSuccess = cap.read(image);
// //     if(!isSuccess){
// //       cout << "Could not Load Image" << endl;
// //       break;
// //     }

// //     int image_heigh = image.cols;
// //     int image_width = image.rows;
    
// //     auto start = getTickCount();

// //     Mat blob = blobFromImage(image, 1.0, Size(640,640), Scalar(127.5, 127.5, 127.5),true,false);
// //     net.setInput(blob);
    
// //     Mat output = net.forward();
// //     auto end = getTickCount();

// //     Mat results(output.size[2], output.size[3], CV_32F, output.ptr<float>());

// //     for(int i=0; i < results.rows; i++){
// //       int class_id = int(results.at<float>(i,1));
// //       float confidence = results.at<float>(i,2);

// //       if(confidence > min_confidence_score){
// //         int bboxX = int(results.at<float>(i,3)*image.cols);
// //         int bboxY = int(results.at<float>(i,4)*image.rows);
// //         int bboxWidht = int(results.at<float>(i,5)*image.rows-bboxX);
// //         int bboxHeight = int(results.at<float>(i,6)*image.rows-bboxY);
// //         rectangle(image, Point(bboxX, bboxY), Point(bboxX+bboxWidht, bboxY+bboxHeight), Scalar(0,0,255),2);
// //         string class_name = class_names[class_id-1];
// //         putText(image, class_name + " " + to_string(int(confidence*100))+ "%", Point(bboxX,bboxY-10), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,0),2);

// //       }
// //     }

// //   auto totalTime = (end-start) / getTickFrequency();
// //   cout << "time " << totalTime << endl;

// //   putText(image, "FPS: " + to_string(int(1/totalTime)), Point(50,50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,0),2, false);
// //   imshow("image", image);
// //   int k = waitKey(10);
// //   if(k == 113){
// //     break;
// //   }
// //   }
// //   cap.release();
// //   destroyAllWindows();

// // }