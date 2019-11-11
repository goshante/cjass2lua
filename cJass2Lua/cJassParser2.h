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
		std::string				_operator;
		size_t					_index;
		GlobalSpace				_rootNode;
		Node*					_activeNode;
		Node*					_lastAddedNode;
		bool					_unitIsComplete;

		Node* _addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive = false);
		Node* _addNode(std::shared_ptr<cJass::Node> node, bool makeActive = false);
		void _pop();
		size_t _depth() const;

	public:
		Parser2(csref_t text, OutputInterface::Type outputType, OutputInterface::NewLineType nlType, void* outputPtr = nullptr);

		void Parse();
		void ToLua();
	};
}