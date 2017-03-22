#include "common.h"
#include "pascal/Annotation_Include.h"
#include "BinaryCvMat.hpp"
#include <boost/filesystem.hpp>
#include <random>
#include <iostream>
#include <fstream>

#include <opencv2/ml/ml.hpp>

using namespace boost::filesystem;

/**
 * Applies a callback for each entry in a directory
 */
void applyForDir(string dirPath, std::function<void (const path)> fn) {
  path p(dirPath);
  for(auto i = directory_iterator(p); i != directory_iterator(); i++) {
    path tmpPath = i->path();
    string name = tmpPath.filename().string();
    if(name == "." || name == ".." || name[0] == '.') continue;
    fn(tmpPath);
  }
}

/**
 * Generated prediceted bounding boxes for image
 */
void predictImageLocations(Mat im, Mat features, Ptr<Boost> model, std::vector<cv::Rect>& rects, int windowWidth, int windowHeight, int stride) {
  Mat samples(1, features.rows, CV_32FC1);
  int rows = im.rows;
  int cols = im.cols;

  for(int i = 0; i < rows; i += stride) {

    for(int j = 0; j < cols; j += stride) {

      if(j + windowWidth > cols || i + windowHeight > rows) continue;

      Rect roi = Rect(j, i, windowWidth, windowHeight);
      Mat image = im(roi);
      vector<Mat> channels;
      computeICF(channels, image, 1);
      computeICFVector(samples, features, channels, 0);

      float prediction = model->predict(samples.row(0));

      if(prediction < 1) {
        rects.push_back(roi);
      }

    }

  }

}


/**
 * Tested INRIA anotated dataset
 */
void testPascalDataDir(string intDirPath) {

  int max = 0;
  int count = 0;
  int negativeCount = 0;
  int featureLength = 10000;

  Mat features;
  Ptr<Boost> model = Algorithm::load<Boost>(intDirPath + "/adaboost_model.bin");
  LoadMatBinary(intDirPath + "/features.bin", features);

  omp_lock_t writelock;
  omp_init_lock(&writelock);

  applyForDir(intDirPath + "/Test/anonations", [&](const path filePath)->void {

    Annotation::Data d;
    d.read(filePath.string());
    vector<cv::Rect> rects;
    Mat image = imread(intDirPath  + "/" + d.getImageName(), 1);

    cout << d.getImageName() << endl;

    int dWidth = 320;
    float newScale = (float)dWidth / (float)image.cols;
    resize(image, image, Size(image.cols * newScale, image.rows * newScale));

    #pragma omp parallel for
    for(int k = -10; k <= 10; k++) {

      float scale = powf(2, k / 10.0);
      Mat tmpImage;
      resize(image, tmpImage, Size(), scale, scale);

      vector<cv::Rect> tmpRects;
      predictImageLocations(tmpImage, features, model, tmpRects, 96, 160, 8);

      for(auto const& value: tmpRects) {
        omp_set_lock(&writelock);
        rects.push_back(Rect(value.x / scale, value.y / scale, value.width / scale, value.height / scale));
        omp_unset_lock(&writelock);
      }

    }

    vector<cv::Rect> finalRects;
    nms(rects, finalRects, 0.6);
    for(auto const& value: finalRects) {
      rectangle(image, value, Scalar(0, 255, 0), 1);
    }

    int peopleCount = d.getElements().size();
    for(int i = 0; i < peopleCount; i++) {
      rectangle(image, Point(d[i].getXmin() * newScale, d[i].getYmin() * newScale), Point(d[i].getXmax() * newScale, d[i].getYmax() * newScale), Scalar(255, 0, 0), 1);
    }

    imshow("Test", image);
    waitKey(0);

  });

}

/**
 * Builds features and trains classifier
 */
void buildDirFeatures(string intDirPath, string outFilePath, int cellWidth, int cellHeight) {
  int featureLength = 10000;

  Mat features = generateRandomFeatures(featureLength, 8, 96, 160, 5, 5);
  vector<string> files;

  applyForDir(intDirPath + "/pos/train", [&](const path filePath)->void {
    files.push_back(filePath.string());
  });
  int positiveCount = files.size();
  cout << "Positive Samples: " << positiveCount << endl;

  applyForDir(intDirPath + + "/neg/train", [&](const path filePath)->void {
    files.push_back(filePath.string());
  });
  int negativeCount = files.size() - positiveCount;
  cout << "Negattive Count: " << negativeCount << endl;

  int samplesCount = files.size();
  cout << "Samples: " << samplesCount << endl;

  Mat samples = Mat::zeros(samplesCount, featureLength, CV_32FC1);
  Mat labels = Mat(samplesCount, 1, CV_32SC1, 0.0);

  labels(Range(positiveCount, samplesCount), Range::all()).setTo(Scalar(1.0));

  SaveMatBinary(outFilePath + "/features.bin", features);

  #pragma omp parallel for
  for(int i = 0; i < samplesCount; i++) {
    Mat image = imread(files[i], 1);
    vector<Mat> channels;
    computeICF(channels, image, 1);
    computeICFVector(samples, features, channels, i);
  }

  auto model = cv::ml::Boost::create();
  model->setBoostType(cv::ml::Boost::DISCRETE);
  model->setWeakCount(1000);
  model->setMaxDepth(2);
  model->setWeightTrimRate(0.95);
  model->train(samples, ml::ROW_SAMPLE, labels);
  model->save(outFilePath + "/adaboost_model.bin");

}
