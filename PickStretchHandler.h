#pragma once

#include <set>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <osg/observer_ptr>

class OsgControlPoints;
// class to handle events with a pick
class PickStretchHandler : public osgGA::GUIEventHandler
{
public:
    enum class EditMode {None, DragPoint};
    PickStretchHandler(osgViewer::View* pView, double offset);

    ~PickStretchHandler();

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

    // Pick "control point" or curve. support xor selection(press ctrl) on picking curve.
    virtual bool pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

private:
    void stretch(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool clone);
    void cloneDraggedObject();
    void releaseDraggedObject();
    void updateGripPoints();
    osg::Matrix VPWmatrix(osg::Camera* cam);

    osg::observer_ptr<osgViewer::View> m_pView;
    osg::ref_ptr<OsgControlPoints> m_gripPoints;
    double _offset;
    EditMode _mode;
};

