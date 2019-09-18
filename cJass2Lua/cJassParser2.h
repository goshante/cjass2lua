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
		std::stack<ParseTag2_t>	_subjectStack;
		GlobalSpace				_rootNode;
		Node*					_activeNode;
		bool					_unitIsComplete;

		Node* _addNode(cJass::Node::Type type, const std::vector<std::string> data, bool makeActive = false);
		void _pop();

	public:
		Parser2(csref_t text, OutputInterface::Type outputType, void* outputPtr = nullptr);

		void Parse();
		void ToLua();
	};
}