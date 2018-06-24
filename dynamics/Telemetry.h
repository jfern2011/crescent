#pragma once

#include <fstream>
#include <map>

#include "Event.h"
#include "SharedData.h"

/**
 * @class Telemetry
 *
 * Runs at the bottom of every event cycle to write a configured
 * set of outputs from each algorithm to one or more binary
 * streams. A separate stream is created for each set of outputs
 * with a common output rate
 */
class Telemetry : public Event
{
	/**
	 * Represents a single telemetry variable
	 */
	struct stream_element
	{
		/**
		 * Constructor
		 */
		stream_element(Handle<SharedData> _shared, int _shared_id)
			: shared(_shared), shared_id(_shared_id)
		{
		}

		virtual void update() = 0;

		/**
		 * The shared data system from which to pull
		 * telemetry outputs
		 */
		Handle<SharedData> shared;

		/**
		 *  The unique element ID within \ref shared
		 */
		int shared_id;

		/**
		 * The binary stream to write to
		 */
		Handle<std::ofstream>
			stream;
	};

	/**
	 * Represents a typed telemetry output variable
	 */
	template <typename T>
	struct parameter : public stream_element
	{
		/**
		 * Constructor
		 */
		parameter(Handle<SharedData> shared, int shared_id)
			: stream_element(shared, shared_id)
		{
		}

		/**
		 * Write the next value of this parameter
		 */
		void update()
		{
			if (stream)
			{
				stream->write(reinterpret_cast<char*>(
					&shared->load<T>(shared_id)), sizeof(T));
			}
		}
	};

	/**
	 * A telemetry flow consists of all parameters
	 * with a common output rate
	 */
	struct flow
	{
		/**
		 * Output stream to write \ref params to
		 */
		Handle<std::ofstream> file;

		/**
		 * Number of 100Hz steps per update
		 */
		int64 period;

		/**
		 * The parameters to include in this flow
		 */
		std::vector< Handle<stream_element> >
			params;
	};

public:

	/**
	 * The maximum output rate for any variable
	 */
	const size_t max_freq = 100;

	Telemetry();

	~Telemetry();

	bool init(Handle<SharedData> shared,
			  const std::string& config);

	int64 dispatch(int64 t_now);

private:

	Handle<stream_element>
		_create_element(Handle<SharedData> shared,
						const std::string& path);

	bool _read_config(Handle<SharedData> shared,
					  const std::string& name);

	/**
	 * The set of telemetry flows
	 */
	std::vector<flow> _flows;
};
