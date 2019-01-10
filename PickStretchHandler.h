#pragma once

#include <set>
#include <vector>
#include <osgGA/GUIEventHandler>
#include <osg/Camera>
#include <osg/observer_ptr>

#include "ControlPointsInViewport.h"

class OsgControlPoints;
// class to handle events with a pick
class PickStretchHandler : public osgGA::GUIEventHandler
{
public:
    enum class EditMode {None, DragPoint};
    PickStretchHandler(osg::Camera* camera, osg::Node* node, double offset);

    ~PickStretchHandler();

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

    // Pick "control point" or curve. support xor selection(press ctrl) on picking curve.
    virtual bool pick(const osgGA::GUIEventAdapter& ea);

private:
    void stretch(const osgGA::GUIEventAdapter& ea, bool clone);
    void cloneDraggedObject();
    void releaseDraggedObject();
    void updateGripPoints();
    osg::Matrix VPWmatrix(osg::Camera* cam);
    bool CollectControlPointsInViewport();
    void SnapPoint(osg::Vec2d& pt/*in, out*/, const osg::NodePath& obj, const osg::Matrix& M, const osg::Matrix& invM, const osg::Matrix & VPW, const osg::Matrix& invVPW);

    osg::observer_ptr<osg::Camera> m_pCamera;
    osg::observer_ptr<osg::Group> m_pSceneData;
    osg::ref_ptr<OsgControlPoints> m_gripPoints;
    std::vector<ControlPointsInViewport> m_controlPointsInViewport;
    double _offset;
    EditMode _mode;

    friend std::vector<osg::NodePath> GetStretchSelections(osg::Camera * camera);
};

