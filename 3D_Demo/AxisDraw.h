
#include <osg/Drawable>

// draw callback that will set the matrix just
// before rendering a drawable.
class AxisDrawCallback : public osg::Drawable::DrawCallback
{
public:
	AxisDrawCallback(double radius)
		: _radius(radius) {}

	virtual void drawImplementation(osg::RenderInfo& renderInfo,
		const osg::Drawable* drawable) const
	{
		osg::Camera* currentCamera = renderInfo.getCurrentCamera();
		if (currentCamera)
		{
			// Get the current camera position.
			osg::Vec3 eye, center, up;
			renderInfo.getCurrentCamera()->getViewMatrixAsLookAt( eye, center, up);

			// Get the max distance we need the far plane to be at,
			// which is the distance between the eye and the origin
			// plus the distant from the origin to the object (star sphere
			// radius, sun distance etc), and then some.
			double distance = eye.length() + _radius*2;

			// Save old values.
			osg::ref_ptr<osg::RefMatrixd> oldProjectionMatrix = new osg::RefMatrix;
			oldProjectionMatrix->set( renderInfo.getState()->getProjectionMatrix());

			// Get the individual values
			double left, right, bottom, top, zNear, zFar;
			oldProjectionMatrix->getFrustum( left, right, bottom, top, zNear, zFar);

			// Build a new projection matrix with a modified far plane
			osg::ref_ptr<osg::RefMatrixd> projectionMatrix = new osg::RefMatrix;
			projectionMatrix->makeFrustum( left, right, bottom, top, zNear, distance );

			renderInfo.getState()->applyProjectionMatrix( projectionMatrix.get());

			// Draw the drawable
			drawable->drawImplementation(renderInfo);

			// Reset the far plane to the old value.
			renderInfo.getState()->applyProjectionMatrix( oldProjectionMatrix.get() );
		}
		else
		{
			drawable->drawImplementation(renderInfo);
		}
	}

protected:
	double _radius;
};