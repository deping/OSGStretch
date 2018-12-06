#include <stdafx.h>
#include "Utility.h"
#include <osg/Transform>
#include <osgViewer/View>

bool XLinePlane(const PlaneON & plane, const osg::Vec3d & p1, const osg::Vec3d & p2, osg::Vec3d& res)
{
    // If plane normal is 0, return false
    double n_len2 = plane.normal.length2();
    if (n_len2 == 0.0)
        return false;
    auto l = p2 - p1;
    // If p1 is very close to p2, return false.
    double l_len2 = l.length2();
    if (l_len2 < 1E-12)
        return false;
    auto v1 = plane.origin - p1;
    auto dp_ln = l * plane.normal;
    auto dp_v1n = v1 * plane.normal;
    // If p1->p2 is almost parallel to plane, return false.
    const double minCos_2 = 1E-4;
    if (dp_ln * dp_ln < minCos_2 * n_len2 * l_len2)
        return false;
    double t = dp_v1n / dp_ln;
    res = p1  + l * t;
    return true;
}

void getFarNearPoint(double screenX, double screenY, const osg::Matrix & invVPW, osg::Vec3d & nearPt, osg::Vec3d & farPt)
{
    nearPt = osg::Vec3d(screenX, screenY, 0.0) * invVPW;
    farPt = osg::Vec3d(screenX, screenY, 1.0) * invVPW;
}

PlaneON getNodePlane(const osg::NodePath & nodePath)
{
    auto matrix = osg::computeLocalToWorld(nodePath);
    return getPlane(matrix);
}

PlaneON getPlane(const osg::Matrix & matrix)
{
    osg::Vec3d origin = osg::Vec3d(0, 0, 0) * matrix;
    osg::Vec3d x = osg::Vec3d(1, 0, 0) * matrix;
    osg::Vec3d y = osg::Vec3d(0, 1, 0) * matrix;
    return { origin, x ^ y };
}

bool XPointPlane(double screenX, double screenY, const osg::Matrix & invVPW, const osg::Matrix & matrix, osg::Vec3d& res)
{
    auto plane = getPlane(matrix);
    osg::Vec3d nearPt, farPt;
    getFarNearPoint(screenX, screenY, invVPW, nearPt, farPt);
    return XLinePlane(plane, nearPt, farPt, res);
}

bool XPointPlane(const osgGA::GUIEventAdapter & ea, const osgGA::GUIActionAdapter & aa, const osg::Matrix & matrix, osg::Vec3d & res)
{
    osg::Matrix VPW, invVPW;
    bool success = getViewProjectionWindowMatrix(aa, VPW, invVPW);
    return success && XPointPlane(ea.getX(), ea.getY(), invVPW, matrix, res);
}

bool getViewProjectionWindowMatrix(const osgGA::GUIActionAdapter &aa, osg::Matrix &VPW, osg::Matrix &invVPW)
{
    auto view = dynamic_cast<const osgViewer::View*>(&aa);
    if (!view)
        return false;
    const osg::Camera* camera = view->getCamera();
    if (!camera)
        return false;
    if (!camera->getViewport())
        return false;

    /* get far and near points of the ray */
    VPW = camera->getViewMatrix()
        * camera->getProjectionMatrix()
        * camera->getViewport()->computeWindowMatrix();
    if (!invVPW.invert(VPW))
        return false;

    return true;
}

osg::Vec2d Nearest(const osg::Vec2d & p, const osg::Vec2d & s, const osg::Vec2d & e)
{
    if (s == e)
    {
        return s;
    }
    osg::Vec2d res;
    auto dir1 = e - s;
    auto dir2 = p - s;
    double len1_2 = dir1.length2();
    auto dp = dir1 * dir2;
    double t = dp / len1_2;
    if (t < 0)
    {
        res = s;
    }
    else if (t > 1)
    {
        res = e;
    }
    else
    {
        res = s + dir1 * t;
    }
    return res;
}

bool IsMatrixSamePlane(const osg::Matrix & m1, const osg::Matrix & m2)
{
    osg::Matrix x1(m1);
    x1.setTrans(0, 0, 0);
    //x1(3, 0) = 0;
    //x1(3, 1) = 0;
    //x1(3, 2) = 0;
    osg::Matrix x2(m2);
    x2.setTrans(0, 0, 0);
    //x2(3, 0) = 0;
    //x2(3, 1) = 0;
    //x2(3, 2) = 0;
    return x1 == x2;
}
