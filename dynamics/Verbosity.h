#pragma once

#include <cstdio>

/**
 * Verbosity levels
 */
enum
{
	/** Don't print anything */
	quiet,

	/** Print just a little  */
	terse,

	/** Print non-essentials */
	verbose,

	/** Print everything     */
	debug
};

namespace Verbosity
{
	/**
	 * The verbosity level
	 */
	extern int level;

	/**
	 * Check if the verbosity level is quiet
	 *
	 * @return True if quiet
	 */
	inline bool is_quiet()
	{
		return level == quiet;
	}

	/**
	 * Check if the verbosity level is terse
	 *
	 * @return True if terse
	 */
	inline bool is_terse()
	{
		return level == terse;
	}

	/**
	 * Check if verbosity level is verbose
	 *
	 * @return True if verbose
	 */
	inline bool is_verbose()
	{
		return level == verbose;
	}

	/**
	 * Check if the verbosity level is debug
	 *
	 * @return True if debug
	 */
	inline bool is_debug()
	{
		return level == debug;
	}

	/**
	 * Set the verbosity level
	 *
	 * @param[in] _level Set the verbosity to this. If less than
	 *                   quiet, the verbosity is set to quiet;
	 *                   if greater than debug, the verbosity is
	 *                   set to debug
	 */
	inline void set(int _level)
	{
		std::printf("setting verbosity to ");

		level = _level;

		if (level <= quiet) level = quiet;
		if (level >= debug) level = debug;

		switch (level)
		{
		case quiet:
			std::printf("quiet.  \n");
			break;
		case terse:
			std::printf("terse.  \n");
			break;
		case verbose:
			std::printf("verbose.\n");
			break;
		default:
			std::printf("debug.  \n");
		}

		std::fflush(stdout);
	}
}
