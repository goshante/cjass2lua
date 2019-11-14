#pragma once
#include <string>
#include <vector>
#include <set>
#include "cJassNodes.h"
#include "defs.h"
#include <stack>

namespace cJass
{
	class Parser2
	{
	private:
		std::string				_text;
		std::string				_word;
		GlobalSpace				_rootNode;
		Node*					_activeNode;
		Node*					_lastAddedNode;
		bool					_unitIsComplete;
		size_t					_line;
		size_t					_wordPos;
		std::string				_fileName;
		bool					_strictMode;

		Node* _addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive = false);
		Node* _addNode(std::shared_ptr<cJass::Node> node, bool makeActive = false);
		void _pop();
		size_t _depth() const;

	public:
		Parser2(OutputInterface::Type outputType, OutputInterface::NewLineType nlType, std::string& fileNameOrString);

		void Parse(csref_t text, csref_t cjassFileName);
		void ToLua();
	};
}