#include "lbp.h"
#include "helper.h"

using namespace cv;

//------------------------------------------------------------------------------
// cv::olbp
//------------------------------------------------------------------------------
namespace libfacerec {

    template <typename _Tp> static
    void olbp_(InputArray _src, OutputArray _dst) {
        // get matrices
        Mat src = _src.getMat();
        // allocate memory for result
        _dst.create(src.rows-2, src.cols-2, CV_8UC1);
        Mat dst = _dst.getMat();
        // zero the result matrix
        dst.setTo(0);
        // calculate patterns
        for(int i=1;i<src.rows-1;i++) {
            for(int j=1;j<src.cols-1;j++) {
                _Tp center = src.at<_Tp>(i,j);
                unsigned char code = 0;
                code |= (src.at<_Tp>(i-1,j-1) >= center) << 7;
                code |= (src.at<_Tp>(i-1,j) >= center) << 6;
                code |= (src.at<_Tp>(i-1,j+1) >= center) << 5;
                code |= (src.at<_Tp>(i,j+1) >= center) << 4;
                code |= (src.at<_Tp>(i+1,j+1) >= center) << 3;
                code |= (src.at<_Tp>(i+1,j) >= center) << 2;
                code |= (src.at<_Tp>(i+1,j-1) >= center) << 1;
                code |= (src.at<_Tp>(i,j-1) >= center) << 0;
                dst.at<unsigned char>(i-1,j-1) = code;
            }
        }
    }

}

void libfacerec::olbp(InputArray src, OutputArray dst) {
    switch (src.getMat().type()) {
        case CV_8SC1:   olbp_<char>(src,dst); break;
        case CV_8UC1:   olbp_<unsigned char>(src,dst); break;
        case CV_16SC1:  olbp_<short>(src,dst); break;
        case CV_16UC1:  olbp_<unsigned short>(src,dst); break;
        case CV_32SC1:  olbp_<int>(src,dst); break;
        case CV_32FC1:  olbp_<float>(src,dst); break;
        case CV_64FC1:  olbp_<double>(src,dst); break;
        default: break;
    }
}

//------------------------------------------------------------------------------
// cv::elbp
//------------------------------------------------------------------------------
namespace libfacerec {
    template <typename _Tp> static
    inline void elbp_(InputArray _src, OutputArray _dst, int radius, int neighbors) {
        //get matrices
        Mat src = _src.getMat();
        // allocate memory for result
        _dst.create(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
        Mat dst = _dst.getMat();
        // zero
        dst.setTo(0);
        for(int n=0; n<neighbors; n++) {
            // sample points
            float x = static_cast<float>(-radius) * sin(2.0*CV_PI*n/static_cast<float>(neighbors));
            float y = static_cast<float>(radius) * cos(2.0*CV_PI*n/static_cast<float>(neighbors));
            // relative indices
            int fx = static_cast<int>(floor(x));
            int fy = static_cast<int>(floor(y));
            int cx = static_cast<int>(ceil(x));
            int cy = static_cast<int>(ceil(y));
            // fractional part
            float ty = y - fy;
            float tx = x - fx;
            // set interpolation weights
            float w1 = (1 - tx) * (1 - ty);
            float w2 =      tx  * (1 - ty);
            float w3 = (1 - tx) *      ty;
            float w4 =      tx  *      ty;
            // iterate through your data
            for(int i=radius; i < src.rows-radius;i++) {
                for(int j=radius;j < src.cols-radius;j++) {
                    // calculate interpolated value
                    float t = w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx);
                    // floating point precision, so check some machine-dependent epsilon
                    dst.at<int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) || (std::abs(t-src.at<_Tp>(i,j)) < std::numeric_limits<float>::epsilon())) << n;
                }
            }
        }
    }

}

void libfacerec::elbp(InputArray src, OutputArray dst, int radius, int neighbors) {
    switch (src.type()) {
        case CV_8SC1:   elbp_<char>(src,dst, radius, neighbors); break;
        case CV_8UC1:   elbp_<unsigned char>(src, dst, radius, neighbors); break;
        case CV_16SC1:  elbp_<short>(src,dst, radius, neighbors); break;
        case CV_16UC1:  elbp_<unsigned short>(src,dst, radius, neighbors); break;
        case CV_32SC1:  elbp_<int>(src,dst, radius, neighbors); break;
        case CV_32FC1:  elbp_<float>(src,dst, radius, neighbors); break;
        case CV_64FC1:  elbp_<double>(src,dst, radius, neighbors); break;
        default: break;
    }
}
