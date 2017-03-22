#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <math.h>

// #include <omp.h>

#include "lib/common.h"

using namespace cv;
using namespace std;
using namespace boost::filesystem;

#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 128
#define CELL_SIZE_W 8
#define CELL_SIZE_H 8
#define BINS 9
#define BLOCK_SIZE 2

int main(int argc, char **argv) {
  string command(argv[1]);

  if(command == "build_features") {
    string dataPath(argv[2]);
    string outPath(argv[3]);

    auto start = std::chrono::system_clock::now();
    buildDirFeatures(dataPath, outPath, CELL_SIZE_W, CELL_SIZE_H);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    auto elapsedSe = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    auto elapsedMSe = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Time :" << elapsedSe.count() << "." << elapsedMSe.count() << '\n';
  }

  else if(command == "test_data") {
    string dataPath(argv[2]);
    testPascalDataDir(dataPath);
  }

  else if(command == "view") {
    string imagePath(argv[2]);
    Mat dxKernel = (Mat_<float>(1,3) << -1.0f, 0.0f, 1.0f);
    Mat dyKernel = (Mat_<float>(3,1) << 1.0f, 0.0f, -1.0f);

    Mat image = imread(imagePath, 1);
    vector<Mat> channels;
    computeICF(channels, image, 1);
    int l = channels.size();
    for(int i = 0; i < l; i++) imshow(std::to_string(i), channels[i]);
    waitKey(0);
  }

  return 0;
}
