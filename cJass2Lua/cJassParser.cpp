#include "cJassParser.h"

namespace cJass
{
	NodePtr NodeFactory::Produce(Node::Type type, NodePtr top)
	{
		switch (type)
		{
		case Node::Type::GlobalSpace:
			return std::shared_ptr<GlobalSpace>(new GlobalSpace);
		case Node::Type::Function:
			return std::shared_ptr<Function>(new Function(top));
		default:
			return nullptr;
		}
	}

	Node::Node(Type type, NodePtr top) 
		: _type(type)
		, _top(top)
		, _isNewLine(false)
		, _tabs(0)
	{
	}

	void Node::AddSubnode(NodePtr node)
	{
		_subnodes.push_back(node);
		ResetIterator();
	}

	Node::Type Node::GetType() const
	{
		return _type;
	}

	NodePtr Node::IterateSubnodes()
	{
		if (_it == _subnodes.begin())
			return *(_it++);
		else if (_it == _subnodes.end())
			return nullptr;
		else
			return *(++_it);
	}

	void Node::ResetIterator()
	{
		_it = _subnodes.begin();
	}


	GlobalSpace::GlobalSpace()
		: Node(Node::Type::GlobalSpace, nullptr)
	{
	}

	std::string GlobalSpace::ToLua()
	{
		return ""; //TODO
	}

	void GlobalSpace::ConstructFrom(const std::string cjCode)
	{

	}

	Function::Function(NodePtr top)
		: Node(Node::Type::Function, top)
		, _name("")
		, _returnType("")
		, _args({})
	{
	}

	std::string Function::ToLua()
	{
		return ""; //TODO
	}

	void Function::ConstructFrom(const std::string cjCode)
	{

	}


} //namespace cJass