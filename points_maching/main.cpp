#include <opencv2/opencv.hpp>
#include "opencv2/xfeatures2d.hpp"
#include <queue>

using namespace cv;

void getImgFeatures(const Mat &img1, const Mat &img2, const Mat &img_matches);
void getImgHarris(const Mat &img1, const Mat &img2, Mat &img_matches);
std::vector<Point> getPointsHarris(const Mat& img);

void featureMatching(Mat img1, Mat img2, bool use_features = true) {
    Mat img_matches{img2};

    if (use_features) {
        getImgFeatures(img1, img2, img_matches);
    } else {
        getImgHarris(img1, img2, img_matches);
    }
    imshow("Good Matches", img_matches);
}

int main() {
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;
    namedWindow("Good Matches", 1);

    std::queue<Mat> frames{};
    Mat frame;
    const unsigned int FACTOR = 10;

    while(1) {
        cap >> frame;
        frames.push(frame.clone());
        if (frames.size() < FACTOR) {
            continue;
        }
        frames.pop();
        try {
            featureMatching(frames.front(), frame, false);
        } catch (Exception e) {
            std::cerr << "catched opencv exception: " << e.what() << std::endl;
        }

        if(waitKey(30) >= 0)
            break;
    }
    return 0;
}

void getImgFeatures(const Mat &img1, const Mat &img2, const Mat &img_matches) {
    int minHessian = 10000;
    auto detector = cv::xfeatures2d::SURF::create(minHessian);
    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector->detect( img1, keypoints_1 );
    detector->detect( img2, keypoints_2 );

    auto extractor = cv::xfeatures2d::SURF::create();

    Mat descriptors_1, descriptors_2;

    extractor->compute(img1, keypoints_1, descriptors_1);
    extractor->compute(img2, keypoints_2, descriptors_2);

    FlannBasedMatcher matcher;
    std::vector<DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);

    double max_dist = -1e6;
    double min_dist = 1e6;
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

    for (auto & element : good_matches) {
        auto & match1 = keypoints_1[element.queryIdx].pt;
        auto & match2 = keypoints_2[element.trainIdx].pt;
        circle(img_matches, match1, 3, ::cv::Scalar_<double>::all(-1));
        if (element.distance > 0) {
            circle(img_matches, match2, 3, Scalar{0, 0, 255, 255});
            line(img_matches, match1, match2, Scalar{0, 255, 0, 255});
        }
    }
}

void getImgHarris(const Mat &img1, const Mat &img2, Mat &img_matches) {
    img_matches = img2.clone();

    std::vector<Point> points1 = getPointsHarris(img1);
    std::vector<Point> points2 = getPointsHarris(img2);

    for (auto &element : points1) {
        circle(img_matches, element, 3, ::cv::Scalar_<double>::all(-1));
    }
    for (auto &element : points2) {
        circle(img_matches, element, 3, Scalar{0, 0, 255, 255});
    }
}
std::vector<Point> getPointsHarris(const Mat& img) {
    Mat src_gray, dst, dst_norm;
    dst = Mat::zeros(img.size(), CV_32FC1);

    /// Detector parameters
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;

    /// Detecting corners
    cvtColor(img, src_gray, COLOR_BGR2GRAY);
    cornerHarris(src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

    /// Normalizing
    normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

    /// Drawing a circle around corners
    std::vector<Point> points{};
    for (int j = 0; j < dst_norm.rows; j++) {
        for (int i = 0; i < dst_norm.cols; i++) {
            if ((int) dst_norm.at<float>(j, i) > 200)
                points.emplace_back(Point(i, j));
        }
    }
    return points;
}
