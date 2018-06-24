#pragma once

#include <cstdint>
#include <memory>

#include "abort.h"
#include "str_util.h"
#include "util.h"

using int64 = std::int64_t;

template <typename T>
using Handle = std::shared_ptr<T>;

namespace Util
{
	/**
	 * Returns \a base raised to the integral power \a exp. Removed
	 * from the standard library as of C++11
	 *
	 * @param[in] base The base value
	 * @param[in] exp  Exponent value
	 *
	 * @return The result of raising \a base to the power of
	 *         \a exp
	 */
	inline double pow(double base, int exp)
	{
		double ans = base;

		if (exp == 0) return 1.0;

		const int nexp = std::abs(exp);

		for (int i = 1; i < nexp; i++)
			ans *= base;

		if (exp < 0)
			return 1.0 / ans;
		return ans;
	}

	/**
	 * Read an input configuration file. Removes comments and filters
	 * out empty (whitespace only) lines, leaving only lines with
	 * actual information
	 *
	 * @param[in]  filename The file to read
	 * @param[out] lines    All lines in the file, excluding comments
	 *                      and pure whitespace
	 *
	 * @return True on success
	 */
	inline bool read_config(const std::string& filename,
							std::vector<std::string>& lines)
	{
		std::vector<std::string> temp;

		AbortIfNot_2(Util::readlines(filename, temp),
			false);

		lines.clear();

		for (auto iter = temp.begin(), end = temp.end();
			 iter != end; ++iter)
		{
			*iter =
				Util::trim(iter->substr(0, iter->find('#')));

			if (!iter->empty()) lines.push_back(*iter);
		}

		return true;
	}

	/**
	 * Replace all occurences of a character in a string with a new
	 * character
	 *
	 * @param[in] str   The input string
	 * @param[in] old   The character to replace in the string
	 * @param[in] young The character replacing \a old
	 *
	 * @return The new string
	 */
	inline std::string strrep(const std::string& str, char old,
		char young)
	{
		std::string ans = str;
		for (size_t i = 0; i < str.size(); i++)
		{
			if (ans[i] == old)  ans[i] = young;
		}

		return ans;
	}

	/**
	 * Remove leading and trailing '/' from a directory path. Used
	 * primarily by the SharedData implementation
	 *
	 * @param[in] path The path to trim
	 *
	 * @return The trimmed path
	 */
	inline std::string trim_path(const std::string& path)
	{
		std::vector< std::string > tokens;
		Util::split(path, tokens, "/");

		std::string ans =
			Util::build_string(tokens, "/");

		return Util::trim(ans);
	}
}
