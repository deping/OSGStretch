#include "stdafx.h"
#undef max
#undef min
#include <limits>

#include "Line.h"
#include "Utility.h"

Line::Line()
    : _color(0.8f, 0.8f, 0.8f, 1.f)
    , _colors(new osg::Vec4Array(osg::Array::BIND_OVERALL))
    , _primitiveSet(new osg::DrawArrays)
{
    _colors->push_back(_color);
    _colors->dirty();
    setColorArray(_colors);
    addPrimitiveSet(_primitiveSet);
    setUseVertexBufferObjects(true);
}


Line::Line(const Line & pg, const osg::CopyOp & copyop)
    : Geometry(pg, copyop)
    , _color(pg._color)
    , _colors(dynamic_cast<osg::Vec4Array*>(copyop(pg._colors)))
    , _points(dynamic_cast<osg::Vec2Array*>(copyop(pg._points)))
    , _primitiveSet(dynamic_cast<osg::DrawArrays*>(copyop(pg._primitiveSet)))
{
    if (!_points->empty() && _points != pg._points)
    {
        _points->dirty();
    }
    if (!_colors->empty() && _colors != pg._colors)
    {
        _colors->dirty();
    }
    setVertexArray(_points);
    setColorArray(_colors);
    addPrimitiveSet(_primitiveSet);
    setUseVertexBufferObjects(true);
}

Line::~Line()
{
}

bool Line::nearest(const osg::Vec2d & localPt, osg::Vec2d& nearest) const
{
    if (!_points.valid() || _points->size() < 2)
        return false;
    double dist_2 = std::numeric_limits<double>::max();
    osg::Vec2d localPt2(localPt.x(), localPt.y());
    osg::Vec2d tmp;
    for (size_t i = 0; i < _points->size()-1; ++i)
    {
        tmp = Nearest(localPt2, (*_points)[i], (*_points)[i + 1]);
        auto tmp_dist_2 = (tmp - localPt2).length2();
        if (tmp_dist_2 < dist_2)
        {
            nearest = tmp;
            dist_2 = tmp_dist_2;
        }
    }
    return true;
}

void Line::getControlPoints(std::vector<GripPoint>& points) const
{
    if (!_points.valid() || _points->empty())
        return;
    points.resize(_points->size());
    for (size_t i = 0; i < _points->size(); ++i)
    {
        points[i].point = (*_points)[i];
        points[i].type = GripType::End;
    }
}

void Line::stretch(int index, const osg::Vec2d & pos)
{
    (*_points)[index] = pos;
    _points->dirty();
}

void Line::setPoints(osg::Vec2Array * points)
{
    if (points != _points.get())
    {
        _points = points;
        if (_points.valid())
        {
            setVertexArray(_points);
            _points->dirty();
            _primitiveSet->set(GL_LINE_STRIP, 0, _points->size());
        }
    }
}

void Line::setColor(const osg::Vec4 & color)
{
    if (_color != color)
    {
        _color = color;
        _colors->dirty();
    }
}

osg::BoundingBox Line::computeBoundingBox() const
{
    osg::BoundingBox  bbox;

    for (size_t i = 0; i < _points->size(); ++i)
    {
        bbox.expandBy(osg::Vec3((*_points)[i], 0));
    }

    return bbox;
}

void Line::drawImplementation(osg::RenderInfo& renderInfo) const
{
    if (!_points.valid() || _points->size() < 2)
        return;
    osg::Geometry::drawImplementation(renderInfo);
}
