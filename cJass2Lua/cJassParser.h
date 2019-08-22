#pragma once

#include <string>
#include <list>
#include <vector>
#include <memory>
#include "singleton.h"

namespace cJass
{
	class Node;
	using NodePtr = std::shared_ptr<Node>;
	using NodeList = std::list<NodePtr>;
	using NL_iter = NodeList::iterator;

	class Node
	{
	public:
		enum class Type
		{
			Undefined,
			GlobalSpace,
			Function
		};

	protected:
		bool _isNewLine;
		size_t _tabs;

	private:
		Type _type;
		NodeList _subnodes;
		NodePtr _top;
		NL_iter _it;

		Node(Node&) = delete;

	public:
		Node(Type type = Type::Undefined, NodePtr top = nullptr);
		void AddSubnode(NodePtr node);
		Type GetType() const;

		NodePtr IterateSubnodes();
		void ResetIterator();
		virtual std::string ToLua() = 0;
		virtual void ConstructFrom(const std::string cjCode) = 0;
	};

	class GlobalSpace : public Node
	{
	public:
		GlobalSpace();
		virtual std::string ToLua() override;
		virtual void ConstructFrom(const std::string cjCode) override;
	};

	class Function : public Node
	{
	private:
		std::string _name;
		std::string _returnType;
		std::string _args;

	public:
		Function(NodePtr top);
		virtual std::string ToLua() override;
		virtual void ConstructFrom(const std::string cjCode) override;
	};

	class NodeFactory : public Singleton<NodeFactory>
	{
	public:
		NodePtr Produce(Node::Type type, NodePtr top);
	};


}