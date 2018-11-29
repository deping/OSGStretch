# StretchCurve
frameworks to stretch OSG planar curves.
![ShxText example picture](https://github.com/deping/images/blob/master/OSGstretch.png)
```C++
struct IPlanarCurve
{
    virtual bool nearest(const osg::Vec2d & localPt, osg::Vec2d& nearest) const = 0;
    virtual void getHandles(std::vector<GripPoint>& points) const = 0;
    // pos is in local coordinate system.
    virtual void stretch(int index, GripType type, const osg::Vec2d& pos) = 0;
};
```
OSG programmer just need to derive a class from osg::Drawable and IPlanarCurve like this:
```C++
class Line : public osg::Geometry, public IPlanarCurve {...};
```
At the beginning of program, call EnableEdit:
```C++
    osgViewer::Viewer viewer;
    ...
    osg::ref_ptr<osg::Group> root = new osg::Group();
    viewer.setSceneData(root.get());
    EnableEdit(&viewer, 5);
```
EnableEdit call must be after setSceneData call.
```
void EnableEdit(osgViewer::View* pView, double offset = 5);
```
offset is the half width of grip point, in pixel, and it is the precision to select curves and grip points.
