#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <vector>
#include <thread>
#include <chrono>

using namespace cv;


std::vector<Point> cornerHarris(Mat &inputFrame, int thresh = 150, int blockSize = 3, int apertureSize = 3, double k = 0.04) {
    Mat grayFrame, resultFrameNorm, resultFrameNormScaled;
    Mat resultFrame = Mat::zeros(inputFrame.size(), CV_32FC1);
    cvtColor(inputFrame, grayFrame, COLOR_BGR2GRAY);
    cornerHarris(grayFrame, resultFrame, blockSize, apertureSize, k, BORDER_DEFAULT );
    normalize(resultFrame, resultFrameNorm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(resultFrameNorm, resultFrameNormScaled);

    std::vector<Point> points;

    for (int j=0; j<resultFrameNorm.rows; j++) {
        for (int i=0; i<resultFrameNorm.cols; i++) {
            if (int(resultFrameNorm.at<float>(j,i)) > thresh) {
                //
                points.push_back(Point{i, j});
            }
        }
    }

    return points;
}

std::vector<Point> cornerFast(Mat &inputFrame, int thresh=50) {
    Mat grayFrame;
    cvtColor(inputFrame, grayFrame, COLOR_BGR2GRAY);

    std::vector<KeyPoint> keyPoints;
    std::vector<Point> points;

    FAST(grayFrame, keyPoints, thresh);

    for (auto keyPoint: keyPoints) {
        points.push_back(Point{int(keyPoint.pt.x), int(keyPoint.pt.y)});
    }

    return points;
}

void drawPoints(Mat &frame, std::vector<Point> points, Scalar color, int lineWidht=2, int radius=5) {
    for (auto point: points)
        circle(frame, point, 5, color, lineWidht);
}

void featureMatching(Mat img1, Mat img2) {
    int minHessian = 400;

    //SurfFeatureDetector detector( minHessian );
}

int main() {
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

    namedWindow("previous", 1);
    namedWindow("current", 1);

    Mat previousFrame, currentFrame;
    std::vector<Point> previousPoints, currentPoints;

    //odczyt pierwszej ramki
    Mat frame;
    cap >> frame;
    previousFrame = frame.clone();
    previousPoints = cornerFast(previousFrame);
    drawPoints(previousFrame, previousPoints, Scalar{255, 0, 0});

    currentFrame = previousFrame.clone();
    currentPoints = previousPoints;

    while(1) {
        Mat frame;
        cap >> frame;
        previousFrame = currentFrame.clone();
        previousPoints = currentPoints;

        currentFrame = frame.clone();
        currentPoints = cornerFast(currentFrame);
        drawPoints(currentFrame, currentPoints, Scalar{255, 0, 0});

        imshow("previous", previousFrame);
        imshow("current", currentFrame);

        // std::this_thread::sleep_for(std::chrono::seconds(1));

        if(waitKey(30) >= 0)
            break;
    }

    return 0;
}

/*
 * int main( int argc, char** argv )
{
  if( argc != 3 )
  { readme(); return -1; }

  Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE );
  Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE );

  if( !img_1.data || !img_2.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector


  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( img_1, keypoints_1 );
  detector.detect( img_2, keypoints_2 );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_1, descriptors_2;

  extractor.compute( img_1, keypoints_1, descriptors_1 );
  extractor.compute( img_2, keypoints_2, descriptors_2 );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
  //-- small)
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance <= max(2*min_dist, 0.02) )
    { good_matches.push_back( matches[i]); }
  }

  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( img_1, keypoints_1, img_2, keypoints_2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imshow( "Good Matches", img_matches );

  for( int i = 0; i < (int)good_matches.size(); i++ )
  { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }

  waitKey(0);

  return 0;
}
 */