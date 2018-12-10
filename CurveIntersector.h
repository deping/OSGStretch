#pragma once
#include <osg/NodeVisitor>
#include <osg/Drawable>
#include "IPlanarCurve.h"

class PlanarCurveVisitor : public osg::NodeVisitor
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
        osg::Node*                   curve;
    };
    typedef std::multiset<Intersection> Intersections;

    PlanarCurveVisitor(Intersections& external, const osg::Matrix& vpw, double x, double y, double offset = 5);

    bool containsIntersections();

    virtual void apply(osg::Node& node) override;

    inline Intersections& getIntersections() { return _intersections; }
    inline Intersection getFirstIntersection() { return _intersections.empty() ? Intersection() : *(_intersections.begin()); }

protected:

    osg::Matrix _VPW;
    double _screenX, _screenY;
    double _offset;
    // Use external variable to store intersections, because IntersectionVisitor::apply(osg::Transform& transform) will push_clone interceptor!!!
    // We will lose intersections if we use internal variables. very ugly!!!
    Intersections& _intersections;
};
