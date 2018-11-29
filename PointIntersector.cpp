#include <stdafx.h>
#include "PointIntersector.h"

#include <assert.h>
#include <osg/Geometry>
#include "IPlanarCurve.h"
#include "Utility.h"
#include "OsgControlPoints.h"

PointIntersector::PointIntersector(double x, double y, double offset)
    : _screenX(x)
    , _screenY(y)
    , _offset(offset)
{
    assert(offset >= 3.0);
}

bool PointIntersector::intersect(const osg::Matrixd& VPW, OsgControlPoints* p)
{
    osg::Matrix invVPW = osg::Matrix::inverse(VPW);
    osg::Vec3d hitPoint;
    osg::Matrix matrix;
    auto& selectSet = p->_selectionSet;
    int hitCount = 0;
    for (auto it = selectSet.begin(); it != selectSet.end(); ++it)
    {
        matrix = osg::computeLocalToWorld(it->first);
        bool success = XPointPlane(_screenX, _screenY, invVPW, matrix, hitPoint);
        if (!success)
        {
            // No control point is hit for this curve.
            it->second.index = -1;
            continue;
        }
        osg::Vec3d ptInWindow = hitPoint * VPW;
        auto& points = it->second.controlPoints;
        double dist_2 = _offset * _offset * 2;
        // pick only one control point in one curve.
        int hit = -1;
        osg::Matrix MVPW = matrix * VPW;
        for (size_t i = 0; i < points.size(); ++i)
        {
            const auto& v = points[i].point;
            auto vInWindow = osg::Vec3d(v, 0) * MVPW;
            double dx = abs(vInWindow.x() - ptInWindow.x());
            double dy = abs(vInWindow.y() - ptInWindow.y());
            if (dx <= _offset && dy <= _offset)
            {
                double i_dist_2 = dx * dx + dy * dy;
                if (i_dist_2 <= dist_2)
                {
                    hit = i;
                    dist_2 = i_dist_2;
                }
            }
        }
        it->second.index = hit;
        if (hit != -1)
        {
            // convert to local coord.
            //osg::Matrix inv = osg::Matrix::inverse(matrix);
            //it->second.hitPoint = hitPoint * inv;
            ++hitCount;
        }
    }
    return hitCount != 0;
}
