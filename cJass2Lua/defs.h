#pragma once

#include <string>
#include <vector>
#include <list>
#include <stack>

#define min(a,b) ((a < b) ? a : b)
#define max(a,b) ((a > b) ? a : b)

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
		none,
		end,
		ignore,
		globals,
		defs,
		func,
		lambda,
		operation,
		statement,
		loop
	};

	enum class ParseTag2_t
	{
		none,
		globals,
		defs,
		func,
		lambda,
		ret
	};

	struct ParseResult_t
	{
		ParseTag_t tag;
		size_t parseEnd;
	};

	using tagstack_t = std::stack<ParseTag_t>;
}