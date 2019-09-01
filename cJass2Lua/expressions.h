#pragma once

namespace cJass
{
	namespace expr
	{
		constexpr char Function0[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*\\)";
		constexpr char Function1[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function2[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function3[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function4[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function5[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function6[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function7[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function8[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function9[]  = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
		constexpr char Function10[] = "([a-zA-Z0-9_])+\\s*([a-zA-Z0-9_]+)\\s*\\(\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\,\\s*[a-zA-Z0-9]+\\s+([a-zA-Z0-9_]+)\\s*\\)";
	}
}