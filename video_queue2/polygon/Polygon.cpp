//
// Created by vka on 28.05.16.
//

#include "Polygon.h"

bool wgml::Polygon::isPointInside(wgml::point &&p) {
    int i, j, c = 0;
    int numPoints = (int) points.size();
    for (i = 0, j = numPoints-1; i < numPoints; j = i++) {
        if ( ((points[i].second>p.second) != (points[j].second>p.second)) &&
         (p.first < (points[j].first-points[i].first) * (p.second-points[i].second) / (points[j].second-points[i].second) + points[i].first) )
            c = !c;
    }
    return c != 0;
}

wgml::Polygon::Polygon(std::vector<point> points) {
    this->points = std::vector<point>{points};
}



