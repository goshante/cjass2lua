#include "cJassParser2.h"
#include <sstream>

#include "reutils.h"
#include "Utils.h"

#include "Settings.h"

#define DOC_LINEPOS "(" << _line << ":" << _wordPos << ")"

void stack_incr_top(std::stack<int>& st)
{
	int i = st.top();
	i++;
	st.pop();
	st.push(i);
}

void bstack_set_top(std::stack<bool>& st, bool b)
{
	st.pop();
	st.push(b);
}

template <class Stack>
void stack_clear(Stack stack)
{
	while (!stack.empty())
		stack.pop();
}


namespace cJass
{
	word_t classifyWord(const std::string& word)
	{
		if (word == "s1")
			word.c_str();

		if (word == "" || word == " " || word == "	")
			return word_t::undefined;

		if (word == ";")
			return word_t::end;

		if (word == "int"
			|| word == "ability"
			|| word == "agent"
			|| word == "aidifficulty"
			|| word == "alliancetype"
			|| word == "array"
			|| word == "attacktype"
			|| word == "blendmode"
			|| word == "boolean"
			|| word == "boolexpr"
			|| word == "buff"
			|| word == "button"
			|| word == "camerafield"
			|| word == "camerasetup"
			|| word == "code"
			|| word == "conditionfunc"
			|| word == "damagetype"
			|| word == "defeatcondition"
			|| word == "destructable"
			|| word == "dialog"
			|| word == "dialogevent"
			|| word == "effect"
			|| word == "effecttype"
			|| word == "event"
			|| word == "eventid"
			|| word == "fgamestate"
			|| word == "filterfunc"
			|| word == "fogmodifier"
			|| word == "fogstate"
			|| word == "force"
			|| word == "gamecache"
			|| word == "gamedifficulty"
			|| word == "gameevent"
			|| word == "gamespeed"
			|| word == "gamestate"
			|| word == "gametype"
			|| word == "group"
			|| word == "handle"
			|| word == "hashtable"
			|| word == "igamestate"
			|| word == "image"
			|| word == "integer"
			|| word == "item"
			|| word == "itempool"
			|| word == "itemtype"
			|| word == "key"
			|| word == "leaderboard"
			|| word == "lightning"
			|| word == "limitop"
			|| word == "location"
			|| word == "mapcontrol"
			|| word == "mapdensity"
			|| word == "mapflag"
			|| word == "mapsetting"
			|| word == "mapvisibility"
			|| word == "multiboard"
			|| word == "multiboarditem"
			|| word == "nothing"
			|| word == "pathingtype"
			|| word == "placement"
			|| word == "player"
			|| word == "playercolor"
			|| word == "playerevent"
			|| word == "playergameresult"
			|| word == "playerscore"
			|| word == "playerslotstate"
			|| word == "playerstate"
			|| word == "playerunitevent"
			|| word == "quest"
			|| word == "questitem"
			|| word == "race"
			|| word == "racepreference"
			|| word == "raritycontrol"
			|| word == "real"
			|| word == "rect"
			|| word == "region"
			|| word == "sound"
			|| word == "soundtype"
			|| word == "startlocprio"
			|| word == "string"
			|| word == "terraindeformation"
			|| word == "texmapflags"
			|| word == "texttag"
			|| word == "thistype"
			|| word == "timer"
			|| word == "timerdialog"
			|| word == "trackable"
			|| word == "trigger"
			|| word == "triggeraction"
			|| word == "triggercondition"
			|| word == "ubersplat"
			|| word == "unit"
			|| word == "unitevent"
			|| word == "unitpool"
			|| word == "unitstate"
			|| word == "unittype"
			|| word == "version"
			|| word == "volumegroup"
			|| word == "weapontype"
			|| word == "weathereffect"
			|| word == "widget"
			|| word == "widgetevent"
			|| word == "int"
			|| word == "float"
			|| word == "double"
			|| word == "long"
			|| word == "void"
			|| word == "bool"
			)
		{
			return word_t::type;
		}

		if (word == "return")
			return word_t::retn;

		if (word == "lambda")
			return word_t::lambda;

		if (word == "-"
			|| word == "+"
			|| word == "="
			|| word == "=="
			|| word == "!="
			|| word == "*"
			|| word == "/"
			|| word == "&&"
			|| word == "||"
			|| word == "!"
			|| word == "or"
			|| word == "and"
			|| word == "not"
			|| word == ">"
			|| word == "<"
			|| word == "<="
			|| word == ">=")
		{
			return word_t::op;
		}

		if (word == "++" || word == "--")
			return word_t::unary;

		if (word == "+="
			|| word == "-="
			|| word == "*="
			|| word == "/=")
		{
			return word_t::unaryExpr;
		}

		if (word[0] == '/' && (word[1] == '/' || word[1] == '*'))
			return word_t::comment;

		if (word == "true"
			|| word == "false"
			|| word == "null"
			|| reu::IsMatching(word, "^[0-9]+$")
			|| reu::IsMatching(word, "^\\.[0-9]+f?$")
			|| reu::IsMatching(word, "^[0-9\\.]{2,}f?$")
			|| reu::IsMatching(word, "^'.*'$")
			|| reu::IsMatching(word, "^\\\".*\\\"$"))
		{
			return word_t::constant;
		}

		if (word == "{")
			return word_t::blockBegin;

		if (word == "}")
			return word_t::blockEnd;

		if (word == ",")
			return word_t::next;

		if (word == "(")
			return word_t::expOpen;

		if (word == ")")
			return word_t::expClose;

		if (word == "[")
			return word_t::indexOpen;

		if (word == "]")
			return word_t::indexClose;

		if (word == "if")
			return word_t::If;

		if (word == "else")
			return word_t::Else;

		if (word == "elseif")
			return word_t::elseif;

		if (word == "while")
			return word_t::loop;

		if (word == "local"
			|| word == "call"
			|| word == "set"
			|| word == "function")
			return word_t::ignore;

		if (reu::IsMatching(word, "^[a-zA-Z_]{1,1}[0-9a-zA-Z_]*$"))
			return word_t::id;

		return word_t::undefined;
	}

	ctype_t classifyChar(char c)
	{
		std::string str = "?";
		str[0] = c;
		
		if (c == '.')
			return ctype_t::dot;
		else if (c == ',')
			return ctype_t::com;
		else if (c == '\\')
			return ctype_t::esc;
		else if (c == '"')
			return ctype_t::str;
		else if (c == '"')
			return ctype_t::raw;
		else if (c == '\'')
			return ctype_t::raw;
		else if (c == '{')
			return ctype_t::bBeg;
		else if (c == '}')
			return ctype_t::bEnd;
		else if (c == '(')
			return ctype_t::aBeg;
		else if (c == ')')
			return ctype_t::aEnd;
		else if (c == '[')
			return ctype_t::iBeg;
		else if (c == ']')
			return ctype_t::iEnd;
		else if (c == ';')
			return ctype_t::opEnd;
		else if (c == '\n')
			return ctype_t::nl;
		else if (c == '\r')
			return ctype_t::nlr;
		else if (reu::IsMatching(str, "^[\\s\\t]$"))
			return ctype_t::emp;
		else if (reu::IsMatching(str, "^[a-zA-Z_]$"))
			return ctype_t::lit;
		else if (reu::IsMatching(str, "^[0-9]$"))
			return ctype_t::dig;
		else if (reu::IsMatching(str, "^[\\\\\\/\\+\\-\\=\\*\\!\\<\\>\\^\\&\\|\\~\\%]$"))
			return ctype_t::oper;

		return ctype_t::unk;
	}

	Parser2::Parser2()
		: _outIf(OutputInterface::Type::File, Settings::OutputNewLineType)
		, _text("")
		, _word("")
		, _rootNode(_outIf)
		, _activeNode(&_rootNode)
		, _lastAddedNode(&_rootNode)
		, _line(0)
		, _wordPos(0)
		, _fileName("")
		, _strictMode(Settings::StrictMode)
	{
	}

	void Parser2::_pop()
	{
		if (_activeNode->Top() != nullptr)
		{
			appLog(Debug) << "Popping node" << Node::ToString(_activeNode);
			_activeNode->Complete(true);
			_activeNode = _activeNode->Top();
		}
		else
			appLog(Warning) << "Attempted to pop root node!";
	}

	Node* Parser2::_addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive)
	{
		auto node = Node::Produce(type, _activeNode, _outIf);
		node->InitData(data);
		_activeNode->AddSubnode(node);
		_lastAddedNode = node->Ptr();
		if (makeActive)
		{
			appLog(Debug) << "Pushing node" << Node::ToString(node->Ptr());
			_activeNode = _activeNode->LastSubnode();
			_activeNode->Complete(false);
			return _activeNode;
		}

		return _activeNode->LastSubnode();
	}

	Node* Parser2::_addNode(std::shared_ptr<cJass::Node> node, bool makeActive)
	{
		_activeNode->AddSubnode(node);
		_lastAddedNode = node->Ptr();
		if (makeActive)
		{
			appLog(Debug) << "Pushing node" << Node::ToString(node->Ptr());
			_activeNode = _activeNode->LastSubnode();
			_activeNode->Complete(false);
			return _activeNode;
		}

		return _activeNode->LastSubnode();
	}

	void Parser2::Parse(csref_t cjassFileName, csref_t outputFileName)
	{
		char c;
		char prev = '\0';
		char next = '\0';
		bool isString = false;
		bool isLineComment = false;
		bool isMultilineComment = false;
		bool isRawCode = false;
		bool isIndex = false;
		
		if (!fileExists(cjassFileName.c_str()))
		{
			appLog(Critical) << "Unable to open file" << cjassFileName;
			return;
		}
		
		_text = FileToString(cjassFileName);
		_activeNode = &_rootNode;
		_lastAddedNode = _activeNode;
		_rootNode.Clear();
		_outIf.SetOutputFile(outputFileName);
		reu::ReplaceAll(_text, "\\.evaluate", "");
		_fileName = cjassFileName;
		_line = 1;
		_wordPos = 1;
		_word = "";
		std::string prevWord;
		size_t len = _text.length();
		ctype_t ctPrev = ctype_t::undefined, ct;
		std::vector<std::string> vec_arg;
		int parseSteps = 0;

		appLog(Info) << "Parsing file" << cjassFileName;

		std::stack<ParseSpecialSubject> subjectStack;
		subjectStack.push(ParseSpecialSubject::none);
		auto parseWord = [this, &prevWord, &parseSteps, &vec_arg, ctPrev, &isString, &isLineComment,
			&isMultilineComment, &isRawCode, &subjectStack](ctype_t explicitParse = ctype_t::undefined, bool clearStatic = false) -> void
		{
			static bool globalVarDeclaration = false;
			static bool ignoreLine = false;
			static word_t wtype = word_t::undefined;
			static word_t wtype_prev = word_t::undefined;
			static std::shared_ptr<cJass::Node> tmpNode;
			static auto prevActive = _activeNode;
			static std::string varName;
			static std::string varArrSize;
			static bool lambda = false;
			static bool waitingForCondExpr = false;
			static bool writingArrDecl = false;
			static std::stack<cJass::Node*> arrNodeStackLast;
			static std::stack<cJass::Node*> arrNodeStackActive;
			static std::stack<cJass::Node*> callNodeStackLast;
			static std::stack<int>			argCount;
			static std::stack<bool>			wrapperStateStack;
			static std::stack<bool>			allowWrappersStack;
			static std::stack<bool>			varAddedStack;
			static std::stack<bool>			unclosedOpStack;

			if (clearStatic)
			{
				globalVarDeclaration = false;
				ignoreLine = false;
				wtype = word_t::undefined;
				wtype_prev = word_t::undefined;
				tmpNode = nullptr;
				prevActive = _activeNode;
				varName = "";
				varArrSize = "";
				lambda = false;
				waitingForCondExpr = false;
				writingArrDecl = false;
				stack_clear(arrNodeStackLast);
				stack_clear(arrNodeStackActive);
				stack_clear(callNodeStackLast);
				stack_clear(argCount);
				stack_clear(wrapperStateStack);
				stack_clear(allowWrappersStack);
				stack_clear(varAddedStack);
				stack_clear(unclosedOpStack);
				return;
			}
			
			auto pops = [this, &subjectStack]() -> void
			{
				_pop();
				subjectStack.pop();
			};

			auto tryToPutNewLine = [this]() -> void
			{
				if (wtype_prev == word_t::blockEnd)
					return;

				bool lastIsLogic = (_activeNode->LastSubnode() != nullptr)
					? (_activeNode->LastSubnode()->GetType() == Node::Type::OperationObject
						&& _activeNode->LastSubnode()->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Logic)
					: false;

				if (_activeNode->IsBlock() || lastIsLogic)
				{
					if (wtype_prev == word_t::nl)
						_addNode(Node::Type::OperationObject, { "N" });
					return;
				}
				
				if (_activeNode->GetType() == Node::Type::OperationObject)
				{
					auto ot = _activeNode->Ptr<OperationObject>()->GetOpType();

					switch (ot)
					{
					case OperationObject::OpType::Logic:
					case OperationObject::OpType::Expression:
					case OperationObject::OpType::Argument:
							_addNode(Node::Type::OperationObject, { "N" });
						break;
					}
				}
			};

			wtype = classifyWord(_word);

			if (_word == "")
			{
				switch (explicitParse)
				{
				case ctype_t::nl:
					ignoreLine = false;
					if (_activeNode->GetDepth() == 0)
						goto parseWordEnd;

					wtype = word_t::nl;
					break;

				case ctype_t::aBeg:
					wtype = word_t::expOpen;
					break;

				case ctype_t::aEnd:
					_word = ")";
					wtype = word_t::expClose;
					break;

				case ctype_t::bBeg:
					if (_activeNode->GetDepth() == 0)
						return;
					wtype = word_t::blockBegin;
					break;

				case ctype_t::bEnd:
					_word = "}";
					wtype = word_t::blockEnd;
					break;

				case ctype_t::iBeg:
					_word = "[";
					wtype = word_t::indexOpen;
					break;

				case ctype_t::iEnd:
					_word = "]";
					wtype = word_t::indexClose;
					break;

				case ctype_t::opEnd:
					_word = ";";
					wtype = word_t::end;
					break;

				case ctype_t::com:
					_word = ",";
					wtype = word_t::next;
					break;

				default:
					return;
				}
			}

			if (ignoreLine)
				goto parseWordEnd;

			if (_word == "//")
			{
				isLineComment = true;
				_word = "";
				return;
			}

			if (_word == "/*")
			{
				isMultilineComment = true;
				goto parseWordEnd;
				_word = "";
				return;
			}

			if (_word == "=" && _depth() == 0)
			{
				globalVarDeclaration = true;
				goto parseWordEnd;
			}

			if (isString)
			{
				_word = "\"" + _word + "\"";
				wtype = word_t::constant;
			}

			if (isRawCode)
			{
				_word = "'" + _word + "'";
				wtype = word_t::constant;
			}

			if (isLineComment || isMultilineComment)
			{
				if (_depth() == 0)
				{
					_word = "\n" + _word;
					if (!Settings::IgnoreComments)
						_addNode(Node::Type::Comment, { _word });
					goto parseWordEnd;
				}
				else
				{
					if (!(isLineComment && wtype == word_t::nl))
						wtype = word_t::comment;
				}
					
			}

			if (_depth() == 0)
			{
				if (_word == "library" || _word == "endlibrary")
				{
					ignoreLine = true;
					goto parseWordEnd;
				}

				if (_word == ",")
					goto parseWordEnd;

				if (subjectStack.top() == ParseSpecialSubject::globals)
				{
					if (_word == "endglobals")
					{
						appLog(Debug) << "End parsing globals";
						subjectStack.pop();
						vec_arg.clear();
						goto parseWordEnd;
					}

					if (vec_arg.size() == 2)
					{
						if (globalVarDeclaration)
						{
							if (_word == ";")
								vec_arg.push_back("");
							else
								vec_arg.push_back(_word);
							_rootNode.InitData(vec_arg);
							vec_arg.clear();
							globalVarDeclaration = false;
							goto parseWordEnd;
						}
						else
						{
							vec_arg.push_back("");
							_rootNode.InitData(vec_arg);
							vec_arg.clear();
							if (_word != ";")
								vec_arg.push_back(_word);
							goto parseWordEnd;
						}
					}
					else if (_word != ";")
						vec_arg.push_back(_word);

				}
				else if (subjectStack.top() == ParseSpecialSubject::defs)
				{
					if (_word == "enddefine" || _word == "}")
					{
						appLog(Debug) << "End parsing defines";
						subjectStack.pop();
						goto parseWordEnd;
					}

					if (vec_arg.size() == 0)
						vec_arg.push_back("");

					vec_arg.push_back(_word);

					if (globalVarDeclaration)
					{
						_rootNode.InitData(vec_arg);
						vec_arg.clear();
						globalVarDeclaration = false;
						goto parseWordEnd;
					}
				}
				else if (subjectStack.top() == ParseSpecialSubject::func)
				{
				_func:
					if (_word == ")")
					{
						appLog(Debug) << "Parsing function" << vec_arg[1];
						_addNode(Node::Type::Function, vec_arg, true);
						subjectStack.pop();
						wrapperStateStack.push(false);
						allowWrappersStack.push(true);
						varAddedStack.push(false);
						unclosedOpStack.push(false);
						vec_arg.clear();
						goto parseWordEnd;
					}

					if (_word != "")
						vec_arg.push_back(_word);
				}
				else if (subjectStack.top() == ParseSpecialSubject::none)
				{
					if (_word == "globals")
					{
						appLog(Debug) << "Parsing globals";
						subjectStack.push(ParseSpecialSubject::globals);
					}
					else if (_word == "define")
					{
						appLog(Debug) << "Parsing defines";
						subjectStack.push(ParseSpecialSubject::defs);
					}
					else if (_word != ";")
					{
						vec_arg.clear();
						subjectStack.push(ParseSpecialSubject::func);
						goto _func;
					}
				}
			}
			else
			{
				bool isLocalTop = _activeNode->GetType() == Node::Type::LocalDeclaration;
				bool isVarExprTop = (_activeNode->GetType() == Node::Type::OperationObject
					&& _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::VarInitExpression);
				

				if (subjectStack.top() == ParseSpecialSubject::call && argCount.top() == 0 && wtype != word_t::expClose)
				{
					callNodeStackLast.push(_lastAddedNode);
					tmpNode = Node::Produce(Node::Type::OperationObject, _activeNode, _outIf);
					tmpNode->InitData({ "a" });
					_activeNode->AddSubnode(tmpNode);
					_activeNode = tmpNode->Ptr();
					_lastAddedNode = tmpNode->Ptr();
					stack_incr_top(argCount);
				}

				if (writingArrDecl)
				{
					if (wtype == word_t::indexClose)
					{
						writingArrDecl = false;
						if (varArrSize == "")
							varArrSize = "0";
					}
					else
						varArrSize += _word;
					goto parseWordEnd;
				}

				if (waitingForCondExpr && wtype != word_t::expOpen)
				{
					appLog(Warning) << "Expected condition expression (" << DOC_LINEPOS;
					goto parseWordEnd;
				}

				if (_activeNode->GetType() == Node::Type::OperationObject
					&& _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Logic)
				{
					if (wtype != word_t::elseif && wtype != word_t::Else && wtype != word_t::nl && wtype != word_t::comment)
						_pop();
				}

				switch (wtype)
				{
				case word_t::retn:
					_addNode(Node::Type::OperationObject, { "r" }, true);
					bstack_set_top(unclosedOpStack, true);
					bstack_set_top(allowWrappersStack, false);
					break;

				case word_t::id:
					if (isLocalTop && !varAddedStack.top())
					{
						varName = _word;
					}
					else
					{
						if (!wrapperStateStack.top() && allowWrappersStack.top())
						{
							_addNode(Node::Type::OperationObject, { "W" }, true);
							bstack_set_top(wrapperStateStack, true);
							bstack_set_top(unclosedOpStack, true);
						}

						_addNode(Node::Type::OperationObject, { "I", _word });
					}
					break;

				case word_t::type:
					if (_activeNode->GetType() == Node::Type::OperationObject
						&& (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Call
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Argument
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Expression
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::VarInitExpression
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Index
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Return) && !lambda)
					{
						appLog(Warning) << "Unexpected typename" << _word << DOC_LINEPOS;
						goto parseWordEnd;
					}

					if (_activeNode->GetType() == Node::Type::OperationObject && lambda)
					{
						if (_activeNode->Ptr<OperationObject>()->GetOpType() != OperationObject::OpType::Lambda)
						{
							if (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::VarInitExpression
								|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Wrapper)
							{
								_addNode(Node::Type::OperationObject, { "ls", _word }, true);
							}
							else
								_addNode(Node::Type::OperationObject, { "l", _word }, true);
						}
						else
							appLog(Warning) << "Lambda typename again twice";
					}
					else if (lambda)
						_addNode(Node::Type::OperationObject, { "l", _word }, true);
					else
					{
						_addNode(Node::Type::LocalDeclaration, { _word }, true);
						bstack_set_top(varAddedStack, false);
						bstack_set_top(allowWrappersStack, false);
						bstack_set_top(unclosedOpStack, true);
					}
					break;

				case word_t::op:
					if (isLocalTop && _word == "=")
					{
						_activeNode = _activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						bstack_set_top(varAddedStack, true);
					}
					else
					{
						if (!wrapperStateStack.top() && allowWrappersStack.top())
						{
							_addNode(Node::Type::OperationObject, { "W" }, true);
							bstack_set_top(wrapperStateStack, true);
							bstack_set_top(unclosedOpStack, true);
						}

						std::string oper = _word;
						if (wtype_prev == word_t::op || _activeNode->CountSubnodes() == 0 && oper == "-")
							oper = "dig_minus";
						_addNode(Node::Type::OperationObject, { "o", oper });
					}
					break;

				case word_t::unary:
					if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
						_lastAddedNode->InitData({ "u", prevWord, _word });
					else
						appLog(Warning) << "Unary operator not after identifier.(" << _line << ":" << _wordPos << ")";
					break;

				case word_t::unaryExpr:
					if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
						_lastAddedNode->InitData({ "ue", prevWord, _word });
					else
						appLog(Warning) << "Unary expression not after identifier.(" << _line << ":" << _wordPos << ")";
					break;

				case word_t::constant:
					if (!wrapperStateStack.top() && allowWrappersStack.top())
					{
						_addNode(Node::Type::OperationObject, { "W" }, true);
						bstack_set_top(wrapperStateStack, true);
						bstack_set_top(unclosedOpStack, true);
					}
					_addNode(Node::Type::OperationObject, { "C", _word });
					break;

				case word_t::expOpen:
					if (waitingForCondExpr)
						waitingForCondExpr = false;

					if (lambda)
						goto parseWordEnd;

					if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
					{
						_lastAddedNode->InitData({ "c", prevWord });
						_activeNode = _lastAddedNode;
						subjectStack.push(ParseSpecialSubject::call);
						argCount.push(0);
					}
					else
						_addNode(Node::Type::OperationObject, { "xb" }, true);
					break;

				case word_t::expClose:
					if (lambda)
						goto parseWordEnd;

					if (_activeNode->GetType() == Node::Type::OperationObject
						&& (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Call
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Argument))
					{
						pops();
						if (argCount.top() > 0)
						{
							_pop();
							_lastAddedNode = callNodeStackLast.top();
							callNodeStackLast.pop();
						}
						argCount.pop();
					}
					else
					{
						_pop();
						if (_activeNode->GetType() == Node::Type::OperationObject
							&& (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::If
							||  _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Elseif
							||  _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::While)
							&& _activeNode->CountSubnodes() == 1)
						{
							bstack_set_top(allowWrappersStack, true);
						}
						
					}
					break;

				case word_t::indexOpen:
					if (_activeNode->GetType() == Node::Type::OperationObject
						&& _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Index)
					{
						appLog(Warning) << "Declaring array index with bracket [, but another array index declaration is already in progress." << DOC_LINEPOS;
						goto parseWordEnd;
					}

					if (isLocalTop)
						writingArrDecl = true;
					else if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
					{
						tmpNode = Node::Produce(Node::Type::OperationObject, _lastAddedNode, _outIf);
						tmpNode->InitData({ "n" });
						_lastAddedNode->AddSubnode(tmpNode);
						arrNodeStackActive.push(_activeNode);
						arrNodeStackLast.push(_lastAddedNode);
						_activeNode = tmpNode->Ptr();
					}
					else
						appLog(Warning) << "Wrong usage of array brackets. Must be after identifier!" << DOC_LINEPOS;
					break;

				case word_t::indexClose:
					if (_activeNode->GetType() != Node::Type::OperationObject
						|| _activeNode->Ptr<OperationObject>()->GetOpType() != OperationObject::OpType::Index)
					{
						appLog(Warning) << "Closing array index ] bracket, but no array has been declared." << DOC_LINEPOS;
						goto parseWordEnd;
					}

					_activeNode = arrNodeStackActive.top();
					_lastAddedNode = arrNodeStackLast.top();
					arrNodeStackLast.pop();
					arrNodeStackActive.pop();
					break;

				case word_t::If:
					_addNode(Node::Type::OperationObject, { "L" }, true);
					_addNode(Node::Type::OperationObject, { "i" }, true);
					bstack_set_top(allowWrappersStack, false);
					waitingForCondExpr = true;
					break;

				case word_t::elseif:
					_addNode(Node::Type::OperationObject, { "E" }, true);
					bstack_set_top(allowWrappersStack, false);
					waitingForCondExpr = true;
					break;

				case word_t::Else:
					_addNode(Node::Type::OperationObject, { "e" }, true);
					break;
					break;

				case word_t::lambda:
					lambda = true;
					break;

				case word_t::comment:
					if (wtype_prev == word_t::nl)
						_word = "\n" + _word;
					if (!Settings::IgnoreComments)
						_addNode(Node::Type::Comment, { _word });
					break;

				case word_t::next:
					if (isLocalTop || isVarExprTop)
					{
						if (isVarExprTop)
						{
							_pop();
							isLocalTop = true;
						}

						if (!varAddedStack.top())
						{
							_activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						}
						else
						{
							bstack_set_top(varAddedStack, false);
						}
						varName = "";
						varArrSize = "";
					}
					else
					{
						if (_activeNode->GetType() == Node::Type::OperationObject
							&& _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Argument)
						{
							_pop();
							tmpNode = Node::Produce(Node::Type::OperationObject, _activeNode, _outIf);
							tmpNode->InitData({ "a" });
							_activeNode->AddSubnode(tmpNode);
							_activeNode = tmpNode->Ptr();
							_lastAddedNode = tmpNode->Ptr();
							stack_incr_top(argCount);
						}
						else
							appLog(Warning) << "Unexpected usage of ," << DOC_LINEPOS << "This is not function call or variable enumeration.";
					}
					break;

				case word_t::loop:
					_addNode(Node::Type::OperationObject, { "w" }, true);
					bstack_set_top(allowWrappersStack, false);
					waitingForCondExpr = true;
					break;
					
				case word_t::nl:
					if (_strictMode)
					{
						tryToPutNewLine();
						break;
					}

					if ((isLocalTop || isVarExprTop)
						|| (_activeNode->GetType() == Node::Type::OperationObject
							&& (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Wrapper
								|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Return
								|| (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Lambda
									&& _activeNode->Ptr<OperationObject>()->LambdaIsSingle()
									&& _activeNode->Ptr<OperationObject>()->BlockClosed()))))
					{
						if (!unclosedOpStack.top())
							goto parseWordEnd;

						if (isVarExprTop)
						{
							_pop();
							isLocalTop = true;
						}

						if (isLocalTop)
						{
							if (!varAddedStack.top())
							{
								_activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
							}
							else
							{
								bstack_set_top(varAddedStack, false);
							}
							varName = "";
							varArrSize = "";
							bstack_set_top(allowWrappersStack, true);
						}

						if (_activeNode->GetType() == Node::Type::OperationObject
							&&  _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Return)
							bstack_set_top(allowWrappersStack, true);

						if (wrapperStateStack.top())
							bstack_set_top(wrapperStateStack, false);
						bstack_set_top(unclosedOpStack, false);
						_pop();
						tryToPutNewLine();
					}
					else
						tryToPutNewLine();
					break;

				case word_t::end:
					if (_activeNode->GetType() == Node::Type::OperationObject
						&& (_activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Expression
							|| _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Call))
					{
						appLog(Warning) << "Missing expression or call close )" << DOC_LINEPOS;
						goto parseWordEnd;
					}

					if (!unclosedOpStack.top())
					{
						appLog(Warning) << "Unexpected ;" << DOC_LINEPOS;
						goto parseWordEnd;
					}

					if (isVarExprTop)
					{
						_pop();
						isLocalTop = true;
					}
						
					if (isLocalTop)
					{
						if (!varAddedStack.top())
						{
							_activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						}
						else
						{
							bstack_set_top(varAddedStack, false);
						}
						varName = "";
						varArrSize = "";
						bstack_set_top(allowWrappersStack, true);
					}

					if (_activeNode->GetType() == Node::Type::OperationObject
						&&  _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Return)
						bstack_set_top(allowWrappersStack, true);
					
					if (wrapperStateStack.top())
						bstack_set_top(wrapperStateStack, false);
					_pop();
					bstack_set_top(unclosedOpStack, false);
					break;

				case word_t::blockBegin:
					wrapperStateStack.push(false);
					allowWrappersStack.push(true);
					varAddedStack.push(false);
					unclosedOpStack.push(false);

					if (lambda)
					{
						lambda = false;
						break;
					}
						
					break;

				case word_t::blockEnd:
					if (_activeNode->GetType() == Node::Type::OperationObject)
						_activeNode->Ptr<OperationObject>()->CloseBlock();
					_pop();
					wrapperStateStack.pop();
					allowWrappersStack.pop();
					varAddedStack.pop();
					unclosedOpStack.pop();
					if (_activeNode->GetDepth() == 0)
						appLog(Debug) << "End parsing function";
					break;

				case word_t::ignore:
					break;

				case word_t::undefined:
					appLog(Warning) << "Unknown identifier" << _word;
					break;
				}
			}

		parseWordEnd:
			prevWord = _word;
			wtype_prev = wtype;
			_word = "";
		};
		parseWord(ctype_t::unk, true);

		appLog(Info) << "Starting parse of \"" << _fileName << "\"";
		appLog(Debug) << "Parsing line" << 1;
		try
		{
			for (size_t i = 0; i < len; i++)
			{
				c = _text[i];
				ct = classifyChar(c);
				if ((i + 1) < len)
					next = _text[i + 1];
				else
					next = '\0';

				if (i > 0)
					prev = _text[i - 1];
				else
					prev = '\0';

				if (ct == ctype_t::nl)
				{
					_wordPos = 1;
					_line++;
					appLog(Debug) << "Parsing line" << _line;
				}
				else
					_wordPos++;
				
				if (isString || isMultilineComment || isLineComment || isRawCode)
				{
					if (isString && (c != '"' || c == '"' && ctPrev == ctype_t::esc))
					{
						_word.push_back(c);
						ctPrev = ct;
						continue;
					}
					else if (isMultilineComment)
					{
						if (c != '*')
						{
							_word.push_back(c);
							ctPrev = ct;
							continue;
						}
						else
						{
							if (next != '/')
							{
								_word.push_back(c);
								ctPrev = ct;
								continue;
							}
						}

					}
					else if (isLineComment && c != '\r' && c != '\n')
					{
						_word.push_back(c);
						ctPrev = ct;
						continue;
					}
					else if (isRawCode && c != '\'')
					{
						_word.push_back(c);
						ctPrev = ct;
						continue;
					}
				}

				if (isString && c == '"' && ctPrev != ctype_t::esc)
				{
					parseWord();
					isString = false;
					ctPrev = ct;
					continue;
				}
				else if (isMultilineComment && c == '*' && next == '/' && ctPrev != ctype_t::esc)
				{
					parseWord();
					isMultilineComment = false;
					i++;
					ctPrev = ct;
					continue;
				}
				else if (isLineComment && (c == '\r' || c == '\n'))
				{
					parseWord();
					parseWord(ct);
					isLineComment = false;
					ctPrev = ct;
					continue;
				}
				else if (isRawCode && c == '\'')
				{
					parseWord();
					isRawCode = false;
					ctPrev = ct;
					continue;
				}

				switch (ct)
				{
				case ctype_t::oper:
					if (ctPrev != ctype_t::oper)
						parseWord();
					_word.push_back(c);
					break;

				case ctype_t::str:
					parseWord();
					isString = true;
					break;

				case ctype_t::lit:
					if (!reu::IsMatching(_word, "^[a-zA-Z_][a-zA-Z0-9_]*$"))
						parseWord();
					_word.push_back(c);
					break;

				case ctype_t::dot:
					if (!reu::IsMatching(_word, "^[0-9\\.]+$"))
					{
						if (reu::IsMatching(_word, "^[a-zA-Z_][a-zA-Z0-9_]*$"))
						{
							appLog(Critical) << "Member/method detected on " << DOC_LINEPOS << "! Classes/Structures/Objects are not yet supported.";
							return;
						}
						else
							parseWord();
					}
					_word.push_back(c);
					break;

				case ctype_t::dig:
					if (!reu::IsMatching(_word, "^[a-zA-Z0-9_\\.]+$"))
						parseWord();
					_word.push_back(c);
					break;

				case ctype_t::raw:
					parseWord();
					isRawCode = true;
					break;

				case ctype_t::esc:
					parseWord();
					break;

				case ctype_t::nlr:
					//Ignore '\r'
					break;

				case ctype_t::unk:
				case ctype_t::emp:
					parseWord();
					if (isMultilineComment || isLineComment)
						_word.push_back(c);
					break;

				case ctype_t::nl:
				case ctype_t::com:
				case ctype_t::iBeg:
				case ctype_t::bBeg:
				case ctype_t::opEnd:
				case ctype_t::bEnd:
				case ctype_t::aBeg:
				case ctype_t::aEnd:
				case ctype_t::iEnd:
					parseWord();
					parseWord(ct);
					break;
				}

				ctPrev = ct;
			}
			appLog(Info) << "Parsing of file " << cjassFileName << " successfuly done!" << _line;
		}
		catch (const std::exception& ex)
		{
			appLog(Warning) << "Possible problem on " << DOC_LINEPOS << ".";
			throw std::runtime_error(ex.what());
		}
	}

	void Parser2::ToLua()
	{
		_rootNode.ToLua();
	}

	size_t Parser2::_depth() const
	{
		return _activeNode->GetDepth();
	}
} //namespace cJass