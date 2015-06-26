#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/PositionAttitudeTransform>
#include <osgShadow/ShadowedScene>
#include <osgShadow/SoftShadowMap>
#include <string>

class cOSG
{
public:
    cOSG(HWND hWnd);
    ~cOSG();

    void InitOSG(std::string filename);
    void InitManipulators(void);
    void InitSceneGraph(void);
    void InitCameraConfig(void);

	void ResetModelColor();
    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    static void Render(void* ptr);

    osgViewer::Viewer* getViewer() { return mViewer; }


	osg::Node* createLights();
	/** create quad at specified position. */
	osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL);
	osg::MatrixTransform* createCylinder();
	osg::MatrixTransform* createBox();
	osg::MatrixTransform* createSphere(float radius);

	osg::MatrixTransform* createPrism();


private:
    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
    osgViewer::Viewer* mViewer;
    osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osgShadow::ShadowedScene> mShadowedSceneRoot;
    osg::ref_ptr<osg::Node> mModel;
    osg::ref_ptr<osgGA::TrackballManipulator> trackball;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

public:
	float spotExpo;
	osg::Light* myLight1;
};
