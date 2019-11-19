#include "cJassNodes.h"
#include "reutils.h"
#include "Settings.h"
#include "Utils.h"
#include <cmath>

#define d if (1 == 2) \

#define PRODUCING_NODE(__node__) case Node::Type::##__node__: \
			return std::shared_ptr<__node__>(new __node__(outIf, top))

namespace cJass
{
	static OperationObject::ConstType g_prevConst = OperationObject::ConstType::Undefined;

	NodePtr Node::Produce(Node::Type type, Node* top, OutputInterface& outIf)
	{
		switch (type)
		{
			PRODUCING_NODE(Function);
			PRODUCING_NODE(Comment);
			PRODUCING_NODE(OperationObject);
			PRODUCING_NODE(LocalDeclaration);
		default:
			throw std::runtime_error("Error! Node GlobalSpace cannot be produced!");
		}
	}

	std::string Node::ToString(Node* node)
	{
		std::string res;
		switch (node->_type)
		{
		case Type::Comment:
			return "Comment";
		
		case Type::Function:
			return "Function";

		case Type::GlobalSpace:
			return "GlobalSpace";

		case Type::LocalDeclaration:
			return "LocalDeclaration";

		case Type::OperationObject:
			res = "OperationObject(";
			switch (node->Ptr<OperationObject>()->GetOpType())
			{
			case OperationObject::OpType::Argument:
				res += "Argument";
				break;

			case OperationObject::OpType::Call:
				res += "Call";
				break;

			case OperationObject::OpType::Constant:
				res += "Constant";
				break;

			case OperationObject::OpType::Else:
				res += "Else";
				break;

			case OperationObject::OpType::Elseif:
				res += "Elseif";
				break;

			case OperationObject::OpType::Expression:
				res += "Expression";
				break;

			case OperationObject::OpType::Id:
				res += "Id";
				break;

			case OperationObject::OpType::If:
				res += "If";
				break;

			case OperationObject::OpType::Index:
				res += "Index";
				break;

			case OperationObject::OpType::Lambda:
				res += "Lambda";
				break;

			case OperationObject::OpType::Operator:
				res += "Operator";
				break;

			case OperationObject::OpType::Return:
				res += "Return";
				break;

			case OperationObject::OpType::UnaryOperator:
				res += "UnaryOperator";
				break;

			case OperationObject::OpType::Unknown:
				res += "Unknown";
				break;

			case OperationObject::OpType::VarInitExpression:
				res += "VarInitExpression";
				break;

			case OperationObject::OpType::While:
				res += "While";
				break;

			case OperationObject::OpType::Wrapper:
				res += "Wrapper";
				break;

			case OperationObject::OpType::Logic:
				res += "Logic";
				break;

			case OperationObject::OpType::WhileNot:
				res += "WhileNot";
				break;

			case OperationObject::OpType::Do:
				res += "Do";
				break;

			case OperationObject::OpType::DoStatement:
				res += "DoStatement";
				break;

			case OperationObject::OpType::ExitWhen:
				res += "ExitWhen";
				break;

			default:
				res += "Unknown";
			}
			res += ")";
			return res;

		default:
			return "Undefined";
		}
	}

	Node::Node(OutputInterface& outIf, Type type, Node* top)
		: _type(type)
		, _out(outIf)
		, _top(top)
		, _isNewLine(false)
		, _tabs(0)
		, _depthIndex(0)
		, _isBlock(false)
		, _isComplete(true)
		, _topIndex(0)
		, _isString(false)
	{
		if (top != nullptr)
		{
			_depthIndex = top->_depthIndex;
			if (top->_subnodes.size() > 0)
				_topIndex = top->_subnodes.size() - 1;
		}
	}

	Node::~Node()
	{
	}

	void Node::AddSubnode(NodePtr node)
	{
		if (node->_depthIndex == 0)
			node->_depthIndex = _depthIndex + 1;

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

	Node* Node::Top() const
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
		if (i >= _subnodes.size())
			throw std::runtime_error("Node::AtNode: Out of subnode's list boundaries.");
		
		_subnodes[i]->_topIndex = i;
		return _subnodes[i];
	}

	void Node::PrintTabs(int substract)
	{
		for (int i = 0; i < _depthIndex - substract; i++)
			_out << "\t";
	}

	size_t Node::CountSubnodes() const
	{
		return _subnodes.size();
	}

	size_t Node::GetDepth() const
	{
		return _depthIndex;
	}

	bool Node::IsBlock() const
	{
		return _isBlock;
	}

	bool Node::IsComplete() const
	{
		return _isComplete;
	}

	void Node::Complete(bool isComplete)
	{
		_isComplete = isComplete;
	}

	bool Node::IsString() const
	{
		return _isString;
	}

	GlobalSpace::GlobalSpace(OutputInterface& outIf)
		: Node(outIf, Node::Type::GlobalSpace, nullptr)
		, _globals({})
	{
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
		{
			appLog(Critical) << "GlobalSpace::InitData: Wrong set of input data. Got" << size << ", expected" << "3";
			appLog(Debug) << "Args:" << strings;
			return;
		}

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

	void GlobalSpace::Clear()
	{
		_tabs = 0;
		_subnodes.clear();
		_globals.clear();
		_strIds.clear();
		ResetIterator();
	}

	void GlobalSpace::InsertStringId(const std::string& str)
	{
		_strIds.push_back(str);
	}

	bool GlobalSpace::HasStringId(const std::string& strId) const
	{
		for (const auto& str : _strIds)
		{
			if (str == strId)
				return true;
		}
		return false;
	}

	Comment::Comment(OutputInterface& outIf, Node* top)
		: Node(outIf, Node::Type::Comment, top)
		, _comment("")
	{
	}

	void Comment::ToLua()
	{
		size_t end = _comment.length() - 1;
		bool multiline = false;
		size_t n, r;
		std::string tmp = _comment;
		if (_comment.length() >= 2)
		{
			tmp = reu::IndexSubstr(_comment, 1, end);
			r = tmp.find("\r");
			n = tmp.find("\n");
			multiline = ((r != std::string::npos) || (n != std::string::npos));
		}
		n = _comment.find("\n");
		if (n == 0 && end == 0)
			return;


		if (n == 0)
		{
			_out << OutputInterface::nl;
			if (Top()->GetType() != Node::Type::GlobalSpace)
				PrintTabs();
		}
		else
		{
			tmp = _comment;
			_out << "\t";
		}
		
		if (multiline)
			_out << "--[[" << OutputInterface::nl << tmp << OutputInterface::nl << "]]--" << OutputInterface::nl;
		else
			_out << "-- " << tmp;

		if (Top()->Top() == nullptr)
		{
			_out << OutputInterface::nl;
		}
	}

	void Comment::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		if (size != 1)
		{
			appLog(Critical) << "Comment::InitData: Wrong set of input data. Got" << size << ", expected" << "1";
			appLog(Debug) << "Args:" << strings;
			return;
		}

		_comment = strings[0];
		size_t last = _comment.size() - 1;
		size_t cutLen = 0;
		if (_comment.length() > 1)
		{
			if (_comment[last] == '\r' || _comment[last] == '\n')
				cutLen++;
			if (last > 0 && _comment[last - 1] == '\r' || _comment[last - 1] == '\n')
				cutLen++;
		}
		if (cutLen > 0)
			_comment = reu::IndexSubstr(_comment, 0, last - cutLen);
	}

	Function::Function(OutputInterface& outIf, Node* top)
		: Node(outIf, Node::Type::Function, top)
		, _name("")
		, _returnType("")
		, _args({})
	{
		_isBlock = true;
	}

	void Function::ToLua()
	{
		_out << OutputInterface::nl << "function " << _name << "(";
		
		if (_args.size() == 0)
			_out << ")";
		else if (_args.size() == 2)
			_out << _args[1] << ")";
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
						_out << ")";
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
		{
			appLog(Critical) << "Function::InitData: Wrong set of input data. Got" << size << ", expected" << "2";
			appLog(Debug) << "Args:" << strings;
			return;
		}

		_returnType = strings[0];
		_name = strings[1];
		_depthIndex++;
		bool insStr = false;

		for (size_t i = 2; i < size; i++)
		{
			_args.push_back(strings[i]);
			if (i % 2 == 0 && strings[i] == "string")
				insStr = true;
			if (i % 2 != 0 && insStr)
			{
				_strIds.push_back(strings[i]);
				insStr = false;
			}
		}
			
	}

	void Function::InsertStringId(const std::string& strId)
	{
		_strIds.push_back(strId);
	}

	bool Function::HasStringId(const std::string& strId) const
	{
		for (const auto& str : _strIds)
		{
			if (str == strId)
				return true;
		}
		return false;
	}

	bool OperationObject::CheckIsString()
	{
		auto top = _top;
		while (top->Top() != nullptr)
			top = top->Top();
		if (top->Ptr<GlobalSpace>()->HasStringId(_opText))
			return true;
		else
		{
			top = Top();
			while (top->Top() != nullptr && top->Top()->GetType() == Node::Type::Function)
				top = top->Top();
			if (top->GetType() == Node::Type::Function)
			{
				if (top->Ptr<Function>()->HasStringId(_opText))
					return true;
			}
		}

		return false;
	}

	OperationObject::OperationObject(OutputInterface& outIf, Node* top)
		: Node(outIf, Node::Type::OperationObject, top)
		, _opText("")
		, _extra("")
		, _otype(OpType::Unknown)
		, _inBrackets(false)
		, _unaryExpr(false)
		, _lambdaIsSingle(false)
		, _blockClosed(false)
		, _cType(ConstType::Undefined)
		, _AssignUnary(false)
	{
	}

	void OperationObject::ToLua()
	{
		size_t nodeCount, lastIndex;
		bool begin = false;
		bool allowNewLines = false;
		size_t stopIndex = 0;
		std::string stmp;
		int itmp;

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
			_out << Utils::const2lua(_opText);
			for (auto& node : _subnodes)
				node->ToLua();
			break;

		case OpType::Constant:
			_out << Utils::const2lua(_opText);
			g_prevConst = _cType;
			break;

		case OpType::Operator:
			_out << Utils::op2lua(_opText, HasNeighbourStrings());
			break;

		case OpType::UnaryOperator:
			if (!_unaryExpr)
			{
				if (_extra == "++")
				{
					if (_AssignUnary)
						_out << _opText << " = " << _opText << " + 1";
					else
					{
						_out << "(" << _opText << " + 1" << ")";
					}
				}
				else if (_extra == "--")
				{
					if (_AssignUnary)
						_out << _opText << " = " << _opText << " - 1";
					else
						_out << "(" << _opText << " - 1" << ")";
				}
					
			}
			else
			{
				if (!_isString)
					_out << _opText << " = " << _opText << std::string({ ' ', _extra[0], ' ' });
				else
					_out << _opText << " = " << _opText << " .. ";
				for (auto& node : _subnodes)
					node->ToLua();
			}
			break;

		case OpType::Argument:
			for (auto& node : _subnodes)
				node->ToLua();
			break;

		case OpType::VarInitExpression:
		case OpType::Expression:
			if (_inBrackets)
				_out << "(";

			for (auto& node : _subnodes)
				node->ToLua();

			if (_inBrackets)
				_out << ")";
			break;

		case OpType::Call:
			_out << _opText << "(";
			nodeCount = CountSubnodes();
			if (nodeCount > 0)
			{
				lastIndex = nodeCount - 1;
				for (size_t i = 0; i <= lastIndex; i++)
				{
					auto node = AtNode(i);
					node->ToLua();
					if (i != lastIndex)
						_out << ", ";
				}
			}
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

		case OpType::DoStatement:
			_out << "until ";
			if (_extra == "not")
				_out << "not( ";
			for (auto& node : _subnodes)
				node->ToLua();
			if (_extra == "not")
				_out << " )";
			break;

		case OpType::ExitWhen:
			_out << OutputInterface::nl;
			PrintTabs();
			_out << "if ";
			for (auto& node : _subnodes)
				node->ToLua();
			_out << " then break end";
			break;

		case OpType::Index:
			_out << "[";
			if (_subnodes.size() == 1 
				&& _subnodes[0]->GetType() == Node::Type::OperationObject
				&& _subnodes[0]->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Constant
				&& reu::IsMatching(_subnodes[0]->Ptr<OperationObject>()->_opText, "^[0-9]+$")
				&& !Settings::DoNotIncrementArrayIndexConstants)
			{
				itmp = atoi(_subnodes[0]->Ptr<OperationObject>()->_opText.c_str()) + 1;
				_subnodes[0]->Ptr<OperationObject>()->_opText = std::to_string(itmp);
				_subnodes[0]->ToLua();
			}
			else
			{
				for (auto& node : _subnodes)
					node->ToLua();
			}
			_out << "]";
			break;

		case OpType::Logic:
			for (size_t i = 0; i < CountSubnodes(); i++)
			{
				auto node = AtNode(i);
				auto prevNode = (i == 0) ? node : AtNode(i - 1);
				bool isNewLine = (node->GetType() == Node::Type::OperationObject
					&& node->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::NewLine);
				bool isNewLinePrev = (prevNode->GetType() == Node::Type::OperationObject
					&& prevNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::NewLine);

				if ((isNewLine && isNewLinePrev) || (isNewLine && i == CountSubnodes() - 1))
				{
					_out << OutputInterface::nl;
					PrintTabs();
					_out << "end";
					allowNewLines = true;
					stopIndex = i;
					break;
				}

				if (!isNewLine)
					node->ToLua();
			}

			if (allowNewLines)
			{
				for (size_t i = stopIndex; i < CountSubnodes(); i++)
				{
					auto node = AtNode(i);
					node->ToLua();
				}
			}
			else
			{
				_out << OutputInterface::nl;
				PrintTabs();
				_out << "end";
			}
				
			break;

		case OpType::If:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "if ";
			if (CountSubnodes() == 0)
				appLog(Warning) << "'if' without statement!";
			for (size_t i = 0; i < CountSubnodes(); i++)
			{
				auto node = AtNode(i);
				node->ToLua();
				if (i == 0)
					_out << " then";
			}
			break;

		case OpType::Elseif:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "elseif ";
			if (CountSubnodes() == 0)
				appLog(Warning) << "'elseif' without statement!";
			for (size_t i = 0; i < CountSubnodes(); i++)
			{
				auto node = AtNode(i);
				node->ToLua();
				if (i == 0)
					_out << " then";
			}
			break;

		case OpType::Else:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "else";
			for (auto& node : _subnodes)
				node->ToLua();
			break;

		case OpType::Loop:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "while true do";
			for (auto& node : _subnodes)
				node->ToLua();
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "end";
			break;

		case OpType::Do:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "repeat";
			for (auto& node : _subnodes)
				node->ToLua();
			_out << OutputInterface::nl;
			PrintTabs(1);
			break;

		case OpType::While:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "while ";
			if (CountSubnodes() == 0)
				appLog(Warning) << "'while' without statement!";
			for (size_t i = 0; i < CountSubnodes(); i++)
			{
				auto node = AtNode(i);
				node->ToLua();
				if (i == 0)
					_out << " do";
			}
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "end";
			break;

		case OpType::WhileNot:
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "while not ( ";
			if (CountSubnodes() == 0)
				appLog(Warning) << "'whilenot' without statement!";
			for (size_t i = 0; i < CountSubnodes(); i++)
			{
				auto node = AtNode(i);
				node->ToLua();
				if (i == 0)
					_out << " ) do";
			}
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "end";
			break;

		case OpType::Lambda:
			_out << "function ()";
			
			for (auto& node : _subnodes)
				node->ToLua();
				
			_out << OutputInterface::nl;
			PrintTabs(1);
			_out << "end";
			break;

		case OpType::NewLine:
			_out << OutputInterface::nl;
			PrintTabs();
			break;
		
		default:
			appLog(Warning) << "OperationObject::ToLua: Unsupported OpType.";
		}
	}

	void OperationObject::InitData(const std::vector<std::string>& strings)
	{
		size_t size = strings.size();
		Node* top = _top;
		
		if (size == 0 || size > 3)
		{
			appLog(Critical) << "OperationObject::InitData: Wrong set of input data. Got" << size << ", expected" << "3";
			appLog(Debug) << "Args:" << strings;
			return;
		}

		auto& s = strings[0];

		if (s.length() < 1 || s.length() > 2)
			throw std::runtime_error("OperationObject::InitData: wrong key length.");

		switch (s[0])
		{
		case 'c':
			_otype = OpType::Call;
			_opText = strings[1];
			break;

		case 'C':
			_otype = OpType::Constant;
			_opText = strings[1];
			_cType = Utils::determConstType(_opText);
			if (_cType == ConstType::String)
				_isString = true;
			
			break;

		case 'I':
			_otype = OpType::Id;
			_opText = strings[1];
			_isString = CheckIsString();
			break;

		case 'x':
			_otype = OpType::Expression;
			if (s[1] == 'b')
				_inBrackets = true;
			if (Top()->GetType() == Node::Type::OperationObject &&
				(Top()->Ptr<OperationObject>()->GetOpType() == OpType::If
					|| Top()->Ptr<OperationObject>()->GetOpType() == OpType::Elseif
					|| Top()->Ptr<OperationObject>()->GetOpType() == OpType::While)
				&& Top()->CountSubnodes() <= 1)
			{
				_inBrackets = false;
			}
			break;

		case 'v':
			_otype = OpType::VarInitExpression;
			break;

		case 'a':
			_otype = OpType::Argument;
			break;

		case 'S':
			_otype = OpType::DoStatement;
			if (s[1] == 'n')
				_extra = "not";
			break;

		case 'r':
			_otype = OpType::Return;
			break;

		case 'T':
			_otype = OpType::ExitWhen;
			break;

		case 'L':
			_otype = OpType::Logic;
			break;

		case 'i':
			_otype = OpType::If;
			_depthIndex++;
			_isBlock = true;
			break;

		case 'E':
			_otype = OpType::Elseif;
			_depthIndex++;
			_isBlock = true;
			break;

		case 'e':
			_otype = OpType::Else;
			_depthIndex++;
			_isBlock = true;
			break;

		case 'P':
			_otype = OpType::Loop;
			_depthIndex++;
			_isBlock = true;
			break;

		case 'D':
			_otype = OpType::Do;
			_depthIndex++;
			_isBlock = true;
			break;

		case 'w':
			if (s[1] == 'n')
				_otype = OpType::WhileNot;
			else
				_otype = OpType::While;
			
			_depthIndex++;
			_isBlock = true;
			break;

		case 'l':
			if (s[1] == 's')
				_lambdaIsSingle = true;
			_otype = OpType::Lambda;
			_opText = strings[1];
			_depthIndex++;
			_isBlock = true;
			break;

		case 'o':
			_otype = OpType::Operator;
			_opText = strings[1];
			break;

		case 'u':
			_otype = OpType::UnaryOperator;
			_opText = strings[1];
			_extra = strings[2];
			if (s[1] == 'e')
				_unaryExpr = true;
			else if (s[1] == 'a')
				_AssignUnary = true;
			_isString = CheckIsString();
			break;

		case 'W':
			_otype = OpType::Wrapper; 
			break;

		case 'n':
			_otype = OpType::Index;
			break;

		case 'N':
			_otype = OpType::NewLine;
			break;

		default:
			appLog(Warning) << "OperationObject::InitData: unknown key.";
		}

	}

	bool  OperationObject::HasNeighbourStrings()
	{
		Node* top = _top;
		size_t this_index = 9999999;

		if (top->IsString())
			return true;

		std::vector<std::string> neighbours;
		for (size_t i = 0; i < top->CountSubnodes(); i++)
		{
			auto node = top->AtNode(i);

			if (i > this_index + 1)
				break;

			if (node->Ptr() == dynamic_cast<Node*>(this) && this_index == 9999999)
			{
				this_index = i;
				if (i > 0)
					i--;
			}

			if (i == this_index - 1)
			{
				node = top->AtNode(i);
				if (node->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Index)
				{
					for (int j = int(i); j >= 0; j--)
					{
						node = top->AtNode(j);
						if (node->Ptr<OperationObject>()->GetOpType() != OperationObject::OpType::Index)
						{
							neighbours.push_back(node->Ptr<OperationObject>()->_opText);
							continue;
						}
					}
				}
				else
					neighbours.push_back(node->Ptr<OperationObject>()->_opText);
			}

			if (i == this_index + 1)
			{
				node = top->AtNode(i);
				neighbours.push_back(node->Ptr<OperationObject>()->_opText);
			}
		}

		Function* fnode = nullptr;
		GlobalSpace* gspace = nullptr;

		top = this;
		while (top != nullptr)
		{
			if (top->GetType() == Node::Type::Function)
			{
				fnode = top->Ptr<Function>();
				break;
			}
			top = top->Top();
		}

		top = _top;
		while (top != nullptr)
		{
			if (top->Top() == nullptr)
			{
				gspace = top->Ptr<GlobalSpace>();
				break;
			}
			top = top->Top();
		}

		if (fnode != nullptr)
		{
			for (auto n : neighbours)
			{
				if (n != "" && fnode->HasStringId(n))
					return true;
			}
		}

		if (gspace != nullptr)
		{
			for (auto n : neighbours)
			{
				if (n != "" && gspace->HasStringId(n))
					return true;
			}
		}
		else
			appLog(Critical) << "OperationObject::HasNeighbourStrings: Root node not found.";

		return false;
	}

	OperationObject::OpType OperationObject::GetOpType() const
	{
		return _otype;
	}

	bool OperationObject::isEmpty() const
	{
		return (_opText == "");
	}

	bool OperationObject::LambdaIsSingle() const
	{
		return _lambdaIsSingle;
	}

	bool OperationObject::BlockClosed() const
	{
		return _blockClosed;
	}

	void OperationObject::CloseBlock()
	{
		_blockClosed = true;
	}

	LocalDeclaration::LocalDeclaration(OutputInterface& outIf, Node* top)
		: Node(outIf, Node::Type::LocalDeclaration, top)
		, _type("")
		, _vars({})
		, _arrSizes({})
	{
	}

	void LocalDeclaration::ToLua()
	{
		_out << OutputInterface::nl;
		PrintTabs();
		_out << "local ";

		//Group vars with init to begin
		std::vector<std::string>		   vv;
		std::vector<std::string>		   av;
		std::vector<std::shared_ptr<Node>> nv;
		for (size_t i = 0; i < _vars.size(); i++)
		{
			if (AtNode(i)->CountSubnodes() != 0 || _arrSizes[i] != "")
			{

				auto v = _vars[i];
				auto a = _arrSizes[i];
				auto n = AtNode(i);

				vv.push_back(v);
				av.push_back(a);
				nv.push_back(n);
			}
		}

		for (size_t i = 0; i < _vars.size(); i++)
		{
			if (AtNode(i)->CountSubnodes() == 0 && _arrSizes[i] == "")
			{

				auto v = _vars[i];
				auto a = _arrSizes[i];
				auto n = AtNode(i);

				vv.push_back(v);
				av.push_back(a);
				nv.push_back(n);
			}
		}

		_vars = vv;
		_arrSizes = av;
		_subnodes = nv;

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
						continue;
					else if (j != 0)
						_out << ", ";

					if (_arrSizes[j] != "")
						_out << "{}";
					else if (expr->CountSubnodes() > 0)
						expr->ToLua();
				}
			}
		}
	}

	void LocalDeclaration::InitData(const std::vector<std::string>& strings)
	{
		appLog(Debug) << "LocalDeclaration::InitData Args:" << strings;
		_type == strings[0];
		if (_type == "string")
			_isString = true;
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
		appLog(Debug) << "Adding local variable" << name;
		_vars.push_back(name);
		_arrSizes.push_back(arrSize);
		auto node = Node::Produce(Node::Type::OperationObject, this, _out);
		node->InitData({ "v" });
		AddSubnode(node);
		return node->Ptr();
	}
} //namespace cJass