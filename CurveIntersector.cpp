#include <stdafx.h>
#include "CurveIntersector.h"

#include <assert.h>
#include <osg/Geometry>
#include "IPlanarCurve.h"
#include "Utility.h"

CurveIntersector::CurveIntersector(Intersections& external, double x, double y, double offset)
    : osgUtil::Intersector(WINDOW)
    , _screenX(x)
    , _screenY(y)
    , _offset(offset)
    , _intersections(external)
{
    assert(offset >= 3.0);
}

osgUtil::Intersector *CurveIntersector::clone(osgUtil::IntersectionVisitor &iv)
{
    osg::ref_ptr<CurveIntersector> cloned = new CurveIntersector(_intersections, _screenX, _screenY, _offset);
    return cloned.release();
}

bool CurveIntersector::enter(const osg::Node& node)
{
    if (reachedLimit()) return false;
    return true;// !node.isCullingActive();
}

void CurveIntersector::leave()
{
    // do nothing
}

bool CurveIntersector::containsIntersections()
{
    return !_intersections.empty();
}

void CurveIntersector::intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable)
{
    auto pCurve = dynamic_cast<IPlanarCurve*>(drawable);
    if (!pCurve)
        return;
    osg::Matrix* W = iv.getWindowMatrix();
    osg::Matrix* P = iv.getProjectionMatrix();
    osg::Matrix* V = iv.getViewMatrix();
    osg::Matrix* M = iv.getModelMatrix();
    if (!W || !P || !V)
        return;
    osg::Vec3d xpt;
    osg::Matrix VPW = *V * *P * *W;
    osg::Matrix invVPW = osg::Matrix::inverse(VPW);
    osg::Matrix m = M ? *M : osg::computeLocalToWorld(iv.getNodePath());
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
        x.nodePath = iv.getNodePath();
        x.curve = drawable;
        _intersections.insert(x);
    }
}
