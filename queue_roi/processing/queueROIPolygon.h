//
// Created by vka on 11.04.16.
//

#ifndef OPENCVHELLOWORLD_QUEUEROIPOLYGON_H
#define OPENCVHELLOWORLD_QUEUEROIPOLYGON_H

struct queueROIPolygon{
    struct help_me {
        int x;
        int y;
        double cos_alfa;
        double sin_alfa;
        double p;
        bool gOrL;
    };

    int edgeTh;
    int movementTh;
    help_me TR;
    help_me TL;
    help_me BL;
    help_me BR;

};

#endif //OPENCVHELLOWORLD_QUEUEROIPOLYGON_H
