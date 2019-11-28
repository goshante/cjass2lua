#pragma once

#include "defs.h"
#include "cJassNodes.h"

#include <string>
#include <vector>
#include <set>
#include <stack>

namespace cJass
{
	class Parser2
	{
	private:
		OutputInterface				_outIf;
		std::string					_text;
		std::string					_word;
		RootNode					_rootNode;
		Node*						_activeNode;
		Node*						_lastAddedNode;
		size_t						_line;
		size_t						_wordPos;
		std::string					_fileName;
		bool						_strictMode;
		std::vector<std::string>	_customTypeNames;

		word_t classifyWord(const std::string& word);
		Node* _addNode(cJass::Node::Type type, const std::vector<std::string> data = {}, bool makeActive = false);
		Node* _addNode(std::shared_ptr<cJass::Node> node, bool makeActive = false);
		void _pop();
		size_t _depth() const;
		bool _isCustomType(const std::string& typeName) const;

	public:
		Parser2();

		void Parse(csref_t cjassFileName, NotifyCallback lineProgressHandler = nullptr);
		void ToLua(csref_t outputFileName, NotifyCallback nodeProgressHandler = nullptr);
	};
}