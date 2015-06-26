// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"
#include "NodePick.h"


static int ReceivesShadowTraversalMask = 0x1;
static int CastsShadowTraversalMask = 0x2;

cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd) 
{
	spotExpo = 50;
}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void cOSG::InitOSG(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void cOSG::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}

osg::Node* cOSG::createLights()
{
	osg::Group* lightGroup = new osg::Group;

	// create a spot light.
	myLight1 = new osg::Light;
	myLight1->setLightNum(0);
	myLight1->setPosition(osg::Vec4(0,10,0,1.0f));
	myLight1->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight1->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight1->setSpecular(osg::Vec4(1,0,0,1));
	myLight1->setSpotCutoff(100.0f);
	myLight1->setSpotExponent(50.0f);
	myLight1->setDirection(osg::Vec3(1.0f,1.0f,-1.0f));

	osg::LightSource* lightS1 = new osg::LightSource;    
	lightS1->setLight(myLight1);
	lightS1->setLocalStateSetModes(osg::StateAttribute::ON); 

	//lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	lightGroup->addChild(lightS1);

	return lightGroup;
}

/** create quad at specified position. */
osg::Drawable* cOSG::createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image)
{
	// set up the Geometry.
	osg::Geometry* geom = new osg::Geometry;

	osg::Vec3Array* coords = new osg::Vec3Array(4);
	(*coords)[0] = corner;
	(*coords)[1] = corner+width;
	(*coords)[2] = corner+width+height;
	(*coords)[3] = corner+height;


	geom->setVertexArray(coords);

	osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0] = width^height;
	(*norms)[0].normalize();

	geom->setNormalArray(norms);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
	(*tcoords)[0].set(0.0f,0.0f);
	(*tcoords)[1].set(1.0f,0.0f);
	(*tcoords)[2].set(1.0f,1.0f);
	(*tcoords)[3].set(0.0f,1.0f);
	geom->setTexCoordArray(0,tcoords);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

// 	if (image)
// 	{
// 		osg::StateSet* stateset = new osg::StateSet;
// 		osg::Texture2D* texture = new osg::Texture2D;
// 		texture->setImage(image);
// 		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
// 		geom->setStateSet(stateset);
// 	}

	return geom;
}


osg::MatrixTransform* cOSG::createCylinder()
{
	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 3.0f) );
	doorShape->setColor( osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();

	// 	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func    
	// 	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);       
	// 	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);        
	// 	stateset->setAttributeAndModes( blendFunc );
	// 	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0, 0.0, 0.0));
	trans->addChild( geode.get() );

	geode->setName("Cylinder");
	return trans.release();
}
osg::MatrixTransform* cOSG::createBox()
{
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D();
	texture->setImage(image.get());

	//设置自动生成纹理坐标
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen();
	texgen->setMode(osg::TexGen::SPHERE_MAP);

	//设置纹理环境，模式为BLEND
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::Mode::ADD);
	texenv->setColor(osg::Vec4(1.0,0.0,0.0,1.0));

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 2.0f, 2.0f) );
	doorShape->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();

	//stateset->setTextureAttributeAndModes(1,texture.get(),osg::StateAttribute::ON);
	stateset->setAttributeAndModes(texture.get());
	//stateset->setTextureAttributeAndModes(1,texgen.get(),osg::StateAttribute::ON);
	stateset->setAttribute(texenv.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 2.0, -2.0));
	trans->addChild( geode.get() );

	geode->setNodeMask(CastsShadowTraversalMask);//阴影
	geode->setName("Box");
	return trans.release();
}

osg::MatrixTransform* cOSG::createSphere(float radius)  
{  
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D();
	texture->setImage(image.get());

	//设置自动生成纹理坐标
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen();
	texgen->setMode(osg::TexGen::SPHERE_MAP);

	//设置纹理环境，模式为BLEND
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::Mode::REPLACE);
	texenv->setColor(osg::Vec4(0.6,0.6,0.6,0.0));

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Sphere(osg::Vec3(0.0,0.0,0.0), radius) );
	//doorShape->setColor( osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();

	//stateset->setTextureAttributeAndModes(1,texture.get(),osg::StateAttribute::ON);
	//stateset->setTextureAttributeAndModes(1,texgen.get(),osg::StateAttribute::ON);
	//stateset->setTextureAttribute(1,texenv.get());
	stateset->setAttributeAndModes(texture.get());
	stateset->setAttribute(texenv.get());

	// 	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();  // blend func    
	// 	blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);       
	// 	blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);        
	// 	stateset->setAttributeAndModes( blendFunc );
	// 	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);  

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );

	geode->setNodeMask(CastsShadowTraversalMask);//阴影

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 1, 0.0));
	trans->addChild( geode.get() );

	geode->setName("Sphere");

	return trans.release();

}

osg::MatrixTransform* cOSG::createPrism()  
{  
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL; 

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;//菱形柱
	geode->addDrawable( createSquare(osg::Vec3(2.0f,0.0f,0.f),osg::Vec3(-2.0f,1.0f,0.0f),osg::Vec3(0.0f,0.0f,3.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(2.0f,0.0f,0.f),osg::Vec3(-2.0f,-1.0f,0.0f),osg::Vec3(0.0f,0.0f,3.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(-2.0f,0.0f,0.f),osg::Vec3(0.0f,0.0f,3.0f),osg::Vec3(2.0f,1.0f,0.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(-2.0f,0.0f,0.f),osg::Vec3(0.0f,0.0f,3.0f),osg::Vec3(2.0f,-1.0f,0.0f), image) );

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 3.0, 5.0));
	trans->addChild( geode.get() );

	geode->setName("Prism");
	geode->setNodeMask(CastsShadowTraversalMask);

	return trans.release();

}  

void cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
    //mRoot  = new osg::Group;
	mShadowedSceneRoot = new osgShadow::ShadowedScene;

	osg::ref_ptr<osgShadow::SoftShadowMap > ssm = new osgShadow::SoftShadowMap;
	// ssm->setAmbientBias(osg::Vec2(0.7,0.7));//0.5,0.5
	float bias = ssm->getBias();
	ssm->setBias(bias*2.0);
	mShadowedSceneRoot->setShadowTechnique(ssm.get());

	mShadowedSceneRoot->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	mShadowedSceneRoot->setCastsShadowTraversalMask(CastsShadowTraversalMask);

    // Load the Model from the model name
    //mModel = osgDB::readNodeFile("cube-tex.obj");
    // Optimize the model
//     osgUtil::Optimizer optimizer;
//     optimizer.optimize(mModel.get());
//     optimizer.reset();

    // Add the model to the scene
    //mRoot->addChild(mModel.get());
	//mRoot->addChild(osgDB::readNodeFile("floor.3ds"));
 	//mRoot->addChild(createBox());
 	//mRoot->addChild(createCylinder());
	//mRoot->addChild(createSphere(1));
	//mRoot->addChild(createLights());
	//mRoot->addChild(createXXX());
	osg::ref_ptr<osg::Node> floor = osgDB::readNodeFile("floor.3ds");
	if(floor)
		floor->setNodeMask(ReceivesShadowTraversalMask);
	mShadowedSceneRoot->addChild(floor.get());

	mShadowedSceneRoot->addChild(createSphere(1));
	mShadowedSceneRoot->addChild(createPrism());
	mShadowedSceneRoot->addChild(createBox());
	mShadowedSceneRoot->addChild(createLights());

}

void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);

	// add  a pick handler to the viewer
	mViewer->addEventHandler(new CPickHandler(mViewer));
    
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init a new Camera (Master for this View)
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

	

    // Set the Scene Data
    //mViewer->setSceneData(mRoot.get());
	mViewer->setSceneData(mShadowedSceneRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::ResetModelColor()
{

}
void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    //viewer->run();
    while(!viewer->done())
    {
        osg->PreFrameUpdate();
        viewer->frame();
        osg->PostFrameUpdate();
        //Sleep(10);         // Use this command if you need to allow other processes to have cpu time
    }

    // For some reason this has to be here to avoid issue: 
    // if you have multiple OSG windows up 
    // and you exit one then all stop rendering
    //AfxMessageBox("Exit Rendering Thread");

    _endthread();
}
