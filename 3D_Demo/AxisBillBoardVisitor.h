#include <osg/NodeVisitor>
#include <osgSim/DOFTransform>
#include <osg/Material>


class AxisBillBoardVisitor : public osg::NodeVisitor
{
public:
	AxisBillBoardVisitor();
	virtual void apply(osg::Billboard &searchNode);
private:

};
AxisBillBoardVisitor::AxisBillBoardVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}

void AxisBillBoardVisitor::apply(osg::Billboard &searchNode)
{
	searchNode.setMode(osg::Billboard::POINT_ROT_EYE);
	//只遍历有名字的node
	if (!searchNode.getName().empty())
	{
		//将所有的模型材质设置为白色
		// material
		osg::ref_ptr<osg::Material> matirial = new osg::Material;
		matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
		matirial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
		matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
		matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
		searchNode.getOrCreateStateSet()->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);
	}
	traverse(searchNode);
}
