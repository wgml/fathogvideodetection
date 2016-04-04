#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

Mat src, src_gray;
int thresh = 20;
int max_thresh = 50;

const char *source_window = "Source image";
const char *corners_window = "Corners detected";

void Fast_demo(int, void *) {

    std::vector<KeyPoint> points;
    points.clear();
    FAST(src_gray, points, thresh);

    auto temp = src_gray.clone();

    for (auto keyPoint: points) {
        int x = static_cast<int>(keyPoint.pt.x);
        int y = static_cast<int>(keyPoint.pt.y);
        Point p{x, y};
        circle(temp, p, 5, Scalar(0));
    }

    namedWindow(corners_window, WINDOW_AUTOSIZE);
    imshow(corners_window, temp);
}

int main(int argc, char **argv) {

    /// Load source image and convert it to gray
    src = imread(argv[1], 1);
    cvtColor(src, src_gray, COLOR_BGR2GRAY);

    /// Create a window and a trackbar
    namedWindow(source_window, WINDOW_AUTOSIZE);
    createTrackbar("Threshold: ", source_window, &thresh, max_thresh, Fast_demo);
    imshow(source_window, src);

    Fast_demo(0, 0);

    waitKey(0);
    return (0);
}