//
// Created by vka on 28.05.16.
//

#ifndef CAMERA_POLYGON_H
#define CAMERA_POLYGON_H

#include <utility>
#include <vector>

namespace wgml {
    using point = std::pair<int, int>;
    class Polygon {
    public:
        Polygon(std::vector<point> points);
        Polygon() { points = std::vector<point>(); };
        bool isPointInside(point&& p);

        point getPoint(int i) {
            return points[i];
        }
    private:
        std::vector<point> points;
    };
}


#endif //CAMERA_POLYGON_H
