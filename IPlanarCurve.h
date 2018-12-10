#pragma once
#include <set>
#include <list>

#include <osg/Geode>
#include <osg/NodeCallback>
#include <osg/OperationThread>
#include <osg/Camera>
#include <osgGA/GUIEventAdapter>

enum class GripType : char
{
    End,
    Mid,
    Cen,
    Ctl
};

struct GripPoint
{
    osg::Vec2 point;
    GripType type;
};

// All points are in local coordinate system.
struct IPlanarCurve
{
    virtual bool nearest(const osg::Vec2d & localPt, osg::Vec2d& nearest) const = 0;
    virtual void getControlPoints(std::vector<GripPoint>& points) const = 0;
    virtual void stretch(int index, const osg::Vec2d& pos, const osgGA::GUIEventAdapter& ea) = 0;
};

const osg::Node::NodeMask HANDLE_NODE_MASK = 0x40000;

void EnableStretch(osg::Camera* camera, double offset = 5);
