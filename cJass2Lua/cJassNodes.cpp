#include "cJassNodes.h"
#include "reutils.h"
#include <iostream>

#define PRODUCING_NODE_ROOT(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<##__node__>(new __node__(outputType, nlType, outputPtr))

#define PRODUCING_NODE(__node__) case Node::Type::##__node__: \
							return std::shared_ptr<__node__>(new __node__(top))

namespace cJass
{
	OutputInterface::OutputInterface() 
		: _type(Type::None)
		, _hFile(NULL)
		, _strPtr(nullptr)
		, _nl("\r\n")
	{
	}

	OutputInterface::OutputInterface(Type type, NewLineType nlType, void* ptr)
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

		switch (nlType)
		{
		case NewLineType::CR:
			_nl = "\r";
			break;
		case NewLineType::LF:
			_nl = "\n";
			break;

		case NewLineType::CRLF:
			_nl = "\r\n";
			break;
		}
	}

	OutputInterface::OutputInterface(const OutputInterface& copy)
		: _type(copy._type)
		, _hFile(copy._hFile)
		, _strPtr(copy._strPtr)
		, _nl(copy._nl)
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

	OutputInterface& OutputInterface::operator<<(const NewLine&)
	{
		(*this) << _nl;
		return *this;
	}

	bool OutputInterface::IsReady() const
	{
		return (_type != Type::None);
	}

	NodePtr Node::Produce(Node::Type type, Node* top, OutputInterface::Type outputType, OutputInterface::NewLineType nlType, void* outputPtr)
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
		, _depthIndex(0)
	{
		if (top != nullptr)
		{
			_depthIndex = top->_depthIndex + 1;
			_out = top->_out;
		}
	}

	Node::~Node()
	{
	}

	void Node::AddSubnode(NodePtr node)
	{
		if (node->_depthIndex == 0)
			node->_depthIndex = _depthIndex + 1;
		if (!node->_out.IsReady())
			node->_out = _out;
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

	NodePtr Node::AtNode(size_t i)
	{
		auto it = _subnodes.begin();
		size_t n = 0;
		while (it != _subnodes.end())
		{
			if (n == i)
				return *it;
			it++;
			n++;
		}

		throw std::runtime_error("Node::AtNode:Out of subnode's list boundaries.");
		return nullptr;
	}

	void Node::PrintTabs()
	{
		for (size_t i = 0; i < _depthIndex-1; i++)
			_out << "\t";
	}

	size_t Node::CountSubnodes() const
	{
		size_t count = 0;

		for (const auto& node : _subnodes)
			count++;

		return count;
	}

	size_t Node::GetDepth() const
	{
		return _depthIndex;
	}

	GlobalSpace::GlobalSpace(OutputInterface::Type outputType, OutputInterface::NewLineType nlType, void* outputPtr)
		: Node(Node::Type::GlobalSpace, nullptr)
		, _globals({})
	{
		_out = OutputInterface(outputType, nlType, outputPtr);
	}

	void GlobalSpace::ToLua() 
	{
		for (auto& var : _globals)
		{
			if (var.initValue == "" || var.initValue == "null")
				_out << "-- " << var.type << " " << var.name << OutputInterface::nl;
			else
				_out << var.name << " = " << var.initValue << OutputInterface::nl;
		}

		_out << OutputInterface::nl;

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
		size_t end = _comment.length() - 1;
		bool multiline = (_comment.find("\r") != std::string::npos || _comment.find("\n") != std::string::npos);

		PrintTabs();
		if (multiline)
			 _out << "--[[" << _comment << "]]--" << OutputInterface::nl;
		else
			_out << "--" << _comment << OutputInterface::nl;	
	}

	void Comment::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size != 1)
			throw std::runtime_error("Comment::InitData - wrong set of input data.");

		_comment = strings[0];
		size_t last = _comment.size() - 1;
		size_t cutLen = 0;
		if (_comment[last] == '\r' || _comment[last] == '\n')
			cutLen++;
		if (last > 0 && _comment[last-1] == '\r' || _comment[last-1] == '\n')
			cutLen++;
		if (cutLen > 0)
			_comment = reu::IndexSubstr(_comment, 0, last - cutLen);
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
		_out << OutputInterface::nl << "function " << _name << "(";
		
		if (_args.size() == 0)
			_out << ")" << OutputInterface::nl;
		else if (_args.size() == 2)
			_out << _args[1] << ")" << OutputInterface::nl;
		else
		{
			for (size_t i = 1; i < _args.size(); i++)
			{
				if (i % 2 != 0)
				{
					_out << _args[i];
					if (i != _args.size() - 1)
						_out << ", ";
					else
						_out << ")" << OutputInterface::nl;
				}
			}
		}

		for (auto& node : _subnodes)
			node->ToLua();

		_out << OutputInterface::nl << "end" << OutputInterface::nl;
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

	std::string op2lua(const std::string& op)
	{
		if (op == "&&")
			return " and ";
		else if (op == "||")
			return " or ";
		else if (op == "!")
			return " not ";
		else if (op == "++" || op == "--" || op == "+=" || op == "-=" || op == "*=" || op == "/=")
			return "";
		if (op == "dig_minus")
			return "-";

		return " " + op + " ";
	}

	std::string const2lua(const std::string& cnst)
	{
		if (cnst[0] == '\'')
			return "FourCC(" + cnst + ")";

		if (cnst == "null")
			return "nil";

		return cnst;
	}

	OperationUnit::OperationUnit(Node* top)
		: Node(Node::Type::OperationUnit, top)
		, _opText("")
		, _extra("")
		, _otype(OpType::Unknown)
		, _inBrackets(false)
	{
	}

	void OperationUnit::ToLua()
	{
		if (_otype != OpType::Unknown)
		{
			if (_isNewLine)
			{
				_out << OutputInterface::nl;
				PrintTabs();
			}
		}


		switch (_otype)
		{
		case OpType::Id:
			_out << const2lua(_opText);
			for (auto& node : _subnodes)
				node->ToLua();
			break;

		case OpType::Constant:
			_out << const2lua(_opText);
			break;

		case OpType::Operator:
			_out << op2lua(_opText);
			break;

		case OpType::Expression:
			if (_inBrackets)
				_out << "(";

			for (auto& node : _subnodes)
				node->ToLua();

			if (_inBrackets)
				_out << ")";
			break;

		case OpType::Wrapper:
			_out << OutputInterface::nl;
			PrintTabs();
			if (_inBrackets)
				_out << "(";

			for (auto& node : _subnodes)
				node->ToLua();

			if (_inBrackets)
				_out << ")";
			break;

		case OpType::Return:
			_out << OutputInterface::nl;
			PrintTabs();
			_out << "return ";
			for (auto& node : _subnodes)
				node->ToLua();
			break;

		case OpType::Index:
			_out << "[";
			for (auto& node : _subnodes)
				node->ToLua();
			_out << "]";
			break;
		}
	}

	void OperationUnit::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		
		if (size == 0 || size > 3)
			throw std::runtime_error("Operationl::InitData - wrong set of input data.");

		auto& s = strings[0];

		if (s.length() < 1 || s.length() > 2)
			throw std::runtime_error("Operationl::InitData - wrong key length.");

		switch (s[0])
		{
		case 'c':
			_otype = OpType::Call;
			_opText = strings[1];
			break;

		case 'C':
			_otype = OpType::Constant;
			_opText = strings[1];
			break;

		case 'I':
			_otype = OpType::Id;
			_opText = strings[1];
			break;

		case 'x':
			_otype = OpType::Expression;
			if (s[1] == 'b')
				_inBrackets = true;
			break;

		case 'r':
			_otype = OpType::Return;
			break;

		case 'i':
			_otype = OpType::If;
			break;

		case 'E':
			_otype = OpType::Elseif;
			break;

		case 'e':
			_otype = OpType::Else;
			break;

		case 'w':
			_otype = OpType::While;
			if (s[1] == 'b')
				_inBrackets = true;
			break;

		case 'l':
			_otype = OpType::Lambda;
			break;

		case 'a':
			_otype = OpType::AtomicOperand;
			_opText = strings[1];
			break;

		case 'o':
			_otype = OpType::Operator;
			_opText = strings[1];
			break;

		case 'u':
			_otype = OpType::UnaryOperator;
			_opText = strings[1];
			_extra = strings[2];
			break;

		case 'W':
			_otype = OpType::Wrapper; 
			break;

		case 'n':
			_otype = OpType::Index;
			break;

		default:
			throw std::runtime_error("Operationl::InitData - unknown key.");
		}

	}

	OperationUnit::OpType OperationUnit::GetOpType() const
	{
		return _otype;
	}

	bool OperationUnit::isEmpty() const
	{
		return (_opText == "");
	}

	LocalDeclaration::LocalDeclaration(Node* top)
		: Node(Node::Type::LocalDeclaration, top)
		, _type("")
		, _vars({})
		, _arrSizes({})
	{
	}

	void LocalDeclaration::ToLua()
	{
		PrintTabs();
		_out << "local ";

		//Group vars with init to begin
		size_t lastEmptyVar = ~0;
		auto it = _subnodes.begin();
		auto eit = _subnodes.end();
		for (size_t i = 0; i < _vars.size(); i++)
		{
			auto expr = AtNode(i);
			if (expr->CountSubnodes() > 0 || _arrSizes[i] != "")
			{
				if (i > lastEmptyVar)
				{
					std::swap(*it, *eit);
					std::swap(_vars[i], _vars[lastEmptyVar]);
					std::swap(_arrSizes[i], _arrSizes[lastEmptyVar]);

					lastEmptyVar = i;
					eit = it;
				}
			}
			else if (lastEmptyVar == ~0)
			{
				lastEmptyVar = i;
				eit = it;
			}

			it++;
		}

		for (size_t i = 0; i < _vars.size(); i++)
		{
			_out << _vars[i];
			auto expr = AtNode(0);

			if (i != _vars.size() - 1)
				_out << ", ";
			else if (_arrSizes[0] != "" || expr->CountSubnodes() > 0)
			{
				_out << " = ";

				for (size_t j = 0; j < _vars.size(); j++)
				{
					expr = AtNode(j);
					if (_arrSizes[j] == "" && expr->CountSubnodes() == 0)
						break;
					else if (j != 0)
						_out << ", ";

					if (_arrSizes[j] != "")
						_out << "{}";
					else if (expr->CountSubnodes() > 0)
						expr->ToLua();
				}
				_out << OutputInterface::nl;
			}
			else
				_out << OutputInterface::nl;
		}
	}

	void LocalDeclaration::InitData(const std::vector<std::string>& strings)
	{
		_type == strings[0];
	}

	size_t LocalDeclaration::GetVarCount() const
	{
		return _vars.size();
	}

	bool LocalDeclaration::hasType() const
	{
		return (_type != "");
	}

	Node* LocalDeclaration::AddVariable(const std::string& name, const std::string& arrSize)
	{
		_vars.push_back(name);
		_arrSizes.push_back(arrSize);
		auto node = Node::Produce(Node::Type::OperationUnit, this);
		node->InitData({ "x" });
		AddSubnode(node);
		return node->Ptr();
	}
} //namespace cJass