#include "reutils.h"
#include <regex>
#include <stdexcept>

namespace reu
{
	struct rf_t
	{
		std::string re;
		std::regex_constants::syntax_option_type flags;
	};

	std::string IndexSubstr(const std::string& str, size_t begin, size_t end)
	{
		return str.substr(begin, end - begin + 1);
	}

	rf_t SeparateFlags(const std::string re)
	{
		size_t i = re.find('/');
		if (i == std::string::npos || (i > 0 && re[i - 1] == '\\'))
			return { re, static_cast<std::regex::flag_type>(0) };

		rf_t rf;
		std::string flags = IndexSubstr(re, i + 1, re.length() - 1);
		rf.re = re.substr(0, i);
		rf.flags = static_cast<std::regex::flag_type>(0);

		for (auto c : flags)
		{
			switch (c)
			{
			case 'E':
				rf.flags |= std::regex_constants::ECMAScript;
				break;
			case 'r':
				rf.flags |= std::regex_constants::grep;
				break;
			case 'i':
				rf.flags |= std::regex_constants::icase;
				break;
			case 'x':
				rf.flags |= std::regex_constants::extended;
				break;
			case 'a':
				rf.flags |= std::regex_constants::awk;
				break;
			case 'b':
				rf.flags |= std::regex_constants::basic;
				break;
			case 'e':
				rf.flags |= std::regex_constants::egrep;
				break;
			case 'c':
				rf.flags |= std::regex_constants::collate;
				break;
			case 'o':
				rf.flags |= std::regex_constants::optimize;
				break;
			case 'n':
				rf.flags |= std::regex_constants::nosubs;
				break;
			default:
				continue;
			}
		}

		return rf;
	}

	void ReplaceStrIndexRange(std::string& str, const std::string& replacingStr, size_t begin, size_t end)
	{
		size_t len = str.length();
		if (len < 2)
			return;

		if (begin > end)
		{
			size_t t;
			t = begin;
			begin = end;
			end = t;
		}

		if (begin < 0 || end >= len)
			throw std::runtime_error("ReUtils::ReplaceStrIndexRange - index out of range.");

		if (begin == end)
		{
			if (begin != 0)
				begin--;
			else
				end++;
		}

		std::string pre, post;
		pre = str.substr(0, begin);
		post = str.substr(end + 1, len - end);
		str = pre + replacingStr + post;
	}

	void PatternReplace(std::string& str, size_t begin, size_t end, const std::string pattern, const std::vector<std::string>& groups)
	{
		std::string insertion;
		auto isInt = [](const std::string& str) -> bool
		{
			std::string nums = "0123456789";

			for (size_t i = 0; i < str.length(); i++)
			{
				if (nums.find(str[i]) == std::string::npos)
					return false;
			}

			return true;
		};

		auto atou = [](const char* a) -> size_t
		{
			size_t u = 0;
			size_t t(1);

			for (size_t i = strlen(a) - 1; i >= 0; --i)
			{
				if (a[i] >= '0' && a[i] <= '9')
				{
					u += (a[i] - '0') * t;
					t *= 10;
				}
			}

			return u;
		};

		bool parsingFlag = false;
		bool bracketOpened = false;
		std::string sNum;
		size_t iNum;
		for (size_t i = 0; i < pattern.size(); i++)
		{
			if (!parsingFlag)
			{
				if (bracketOpened)
				{
					if (isInt(sNum))
					{
						iNum = atou(sNum.c_str());

						if (iNum == 0)
							insertion += IndexSubstr(str, begin, end);
						else
							insertion += groups[iNum - 1];
					}
					sNum = "";
					bracketOpened = false;
				}
				if ((pattern[i] != '$') || (pattern[i] == '$' && i > 0 && pattern[i - 1] == '\\'))
					insertion.push_back(pattern[i]);
				else
					parsingFlag = true;
			}
			else
			{
				if (pattern[i - 1] == '$' && pattern[i] == '[' && !bracketOpened)
				{
					bracketOpened = true;
					sNum = "";
					continue;
				}

				if (!bracketOpened)
				{
					parsingFlag = false;
					bracketOpened = false;
					sNum.push_back(pattern[i]);
				}
				else
				{
					if (pattern[i] == ']')
						parsingFlag = false;
					else
						sNum.push_back(pattern[i]);
				}
			}
		}

		ReplaceStrIndexRange(str, insertion, begin, end);
	}

	//match_t
	match_t::match_t(std::string& str, size_t begin, size_t end, const std::vector<std::string>& groups, const std::vector<range_t>& granges)
		: _str(&str)
		, _begin(begin)
		, _end(end)
		, _groups(groups)
		, _granges(granges)
	{
	}

	match_t::match_t(const match_t& copy)
		: _str(copy._str)
		, _begin(copy._begin)
		, _end(copy._end)
		, _groups(copy._groups)
		, _granges(copy._granges)
	{
	}

	match_t& match_t::operator=(const match_t& copy)
	{
		_str = copy._str;
		_begin = copy._begin;
		_end = copy._end;
		_groups = copy._groups;
		_granges = copy._granges;
		return *this;
	}

	void match_t::Replace(const std::string& pattern)
	{
		if (IsMatching())
			PatternReplace(*_str, _begin, _end, pattern, _groups);
	}

	std::string match_t::operator[](size_t i) const
	{
		return GetGroup(i);
	}

	size_t match_t::Begin() const
	{
		return _begin;
	}

	size_t match_t::End() const
	{
		return _end;
	}

	std::string match_t::GetMatchingString() const
	{
		if (!IsMatching())
			return "";

		return IndexSubstr(*_str, _begin, _end);
	}

	std::string match_t::GetGroup(size_t n) const
	{
		if (n == 0)
			return GetMatchingString();
		if (n > _groups.size())
			return "";
		return _groups[n - 1];
	}

	range_t match_t::GetGroupRange(size_t n)
	{
		if (n == 0)
			return { _begin, _end };

		if (n > _granges.size())
			return { std::string::npos, std::string::npos };

		return _granges[n - 1];
	}

	bool match_t::IsMatching() const
	{
		return (_begin != std::string::npos);
	}

	size_t match_t::Length() const
	{
		return ((_end - _begin) + 1);
	}

	std::string match_t::PatternExplode(const std::string& pattern) const
	{
		std::string insertion;
		auto isInt = [](const std::string& str) -> bool
		{
			std::string nums = "0123456789";

			for (size_t i = 0; i < str.length(); i++)
			{
				if (nums.find(str[i]) == std::string::npos)
					return false;
			}

			return true;
		};

		auto atou = [](const char* a) -> size_t
		{
			size_t u = 0;
			size_t t(1);

			for (size_t i = strlen(a) - 1; i >= 0; --i)
			{
				if (a[i] >= '0' && a[i] <= '9')
				{
					u += (a[i] - '0') * t;
					t *= 10;
				}
			}

			return u;
		};

		bool parsingFlag = false;
		bool bracketOpened = false;
		std::string sNum;
		size_t iNum;
		for (size_t i = 0; i < pattern.size(); i++)
		{
			if (!parsingFlag)
			{
				if (bracketOpened)
				{
					if (isInt(sNum))
					{
						iNum = atou(sNum.c_str());

						if (iNum == 0)
							insertion += IndexSubstr(*_str, _begin, _end);
						else
							insertion += _groups[iNum - 1];
					}
					sNum = "";
					bracketOpened = false;
				}
				if ((pattern[i] != '$') || (pattern[i] == '$' && i > 0 && pattern[i - 1] == '\\'))
					insertion.push_back(pattern[i]);
				else
					parsingFlag = true;
			}
			else
			{
				if (pattern[i - 1] == '$' && pattern[i] == '[' && !bracketOpened)
				{
					bracketOpened = true;
					sNum = "";
					continue;
				}

				if (!bracketOpened)
				{
					parsingFlag = false;
					bracketOpened = false;
					sNum.push_back(pattern[i]);
				}
				else
				{
					if (pattern[i] == ']')
						parsingFlag = false;
					else
						sNum.push_back(pattern[i]);
				}
			}
		}

		return insertion;
	}

	//matches_t
	matches_t::matches_t(std::string& strref)
		: _str(&strref)
	{
	}

	matches_t::matches_t(const matches_t& copy)
		: _str(copy._str)
		, _mvec(copy._mvec)

	{
	}

	matches_t& matches_t::operator=(const matches_t& copy)
	{
		_mvec = copy._mvec;
		_str = copy._str;
		return *this;
	}

	matches_t& matches_t::operator+=(const match_t& m)
	{
		_mvec.push_back(m);
		return *this;
	}

	match_t& matches_t::operator[](size_t i)
	{
		return _mvec.at(i);
	}

	void matches_t::AddMatch(const match_t& m)
	{
		_mvec.push_back(m);
	}

	void matches_t::ReplaceAll(const std::string& pattern)
	{
		//this function is broken and must be re-implemented
	}

	void matches_t::ExcludeIndexRange(range_t range)
	{
		for (auto it = _mvec.begin(); it != _mvec.end(); it++)
		{
			auto& m = *it;
			if (range.isInRange(m.Begin()) || range.isInRange(m.End()))
			{
				_mvec.erase(it);
				it = _mvec.begin();
			}
		}
	}

	void matches_t::ExcludeIndexRanges(ranges_t& ranges)
	{
		for (auto& r : ranges)
			ExcludeIndexRange(r);
	}

	std::vector<match_t>::iterator matches_t::begin()
	{
		return _mvec.begin();
	}

	std::vector<match_t>::iterator matches_t::end()
	{
		return _mvec.end();
	}

	match_t matches_t::Get(size_t i) const
	{
		return _mvec.at(i);
	}

	size_t matches_t::Count() const
	{
		return _mvec.size();
	}

	bool matches_t::HasMatches() const
	{
		if (_mvec.size() == 0)
			return false;
		return true;
	}

	//Utils
	match_t Search(std::string& str, const std::string re, size_t offset)
	{
		std::string a;
		rf_t rf = SeparateFlags(re);
		std::regex exp(rf.re, rf.flags);
		std::smatch sm;
		std::vector<std::string> groups;
		std::vector<range_t> ranges;
		size_t begin = std::string::npos;
		size_t end = std::string::npos;
		std::string offstr = "";
		std::string* pstr = &str;



		auto countGroupRange = [](const std::smatch::value_type& g, const std::string& str, size_t offset) -> range_t
		{
			size_t i = 0;
			int n = 0;
			range_t range = { std::string::npos, std::string::npos };
			for (auto it = str.begin(); it != str.end(); it++)
			{
				if (it == g.first)
				{
					range.begin = i;
					n++;
				}

				if (it == g.second)
				{
					range.end = i;
					n++;
				}

				if (n == 2)
					break;
				i++;
			}

			if (range.begin != std::string::npos && range.end == std::string::npos)
				range.end = str.length() - 1;
			else
				range.end--;

			if (offset > 0)
			{
				range.begin += offset;
				range.end += offset;
			}

			return range;
		};

		if (offset > 0)
		{
			offstr = str.substr(offset, str.length() - offset);
			pstr = &offstr;
		}

		if (offset >= str.length())
			return match_t(str, begin, end, groups, ranges);

		if (std::regex_search(*pstr, sm, exp))
		{
			for (size_t i = 0; i < sm.size(); i++)
			{
				if (i > 0)
				{
					groups.push_back(sm[i]);
					ranges.push_back(countGroupRange(sm[i], *pstr, offset));
				}
				else
				{
					auto range = countGroupRange(sm[i], *pstr, offset);
					begin = range.begin;
					end = range.end;
				}
			}

		}

		return match_t(str, begin, end, groups, ranges);
	}

	matches_t SearchAll(std::string& str, const std::string re, size_t offset)
	{
		matches_t ms(str);
		match_t m = Search(str, re, offset);

		if (!m.IsMatching())
			return ms;

		do
		{
			ms += m;
			offset = m.End() + 1;
			m = Search(str, re, offset);
		} while (m.IsMatching());

		return ms;
	}

	bool IsMatching(const std::string& str, std::string re)
	{
		rf_t rf = SeparateFlags(re);
		std::regex exp(rf.re, rf.flags);
		return std::regex_match(str, exp);
	}

	size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern)
	{
		size_t off = 0;
		size_t replaces = 0;
		match_t m = Search(str, re, off);

		while (m.IsMatching())
		{
			m.Replace(pattern);
			m = Search(str, re, off);
			replaces++;
			off = m.End() + 1;
		}

		return replaces;
	}

	bool RangeCheck(const ranges_t& ranges, size_t n)
	{
		for (auto r : ranges)
		{
			if (r.isInRange(n))
				return true;
		}

		return false;
	}

} //namespace reu