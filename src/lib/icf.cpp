#include "common.h"
#include <boost/filesystem.hpp>
#include <random>
#include <iostream>
#include <fstream>

/**
 * Generate vector of channels for an image
 */
void computeICF(vector<Mat>&channels, Mat _image, int mode) {

  Mat image;
  cvtColor(_image, image, CV_BGR2GRAY);

  // gray channel
  image.convertTo(image, CV_32FC1, 1.0 / 255.5);
  channels.push_back(image);

  Mat gradX;
  Mat gradY;

  Sobel(image, gradX, CV_32FC1, 1, 0, 1);
  Sobel(image, gradY, CV_32FC1, 0, 1, 1);

  Mat magnitude;
  cv::magnitude(gradX, gradY, magnitude);
  channels.push_back(magnitude);

  // orientations channels
  for(int i = 0; i < 6; i++) channels.push_back(Mat::zeros(image.rows, image.cols, CV_32FC1));

  for(int i = 0; i < image.rows; i++) {
    for(int j = 0; j < image.cols; j++) {
      float orientationValue = atan2(gradY.at<float>(i, j), gradX.at<float>(i, j)) * (180 / M_PI);
      if(orientationValue < 0) orientationValue += 180.0;
      int histIndex = (int)(orientationValue / 30);
      channels[2 + (histIndex-1)].at<float>(i,j) = magnitude.at<float>(i, j);
    }
  }

  if(mode == 1) {
    int l = channels.size();
    Mat out;
    for(int i = 0; i < l; i++) {
      integral(channels[i], out, CV_32F);
      channels[i] = out.clone();
    }
  }

}

/**
 * Compute feature vector
 */
void computeICFVector(Mat samples, Mat features, vector<Mat>&channels, int row) {
  int featureLength = features.rows;
  for(int i = 0; i < featureLength; i++) {
    int channel = features.at<int>(i, 0);
    samples.at<float>(row, i) = calculateIntegralSum(channels[channel], features.at<int>(i, 1), features.at<int>(i, 2), features.at<int>(i, 3), features.at<int>(i, 4));
  }
}

/**
 * Calculate integral sum

 */
float calculateIntegralSum(Mat ch, int col, int row, int colEnd, int rowEnd) {
  float S = ch.at<float>(rowEnd, colEnd);
  if (row - 1 >= 0 && col - 1 >= 0) S += ch.at<float>(row - 1, col - 1);
  if (row - 1 >= 0) S -= ch.at<float>(row - 1, colEnd);
  if (col - 1 >= 0) S -= ch.at<float>(rowEnd, col - 1);
  return S;
}

/**
 * Generates random rectangles
 */
typedef uniform_int_distribution<> uniform;
mt19937 gen(0);

Mat generateRandomFeatures(int count, int channelCount, int maxWidth, int maxHeight, int minWidth, int minHeight) {
  uniform channelGen(0, channelCount - 1);
  uniform rowGen(0, maxHeight - minHeight);
  uniform colGen(0, maxWidth - minWidth);

  Mat rects(count, 5, CV_32S);

  for(int i = 0; i < count; i++) {

    int row = rowGen(gen);
    int col = colGen(gen);

    uniform rowEndGen(row + minHeight, maxHeight);
    uniform colEndGen(col + minWidth, maxWidth);
    int rowEnd = rowEndGen(gen);
    int colEnd = colEndGen(gen);

    rects.at<int>(i, 0) = channelGen(gen);
    rects.at<int>(i, 1) = col;
    rects.at<int>(i, 2) = row;
    rects.at<int>(i, 3) = colEnd;
    rects.at<int>(i, 4) = rowEnd;

  }

  return rects;
}

void nms(const std::vector<cv::Rect>& srcRects, std::vector<cv::Rect>& resRects, float thresh) {
  resRects.clear();

  const size_t size = srcRects.size();
  if(!size) {
    return;
  }

  // Sort the bounding boxes by the bottom - right y - coordinate of the bounding box
  std::multimap<int, size_t> idxs;
  for (size_t i = 0; i < size; ++i) {
    idxs.insert(std::pair<int, size_t>(srcRects[i].br().y, i));
  }

  // keep looping while some indexes still remain in the indexes list
  while (idxs.size() > 0) {
    // grab the last rectangle
    auto lastElem = --std::end(idxs);
    const cv::Rect& rect1 = srcRects[lastElem->second];

    resRects.push_back(rect1);

    idxs.erase(lastElem);

    for (auto pos = std::begin(idxs); pos != std::end(idxs); ) {

      // grab the current rectangle
      const cv::Rect& rect2 = srcRects[pos->second];

      float intArea = (rect1 & rect2).area();
      float unionArea = rect1.area() + rect2.area() - intArea;
      float overlap = intArea / unionArea;

      // if there is sufficient overlap, suppress the current bounding box
      if(overlap > thresh) {
        pos = idxs.erase(pos);
      }
      else {
        ++pos;
      }
    }
  }
}
