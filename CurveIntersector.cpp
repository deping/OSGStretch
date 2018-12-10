#include <stdafx.h>
#include "CurveIntersector.h"

#include <assert.h>
#include <osg/Geometry>
#include "IPlanarCurve.h"
#include "Utility.h"

PlanarCurveVisitor::PlanarCurveVisitor(Intersections& external, const osg::Matrix& vpw, double x, double y, double offset)
    : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
    , _VPW(vpw)
    , _screenX(x)
    , _screenY(y)
    , _offset(offset)
    , _intersections(external)
{
    assert(offset >= 3.0);
}

bool PlanarCurveVisitor::containsIntersections()
{
    return !_intersections.empty();
}

void PlanarCurveVisitor::apply(osg::Node& node)
{
    auto pCurve = dynamic_cast<IPlanarCurve*>(&node);
    if (pCurve)
    {
        osg::Vec3d xpt;
        const osg::Matrix& VPW = _VPW;
        osg::Matrix invVPW = osg::Matrix::inverse(VPW);
        auto nodePath = getNodePath();
        osg::Matrix m = osg::computeLocalToWorld(nodePath);
        bool success = XPointPlane(_screenX, _screenY, invVPW, m, xpt);
        if (!success)
            return;
        osg::Matrix invM = osg::Matrix::inverse(m);
        osg::Vec3d localXpt = xpt * invM;
        osg::Vec2d nearestPt;
        success = pCurve->nearest(osg::Vec2d(localXpt.x(), localXpt.y()), nearestPt);
        if (!success)
            return;
        auto nearestPtInWindow = osg::Vec3d(nearestPt, 0) * m * VPW;
        if (abs(nearestPtInWindow.x() - _screenX) <= _offset && abs(nearestPtInWindow.y() - _screenY) <= _offset)
        {
            Intersection x;
            x.depth = nearestPtInWindow.z();
            x.nodePath = nodePath;
            x.curve = &node;
            _intersections.insert(x);
        }
        // Don't traverse its children.
    }
    else
    {
        traverse(node);
        return;
    }
}
