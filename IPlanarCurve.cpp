#include "stdafx.h"

#include <assert.h>

//#include <algorithm>
#include <numeric>

#include <osgViewer/View>

#include "IPlanarCurve.h"
#include "PickStretchHandler.h"
#include "OsgControlPoints.h"

void EnableStretch(osg::Camera* camera, double offset)
{
    if (!camera)
        return;
    auto view = dynamic_cast<osgViewer::View*>(camera->getView());
    if (!view)
        return;
    osg::View::Slave* slave = view->findSlaveForCamera(camera);
    osg::Node* node;
    if (slave)
    {
        assert(camera->getNumChildren() == 1);
        node = camera->getChild(0);
    }
    else
    {
        node = view->getSceneData();
    }
    if (!node)
        return;
    auto cb = camera->getEventCallback();
    bool  found = false;
    while (cb)
    {
        if (dynamic_cast<PickStretchHandler*>(cb))
        {
            found = true;
            break;
        }
        cb = cb->getNestedCallback();
    }
    if (!found)
    {
        camera->addEventCallback(new PickStretchHandler(camera, node, offset));
    }
}

std::vector<osg::NodePath> GetStretchSelections(osg::Camera * camera)
{
    std::vector<osg::NodePath> res;
    if (!camera)
        return res;
    auto view = dynamic_cast<osgViewer::View*>(camera->getView());
    if (!view)
        return res;
    auto cb = camera->getEventCallback();
    PickStretchHandler* handler;
    while (cb)
    {
        if (handler = dynamic_cast<PickStretchHandler*>(cb))
        {
            break;
        }
        cb = cb->getNestedCallback();
    }
    if (!handler)
    {
        return res;
    }
    handler->m_gripPoints->removeInvalidSelections();
    for (auto const& pair : handler->m_gripPoints->_selectionSet) {
        res.push_back(pair.first);
    }
    return res;
}
