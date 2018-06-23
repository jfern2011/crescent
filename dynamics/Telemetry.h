#pragma once

#include <fstream>
#include <map>

#include "Event.h"
#include "SharedData.h"

class Telemetry : public Event
{
	struct stream_element
	{
		stream_element(Handle<SharedData> _shared, int _shared_id)
			: shared(_shared), shared_id(_shared_id)
		{
		}

		virtual void update() = 0;

		Handle<SharedData>
			shared;

		int shared_id;

		Handle<std::ofstream>
			stream;
	};

	template <typename T>
	struct parameter : public stream_element
	{
		parameter(Handle<SharedData> shared, int shared_id)
			: stream_element(shared, shared_id)
		{
		}

		void update()
		{
			if (stream)
			{
				stream->write(reinterpret_cast<char*>(
					&shared->load<T>(shared_id)), sizeof(T));
			}
		}
	};

	struct flow
	{
		Handle<std::ofstream> file;

		/**
		 * Number of 100Hz steps per update
		 */
		int64 period;

		std::vector< Handle<stream_element> >
			params;
	};

public:

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

	std::vector<flow> _flows;
};
