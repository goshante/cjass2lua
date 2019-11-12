#pragma once

#include "defs.h"
#include <memory>
#include <Windows.h>

#include "OutputInterface.h"

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
			OperationObject,
			LocalDeclaration
		};

	protected:
		bool				_isNewLine;
		size_t				_tabs;
		OutputInterface		_out;
		NodeList			_subnodes;
		size_t				_depthIndex;

	private:
		Type				_type;
		Node*				_top;
		NL_iter				_it;


		Node(Node&) = delete;

	public:
		static NodePtr Produce(Node::Type type, Node* top
			, OutputInterface::Type outputType = OutputInterface::Type::None
			, OutputInterface::NewLineType nlType = OutputInterface::NewLineType::LF
			, void* outputPtr = nullptr);

		Node(Type type = Type::Undefined, Node* top = nullptr);
		virtual ~Node();

		void AddSubnode(NodePtr node);
		Type GetType() const;
		size_t CountSubnodes() const;
		Node* Top();
		Node* LastSubnode();
		void PrintTabs();
		size_t GetDepth() const;

		NodePtr IterateSubnodes();
		NodePtr AtNode(size_t i);
		void ResetIterator();
		template <class PtrType = Node> PtrType* Ptr() { return dynamic_cast<PtrType*>(this); }
		virtual void ToLua() = 0;
		virtual void InitData(const std::vector<std::string>& strings) = 0;
	};

	class GlobalSpace : public Node
	{
	private:
		std::vector<variable_t> _globals;

	public:
		GlobalSpace(OutputInterface::Type outputType, OutputInterface::NewLineType nlType, void* outputPtr);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class Comment : public Node
	{
	private:
		std::string _comment;

	public:
		Comment(Node* top);
		virtual void ToLua() override;
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
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class OperationObject : public Node
	{
	public:
		enum class OpType
		{
			Unknown,
			Constant,
			Argument,
			Expression,
			Call,
			If,
			Return,
			Elseif,
			Else,
			While,
			Lambda,
			Operator,
			UnaryOperator,
			Id,
			Wrapper,
			Index
		};

	private:
		std::string		_opText;
		std::string		_extra;
		OpType			_otype;
		bool			_inBrackets;
		bool			_unaryExpr;

	public:
		OperationObject(Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		OpType GetOpType() const;
		bool isEmpty() const;
	};

	class LocalDeclaration : public Node
	{
	private:
		std::string					 _type;
		std::vector<std::string>	 _vars;
		std::vector<std::string>	_arrSizes;

	public:
		LocalDeclaration(Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		size_t GetVarCount() const;
		Node* AddVariable(const std::string& name, const std::string& arrSize = 0);
		bool hasType() const;
	};
}