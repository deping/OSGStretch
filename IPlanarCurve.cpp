#include "stdafx.h"

#include <assert.h>

//#include <algorithm>
#include <numeric>

#include <osgViewer/View>

#include "IPlanarCurve.h"
#include "PickStretchHandler.h"

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
