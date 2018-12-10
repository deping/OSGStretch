#pragma once

#include <osg/Geometry>

#include "IPlanarCurve.h"

class Line : public osg::Geometry, public IPlanarCurve
{
public:
    Line();
    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    Line(const Line& pg, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    ~Line();
    virtual osg::Object* cloneType() const { return new Line(); }
    virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new Line(*this, copyop); }
    virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const Line*>(obj) != NULL; }
    virtual const char* className() const { return "Line"; }
    virtual const char* libraryName() const { return "osg"; }

    virtual bool nearest(const osg::Vec2d & localPt, osg::Vec2d& nearest) const override;
    virtual void getControlPoints(std::vector<GripPoint>& points) const override;
    // pos is in local coordinate system.
    virtual void stretch(int index, const osg::Vec2d& pos, const osgGA::GUIEventAdapter& ea) override;
    void setPoints(osg::Vec2Array* points);
    void setColor(const osg::Vec4& color);
    virtual osg::BoundingBox Line::computeBoundingBox() const override;
    virtual void Line::drawImplementation(osg::RenderInfo& renderInfo) const override;
private:
    osg::Vec4 _color;
    osg::ref_ptr<osg::Vec2Array> _points;
    osg::ref_ptr<osg::DrawArrays> _primitiveSet;
    mutable osg::ref_ptr<osg::Vec4Array> _colors;
};

