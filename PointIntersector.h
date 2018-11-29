#pragma once

#include <osg/Drawable>
#include "IPlanarCurve.h"

class OsgControlPoints;
class PointIntersector
{
public:
    PointIntersector(double x, double y, double offset = 5);

    bool intersect(const osg::Matrixd& VPW, OsgControlPoints* p);

protected:

    double _screenX, _screenY;
    double _offset;
};
