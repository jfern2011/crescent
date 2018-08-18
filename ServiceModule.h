#pragma once

namespace Crescent
{
	/**
	 * Represents the Apollo service module
	 */
	class ServiceModule
	{

	public:

		ServiceModule(bool detached = false);

		~ServiceModule();

		void detach();

		bool is_detached() const;

	private:

		/**
		 * If true, we're attached to the command module
		 */
		bool _is_attached;
	};
}
