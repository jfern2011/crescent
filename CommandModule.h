#pragma once

namespace Crescent
{
	/**
	* Represents the Apollo Command Module
	*/
	class CommandModule
	{

	public:

		CommandModule(bool docked = true, bool detached = false);

		~CommandModule();

		void detach_sm();

		bool dock(bool val);

		bool is_detached() const;

		bool is_docked() const;

	private:

		/**
		 * True if we're attached to the service module
		 */
		bool _is_attached;

		/**
		*  If true, we are docked with the lunar module
		*/
		bool _is_docked;
	};
}
