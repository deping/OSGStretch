#pragma once

#include <osg/observer_ptr>
#include <osg/Plane>
#include <osg/Node>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>

template<typename T>
osg::ref_ptr<T> lock(const osg::observer_ptr<T>& weak)
{
    osg::ref_ptr<T> res;
    weak.lock(res);
    return res;
}

struct PlaneON
{
    osg::Vec3d origin;
    osg::Vec3d normal;
};

// res is in world coord system
bool XLinePlane(const PlaneON &plane, const osg::Vec3d &p1, const osg::Vec3d &p2, osg::Vec3d& res);
void getFarNearPoint(double screenX, double screenY, const osg::Matrix &invVPW, osg::Vec3d &nearPt, osg::Vec3d &farPt);
PlaneON getNodePlane(const osg::NodePath& nodePath);
PlaneON getPlane(const osg::Matrix& matrix);
bool XPointPlane(double screenX, double screenY, const osg::Matrix & invVPW, const osg::Matrix& matrix, osg::Vec3d& res);
bool XPointPlane(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter & aa, const osg::Matrix& matrix, osg::Vec3d& res);
bool getViewProjectionWindowMatrix(const osgGA::GUIActionAdapter &aa, osg::Matrix &VPW, osg::Matrix &invVPW);
osg::Vec2d Nearest(const osg::Vec2d & p, const osg::Vec2d& s, const osg::Vec2d& e);
bool IsMatrixSamePlane(const osg::Matrix & m1, const osg::Matrix & m2);