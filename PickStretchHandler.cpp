#include <stdafx.h>
#include <assert.h>
#include <osg/Point>
#include "PickStretchHandler.h"
#include "Utility.h"
#include "IPlanarCurve.h"
#include "PointIntersector.h"
#include "CurveIntersector.h"
#include "ControlPointsInViewportIntersector.h"
#include "OsgControlPoints.h"


PickStretchHandler::PickStretchHandler(osg::Camera* camera, osg::Node* node, double offset)
    : m_pCamera(camera)
    , m_gripPoints(new OsgControlPoints)
    , _offset(offset)
    , _mode(EditMode::None)
{
    assert(offset >= 3);
    assert(m_pCamera.valid());
    assert(node);
    auto group = node->asGroup();
    assert(group);
    assert(!group->asTransform());
    m_pSceneData = group;
    group->getOrCreateStateSet()->setAttribute(new osg::Point(10));
    auto geode = new osg::Geode;
    group->addChild(geode);
    geode->addDrawable(m_gripPoints);
    // off - square, on - circle
    m_gripPoints->getOrCreateStateSet()->setMode(GL_POINT_SMOOTH, osg::StateAttribute::OFF);
}

PickStretchHandler::~PickStretchHandler()
{
    auto camera = lock(m_pCamera);
    if (camera.valid())
    {
        camera->removeEventCallback(this);
    }
    m_pCamera = nullptr;
}

bool PickStretchHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    switch (ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::PUSH):
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            if (pick(ea))
            {
                _mode = EditMode::DragPoint;
                CollectControlPointsInViewport();
                cloneDraggedObject();
                return true;
            }

        }
        break;
    case(osgGA::GUIEventAdapter::RELEASE):
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            if (_mode == EditMode::DragPoint)
            {
                _mode = EditMode::None;
                releaseDraggedObject();
                stretch(ea, false);
                updateGripPoints();
                return true;
            }
        }
        break;
    case(osgGA::GUIEventAdapter::DRAG):
        if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            if (_mode == EditMode::DragPoint)
            {
                stretch(ea, true);
                return true;
            }
        }
        break;
    case(osgGA::GUIEventAdapter::KEYDOWN):
        if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Escape)
        {
            if (_mode == EditMode::DragPoint)
            {
                _mode = EditMode::None;
                releaseDraggedObject();
            }
            else
            {
                m_gripPoints->_selectionSet.clear();
            }
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

bool PickStretchHandler::pick(const osgGA::GUIEventAdapter& ea)
{
    auto cam = lock(m_pCamera);
    if (!cam)
        return false;
    if (!cam->getViewport())
        return false;
    osg::Matrix VPW = VPWmatrix(cam);
    if (!m_gripPoints->_selectionSet.empty())
    {
        PointIntersector pointPicker(ea.getX(), ea.getY(), _offset);
        bool hit = pointPicker.intersect(VPW, m_gripPoints.get());
        // If any control point is hit, return in order not to select curve.
        if (hit)
        {
            m_gripPoints->build();
            return true;
        }
    }

    auto root = lock(m_pSceneData);
    if (!root)
        return false;

    PlanarCurveVisitor::Intersections selectionSet;
    PlanarCurveVisitor curvePicker(selectionSet, VPW, ea.getX(), ea.getY(), _offset);
    curvePicker.setTraversalMask(~HANDLE_NODE_MASK);
    root->accept(curvePicker);

    // IntersectionVisitor::apply(osg::Transform& transform) will push_clone interceptor!!!
    //auto& selectionSet = curvePicker->getIntersections();
    auto modKey = ea.getModKeyMask();
    bool ctrlDown = (modKey == osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL || modKey == osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL);
    auto& curSelectionSet = m_gripPoints->_selectionSet;
    if (!ctrlDown)
    {
        curSelectionSet.clear();
    }
    for (auto it = selectionSet.begin(); it != selectionSet.end(); ++it)
    {
        if (!it->nodePath.empty())
        {
            auto it2 = curSelectionSet.find(it->nodePath);
            if (ctrlDown)
            {
                if (it2 != curSelectionSet.end())
                {
                    curSelectionSet.erase(it2);
                    continue;
                }
            }
            auto& info = curSelectionSet[it->nodePath];
            info.curve = it->curve;
            info.clone = nullptr;
            info.index = -1;
        }
    }
    updateGripPoints();
    return false;
}

void PickStretchHandler::stretch(const osgGA::GUIEventAdapter & ea, bool clone)
{
    osg::ref_ptr<osg::Camera> cam = lock(m_pCamera);
    if (!cam.valid())
        return;
    if (!cam->getViewport())
        return;
    osg::Matrix VPW = VPWmatrix(cam);
    osg::Matrix invVPW = osg::Matrix::inverse(VPW);
    auto& curSelectionSet = m_gripPoints->_selectionSet;
    for (auto it = curSelectionSet.begin(); it != curSelectionSet.end(); ++it)
    {
        auto& info = it->second;
        if (info.index != -1)
        {
            osg::Matrix m = osg::computeLocalToWorld(it->first);
            osg::Vec3d pos;
            bool success = XPointPlane(ea.getX(), ea.getY(), invVPW, m, pos);
            if (!success)
                return;
            osg::Matrix invM = osg::Matrix::inverse(m);
            pos = pos * invM;
            osg::Vec2d tmp(pos.x(), pos.y());
            SnapPoint(tmp, it->first, m, invM, VPW, invVPW);
            if (clone)
                dynamic_cast<IPlanarCurve*>(info.clone.get())->stretch(info.index, tmp, ea);
            else
                dynamic_cast<IPlanarCurve*>(info.curve.get())->stretch(info.index, tmp, ea);
        }
    }
}

osg::Group* getParent(const osg::NodePath& nodePath)
{
    for (auto it = nodePath.rbegin(); it != nodePath.rend(); ++it)
    {
        auto group = (*it)->asGroup();
        if (group)
            return group;
    }
    return nullptr;
}

void PickStretchHandler::cloneDraggedObject()
{
    auto& curSelectionSet = m_gripPoints->_selectionSet;
    for (auto it = curSelectionSet.begin(); it != curSelectionSet.end(); ++it)
    {
        auto& info = it->second;
        if (info.index != -1)
        {
            auto parent = getParent(it->first);
            assert(parent);
            auto node = info.curve.get();
            osg::ref_ptr<osg::Object> clone = node->clone(osg::CopyOp::DEEP_COPY_ALL);
            auto curve2 = dynamic_cast<IPlanarCurve*>(clone.get());
            if (curve2)
            {
                info.clone = dynamic_cast<osg::Node*>(curve2);
                // Geode::addDrawable == Group::addChild
                parent->addChild(info.clone);
                //geode->setDataVariance(osg::Object::DYNAMIC);
            }
        }
    }
}

void PickStretchHandler::releaseDraggedObject()
{
    auto& curSelectionSet = m_gripPoints->_selectionSet;
    for (auto it = curSelectionSet.begin(); it != curSelectionSet.end(); ++it)
    {
        auto& info = it->second;
        if (info.clone.valid())
        {
            auto parent = getParent(it->first);
            assert(parent);
            parent->removeChild(info.clone);
            info.clone = nullptr;
        }
    }
}

void PickStretchHandler::updateGripPoints()
{
    auto& curSelectionSet = m_gripPoints->_selectionSet;
    for (auto it = curSelectionSet.begin(); it != curSelectionSet.end(); ++it)
    {
        auto& info = it->second;
        dynamic_cast<IPlanarCurve*>(info.curve.get())->getControlPoints(info.controlPoints);
    }
    m_gripPoints->build();
}

osg::Matrix PickStretchHandler::VPWmatrix(osg::Camera * cam)
{
    osg::Matrix VPW = cam->getViewport()->computeWindowMatrix();
    VPW.preMult(cam->getProjectionMatrix());
    VPW.preMult(cam->getViewMatrix());
    return  VPW;
}

bool PickStretchHandler::CollectControlPointsInViewport()
{
    m_controlPointsInViewport.clear();
    auto cam = lock(m_pCamera);
    if (!cam.valid())
        return false;
    auto viewport = cam->getViewport();
    if (!viewport)
        return false;
    osg::Matrix VPW = VPWmatrix(cam);

    auto root = lock(m_pSceneData);
    if (!root)
        return false;

    osg::ref_ptr<ControlPointsInViewportIntersector> controlPointsPicker = new ControlPointsInViewportIntersector(m_controlPointsInViewport, VPW, viewport->x(), viewport->y(), viewport->width(), viewport->height());
    osgUtil::IntersectionVisitor iv(controlPointsPicker.get());
    iv.pushWindowMatrix(cam->getViewport());
    iv.pushViewMatrix(new osg::RefMatrix(cam->getViewMatrix()));
    iv.pushProjectionMatrix(new osg::RefMatrix(cam->getProjectionMatrix()));
    iv.setTraversalMask(~HANDLE_NODE_MASK);
    root->accept(iv);
    return true;
}

void PickStretchHandler::SnapPoint(osg::Vec2d& pt/*in, out*/, const osg::NodePath& obj, const osg::Matrix& M, const osg::Matrix& invM, const osg::Matrix & VPW, const osg::Matrix& invVPW)
{
    auto ptInViewport = osg::Vec3d(pt, 0) * M * VPW;
    for (const auto& curve : m_controlPointsInViewport)
    {
        // Don't snap control points on itself.
        if (curve._obj == obj)
            continue;
        if (IsMatrixSamePlane(curve._matrix, M))
        {
            auto MVPW = curve._matrix * VPW;
            for (const auto& cp : curve._controlPoints)
            {
                auto cpInViewport = osg::Vec3d(cp, 0) * MVPW;

                if (abs(cpInViewport.x() - ptInViewport.x()) <= _offset && abs(cpInViewport.y() - ptInViewport.y()) <= _offset)
                {
                    auto snapPt = cpInViewport * invVPW * invM;
                    pt.x() = snapPt.x();
                    pt.y() = snapPt.y();
                    // return on first snap, don't find the closest control point.
                    return;
                }
            }
        }
    }
}
