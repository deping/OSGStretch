#pragma once
#include <osgUtil/IntersectionVisitor>
#include <osg/Drawable>
#include "IPlanarCurve.h"
#include "ControlPointsInViewport.h"

class ControlPointsInViewportIntersector : public osgUtil::Intersector
{
public:
    typedef std::vector<ControlPointsInViewport> Intersections;

    ControlPointsInViewportIntersector(Intersections& external, const osg::Matrix& vpw, double x, double y, double w, double h);

    virtual Intersector* clone(osgUtil::IntersectionVisitor &iv);

    virtual bool enter(const osg::Node& node) override;
    virtual void leave() override;

    virtual bool containsIntersections() override;

    virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable) override;

    inline Intersections& getIntersections() { return _intersections; }

protected:

    osg::Matrix _VPW;
    // viewport extent
    double _x1, _y1, _x2, _y2;
    // Use external variable to store intersections, because IntersectionVisitor::apply(osg::Transform& transform) will push_clone interceptor!!!
    // We will lose intersections if we use internal variables. very ugly!!!
    Intersections& _intersections;
};
