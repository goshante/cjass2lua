#pragma once

#include <string>
#include <vector>

namespace reu
{
	void ReplaceStrIndexRange(std::string& str, const std::string& replacingStr, size_t begin, size_t end);
	std::string IndexSubstr(const std::string& str, size_t begin, size_t end);

	struct range_t
	{
		size_t begin;
		size_t end;
		range_t() : begin(0), end(0) {}
		range_t(size_t b, size_t e) : begin(b), end(e) {}
		range_t(const std::pair<size_t, size_t>& p) : begin(p.first), end(p.second) {}
		range_t(const range_t& r) : begin(r.begin), end(r.end) {}
		range_t& operator=(const range_t& r)
		{
			begin = r.begin;
			end = r.end;
			return *this;
		}
		range_t& operator=(const std::pair<size_t, size_t>& p)
		{
			begin = p.first;
			end = p.second;
			return *this;
		}
		size_t len() { return ((end - begin) + 1); }
		bool isInRange(size_t n) { if (n >= begin && n <= end) { return true; } return false; }
	};

	using pair_t = std::pair<size_t, size_t>;
	using ranges_t = std::vector<range_t>;

	class match_t
	{
	private:
		size_t _begin;
		size_t _end;
		std::vector<std::string> _groups;
		std::vector<range_t> _granges;
		std::string* _str;

		match_t() = delete;

	public:
		match_t(std::string& str, size_t begin, size_t end,
			const std::vector<std::string>& groups,
			const std::vector<range_t>& granges);
		match_t(const match_t& copy);
		match_t& operator=(const match_t& copy);

		void Replace(const std::string& pattern);

		std::string operator[](size_t i) const;
		size_t Begin() const;
		size_t End() const;
		std::string GetMatchingString() const;
		std::string GetGroup(size_t n) const;
		range_t GetGroupRange(size_t n);
		bool IsMatching() const;
		size_t Length() const;
		std::string PatternExplode(const std::string& pattern) const;
	};

	class matches_t
	{
		friend matches_t SearchAll(std::string& str, const std::string re, size_t offset);

	private:
		std::string* _str;
		std::vector<match_t> _mvec;

	protected:
		void AddMatch(const match_t& m);
		matches_t& operator+=(const match_t& m);

	public:

		matches_t(std::string& strref);
		matches_t(const matches_t& copy);
		matches_t& operator=(const matches_t& copy);
		match_t& operator[](size_t i);

		void ReplaceAll(const std::string& pattern);
		void ExcludeIndexRange(range_t range);
		void ExcludeIndexRanges(ranges_t& ranges);

		std::vector<match_t>::iterator begin();
		std::vector<match_t>::iterator end();

		match_t Get(size_t i) const;
		size_t Count() const;
		bool HasMatches() const;
	};

	match_t Search(std::string& str, const std::string re, size_t offset = 0);
	matches_t SearchAll(std::string& str, const std::string re, size_t offset = 0);
	bool IsMatching(const std::string& str, std::string re);
	size_t ReplaceAll(std::string& str, const std::string& re, const std::string& pattern);
	bool RangeCheck(const ranges_t& ranges, size_t n);
}