#pragma once

#include "defs.h"
#include <memory>

namespace cJass
{
	struct variable_t
	{
		std::string type;
		std::string name;
		std::string initValue;
	};

	class Node
	{
	public:
		enum class Type
		{
			Undefined,
			GlobalSpace,
			Function,
			Comment,
			OperationUnit,
			LocalDeclaration
		};

	protected:
		bool _isNewLine;
		size_t _tabs;

	private:
		Type		_type;
		NodeList	_subnodes;
		Node*		_top;
		NL_iter		_it;

		Node(Node&) = delete;

	public:
		static NodePtr Produce(Node::Type type, Node* top);

		Node(Type type = Type::Undefined, Node* top = nullptr);
		virtual ~Node() {}

		void AddSubnode(NodePtr node);
		Type GetType() const;
		Node* Top();
		Node* LastSubnode();

		NodePtr IterateSubnodes();
		void ResetIterator();
		template <class PtrType = Node> PtrType* Ptr() { return dynamic_cast<PtrType*>(this); }
		virtual std::string ToLua() = 0;
		virtual void InitData(const std::vector<std::string>& strings) = 0;
	};

	class GlobalSpace : public Node
	{
	private:
		std::vector<variable_t> _globals;

	public:
		GlobalSpace();
		virtual std::string ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class Comment : public Node
	{
	private:
		std::string _comment;

	public:
		Comment(Node* top);
		virtual std::string ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class Function : public Node
	{
	private:
		std::string					_name;
		std::string					_returnType;
		std::vector<std::string>	_args;

	public:
		Function(Node* top);
		virtual std::string ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class OperationUnit : public Node
	{
	public:
		enum class OpType
		{
			Unknown,
			Expression,
			Call,
			If,
			Return,
			Elseif,
			Else,
			While,
			Lambda,
			AtomicOperand,
			Operator,
			UnaryOperator
		};

	private:
		std::string		_opText;
		OpType			_type;
		bool			_inBrackets;

	public:
		OperationUnit(Node* top);
		virtual std::string ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		OpType GetOpType() const;
	};

	class LocalDeclaration : public Node
	{
	private:
		variable_t	_var;
		bool		_isArr;
		size_t		_arrSize;

	public:
		LocalDeclaration(Node* top);
		virtual std::string ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};
}