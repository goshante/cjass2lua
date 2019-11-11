#pragma once

#include <string>
#include <vector>
#include <set>
#include "cJassNodes.h"
#include "defs.h"

namespace cJass
{
	class Parser
	{
	private:
		std::string _text;
		bool _typenameAssigning;
		int _depth;
		size_t _index;
		ranges_t _stringRanges;
		ranges_t _commentRanges;
		std::set<size_t> _parsedComments;
		GlobalSpace _rootNode;
		Node* _activeNode;
		tagstack_t _tags;
		ParseTag_t _lastBlock;

		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		std::string _getCommentByRandomIndex(size_t i);
		bool _isCommentParsed(size_t i);

		void _findRanges();
		void _removeJunk();
		line_t _nextLine(size_t off);
		std::string _nextPart();
		ParseResult_t _parseLine(line_t& line);
		line_t _normalizeLine(const std::string& s, size_t begin, size_t end);
		Node* _addNode(cJass::Node::Type type, const std::vector<std::string> data);

		bool _isComment(size_t i);
		bool _isString(size_t i);
		bool _checkTag(ParseTag_t t);
		void _touchTag(ParseTag_t t);

	public:
		Parser(csref_t text, OutputInterface::Type outputType, OutputInterface::NewLineType nlType = OutputInterface::NewLineType::LF, void* outputPtr = nullptr);

		void Parse();
		void ToLua();
	};
}