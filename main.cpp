#include "stdafx.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/LineWidth>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include "IPlanarCurve.h"
#include "Line.h"

osg::MatrixTransform* createTest(const osg::Matrix& m, const osg::Vec4& lineColor, const osg::Vec4& planeColor)
{
    auto l1 = new Line;
    auto points = new osg::Vec2Array;
    points->push_back(osg::Vec2(0, 0));
    points->push_back(osg::Vec2(0, 10));
    points->push_back(osg::Vec2(10, 10));
    l1->setPoints(points);
    l1->setColor(lineColor);
    auto geode = new osg::Geode;
    geode->addDrawable(l1);
    geode->setDataVariance(osg::Object::DYNAMIC);
    auto refBox = new osg::ShapeDrawable;
    refBox->setShape(new osg::Box(osg::Vec3d(5, 5, -1), 10, 10, 0.1));
    refBox->setColor(planeColor);
    geode->addChild(refBox);
    auto res = new osg::MatrixTransform;
    res->setMatrix(m);
    res->addChild(geode);
    return res;
}

int main(int, char**)
{
    osgViewer::Viewer viewer;
    //viewer.setUpViewInWindow(100, 100, 600, 800);
    osg::ref_ptr<osg::GraphicsContext::Traits> traits =
        new osg::GraphicsContext::Traits;
    traits->x = 50;
    traits->y = 50;
    traits->width = 800;
    traits->height = 600;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    //traits->samples = 4;

    osg::ref_ptr<osg::GraphicsContext> gc =
        osg::GraphicsContext::createGraphicsContext(traits.get());
    osg::ref_ptr<osg::Camera> camera = viewer.getCamera();
    camera->setGraphicsContext(gc);
    camera->setViewport(
        new osg::Viewport(0, 0, traits->width, traits->height));
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));

    osg::ref_ptr<osg::Group> root = new osg::Group();
    viewer.setSceneData(root.get());
    EnableStretch(viewer.getCamera(), 5);

    root->addChild(createTest(osg::Matrix::identity(), osg::Vec4(0.5, 0.5, 0, 1), osg::Vec4(0, 0.5, 0.5, 1)));
    root->addChild(createTest(osg::Matrix::rotate(osg::PI_2, osg::X_AXIS), osg::Vec4(0.5, 0.5, 0, 1), osg::Vec4(0.5, 0, 0.5, 1)));
    root->addChild(createTest(osg::Matrix::rotate(osg::PI_2, osg::X_AXIS) * osg::Matrix::translate(15, 0, 10), osg::Vec4(0.5, 0.5, 0, 1), osg::Vec4(0.5, 0, 0.5, 1)));

    root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    root->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    root->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    // add the state manipulator
    //viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

    // add the stats handler
    //viewer.addEventHandler(new osgViewer::StatsHandler);
    osg::ref_ptr<osgGA::TrackballManipulator> man = new osgGA::TrackballManipulator();
    man->setHomePosition(osg::Vec3d(5, 5, 20), osg::Vec3d(5, 5, 0), osg::Vec3d(0, 1, 0));
    viewer.setCameraManipulator(man.get());
    viewer.realize();
    viewer.setRunFrameScheme(osgViewer::Viewer::ON_DEMAND);
    return viewer.run();
}
