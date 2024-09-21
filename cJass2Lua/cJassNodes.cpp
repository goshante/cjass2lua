#include "cJassNodes.h"
#include "reutils.h"
#include <iostream>

#define PRODUCING_NODE_ROOT(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<##__node__>(new __node__(outputType, outputPtr))

#define PRODUCING_NODE(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<__node__>(new __node__(top))

namespace cJass
{
	OutputInterface::OutputInterface() 
		: _type(Type::None)
		, _hFile(NULL)
		, _strPtr(nullptr)
	{
	}

	OutputInterface::OutputInterface(Type type, void* ptr)
		: _type(type)
		, _hFile(NULL)
		, _strPtr(nullptr)
	{
		switch (type)
		{
		case Type::String:
			_strPtr = static_cast<std::string*>(ptr);
			break;
		case Type::File:
			_hFile = static_cast<HANDLE>(ptr);
			break;
		}
	}

	OutputInterface::OutputInterface(const OutputInterface& copy)
		: _type(copy._type)
		, _hFile(copy._hFile)
		, _strPtr(copy._strPtr)
	{
	}

	OutputInterface& OutputInterface::operator=(const OutputInterface& copy)
	{
		_type = copy._type;
		_hFile = copy._hFile;
		_strPtr = copy._strPtr;
		return *this;
	}

	OutputInterface& OutputInterface::operator<<(const std::string& str)
	{
		DWORD dw = 0;
		switch (_type)
		{
			case Type::None:
				return *this;

			case Type::Console:
				std::cout << str;
				break;

			case Type::String:
				(*_strPtr) += str;
				break;

			case Type::File:
				WriteFile(_hFile, &str[0], static_cast<DWORD>(str.length()), &dw, NULL);
				break;
		}

		return *this;
	}

	NodePtr Node::Produce(Node::Type type, Node* top, OutputInterface::Type outputType, void* outputPtr)
	{
		switch (type)
		{
			PRODUCING_NODE_ROOT(GlobalSpace);
			PRODUCING_NODE(Function);
			PRODUCING_NODE(Comment);
			PRODUCING_NODE(OperationUnit);
			PRODUCING_NODE(LocalDeclaration);
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
		LastSubnode()->_out = _out;
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

	GlobalSpace::GlobalSpace(OutputInterface::Type outputType, void* outputPtr)
		: Node(Node::Type::GlobalSpace, nullptr)
		, _globals({})
	{
		_out = OutputInterface(outputType, outputPtr);
	}

	void GlobalSpace::ToLua() 
	{
		for (auto& node : _subnodes)
			node->ToLua();
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

	void Comment::ToLua()
	{
		//TODO
	}

	void Comment::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size != 1)
			throw std::runtime_error("Comment::InitData - wrong set of input data.");

		_comment = strings[0];
	}

	Function::Function(Node* top)
		: Node(Node::Type::Function, top)
		, _name("")
		, _returnType("")
		, _args({})
	{
	}

	void Function::ToLua()
	{
		//TODO
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

	OperationUnit::OperationUnit(Node* top)
		: Node(Node::Type::Function, top)
		, _opText("")
		, _type(OpType::Unknown)
		, _inBrackets(false)
	{
	}

	void OperationUnit::ToLua()
	{
		//TODO
	}

	void OperationUnit::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		
		if (size == 0 || size > 2)
			throw std::runtime_error("Operationl::InitData - wrong set of input data.");

		auto& s = strings[0];

		if (s.length() < 1 || s.length() > 2)
			throw std::runtime_error("Operationl::InitData - wrong key length.");

		_inBrackets = (s.length() == 2 && s[1] == 'b' ? true : false);

		switch (s[0])
		{
		case 'c':
			_type = OpType::Call;
			_opText = strings[1];
			break;

		case 'x':
			_type = OpType::Expression;
			break;

		case 'r':
			_type = OpType::Return;
			break;

		case 'i':
			_type = OpType::If;
			break;

		case 'E':
			_type = OpType::Elseif;
			break;

		case 'e':
			_type = OpType::Else;
			break;

		case 'w':
			_type = OpType::While;
			break;

		case 'l':
			_type = OpType::Lambda;
			break;

		case 'a':
			_type = OpType::AtomicOperand;
			_opText = strings[1];
			break;

		case 'o':
			_type = OpType::Operator;
			_opText = strings[1];
			break;

		case 'u':
			_type = OpType::UnaryOperator;
			_opText = strings[1];
			break;

		default:
			throw std::runtime_error("Operationl::InitData - unknown key.");
		}

	}

	OperationUnit::OpType OperationUnit::GetOpType() const
	{
		return _type;
	}

	LocalDeclaration::LocalDeclaration(Node* top)
		: Node(Node::Type::Function, top)
		, _var({"", "", ""})
		, _isArr(false)
		, _arrSize(0)
	{
	}

	void LocalDeclaration::ToLua()
	{
		//TODO
	}

	void LocalDeclaration::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();

		auto atou = [](const char *a) -> size_t
		{
			size_t u = 0;
			size_t t(1);

			for (size_t i = strlen(a) - 1; i >= 0; --i)
			{
				if (a[i] >= '0' && a[i] <= '9')
				{
					u += (a[i] - '0') * t;
					t *= 10;
				}
			}

			return u;
		};

		if (size < 2 || size > 3)
			throw std::runtime_error("Operationl::LocalDeclaration - wrong set of input data.");

		_var.type = strings[0];
		_var.name = strings[1];

		auto m = reu::Search(_var.name, "^(.+)[\\s\\t]*\\[(\\d*)\\]");
		if (m.IsMatching())
		{
			_var.name = m[1];
			_isArr = true;
			_arrSize = 0;
			if (m[2].length() > 0)
				_arrSize = atou(m[2].c_str());
		}

		if (size == 3)
			_var.initValue = strings[2];
	}
} //namespace cJass