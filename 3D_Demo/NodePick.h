#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/Node>
#include <osgFX/Scribe>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

class CPickHandler:public osgGA::GUIEventHandler
{
public:
	CPickHandler(osgViewer::Viewer * viewer):mViewer(viewer){};
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &&aa)
	{
		switch(ea.getEventType())
		{
			// PUSH, button down
		case osgGA::GUIEventAdapter::PUSH:
			// 1,left button push
			if (ea.getButton()==1)
			{
				Pick(ea.getX(),ea.getY());
			}
			return false;
		}
		return false;
	}
protected:
	void Pick(float x, float y)
	{
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (mViewer->computeIntersections(x,y,intersections))
		{
			for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
				hitr != intersections.end();
				++ hitr)
			{
				bool flag = false;
				for (int i = hitr->nodePath.size()-1; i >=0; i--)
				{
					if ( ! (hitr->nodePath[i]->getName().empty()))
					{
						//将拾取到的模型的材质设置为红色
						osg::Geode* pNode = (osg::Geode*)(hitr->nodePath[i]);
						// material
						osg::ref_ptr<osg::Material> matirial = new osg::Material;
						matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 0, 0, 1));
						matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 0, 0, 1));
						matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 0, 0, 1));
						matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
						pNode->getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);
						flag = true;
						break;
					}
					
				}
				if (flag)
					break;

			}
		}
	}
	osgViewer::Viewer *mViewer;
};

