#ifndef _QUICKFIX_H
#define _QUICKFIX_H

#include "quickfix/core.h"

extern "C" {
    Beacon2D *Beacon2D_new(float minX, float minY, float maxX, float maxY);
    void Beacon2D_Anchor(Beacon2D *b, int id, float x, float y);
    void Beacon2D_Range(Beacon2D *b, int id, float r);
    void Beacon2D_Reading(Beacon2D *b, float x, float y, float r);
    float Beacon2D_X(Beacon2D *b);
    float Beacon2D_Y(Beacon2D *b);
    float Beacon2D_Error(Beacon2D *b);
    int Beacon2D_Anchors(Beacon2D *b);
    void Beacon2D_Fix(Beacon2D *b, float tick, float rmsError);
    bool Beacon2D_Update(Beacon2D *b, float tick, float rmsThreshold);
}

#endif
