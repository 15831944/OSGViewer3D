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
		return false;
		switch(ea.getEventType())
		{
			// PUSH, button down
		case osgGA::GUIEventAdapter::PUSH:
			// 1,left button push
			if (ea.getButton()==1)
			{
				Pick(ea.getX(),ea.getY());
			}
			return true;
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
				if (! hitr->nodePath.empty() && ! (hitr->nodePath.back()->getName().empty()))
				{
// 					osg::Geode* pNode = (osg::Geode*)(hitr->nodePath.back());
// 					for(int i = 0; i < pNode->getNumDrawables(); i++)
// 					{
// 
// 					}
// 					osg::ref_ptr<osg::ShapeDrawable> lxq = (osg::ShapeDrawable*)((osg::Geode*)(hitr->nodePath.back()))->getDrawable(0);
// 					lxq->setColor( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); 
// 					break;
				}

			}
		}
	}
	osgViewer::Viewer *mViewer;
};

