#pragma once

#include <string>
#include <vector>
#include <list>
#include <stack>

namespace cJass
{
	using csref_t = const std::string&;
	using pair_t = std::pair<size_t, size_t>;
	using ranges_t = std::vector<pair_t>;

	struct line_t
	{
		std::string line;
		size_t begin;
		size_t end;
	};

	class Node;
	using NodePtr = std::shared_ptr<Node>;
	using NodeList = std::list<NodePtr>;
	using NL_iter = NodeList::iterator;

	enum class ParseTag_t
	{
		end,
		ignore,
		globals,
		defs,
		func,
		lambda,
		operand,
		Operator,
		caller,
		declaration
	};

	struct ParseResult_t
	{
		ParseTag_t tag;
		size_t parseEnd;
	};

	using tagstack_t = std::stack<ParseTag_t>;
}