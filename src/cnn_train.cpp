#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

int main(int, char**){

  string file_path = "/home/thaidy/Documents/Master_Thesis/src/";
  vector<string> class_names;
  ifstream ifs(string(file_path + "classes.txt").c_str());
  string line;

  while(getline(ifs, line))
  {
    cout << line << endl;
    class_names.push_back(line);
  }

  auto net = readNet(file_path + "opt.pb",
  file_path + "opt.pbtxt", "TensorFlow");

  VideoCapture cap(0);

  // net.setPreferableBackend(DNN_TARGET_CUDA_FP16);
  // net.setPreferableTarget(DNN_TARGET_CUDA_FP16);

  float min_confidence_score = 0.5;

  while (cap.isOpened())
  {
    Mat image;
    bool isSuccess = cap.read(image);
    if(!isSuccess){
      cout << "Could not Load Image" << endl;
      break;
    }

    int image_heigh = image.cols;
    int image_width = image.rows;
    
    auto start = getTickCount();

    Mat blob = blobFromImage(image, 1.0, Size(300,300), Scalar(127.5, 127.5, 127.5),true,false);
    net.setInput(blob);
    
    Mat output = net.forward();
    auto end = getTickCount();

    Mat results(output.size[2], output.size[3], CV_32F, output.ptr<float>());

    for(int i=0; i < results.rows; i++){
      int class_id = int(results.at<float>(i,1));
      float confidence = results.at<float>(i,2);

      if(confidence > min_confidence_score){
        int bboxX = int(results.at<float>(i,3)*image.cols);
        int bboxY = int(results.at<float>(i,4)*image.rows);
        int bboxWidht = int(results.at<float>(i,5)*image.rows-bboxX);
        int bboxHeight = int(results.at<float>(i,6)*image.rows-bboxY);
        rectangle(image, Point(bboxX, bboxY), Point(bboxX+bboxWidht, bboxY+bboxHeight), Scalar(0,0,255),2);
        string class_name = class_names[class_id-1];
        putText(image, class_name + " " + to_string(int(confidence*100))+ "%", Point(bboxX,bboxY-10), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,0),2);

      }
    }

  auto totalTime = (end-start) / getTickFrequency();
  cout << "time " << totalTime << endl;

  putText(image, "FPS: " + to_string(int(1/totalTime)), Point(50,50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0,255,0),2, false);
  imshow("image", image);
  int k = waitKey(10);
  if(k == 113){
    break;
  }
  }
  cap.release();
  destroyAllWindows();

}