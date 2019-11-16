#pragma once

#include <string>
#include <vector>
#include <list>
#include <stack>

#include "Logger.h"

#pragma warning(disable : 4005)
#pragma warning(disable : 4834)


#define APP_VER		"1.02"
#ifdef _WIN64
#define APP_BUILD	"x64"
#else
#define APP_BUILD	"x86"
#endif

#define min(a,b) ((a < b) ? a : b)
#define max(a,b) ((a > b) ? a : b)

namespace cJass
{
	using csref_t = const ::std::string&;

	class Node;
	using NodePtr = ::std::shared_ptr<Node>;
	using NodeList = ::std::vector<NodePtr>;
	using NL_iter = NodeList::iterator;

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
		opEnd,	// ;
		esc		//Backslash
	};

	enum class word_t
	{
		undefined,
		nl,
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
		blockBegin,
		blockEnd,
		ignore
	};

	enum class ParseSpecialSubject
	{
		none,
		globals,
		defs,
		func,
		call
	};
}