#include "stdafx.h"
#include "OsgControlPoints.h"


OsgControlPoints::OsgControlPoints()
    : _types(new NotIndexByteArray)
    , _points(new osg::Vec3Array)
    , _colors(new osg::Vec3Array)
    , _primitiveSet(new osg::DrawArrays)
{
    setDataVariance(DYNAMIC);
    addPrimitiveSet(_primitiveSet);
}

OsgControlPoints::~OsgControlPoints()
{
}

void OsgControlPoints::build()
{
    size_t ptCount = 0;
    for (auto it = _selectionSet.begin(); it != _selectionSet.end(); ++it)
    {
        ptCount += it->second.controlPoints.size();
    }
    _types->resize(ptCount);
    _points->resize(ptCount);
    _colors->resize(ptCount, osg::Vec3(0.75, 0.75, 0.2));
    int ptIndex = 0;
    for (auto it = _selectionSet.begin(); it != _selectionSet.end(); ++it)
    {
        auto matrix = osg::computeLocalToWorld(it->first);
        auto& points = it->second.controlPoints;
        for (size_t i = 0; i < points.size(); ++i)
        {
            (*_types)[ptIndex] = GLbyte(points[i].type);
            (*_points)[ptIndex] = osg::Vec3d(points[i].point, 0) * matrix;
            if (i == it->second.index)
            {
                (*_colors)[ptIndex] = osg::Vec3(0.75, 0.2, 0.2);
            }
            ++ptIndex;
        }
    }
    _types->dirty();
    _points->dirty();
    _colors->dirty();

    setVertexArray(_points);
    setColorArray(_colors);
    _primitiveSet->set(GL_POINTS, 0, ptCount);
}

void OsgControlPoints::RemoveNode(const osg::NodePath & node)
{
    _selectionSet.erase(node);
}
