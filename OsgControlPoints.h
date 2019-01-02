#pragma once
#include <set>
#include <list>

#include <osg/Geode>
#include <osg/Geometry>

#include "IPlanarCurve.h"
typedef osg::TemplateArray<GLbyte, osg::Array::ByteArrayType, 1, GL_BYTE> NotIndexByteArray;

struct IPlanarCurve;
struct SelectedCurveInfo
{
    osg::observer_ptr<osg::Node> curve;
    osg::ref_ptr<osg::Node> clone;
    std::vector<GripPoint> controlPoints;
    //osg::Vec3d hitPoint; // local coordinate
    int index;
};

// Draw control points of other objects which implements interface IPlanarCurve
class OsgControlPoints : public osg::Geometry//Drawable
{
public:
    OsgControlPoints();
    ~OsgControlPoints();
    void build();
    void RemoveNode(const osg::NodePath& node);
    std::map<osg::NodePath, SelectedCurveInfo> _selectionSet;
    void removeInvalidSelections();

private:
    osg::ref_ptr<NotIndexByteArray> _types;
    osg::ref_ptr<osg::Vec3Array> _points;
    osg::ref_ptr<osg::Vec3Array> _colors;
    osg::ref_ptr<osg::DrawArrays> _primitiveSet;
};
