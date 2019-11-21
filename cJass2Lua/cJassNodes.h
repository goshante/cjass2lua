#pragma once

#include "defs.h"
#include <memory>
#include <Windows.h>
#include <functional>

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
			RootNode,
			Function,
			Comment,
			OperationObject,
			LocalDeclaration
		};

		friend class Comment;
		friend class Function;
		friend class OperationObject;
		friend class LocalDeclaration;
		friend class RootNode;

	protected:
		bool				_isNewLine;
		size_t				_tabs;
		OutputInterface&	_out;
		NodeList			_subnodes;
		int					_depthIndex;
		bool				_isBlock;
		bool				_isComplete;
		size_t				_topIndex;
		Node*				_top;
		bool				_isString;
		bool				_isWrapper;
		RootNode*			_root;

	private:
		Type				_type;
		NL_iter				_it;

	protected:
		virtual void CountAllNodes() = 0;

		Node(Node&) = delete;

	public:
		static NodePtr Produce(Node::Type type, Node* top, OutputInterface& outIf);

		static std::string ToString(Node* node);

		Node(OutputInterface& outIf, Type type = Type::Undefined, Node* top = nullptr);
		virtual ~Node();

		void AddSubnode(NodePtr node);
		Type GetType() const;
		size_t CountSubnodes() const;
		Node* Top() const;
		Node* LastSubnode();
		void PrintTabs(int substract = 0);
		size_t GetDepth() const;
		bool IsBlock() const;
		bool IsComplete() const;
		void Complete(bool isComplete);
		bool IsString() const;
		bool IsWrapper() const;

		NodePtr IterateSubnodes();
		NodePtr AtNode(size_t i);
		void ResetIterator();
		template <class PtrType = Node> PtrType* Ptr() { return dynamic_cast<PtrType*>(this); }

		virtual void ToLua() = 0;
		virtual void InitData(const std::vector<std::string>& strings) = 0;
	};

	class RootNode : public Node
	{
	public:
		friend class Function;
		friend class OperationObject;
		friend class LocalDeclaration;

	private:
		std::vector<variable_t>								_globals;
		std::vector<std::string>							_strIds;
		int													_totalNodes;
		int													_printedNodes;
		std::shared_ptr<std::function<void(int,int)>>		_notifyCallback;
	
	protected:
		void IncrementNodeCount(size_t i);
		virtual void CountAllNodes() override;
		 
	public:
		RootNode(OutputInterface& outIf);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		void InsertStringId(const std::string& strId);
		bool HasStringId(const std::string& str) const;
		void TryToCreatePrintNotify();
		void AddNotifyCallback(NotifyCallback callback);
		void Clear();
	};

	class Comment : public Node
	{
	private:
		std::string _comment;

	protected:
		virtual void CountAllNodes() override;

	public:
		Comment(OutputInterface& outIf, Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
	};

	class Function : public Node
	{
	private:
		std::string					_name;
		std::string					_returnType;
		std::vector<std::string>	_args;
		std::vector<std::string>	_strIds;

	protected:
		virtual void CountAllNodes() override;

	public:
		Function(OutputInterface& outIf, Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		void InsertStringId(const std::string& strId);
		bool HasStringId(const std::string& strId) const;
	};

	class OperationObject : public Node
	{
	public:
		enum class OpType
		{
			Unknown,
			
			//Fundamental
			Id,
			Constant,
			Operator,
			UnaryOperator,
			NewLine,

			//Expressions
			ExitWhen,
			DoStatement,
			Return,
			Expression,			//General expression in () brackets or without them
			Call,				//Function call
			Index,				//Array index expression [] (not definition)
			
			//Blocks
			If,
			Elseif,
			Else,
			While,
			WhileNot,
			Do,
			Loop,
			Lambda,

			//Special wrappers
			Wrapper,			//General wrapper for operations like call, assignment and return
			Logic,				//Wrapper for logical blocks
			VarInitExpression,  //Wrapper for local variable initializer
			Argument			//Wrapper for an argument of function call
		};

		enum class ConstType
		{
			Undefined,
			Integer,
			RawCode,
			Float,
			Bool,
			String,
			null
		};

	private:
		std::string		_opText;
		std::string		_extra;
		OpType			_otype;
		bool			_inBrackets;
		bool			_unaryExpr;
		bool			_lambdaIsSingle;
		bool			_blockClosed;
		ConstType		_cType;
		bool			_AssignUnary;

	protected:
		virtual void CountAllNodes() override;

	public:
		OperationObject(OutputInterface& outIf, Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		OpType GetOpType() const;
		bool isEmpty() const;
		bool LambdaIsSingle() const;
		bool BlockClosed() const;
		void CloseBlock();
		bool CheckIsString();
		bool HasNeighbourStrings();
	};

	class LocalDeclaration : public Node
	{
	private:
		std::string					 _type;
		std::vector<std::string>	 _vars;
		std::vector<std::string>	_arrSizes;

	protected:
		virtual void CountAllNodes() override;

	public:
		LocalDeclaration(OutputInterface& outIf, Node* top);
		virtual void ToLua() override;
		virtual void InitData(const std::vector<std::string>& strings) override;
		size_t GetVarCount() const;
		Node* AddVariable(const std::string& name, const std::string& arrSize = 0);
		bool hasType() const;
	};
}