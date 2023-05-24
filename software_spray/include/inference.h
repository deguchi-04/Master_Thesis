#ifndef INFERENCE_H
#define INFERENCE_H

#include <chrono>
#include <iostream>
#include <unordered_map>

// Includes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"

static const std::vector<std::string> labelMap = {"grape_bunch"};
static std::atomic<bool> fullFrameTracking{false};


using namespace std;
using namespace std::chrono;

template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

#endif