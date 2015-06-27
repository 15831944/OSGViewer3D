#include <osg/NodeVisitor>
#include <osgSim/DOFTransform>


class findNodeVisitor : public osg::NodeVisitor
{
public:
	findNodeVisitor();
	findNodeVisitor(const std::string &searchName) ;
	virtual void apply(osg::Node &searchNode);
	//virtual void apply(osg::Transform &searchNode);
	void setNameToFind(const std::string &searchName);
	osg::Node* getFirst();
	typedef std::vector<osg::Node*> nodeListType;
	nodeListType& getNodeList() { return foundNodeList; }
private:
	std::string searchForName;
	nodeListType foundNodeList;
};
findNodeVisitor::findNodeVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),searchForName()
{
}
findNodeVisitor::findNodeVisitor(const std::string &searchName):osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),searchForName(searchName)
{
}
void findNodeVisitor::setNameToFind(const std::string &searchName)
{
	searchForName = searchName;
	foundNodeList.clear();
}
osg::Node* findNodeVisitor::getFirst()
{
	return *(foundNodeList.begin());
}
void findNodeVisitor::apply(osg::Node &searchNode)
{
	if (searchNode.getName() == searchForName)
	{
		foundNodeList.push_back(&searchNode);
	}
	traverse(searchNode);
}
//void findNodeVisitor::apply(osg::Transform &searchNode)
//{
//	osgSim::DOFTransform* dofNode =
//		dynamic_cast<osgSim::DOFTransform*> (&searchNode);
//	if (dofNode)
//	{
//		dofNode->setAnimationOn(false);
//	}
//	apply ( (osg::Node&) searchNode);
//	traverse(searchNode);
//}