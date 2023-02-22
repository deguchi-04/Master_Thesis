#ifndef CNN_H
#define CNN_H

//basic C++ includes
#include <iostream>
#include <stdio.h>
#include <vector>
#include <dirent.h> 
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

//Deep Learning bib
#include <mlpack.hpp>
#include <iostream>

class VisualConcept {
   public:
      double length;   // Length of a box
      double breadth;  // Breadth of a box
      double height;   // Height of a box
};



#endif