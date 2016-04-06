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
using namespace cv::xfeatures2d;

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

    auto detector = SurfFeatureDetector::create(minHessian);
    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector->detect( img1, keypoints_1 );
    detector->detect( img2, keypoints_2 );

    auto extractor = SurfDescriptorExtractor::create();

    Mat descriptors_1, descriptors_2;

    extractor->compute(img1, keypoints_1, descriptors_1);
    extractor->compute(img2, keypoints_2, descriptors_2);

    FlannBasedMatcher matcher;
    std::vector<DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);

    double max_dist = 0; double min_dist = 100;
    for( int i = 0; i < descriptors_1.rows; i++ ) {
        double dist = matches[i].distance;
        if (dist < min_dist)
            min_dist = dist;
        if (dist > max_dist)
            max_dist = dist;
    }

    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_1.rows; i++ )
    { if( matches[i].distance <= max(2*min_dist, 0.02) )
        { good_matches.push_back( matches[i]); }
    }

    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches(img1, keypoints_1, img2, keypoints_2,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Show detected matches
    imshow("Good Matches", img_matches);

}

int main() {
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;
    namedWindow("Good Matches", 1);
//    namedWindow("previous", 1);
//    namedWindow("current", 1);

    Mat previousFrame, currentFrame;
    std::vector<Point> previousPoints, currentPoints;

    //odczyt pierwszej ramki
    Mat frame;
    cap >> frame;
    previousFrame = frame.clone();
    //previousPoints = cornerFast(previousFrame);
    //drawPoints(previousFrame, previousPoints, Scalar{255, 0, 0});

    currentFrame = previousFrame.clone();
    currentPoints = previousPoints;

    while(1) {
        Mat frame;
        cap >> frame;
        previousFrame = currentFrame.clone();
        previousPoints = currentPoints;

        currentFrame = frame.clone();
        //currentPoints = cornerFast(currentFrame);
        //drawPoints(currentFrame, currentPoints, Scalar{255, 0, 0});

//        imshow("previous", previousFrame);
//        imshow("current", currentFrame);

        featureMatching(previousFrame, currentFrame);

        // std::this_thread::sleep_for(std::chrono::seconds(1));

        if(waitKey(30) >= 0)
            break;
    }

    return 0;
}
