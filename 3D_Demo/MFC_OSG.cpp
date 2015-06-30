// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"
#include "NodePick.h"
#include "FindNodeVisitor.h"
#include "TraverseNodeVisitor.h"
#include "Skybox.h"
#include "AxisBillBoardVisitor.h"

//阴影投射掩码
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
//创建方向光源
osg::Node* cOSG::createDirectionalLight()
{
	osg::Group* lightGroup = new osg::Group;
	// create a directional light.
	osg::Light* myLight = new osg::Light;
	myLight->setLightNum(0);
	myLight->setPosition(osg::Vec4(1,1,1,0.0f));//方向 1，1，1
	myLight->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight->setSpecular(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	osg::LightSource* lightS = new osg::LightSource;    
	lightS->setLight(myLight);
	lightS->setLocalStateSetModes(osg::StateAttribute::ON);
	//lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	lightGroup->addChild(lightS);

	return lightGroup;
}
//创建点光源
osg::Node* cOSG::createPointLight()
{
	osg::Group* lightGroup = new osg::Group;

	osg::Light* myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(osg::Vec4(0,20,0,1.0f));
	myLight->setAmbient(osg::Vec4(0.5f,0.5f,0.5f,1.0f));
	myLight->setDiffuse(osg::Vec4(0.5f,0.5f,0.5f,1.0f));
	myLight->setSpecular(osg::Vec4(1,0,0,1));

	osg::LightSource* lightS = new osg::LightSource;    
	lightS->setLight(myLight);
	lightS->setLocalStateSetModes(osg::StateAttribute::ON);
	//lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	lightGroup->addChild(lightS);

	return lightGroup;
}
//创建一个聚光灯，控制移动
osg::Node* cOSG::createMoveLight()
{
	osg::Group* lightGroup = new osg::Group;
	// create a spot light.
	osg::Light* myLight = new osg::Light;
	myLight->setLightNum(2);
	myLight->setPosition(osg::Vec4(0,0,0,1.0f));
	myLight->setAmbient(osg::Vec4(10.0f,0.0f,0.0f,1.0f));
	myLight->setDiffuse(osg::Vec4(10.0f,0.0f,0.0f,1.0f));
	myLight->setSpecular(osg::Vec4(1,0,0,1));
	myLight->setSpotCutoff(50.0f);
	myLight->setSpotExponent(50.0f);
	myLight->setDirection(osg::Vec3(0.0f,-1.0f,0.0f));

	osg::LightSource* lightS = new osg::LightSource;    
	lightS->setLight(myLight);
	lightS->setLocalStateSetModes(osg::StateAttribute::ON);
	//lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	lightGroup->addChild(lightS);

	return lightGroup;
}

osg::Camera* cOSG::createHUD()
{
	// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
	osg::Camera* camera = new osg::Camera;

	camera->setViewport(0,0,200,200);
	// set the projection matrix
	camera->setProjectionMatrix(osg::Matrix::ortho(-1.5,1.5,-1.5,1.5,-50,100));

	// set the view matrix    
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//camera->setViewMatrix(osg::Matrix::identity());
	osg::Vec3 eye(0,50,0), center(0,0,0), up(0,0,1);
	camera->setViewMatrix(osg::Matrix::lookAt(eye,center,up));	

	// only clear the depth buffer
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	// draw subgraph after main camera view.
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	// we don't want the camera to grab event focus from the viewers main camera(s).
	camera->setAllowEventFocus(false);

	{
		osg::ref_ptr<osg::Node> axis = osgDB::readNodeFile("axes.osgt");

		AxisBillBoardVisitor AxisTextVisitor;
		axis->accept(AxisTextVisitor);
		camera->addChild(axis);
	}


	return camera;
}

//创建quad
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
	(*norms)[0] = height^width;
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

	if (image)
	{
		osg::StateSet* stateset = new osg::StateSet;
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		geom->setStateSet(stateset);
	}

	return geom;
}

//创建圆柱
osg::MatrixTransform* cOSG::createCylinder()
{
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D(image.get());

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 3.0f) );
	//doorShape->setColor( osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f) );   // alpha value

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );

	//设置模型材质
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture.get(), osg::StateAttribute::ON);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(4.0, 4.0, 6.0));
	trans->addChild( geode.get() );

	geode->setNodeMask(CastsShadowTraversalMask);

	geode->setName("Cylinder");
	return trans.release();
}

//创建立方体
osg::MatrixTransform* cOSG::createBox()
{
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D(image.get());

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 2.0f, 2.0f) );
	//doorShape->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();

	stateset->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );

	// material
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 2.0, -4.0));
	trans->addChild( geode.get() );

	geode->setNodeMask(CastsShadowTraversalMask);//阴影
	geode->setName("Box");
	return trans.release();
}
//创建球体
osg::MatrixTransform* cOSG::createSphere(float radius)  
{  
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("tree.png");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D(image.get());

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Sphere(osg::Vec3(0.0,0.0,0.0), radius) );
	//doorShape->setColor( osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );
	geode->setNodeMask(CastsShadowTraversalMask);//阴影

	// material
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 4, -2.0));
	trans->addChild( geode.get() );

	geode->setName("Sphere");
	return trans.release();

}
//创建圆锥
osg::MatrixTransform* cOSG::createCone(float radius, float height)  
{  
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;

	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D(image.get());

	osg::ref_ptr<osg::ShapeDrawable> doorShape =
		new osg::ShapeDrawable( new osg::Cone(osg::Vec3(0.0,0.0,0.0), radius,height) );
	//doorShape->setColor( osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );   // alpha value

	osg::StateSet* stateset = doorShape->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( doorShape.get() );
	geode->setNodeMask(CastsShadowTraversalMask);//阴影

	// material
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(-3, 4, -2.0));
	trans->addChild( geode.get() );

	geode->setName("Cone");
	return trans.release();

}
//创建圆环
osg::MatrixTransform* cOSG::createTorusGeode(float MinorRadius, float MajorRadius)
{
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL;
	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D(image.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::Geometry *geometry = new osg::Geometry;
	osg::Vec3Array *vertexArray = new osg::Vec3Array;
	osg::Vec3Array *normalArray = new osg::Vec3Array;

	for (int i=0; i<20; i++ )										// Stacks
	{
		for (int j=-1; j<20; j++)									// Slices
		{
			float wrapFrac = (j%20)/(float)20;
			float phi = 2* osg::PI*wrapFrac;
			float sinphi = float(sin(phi));
			float cosphi = float(cos(phi));

			float r = MajorRadius + MinorRadius*cosphi;

			normalArray->push_back(osg::Vec3(float(sin(2* osg::PI*(i%20+wrapFrac)/(float)20))*cosphi, sinphi, float(cos(2* osg::PI*(i%20+wrapFrac)/(float)20))*cosphi));
			vertexArray->push_back(osg::Vec3(float(sin(2* osg::PI*(i%20+wrapFrac)/(float)20))*r,MinorRadius*sinphi,float(cos(2* osg::PI*(i%20+wrapFrac)/(float)20))*r));
			normalArray->push_back(osg::Vec3(float(sin(2* osg::PI*(i+1%20+wrapFrac)/(float)20))*cosphi, sinphi, float(cos(2* osg::PI*(i+1%20+wrapFrac)/(float)20))*cosphi));
			vertexArray->push_back(osg::Vec3(float(sin(2* osg::PI*(i+1%20+wrapFrac)/(float)20))*r,MinorRadius*sinphi,float(cos(2* osg::PI*(i+1%20+wrapFrac)/(float)20))*r));
		}
	}

	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray);
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertexArray->size()));

	//设置自动生成纹理坐标
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen();
	texgen->setMode(osg::TexGen::EYE_LINEAR);

	geometry->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON);
	geometry->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texgen.get(), osg::StateAttribute::ON);

	geode->addDrawable(geometry);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(-5.0, 3.0, 5.0));
	trans->addChild( geode.get() );

	geode->setName("Torus");
	geode->setNodeMask(CastsShadowTraversalMask);

	// material
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	return trans.release();
}
//创建菱形柱
osg::MatrixTransform* cOSG::createPrism()  
{  
	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("whitemetal.jpg");
	if (!image.get())
		return NULL; 

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;//菱形柱
	geode->addDrawable( createSquare(osg::Vec3(2.0f,0.0f,0.f),osg::Vec3(0.0f,0.0f,3.0f),osg::Vec3(-2.0f,1.0f,0.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(2.0f,0.0f,0.f),osg::Vec3(0.0f,0.0f,3.0f),osg::Vec3(-2.0f,-1.0f,0.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(-2.0f,0.0f,0.f),osg::Vec3(2.0f,1.0f,0.0f),osg::Vec3(0.0f,0.0f,3.0f), image) );
	geode->addDrawable( createSquare(osg::Vec3(-2.0f,0.0f,0.f),osg::Vec3(2.0f,-1.0f,0.0f),osg::Vec3(0.0f,0.0f,3.0f), image) );

	// material
	osg::ref_ptr<osg::Material> matirial = new osg::Material;
	matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
	matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
	geode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);

	//设置矩阵，用来变换模型
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(0.0, 3.0, 5.0));
	trans->addChild( geode.get() );

	geode->setName("Prism");
	geode->setNodeMask(CastsShadowTraversalMask);

	return trans.release();

}  

void cOSG::InitSceneGraph(void)
{
    //总根节点
    mRoot  = new osg::Group;
	//创建天空盒
	mRoot->addChild(createSkyBox());

	//总阴影根节点，所有带阴影的模型都放在这个节点下
	mShadowedSceneRoot = new osgShadow::ShadowedScene;
	//阴影map
	osg::ref_ptr<osgShadow::SoftShadowMap > ssm = new osgShadow::SoftShadowMap;
	float bias = ssm->getBias();
	ssm->setBias(bias*2.0);
	mShadowedSceneRoot->setShadowTechnique(ssm.get());
	mShadowedSceneRoot->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	mShadowedSceneRoot->setCastsShadowTraversalMask(CastsShadowTraversalMask);

	//载入汽车模型（obj格式）
	osg::ref_ptr<osg::Node> car = osgDB::readNodeFile("car/car-n.obj");
	if(car)
		car->setNodeMask(CastsShadowTraversalMask);
	//旋转和移动汽车
	mTrans = new osg::MatrixTransform;
	mTrans->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(90.0),-1,0,0)*osg::Matrix::translate(6, 2, -8));
	mTrans->addChild( car.get() );

	//创建可移动的聚光灯
	mTrans->addChild(createMoveLight());

	mShadowedSceneRoot->addChild(mTrans.get());

    //载入地板模型（3ds格式）
	osg::ref_ptr<osg::Node> floor = osgDB::readNodeFile("floor.3ds");
	if(floor)
		floor->setNodeMask(ReceivesShadowTraversalMask);
	mShadowedSceneRoot->addChild(floor.get());
	
	////创建球体
	mShadowedSceneRoot->addChild(createSphere(1));
	////创建圆锥体
	mShadowedSceneRoot->addChild(createCone(1,3));
	//创建菱形柱
	mShadowedSceneRoot->addChild(createPrism());
	//创建方柱
	mShadowedSceneRoot->addChild(createBox());
	//创建圆柱
	mShadowedSceneRoot->addChild(createCylinder());
	//创建圆环
	mShadowedSceneRoot->addChild(createTorusGeode(0.5, 2));
	//创建点光源
	mShadowedSceneRoot->addChild(createPointLight());
	//创建方向光源
	mShadowedSceneRoot->addChild(createDirectionalLight());

	osg::StateSet * rootState = mShadowedSceneRoot->getOrCreateStateSet();
	rootState->setMode(GL_LIGHTING,osg::StateAttribute::ON); 
	rootState->setMode(GL_LIGHT0,osg::StateAttribute::ON); 
	rootState->setMode(GL_LIGHT1,osg::StateAttribute::ON); 
	rootState->setMode(GL_LIGHT2,osg::StateAttribute::ON); 
}
//事件和视口设置
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

	osg::Vec3 eye(0,50,0), center(0,5,0), up(0,0,1);
	keyswitchManipulator->setHomePosition(eye,center,up);

    // Set the Scene Data
    //mViewer->setSceneData(mRoot.get());
	mRoot->addChild(mShadowedSceneRoot);
	mHudCamera = createHUD();
	mRoot->addChild(mHudCamera.get());
	mViewer->setSceneData(mRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}
//重置拾取后模型效果
void cOSG::ResetModelColor()
{
	TraverseNodeVisitor resetNodeMaterial;
	mShadowedSceneRoot->accept(resetNodeMaterial);
}
//控制car
void cOSG::TransModel(osg::Matrix& m)
{
	mTrans->postMult(m);
}
void cOSG::PreFrameUpdate()
{
    //设置左下角轴箭头的变换矩阵
	osg::Vec3 eye, center, up;
	mViewer->getCamera()->getViewMatrix().getLookAt(eye,center,up);
	eye = eye - center;
	eye.normalize();
	mHudCamera->setViewMatrix(osg::Matrix::lookAt(eye*50,osg::Vec3(0,0,0),up));
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
