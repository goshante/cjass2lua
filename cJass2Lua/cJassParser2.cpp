#include "cJassParser2.h"
#include "reutils.h"
#include <sstream>

void stack_incr_top(std::stack<int>& st)
{
	int i = st.top();
	i++;
	st.pop();
	st.push(i);
}

namespace cJass
{
	enum class ctype_t
	{
		undefined,
		unk,	//Other
		emp,	//Empty - space or tab
		nl,		//Newline - \n
		nlr,	//Newline - \r
		lit,	//Literal
		dig,	//Digit
		oper,	//Operator
		dot,	//Dot
		com,	//Comma
		str,	//String "
		raw,	//Rawcode '
		bBeg,	//Block begin {
		bEnd,	//Block end }
		aBeg,	//Args begin (
		aEnd,	//Args end )
		iBeg,	//Index begin [
		iEnd,	//Index end ]
		opEnd	// ;
	};

	enum class word_t
	{
		undefined,
		id,
		type,
		retn,
		lambda,
		op,
		unary,
		unaryExpr,
		constant,
		expOpen,
		expClose,
		indexOpen,
		indexClose,
		If,
		Else,
		elseif,
		comment,
		next,
		loop,
		end,
		blocklEnd
	};

	word_t classifyWord(const std::string& word)
	{
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
			|| reu::IsMatching(word, "^.[0-9]+$")
			|| reu::IsMatching(word, "^[0-9\\.]{2,}$")
			|| reu::IsMatching(word, "^\\\".*\\\"$")
			|| reu::IsMatching(word, "^\\'.*\\'$"))
		{
			return word_t::constant;
		}

		if (word == "}")
			return word_t::blocklEnd;

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

		if (word != "local"
			&& word != "call"
			&& word != "set"
			&& word != "function"
			&& reu::IsMatching(word, "^[a-zA-Z_]{1,1}[0-9a-zA-Z_]*$"))
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

	Parser2::Parser2(OutputInterface::Type outputType, OutputInterface::NewLineType nlType, std::string& fileNameOrString)
		: _text("")
		, _word("")
		, _rootNode(outputType, nlType, fileNameOrString)
		, _activeNode(&_rootNode)
		, _lastAddedNode(&_rootNode)
		, _unitIsComplete(false)
		, _line(0)
		, _wordPos(0)
		, _fileName("")
	{
	}

	void Parser2::_pop()
	{
		if (_activeNode->Top() != nullptr)
			_activeNode = _activeNode->Top();
	}

	Node* Parser2::_addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive)
	{
		auto node = Node::Produce(type, _activeNode);
		node->InitData(data);
		_activeNode->AddSubnode(node);
		_lastAddedNode = node->Ptr();
		if (makeActive)
		{
			_activeNode = _activeNode->LastSubnode();
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
			_activeNode = _activeNode->LastSubnode();
			return _activeNode;
		}

		return _activeNode->LastSubnode();
	}

	void Parser2::Parse(csref_t text, csref_t cjassFileName)
	{
		char c;
		char prev = '\0';
		char next = '\0';
		bool isString = false;
		bool isLineComment = false;
		bool isMultilineComment = false;
		bool isRawCode = false;
		bool isIndex = false;
		_text = text;
		_fileName = cjassFileName;
		_line = 1;
		_wordPos = 1;
		_word = "";
		std::string prevWord;
		size_t len = _text.length();
		ctype_t ctPrev = ctype_t::undefined, ct;
		std::vector<std::string> vec_arg;

		int parseSteps = 0;

		std::stack<ParseTag2_t> subjectStack;
		subjectStack.push(ParseTag2_t::none);
		auto parseWord = [this, &prevWord, &parseSteps, &vec_arg, ctPrev, &isString, &isLineComment,
			&isMultilineComment, &isRawCode, &subjectStack](ctype_t explicitParse = ctype_t::undefined) -> void
		{
			static bool globalVarDeclaration = false;
			static size_t ignoreLine = ~0;
			static word_t wtype = word_t::undefined;
			static word_t wtype_prev = word_t::undefined;
			static std::shared_ptr<cJass::Node> tmpNode;
			static auto prevActive = _activeNode;
			static std::stack<cJass::Node*> arrNodeStackLast;
			static std::stack<cJass::Node*> arrNodeStackActive;
			static std::stack<cJass::Node*> callNodeStackLast;
			static std::stack<int>			argCount;

			auto pops = [this, &subjectStack]() -> void
			{
				_pop();
				subjectStack.pop();
			};

			wtype = classifyWord(_word);

			if (_word == "")
			{
				switch (explicitParse)
				{
				case ctype_t::aBeg:
					wtype = word_t::expOpen;
					break;

				case ctype_t::aEnd:
					_word = ")";
					wtype = word_t::expClose;
					break;

					/*case ctype_t::bBeg:
						_word = "{";
						break;*/

				case ctype_t::bEnd:
					_word = "}";
					wtype = word_t::blocklEnd;
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

			if (ignoreLine == _line)
				return;

			if (_word == "//")
			{
				isLineComment = true;
				goto parseWordEnd;
			}

			if (_word == "/*")
			{
				isMultilineComment = true;
				goto parseWordEnd;
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
				_addNode(Node::Type::Comment, { _word });
				goto parseWordEnd;
			}

			if (_depth() == 0)
			{
				if (_word == "library" || _word == "endlibrary")
					ignoreLine = _line;

				if (_word == ",")
					goto parseWordEnd;

				if (subjectStack.top() == ParseTag2_t::globals)
				{
					if (_word == "endglobals")
					{
						appLog(Debug) << "End parsing globals";
						pops();
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
							vec_arg.push_back(_word);
							goto parseWordEnd;
						}
					}
					else if (_word != ";")
						vec_arg.push_back(_word);

				}
				else if (subjectStack.top() == ParseTag2_t::defs)
				{
					if (_word == "enddefine")
					{
						appLog(Debug) << "End parsing defines";
						pops();
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
				else if (subjectStack.top() == ParseTag2_t::func)
				{
				_func:
					if (_word == ")")
					{
						appLog(Debug) << "Parsing function" << vec_arg[1];
						_addNode(Node::Type::Function, vec_arg, true);
						subjectStack.pop();
						vec_arg.clear();
						goto parseWordEnd;
					}

					if (_word != "")
						vec_arg.push_back(_word);
				}
				else if (subjectStack.top() == ParseTag2_t::none)
				{
					if (_word == "globals")
					{
						appLog(Debug) << "Parsing globals";
						subjectStack.push(ParseTag2_t::globals);
					}
					else if (_word == "define")
					{
						appLog(Debug) << "Parsing defines";
						subjectStack.push(ParseTag2_t::defs);
					}
					else if (_word != ";")
					{
						vec_arg.clear();
						subjectStack.push(ParseTag2_t::func);
						goto _func;
					}
				}
			}
			else
			{
				if (subjectStack.top() == ParseTag2_t::ret)
				{
					if (_word == ";")
					{
						pops();
						goto parseWordEnd;
					}
				}

				static std::string varName;
				static std::string varArrSize;
				static bool varAdded = false;
				static bool writingArrDecl = false;
				static bool wrapperCreated = false;
				static bool allowWrappers = false;
				bool isLocalTop = _activeNode->GetType() == Node::Type::LocalDeclaration;
				bool isVarExprTop = (_activeNode->GetType() == Node::Type::OperationObject
					&& _activeNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::VarInitExpression);

				if (subjectStack.top() == ParseTag2_t::call && argCount.top() == 0 && wtype != word_t::expClose)
				{
					callNodeStackLast.push(_lastAddedNode);
					tmpNode = Node::Produce(Node::Type::OperationObject, _activeNode);
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

				if (_word == "varsdg1")
					_word = _word;

				switch (wtype)
				{
				case word_t::retn:
					subjectStack.push(ParseTag2_t::ret);
					_addNode(Node::Type::OperationObject, { "r" }, true);
					break;

				case word_t::id:
					if (isLocalTop && !varAdded)
					{
						varName = _word;
					}
					else
					{
						if (!wrapperCreated && subjectStack.top() != ParseTag2_t::ret && allowWrappers)
						{
							_addNode(Node::Type::OperationObject, { "W" }, true);
							wrapperCreated = true;
						}

						_addNode(Node::Type::OperationObject, { "I", _word });
					}
					break;

				case word_t::type:
					_addNode(Node::Type::LocalDeclaration, { _word }, true);
					varAdded = false;
					allowWrappers = false;
					break;

				case word_t::op:
					if (isLocalTop && _word == "=")
					{
						_activeNode = _activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						varAdded = true;
					}
					else
					{
						if (!wrapperCreated && subjectStack.top() != ParseTag2_t::ret && allowWrappers)
						{
							_addNode(Node::Type::OperationObject, { "W" }, true);
							wrapperCreated = true;
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
					if (!wrapperCreated && subjectStack.top() != ParseTag2_t::ret && allowWrappers)
					{
						_addNode(Node::Type::OperationObject, { "W" }, true);
						wrapperCreated = true;
					}
					_addNode(Node::Type::OperationObject, { "C", _word });
					break;

				case word_t::expOpen:
					if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
					{
						_lastAddedNode->InitData({ "c", prevWord });
						_activeNode = _lastAddedNode;
						subjectStack.push(ParseTag2_t::call);
						argCount.push(0);
					}
					else
						_addNode(Node::Type::OperationObject, { "xb" }, true);
					break;

				case word_t::expClose:
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
						_pop();
					break;

				case word_t::indexOpen:
					if (isLocalTop)
						writingArrDecl = true;
					else if (_lastAddedNode->GetType() == Node::Type::OperationObject
						&& _lastAddedNode->Ptr<OperationObject>()->GetOpType() == OperationObject::OpType::Id)
					{
						tmpNode = Node::Produce(Node::Type::OperationObject, _lastAddedNode);
						tmpNode->InitData({ "n" });
						_lastAddedNode->AddSubnode(tmpNode);
						arrNodeStackActive.push(_activeNode);
						arrNodeStackLast.push(_lastAddedNode);
						_activeNode = tmpNode->Ptr();
					}
					else
						appLog(Warning) << "Wrong usage of array brackets. Must be after identifier!";
					break;

				case word_t::indexClose:
					_activeNode = arrNodeStackActive.top();
					_lastAddedNode = arrNodeStackLast.top();
					arrNodeStackLast.pop();
					arrNodeStackActive.pop();
					break;

				case word_t::If:
					break;

				case word_t::Else:
					break;

				case word_t::elseif:
					break;

				case word_t::lambda:
					break;

				case word_t::comment:
					_addNode(Node::Type::Comment, { _word });
					break;

				case word_t::next:
					if (isLocalTop || isVarExprTop)
					{
						if (isVarExprTop)
							_pop();

						if (!varAdded)
						{
							_activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						}
						else
						{
							varAdded = false;
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
							tmpNode = Node::Produce(Node::Type::OperationObject, _activeNode);
							tmpNode->InitData({ "a" });
							_activeNode->AddSubnode(tmpNode);
							_activeNode = tmpNode->Ptr();
							_lastAddedNode = tmpNode->Ptr();
							stack_incr_top(argCount);
						}
						else
							appLog(Warning) << "Unexpected comma.(" << _line << ":" << _wordPos << ")This is not function call.";
					}
					break;

				case word_t::loop:
					break;

				case word_t::end:
					if (isVarExprTop)
						_pop();

					if (isLocalTop)
					{
						if (!varAdded)
						{
							_activeNode->Ptr<LocalDeclaration>()->AddVariable(varName, varArrSize);
						}
						else
						{
							varAdded = false;
						}
						allowWrappers = true;
						varName = "";
						varArrSize = "";
					}

					if (wrapperCreated)
						wrapperCreated = false;
					_pop();
					break;

				case word_t::blocklEnd:
					_pop();
					if (_activeNode->GetDepth() == 0)
						appLog(Debug) << "End parsing function";
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
					if (isString && c != '"')
					{
						_word.push_back(c);
						continue;
					}
					else if (isMultilineComment)
					{
						if (c != '*')
						{
							_word.push_back(c);
							continue;
						}
						else
						{
							if (next != '/')
							{
								_word.push_back(c);
								continue;
							}
						}

					}
					else if (isLineComment && c != '\r' && c != '\n')
					{
						_word.push_back(c);
						continue;
					}
					else if (isRawCode && c != '\'')
					{
						_word.push_back(c);
						continue;
					}
				}

				if (isString && c == '"')
				{
					parseWord();
					isString = false;
					ctPrev = ct;
					continue;
				}
				else if (isMultilineComment && c == '*' && next == '/')
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
						parseWord();
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

				case ctype_t::nl:
				case ctype_t::nlr:
				case ctype_t::unk:
				case ctype_t::emp:
					parseWord();
					break;

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
			appLog(Info) << "Parse successfuly done!" << _line;
		}
		catch (const std::exception& ex)
		{
			appLog(Warning) << "Possible problem on line "  << _line << ", position " << _wordPos << ".";
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