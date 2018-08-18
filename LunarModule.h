#pragma once

namespace Crescent
{
	/**
	 * Represents the Apollo Lunar Module
	 */
	class LunarModule
	{

	public:

		LunarModule(bool docked=true);

		~LunarModule();

		bool dock(bool val);

		bool is_docked() const;

	private:

		/**
		 * If true, we're docked with the command module
		 */
		bool _is_docked;
	};
}
