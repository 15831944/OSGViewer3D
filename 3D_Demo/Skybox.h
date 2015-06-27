#include <osg/ShapeDrawable>
#include <osg/Transform>
#include <osg/Material>
#include <osg/NodeCallback>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/VertexProgram>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Optimizer>

#include <osgViewer/Viewer>


class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
	/** Get the transformation matrix which moves from local coords to world coords.*/
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMultTranslate(eyePointLocal);
		}
		return true;
	}

	/** Get the transformation matrix which moves from world coords to local coords.*/
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMultTranslate(-eyePointLocal);
		}
		return true;
	}
};
// Update texture matrix for cubemaps
struct TexMatCallback : public osg::NodeCallback
{
public:

	TexMatCallback(osg::TexMat& tm) :
	  _texMat(tm)
	  {
	  }

	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	  {
		  osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		  if (cv)
		  {
			  const osg::Matrix& MV = *(cv->getModelViewMatrix());
			  /*const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
			  osg::Matrix::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);*/
			  const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(180.0f), 0.0f,0.0f,1.0f);

			  osg::Quat q = MV.getRotate();
			  const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

			  _texMat.setMatrix( C*R );
		  }

		  traverse(node,nv);
	  }

	  osg::TexMat& _texMat;
};
osg::Node* createSkyBox();