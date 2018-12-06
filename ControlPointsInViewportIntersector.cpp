#include <stdafx.h>
#include "ControlPointsInViewportIntersector.h"

#include <assert.h>
#include <osg/Geometry>
#include "IPlanarCurve.h"
#include "Utility.h"

ControlPointsInViewportIntersector::ControlPointsInViewportIntersector(Intersections& external, const osg::Matrix& vpw, double x, double y, double w, double h)
    : osgUtil::Intersector(WINDOW)
    , _VPW(vpw)
    , _x1(x)
    , _y1(y)
    , _x2(x + w)
    , _y2(y + h)
    , _intersections(external)
{
    assert(w >= 0 && h >= 0);
}

osgUtil::Intersector *ControlPointsInViewportIntersector::clone(osgUtil::IntersectionVisitor &iv)
{
    osg::ref_ptr<ControlPointsInViewportIntersector> cloned = new ControlPointsInViewportIntersector(_intersections, _VPW, _x1, _y1, _x2 - _x1, _y2 - _y1);
    return cloned.release();
}

bool ControlPointsInViewportIntersector::enter(const osg::Node& node)
{
    if (reachedLimit()) return false;
    return true;// !node.isCullingActive();
}

void ControlPointsInViewportIntersector::leave()
{
    // do nothing
}

bool ControlPointsInViewportIntersector::containsIntersections()
{
    return !_intersections.empty();
}

void ControlPointsInViewportIntersector::intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable)
{
    auto pCurve = dynamic_cast<IPlanarCurve*>(drawable);
    if (!pCurve)
        return;
    osg::Matrix* M = iv.getModelMatrix();
    osg::Vec3d xpt;
    const osg::Matrix& VPW = _VPW;
    osg::Matrix m = M ? *M : osg::computeLocalToWorld(iv.getNodePath());
    osg::Matrix MVPW = m * VPW;
    std::vector<GripPoint> points;
    pCurve->getControlPoints(points);
    bool firstPoint = true;
    for (const auto& pt : points)
    {
        osg::Vec3d pointInLocalCS(pt.point.x(), pt.point.y(), 0);
        osg::Vec3d pointInViewport = pointInLocalCS * MVPW;
        double x = pointInViewport.x();
        double y = pointInViewport.y();
        if (x >= _x1 && x <= _x2 && y >= _y1 && y <= _y2)
        {
            if (firstPoint)
            {
                _intersections.push_back({ iv.getNodePath(), m, {} });
                firstPoint = false;
            }
            _intersections.back()._controlPoints.push_back(pt.point);
        }
    }
}
