#include "LightNode.h"
#include "LightInstance.h"

LightNode::LightNode (EntityClass* eclass) :
		m_contained(eclass, *this, InstanceSet::TransformChangedCaller(m_instances),
				InstanceSet::BoundsChangedCaller(m_instances),
				InstanceSetEvaluateTransform<LightInstance>::Caller(m_instances))
{
}

LightNode::LightNode (const LightNode& other) :
		scene::Node(other), scene::Instantiable(other), scene::Cloneable(other), scene::Traversable::Observer(other), Nameable(
				other), Snappable(other), Editable(other), TransformNode(other), scene::Traversable(other), EntityNode(
				other), Namespaced(other), m_contained(other.m_contained, *this,
				InstanceSet::TransformChangedCaller(m_instances), InstanceSet::BoundsChangedCaller(m_instances),
				InstanceSetEvaluateTransform<LightInstance>::Caller(m_instances))
{
}

LightNode::~LightNode ()
{
}

scene::Node& LightNode::clone () const
{
	return *(new LightNode(*this));
}

void LightNode::insertChild (scene::Node& child)
{
	m_instances.insertChild(child);
}

void LightNode::eraseChild (scene::Node& child)
{
	m_instances.eraseChild(child);
}

scene::Instance* LightNode::create (const scene::Path& path, scene::Instance* parent)
{
	return new LightInstance(path, parent, m_contained);
}

void LightNode::forEachInstance (const scene::Instantiable::Visitor& visitor)
{
	m_instances.forEachInstance(visitor);
}

void LightNode::insert (scene::Instantiable::Observer* observer, const scene::Path& path, scene::Instance* instance)
{
	m_instances.insert(observer, path, instance);
}

scene::Instance* LightNode::erase (scene::Instantiable::Observer* observer, const scene::Path& path)
{
	return m_instances.erase(observer, path);
}

std::string LightNode::name () const
{
	return m_contained.getNameable().name();
}

void LightNode::attach (const NameCallback& callback)
{
	m_contained.getNameable().attach(callback);
}

void LightNode::detach (const NameCallback& callback)
{
	m_contained.getNameable().detach(callback);
}

const Matrix4& LightNode::getLocalPivot () const
{
	return m_contained.getLocalPivot();
}

void LightNode::snapto (float snap)
{
	m_contained.snapto(snap);
}

const Matrix4& LightNode::localToParent () const
{
	return m_contained.getTransformNode().localToParent();
}

void LightNode::insert (Node& node)
{
	m_contained.getTraversable().insert(node);
}

void LightNode::erase (Node& node)
{
	m_contained.getTraversable().erase(node);
}

void LightNode::traverse (const Walker& walker)
{
	m_contained.getTraversable().traverse(walker);
}

bool LightNode::empty () const
{
	return m_contained.getTraversable().empty();
}

Entity& LightNode::getEntity ()
{
	return m_contained.getEntity();
}

void LightNode::setNamespace (INamespace& space)
{
	m_contained.getNamespaced().setNamespace(space);
}
