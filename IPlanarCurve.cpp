#include "stdafx.h"

//#include <algorithm>
#include <numeric>
#include "IPlanarCurve.h"
#include "PickStretchHandler.h"

void EnableStretch(osgViewer::View * pView, double offset)
{
    if (!pView)
        return;
    auto pNode = pView->getSceneData();
    if (!pNode)
        return;
    auto handlers = pView->getEventHandlers();
    for (auto handler : handlers)
    {
        auto picker = dynamic_cast<PickStretchHandler*>(handler.get());
        if (picker)
        {
            return;
        }
    }
    pView->addEventHandler(new PickStretchHandler(pView, offset));
}
