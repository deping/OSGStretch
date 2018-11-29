#pragma once
#include <osgUtil/IntersectionVisitor>
#include <osg/Drawable>
#include "IPlanarCurve.h"

class CurveIntersector : public osgUtil::Intersector
{
public:
    struct Intersection
    {
        Intersection()
            : depth(-1.0)
        {}
        bool operator < (const Intersection& rhs) const { return depth < rhs.depth; }
        double                          depth;
        osg::NodePath                   nodePath;
        osg::Drawable*                   curve;
    };
    typedef std::multiset<Intersection> Intersections;

    CurveIntersector(Intersections& external, double x, double y, double offset = 5);

    virtual Intersector* clone(osgUtil::IntersectionVisitor &iv);

    virtual bool enter(const osg::Node& node) override;
    virtual void leave() override;

    virtual bool containsIntersections() override;

    virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable) override;

    inline Intersections& getIntersections() { return _intersections; }
    inline Intersection getFirstIntersection() { return _intersections.empty() ? Intersection() : *(_intersections.begin()); }

protected:

    double _screenX, _screenY;
    double _offset;
    // Use external variable to store intersections, because IntersectionVisitor::apply(osg::Transform& transform) will push_clone interceptor!!!
    // We will lose intersections if we use internal variables. very ugly!!!
    Intersections& _intersections;
};
