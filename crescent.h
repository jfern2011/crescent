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

	inline std::string trim_path(const std::string& path)
	{
		std::vector< std::string > tokens;
		Util::split(path, tokens, "/");

		std::string ans =
			Util::build_string(tokens, "/");

		return Util::trim(ans);
	}
}
