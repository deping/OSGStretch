#pragma once

#include <vector>

#include <osg/Matrix>
#include <osg/Node>

struct ControlPointsInViewport
{
    osg::NodePath _obj;
    osg::Matrix _matrix;
    std::vector<osg::Vec2d> _controlPoints;
};
