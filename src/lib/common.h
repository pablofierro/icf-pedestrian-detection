#pragma once
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>
#include <boost/filesystem.hpp>
#include <chrono>

#include <omp.h>

using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace boost::filesystem;

void computeHOG(vector<float>&outVector, Mat source, Mat &dxKernel, Mat &dyKernel, bool previewFeatures, int blockSize, int cellWidth, int cellHeight, int width, int height);

void computeICF(vector<Mat>&channels, Mat _image, int mode);
void computeICFVector(Mat samples, Mat features, vector<Mat>&channels, int row);
Mat generateRandomFeatures(int count, int channelCount, int maxWidth, int maxHeight, int minWidth, int minHeight);
float calculateIntegralSum(Mat ch, int col, int row, int colEnd, int rowEnd);
void testDataDir(string intDirPath);
void testPascalDataDir(string intDirPath);
void nms(const std::vector<cv::Rect>& srcRects, std::vector<cv::Rect>& resRects, float thresh);

void applyForDir(string dirPath, std::function<void (const path)> fn);
void buildDirFeatures(string intDirPath, string outDirPath, int cellWidth, int cellHeight);
