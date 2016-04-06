#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include <vector>

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

int main() {
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

    namedWindow("orginal", 1);
    namedWindow("fast", 1);
    namedWindow("harris", 1);

    while(1) {
        Mat frame;
        cap >> frame;
        imshow("orginal", frame);

        auto fastFrame = frame.clone();
        auto fastPoints = cornerFast(fastFrame);
        drawPoints(fastFrame, fastPoints, Scalar{255, 0, 0});
        imshow("fast", fastFrame);

        auto harrisFrame = frame.clone();
        auto harrisPoints = cornerHarris(harrisFrame);
        drawPoints(harrisFrame, harrisPoints, Scalar{0, 0, 255});
        imshow("harris", harrisFrame);

        if(waitKey(30) >= 0)
            break;
    }

    return 0;
}