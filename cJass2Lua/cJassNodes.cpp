#include "cJassNodes.h"
#include "reutils.h"

#define PRODUCING_NODE_ROOT(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<##__node__>(new __node__)

#define PRODUCING_NODE(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<__node__>(new __node__(top))

namespace cJass
{
	NodePtr Node::Produce(Node::Type type, Node* top)
	{
		switch (type)
		{
			PRODUCING_NODE_ROOT(GlobalSpace);
			PRODUCING_NODE(Function);
			PRODUCING_NODE(Comment);
			PRODUCING_NODE(Operation);
		default:
			return nullptr;
		}
	}

	Node::Node(Type type, Node* top) 
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

	Node* Node::Ptr()
	{
		return dynamic_cast<Node*>(this);
	}

	Node* Node::Top()
	{
		return _top;
	}

	Node* Node::LastSubnode()
	{
		if (_subnodes.size() == 0)
			return nullptr;
		return _subnodes.back()->Ptr();
	}

	GlobalSpace::GlobalSpace()
		: Node(Node::Type::GlobalSpace, nullptr)
		, _globals({})
	{
	}

	std::string GlobalSpace::ToLua()
	{
		return ""; //TODO
	}

	void GlobalSpace::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size % 3 != 0)
			throw std::runtime_error("GlobalSpace::InitData - wrong set of input data.");

		if (size == 0)
			return;

		variable_t var;
		for (size_t i = 0; i < size; i++)
		{
			auto n = i + 1;
			if (n % 3 == 0)
			{
				var.initValue = strings[i];
				_globals.push_back(var);
			}
			else if (n % 2 == 0)
				var.name = strings[i];
			else
				var.type = strings[i];

		}
	}

	Comment::Comment(Node* top)
		: Node(Node::Type::Comment, top)
		, _comment("")
	{

	}

	std::string Comment::ToLua()
	{
		return ""; //TODO
	}

	void Comment::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size != 1)
			throw std::runtime_error("Comment::InitData - wrong set of input data.");

		_comment = strings[0];
		if (_comment.length() >= 2)
			_comment = reu::IndexSubstr(_comment, 2, _comment.length() - 1);
		if (_comment.length() >= 2 && _comment[_comment.length() - 1] == '/' && _comment[_comment.length() - 2] == '*')
		{
			if (_comment.length() == 2)
				_comment = "";
			else
				_comment = reu::IndexSubstr(_comment, 0, _comment.length() - 3);
		}
	}

	Function::Function(Node* top)
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

	void Function::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size < 2)
			throw std::runtime_error("Function::InitData - wrong set of input data.");

		_returnType = strings[0];
		_name = strings[1];

		for (size_t i = 2; i < size; i++)
			_args.push_back(strings[i]);
	}


} //namespace cJass