#include "GenericEntityNode.h"

GenericEntityNode::GenericEntityNode (EntityClass* eclass) :
		m_contained(eclass, *this, InstanceSet::TransformChangedCaller(m_instances),
				InstanceSetEvaluateTransform<GenericEntityInstance>::Caller(m_instances))
{
}

GenericEntityNode::GenericEntityNode (const GenericEntityNode& other) :
		scene::Node(other), scene::Instantiable(other), scene::Cloneable(other), Nameable(other), Snappable(other), TransformNode(
				other), EntityNode(other), Namespaced(other), m_contained(other.m_contained, *this,
				InstanceSet::TransformChangedCaller(m_instances),
				InstanceSetEvaluateTransform<GenericEntityInstance>::Caller(m_instances))
{
}

void GenericEntityNode::snapto (float snap)
{
	m_contained.snapto(snap);
}

const Matrix4& GenericEntityNode::localToParent () const
{
	return m_contained.getTransformNode().localToParent();
}

Entity& GenericEntityNode::getEntity ()
{
	return m_contained.getEntity();
}

void GenericEntityNode::setNamespace (INamespace& space)
{
	m_contained.getNamespaced().setNamespace(space);
}

scene::Node& GenericEntityNode::clone () const
{
	return *(new GenericEntityNode(*this));
}

scene::Instance* GenericEntityNode::create (const scene::Path& path, scene::Instance* parent)
{
	return new GenericEntityInstance(path, parent, m_contained);
}

void GenericEntityNode::forEachInstance (const scene::Instantiable::Visitor& visitor)
{
	m_instances.forEachInstance(visitor);
}

void GenericEntityNode::insert (scene::Instantiable::Observer* observer, const scene::Path& path,
		scene::Instance* instance)
{
	m_instances.insert(observer, path, instance);
}

scene::Instance* GenericEntityNode::erase (scene::Instantiable::Observer* observer, const scene::Path& path)
{
	return m_instances.erase(observer, path);
}

std::string GenericEntityNode::name () const
{
	return m_contained.getNameable().name();
}

void GenericEntityNode::attach (const NameCallback& callback)
{
	m_contained.getNameable().attach(callback);
}

void GenericEntityNode::detach (const NameCallback& callback)
{
	m_contained.getNameable().detach(callback);
}
