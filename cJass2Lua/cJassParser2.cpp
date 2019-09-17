#include "cJassParser2.h"
#include "reutils.h"

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

	ctype_t classifyChar(char c)
	{
		std::string str = "?";
		str[0] = c;
		
		if (reu::IsMatching(str, "[\\s\\t]"))
			return ctype_t::emp;
		else if (reu::IsMatching(str, "[\\n]"))
			return ctype_t::nl;
		else if (reu::IsMatching(str, "[\\r]"))
			return ctype_t::nlr;
		else if (reu::IsMatching(str, "[a-zA-Z_]"))
			return ctype_t::lit;
		else if (reu::IsMatching(str, "[0-9]"))
			return ctype_t::dig;
		else if (reu::IsMatching(str, "[\\\\\\/\\+\\-\\=\\*\\!\\<\\>\\^\\&\\|\\~\\%]"))
			return ctype_t::oper;
		else if (c == '.')
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

		return ctype_t::unk;
	}

	Parser2::Parser2(csref_t text, OutputInterface::Type outputType, void* outputPtr)
		: _text(text)
		, _word("")
		, _operator("")
		, _index(0)
		, _rootNode(outputType, outputPtr)
		, _activeNode(&_rootNode)
		, _unitIsComplete(false)
	{
		_subjectStack.push(ParseTag2_t::none);
	}

	Node* Parser2::_addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive)
	{
		auto node = Node::Produce(type, _activeNode);
		node->InitData(data);
		_activeNode->AddSubnode(node);
		if (makeActive)
		{
			_activeNode = _activeNode->LastSubnode();
			return _activeNode;
		}

		return _activeNode->LastSubnode();
	}

	void Parser2::Parse()
	{
		char c;
		char prev = '\0';
		char next = '\0';
		bool isString = false;
		bool isLineComment = false;
		bool isMultilineComment = false;
		bool isRawCode = false;
		bool nextArg = false;
		bool isIndex = false;
		size_t line = 0;
		size_t depth = 0;
		std::string prevWord;
		size_t len = _text.length();
		ctype_t ctPrev = ctype_t::undefined, ct;
		std::vector<std::string> vec_arg;
		
		int parseSteps = 0;

		auto parseWord = [this, &prevWord, &parseSteps, &vec_arg, ctPrev, &isString, &isLineComment,
			&isMultilineComment, &isRawCode, &depth, line](ctype_t explicitParse = ctype_t::undefined) -> void
		{
			static bool varInit = false;
			static size_t ignoreLine = ~0;

			if (_word == "")
			{
				switch (explicitParse)
				{
				case ctype_t::aEnd:
					_word = ")";
					break;

				case ctype_t::bEnd:
					_word = "}";
					break;

				case ctype_t::iEnd:
					_word = "]";
					break;

				default:
					return;
				}
			}

			if (ignoreLine == line)
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

			if (_word == "=")
			{
				varInit = true;
				goto parseWordEnd;
			}

			if (isString)
				_word = "\"" + _word + "\"";

			if (isRawCode)
				_word = "'" + _word + "'";

			if (isLineComment || isMultilineComment)
			{
				_addNode(Node::Type::Comment, { _word });
				goto parseWordEnd;
			}

			if (depth == 0)
			{
				if (_word == "library" || _word == "endlibrary")
					ignoreLine = line;

				if (_subjectStack.top() == ParseTag2_t::globals)
				{
					if (_word == "endglobals")
					{
						_subjectStack.pop();
						goto parseWordEnd;
					}

					if (vec_arg.size() == 2)
					{
						if (varInit)
						{
							vec_arg.push_back(_word);
							_rootNode.InitData(vec_arg);
							vec_arg.clear();
							varInit = false;
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
					else
						vec_arg.push_back(_word);

				}
				else if (_subjectStack.top() == ParseTag2_t::defs)
				{
					if (_word == "enddefine")
					{
						_subjectStack.pop();
						goto parseWordEnd;
					}

					if (vec_arg.size() == 0)
						vec_arg.push_back("");

					vec_arg.push_back(_word);

					if (varInit)
					{
						_rootNode.InitData(vec_arg);
						vec_arg.clear();
						varInit = false;
						goto parseWordEnd;
					}
				}
				else if (_subjectStack.top() == ParseTag2_t::func)
				{
					_func:
					if (_word == ")")
					{
						_addNode(Node::Type::Function, vec_arg, true);
						_subjectStack.pop();
						vec_arg.clear();
						depth++;
						goto parseWordEnd;
					}

					vec_arg.push_back(_word);
				}
				else if (_subjectStack.top() == ParseTag2_t::none)
				{
					if (_word == "globals")
						_subjectStack.push(ParseTag2_t::globals);
					else if (_word == "define")
						_subjectStack.push(ParseTag2_t::defs);
					else
					{
						_subjectStack.push(ParseTag2_t::func);
						goto _func;
					}
				}
			}
			else
			{
				//ToDo
			}

			parseWordEnd:
			prevWord = _word;
			_word = "";
		};

		for (size_t i = 0; i < len; i++)
		{
			c = _text[i];
			ct = classifyChar(c);
			nextArg = false;
			if ((i+1) < len)
				next = _text[i + 1];
			else
				next = '\0';

			if (i > 0)
				prev = _text[i - 1];
			else
				prev = '\0';

			if (ct == ctype_t::nl)
				line++;

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
			else if (isLineComment && (c == '\r' || next == '\n'))
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
				_word.push_back(c);
				break;

			case ctype_t::str:
				parseWord();
				isString = true;
				break;

			case ctype_t::opEnd:
				parseWord();
				break;

			case ctype_t::nl:
			case ctype_t::nlr:
			case ctype_t::unk:
			case ctype_t::emp:
				parseWord();
				break;

			case ctype_t::lit:
				if (!reu::IsMatching(_word, "^[^0-9][a-zA-Z0-9_]*$"))
					parseWord();
				_word.push_back(c);
				break;

			case ctype_t::dig:
				if (!reu::IsMatching(_word, "^[^0-9][a-zA-Z0-9_]*$") && !reu::IsMatching(_word, "^[0-9]+\\.?[0-9]*$"))
					parseWord();
				_word.push_back(c);
				break;

			case ctype_t::dot:
				_word.push_back(c);
				break;

			case ctype_t::com:
				nextArg = true;
				break;
			
			case ctype_t::raw:
				parseWord();
				isRawCode = true;
				break;

			case ctype_t::bBeg:
			case ctype_t::bEnd:
			case ctype_t::aBeg:
			case ctype_t::aEnd:
			case ctype_t::iBeg:
			case ctype_t::iEnd:
				parseWord();
				parseWord(ct);
				break;
			}

			ctPrev = ct;
		}
	}

	void Parser2::ToLua()
	{

	}
} //namespace cJass