#include "cJassParser.h"
#include "reutils.h"

namespace cJass
{
	Parser::Parser(csref_t text, OutputInterface::Type outputType, void* outputPtr)
		: _text(text)
		, _typenameAssigning(false)
		, _depth(0)
		, _index(0)
		, _rootNode(outputType, outputPtr)
		, _activeNode(nullptr)
		, _lastBlock(ParseTag_t::globals)
	{
		_tags.emplace(ParseTag_t::none);
	}

	bool isIgnoreCharacter(char c)
	{
		if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
			return true;
		return false;
	}

	size_t next_ic(csref_t s, size_t off, bool inverse = false)
	{
		if (off >= s.length() - 1)
			return off;
		for (size_t i = off; i < s.length(); i++)
		{
			bool b = isIgnoreCharacter(s[i]);
			if (b != inverse)
				return i;
		}
		return off;
	}

	std::string Parser::_nextPart()
	{
		size_t len = _text.length();
		size_t end = next_ic(_text, _index + 1);
		if (end > len - 1)
			end--;
		std::string part = reu::IndexSubstr(_text, _index, end);

		_index = end;

		return part;
	}

	size_t NextLineStart(csref_t s, size_t n)
	{
		size_t last = s.length() - 1;
		if (n >= last)
			return n;
		if (n == last - 1 && (s[last] == '\r' || s[last] == '\n'))
			return n;

		for (size_t i = n + 1; i <= last; i++)
		{
			if (s[i] != '\r' && s[i] != '\n')
				return i;
		}

		return n;
	}

	size_t NextLineEnd(csref_t s, size_t off)
	{
		size_t n = s.length() - 1;
		if (off == n)
			return n;

		for (size_t i = off; i < n; i++)
		{
			if (s[i] == '\r' || s[i] == '\n')
				return i;
		}

		return n;
	}

	bool IsLineEmpty(csref_t s)
	{
		if (s.length() == 0)
			return true;

		for (auto c : s)
		{
			if (c != '\n' && c != '\r' && c != ' ' && c != '\t')
				return false;
		}

		return true;
	}

	std::string Parser::_getCommentByRandomIndex(size_t i)
	{
		size_t n = 0;
		while (n < _commentRanges.size())
		{
			auto& p = _commentRanges[n];
			if (i >= p.first && i <= p.second)
				break;
			n++;
		}
		_parsedComments.insert(n);
		return reu::IndexSubstr(_text, _commentRanges[n].first, _commentRanges[n].second);
	}

	bool Parser::_isCommentParsed(size_t i)
	{
		size_t n = 0;
		while (n < _commentRanges.size())
		{
			auto& p = _commentRanges[n];
			if (i >= p.first && i <= p.second)
				break;
			n++;
		}

		if (_parsedComments.find(n) != _parsedComments.end())
			return true;
		return false;
	}

	line_t Parser::_normalizeLine(const std::string& s, size_t begin, size_t end)
	{
		line_t line =  {"", 0, 0};
		size_t j = begin;
		for (size_t i = 0; i <= s.length() - 1; i++)
		{
			if (_isComment(j))
			{
				if (!_isCommentParsed(j))
					_addNode(Node::Type::Comment, { _getCommentByRandomIndex(j) });
			}
			else
				line.line.push_back(s[i]);
			j++;
		}

		size_t i = 0;
		while (i < line.line.length())
		{
			if (line.line[i] != ' ' && line.line[i] != '\t' && line.line[i] != '{' && line.line[i] != ';')
				break;
			i++;
			line.begin++;
		}
		line.line = reu::IndexSubstr(line.line, i, line.line.length() - 1);

		i = 0;
		while (i < line.line.length())
		{
			if (line.line[i] == '{' && line.line[i] != ';')
				line.line[i] = ' ';
			i++;
		}

		i = line.line.length() - 1;
		while (true)
		{
			if (line.line[i] != ' ' && line.line[i] != '\t')
				break;
			if (i == 0)
				break;
			i--;
			line.end++;
		}
		line.line = reu::IndexSubstr(line.line, 0, i);

		return line;
	}

	line_t Parser::_nextLine(size_t off)
	{
		size_t begin = NextLineStart(_text, off);
		if (begin >= _text.length() - 1)
			return { "", std::string::npos, std::string::npos };
		if (off == 0)
			begin = 0;
		size_t end = NextLineEnd(_text, begin);
		if (end != _text.length() - 1)
			end--;
		std::string str = reu::IndexSubstr(_text, begin, end);
		if (IsLineEmpty(str) && end != _text.length() - 1)
			return _nextLine(end);
		else
		{
			auto line = _normalizeLine(str, begin, end);
			line.begin += begin;
			line.end = end - line.end;
			return line;
		}
	}

	void Parser::_findRanges()
	{
		bool lcomment = false;
		bool mcomment = false;
		bool str = false;
		size_t end = _text.length() - 1;
		pair_t pair = { std::string::npos, std::string::npos };
		for (size_t i = 0; i <= end; i++)
		{
			if (lcomment)
			{
				if (_text[i] == '\r' || _text[i] == '\n')
				{
					pair.second = i-1;
					_commentRanges.push_back(pair);
					pair = { std::string::npos, std::string::npos };
					lcomment = false;
				}
				continue;
			}

			if (mcomment)
			{
				if (i != end && _text[i] == '*' && _text[i+1] == '/')
				{
					pair.second = i + 1;
					_commentRanges.push_back(pair);
					pair = { std::string::npos, std::string::npos };
					mcomment = false;
					i++;
				}
				continue;
			}

			if (str)
			{
				if (_text[i] == '"')
				{
					pair.second = i - 1;
					_stringRanges.push_back(pair);
					pair = { std::string::npos, std::string::npos };
					str = false;
					i++;
				}
				continue;
			}

			if (_text[i] == '/')
			{
				if (i != end && _text[i + 1] == '/')
				{
					pair.first = i;
					lcomment = true;
					i++;
				}
				else if (i != end && _text[i + 1] == '*')
				{
					pair.first = i;
					mcomment = true;
					i++;
				}
			}
			else if (_text[i] == '"')
			{
				pair.first = i;
				str = true;
				i++;
			}
		}
	}

	Node* Parser::_addNode(cJass::Node::Type type, const std::vector<std::string> data)
	{
		auto node = Node::Produce(type, _activeNode);
		node->InitData(data);
		_activeNode->AddSubnode(node);
		return _activeNode->LastSubnode();
	}

	bool Parser::_isComment(size_t i)
	{
		for (auto& p : _commentRanges)
		{
			if (i >= p.first && i <= p.second)
				return true;
		}
		return false;
	}

	bool Parser::_isString(size_t i)
	{
		for (auto& p : _stringRanges)
		{
			if (i >= p.first && i <= p.second)
				return true;
		}
		return false;
	}

	void Parser::_removeJunk()
	{
		auto matches = reu::SearchAll(_text, "\\{"); // also add |\\; but not needed
		matches.ExcludeIndexRanges(_stringRanges);
		matches.ExcludeIndexRanges(_commentRanges);
		matches.ReplaceAll("");
	}

	void Parser::_touchTag(ParseTag_t t)
	{
		if (t == ParseTag_t::ignore)
			return;

		if (t == ParseTag_t::end)
		{
			auto top = _tags.top();
			if (top != ParseTag_t::defs && top != ParseTag_t::globals)
			{
				_activeNode = _activeNode->Top();
				_depth--;
			}
			_tags.pop();
			return;
		}

		_tags.push(t);

		if (t != ParseTag_t::defs && t != ParseTag_t::globals)
		{
			_activeNode = _activeNode->LastSubnode();
			_depth++;
		}
	}

	bool Parser::_checkTag(ParseTag_t t)
	{
		return _tags.top() == t;
	}

	std::vector<std::string> decomposeFunctionArguments(std::string args)
	{
		auto matches = reu::SearchAll(args, "[a-zA-Z0-9_]+");
		std::vector<std::string> vargs;

		for (auto& m : matches)
			vargs.push_back(m.GetMatchingString());

		return vargs;
	}

	ParseResult_t Parser::_parseLine(line_t& line)
	{// \\blambda\\b[\\s\\t]+([a-zA-Z0-9_]+)   - Lambda expression
		std::string firstWord = reu::Search(line.line, "^\\w+").GetMatchingString();
		size_t closingBracket = reu::Search(line.line, "\\}").Begin();
		size_t closingArgs = reu::Search(line.line, "\\)").Begin();
		static std::vector<std::string> vec_arg;

		if (closingBracket != std::string::npos)
		{
			if (_isString(line.begin + closingBracket) || _isComment(line.begin + closingBracket))
				closingBracket = std::string::npos;
		}

		if (closingArgs != std::string::npos)
		{
			if (_isString(line.begin + closingArgs) || _isComment(line.begin + closingArgs))
				closingArgs = std::string::npos;
		}

		size_t parseEnd = line.end;
		if ((closingBracket != std::string::npos && closingBracket > 0) || (closingArgs != std::string::npos && closingArgs > 0))
			parseEnd = line.begin + min(closingBracket, closingArgs) - 1;

		if (line.line[0] == '}' || line.line[0] == ')')
			return { ParseTag_t::end, parseEnd };

		if (firstWord == "library" || firstWord == "endlibrary")
			return { ParseTag_t::ignore, parseEnd };
		
		if (_depth == 0)	//Global space
		{
			if (!_checkTag(ParseTag_t::defs) && !_checkTag(ParseTag_t::globals) && firstWord != "")
			{
				if (firstWord == "define")
					return { ParseTag_t::defs, parseEnd };
				else if (firstWord == "globals")
					return { ParseTag_t::globals, parseEnd };
			}

			if (_checkTag(ParseTag_t::defs) && firstWord == "enddefine")
					return { ParseTag_t::end, parseEnd };

			if (_checkTag(ParseTag_t::globals) && firstWord == "endglobals")
				return { ParseTag_t::end, parseEnd };

			if (_checkTag(ParseTag_t::defs))
			{
				auto m = reu::Search(line.line, "([a-zA-Z0-9_]+)[\\s\\t]*=[\\s\\t]*(.*)");
				vec_arg.clear();
				vec_arg.push_back("");
				vec_arg.push_back(m[1]);
				vec_arg.push_back(m[2]);
				_rootNode.InitData(vec_arg);
				return { ParseTag_t::ignore, parseEnd };
			}

			if (_checkTag(ParseTag_t::globals))
			{
				auto m = reu::Search(line.line, "^([a-zA-Z0-9_]+)[\\s\\t]+([a-zA-Z0-9_\\[\\]]+)[\\s\\t]*=[\\s\\t]*([^,;]*);?");
				vec_arg.clear();

				if (m.IsMatching())
				{
					vec_arg.push_back(m[1]);
					vec_arg.push_back(m[2]);
					vec_arg.push_back(m[3]);
					_rootNode.InitData(vec_arg);
					return { ParseTag_t::ignore, parseEnd };
				}

				m = reu::Search(line.line, "^([a-zA-Z0-9_]+)[\\s\\t]+([a-zA-Z0-9_\\[\\]]+);?");
				vec_arg.push_back(m[1]);
				vec_arg.push_back(m[2]);
				vec_arg.push_back("");
				_rootNode.InitData(vec_arg);
				return { ParseTag_t::ignore, parseEnd };
			}


			auto m = reu::Search(line.line, "^([a-zA-Z0-9_]+)[\\s\r\\n\\t]+([a-zA-Z0-9_]+)[\\s\\r\\n\\t]*\\(([a-zA-Z0-9_\\s\\r\\n\t\\,]*)\\)");
			bool isFuncDecl = m.IsMatching();

			if (isFuncDecl)
			{
				vec_arg.clear();
				vec_arg.push_back(m[1]);
				vec_arg.push_back(m[2]);
				if (m[3] != "")
				{
					auto argList = decomposeFunctionArguments(m[3]);
					for (auto& a : argList)
						vec_arg.push_back(a);
				}
				_addNode(Node::Type::Function, vec_arg);
				_lastBlock = ParseTag_t::func;
				return { ParseTag_t::func, parseEnd };
			}			
		}
		else
		{
			vec_arg.clear();
			if (line.line[0] == '(')
			{

			}
			else if (line.line[0] == ';')
			{
				return { ParseTag_t::end, parseEnd };
			}
			else if (line.line[0] == '"')
			{
				auto m = reu::Search(line.line, "^\\\"(.*[^\\\\])\\\"", 6);
				std::string s = "\"" + m[1] + "\"";
				parseEnd += 2 + s.length();
				_addNode(Node::Type::OperationUnit, { "a", s });
				return { ParseTag_t::ignore, parseEnd };
			}
			if (firstWord == "return")
			{
				auto nextExpr = reu::Search(line.line, "[^\\s\\t]", 6).Begin();
				parseEnd = nextExpr;
				vec_arg.push_back("r");
				_addNode(Node::Type::OperationUnit, { "r" });
				return { ParseTag_t::operation, parseEnd };
			}
			else if (firstWord == "if")
			{

			}
			else if (firstWord == "elseif")
			{

			}
			else if (firstWord == "else")
			{

			}
			else if (firstWord == "while")
			{

			}
			else if (firstWord == "lambda")
			{

			}
			else
			{
				auto m = reu::Search(line.line, "");
				bool isUnaryOperation = m.IsMatching();

				m = reu::Search(line.line, "");
				bool isDeclaration = m.IsMatching();

				m = reu::Search(line.line, "");
				bool isAssignment = m.IsMatching();

				m = reu::Search(line.line, "");
				bool isCall = m.IsMatching(); //TODO: ^Expressions, operation nodes, lambdas, logical expressions
			}

		}

		return { ParseTag_t::ignore, parseEnd };
	}

	void Parser::Parse()
	{
		line_t line = { "", 0, 0 };
		ParseResult_t parseResult = { ParseTag_t::ignore, 0 };
		_findRanges();
		//_removeJunk();
		_activeNode = _rootNode.Ptr();
		do
		{
			if (parseResult.parseEnd > line.end)
				line.line = reu::IndexSubstr(line.line, parseResult.parseEnd + 1, line.end);
			else
				line = _nextLine(_index);
			if (line.begin == std::string::npos)
				break;
			parseResult = _parseLine(line);
			_touchTag(parseResult.tag);
			_index = parseResult.parseEnd;
		} while (line.end != _text.length() - 1);
		
	}

	void Parser::ToLua()
	{
		_rootNode.ToLua();
	}
} //namespace cJass