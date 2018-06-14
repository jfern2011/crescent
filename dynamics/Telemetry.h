#pragma once

#include "Event.h"
#include <fstream>
#include "SharedData.h"

class Telemetry : public Event
{
	struct stream_element
	{
		stream_element(Handle<SharedData> _shared, int _shared_id)
			: shared(_shared), shared_id(_shared_id)
		{
		}

		virtual void update(std::ofstream& stream) = 0;

		Handle<SharedData> shared;

		int shared_id;
	};

	template <typename T>
	struct parameter : public stream_element
	{
		parameter(Handle<SharedData> shared, int shared_id)
			: stream_element(shared, shared_id)
		{
		}

		void update(std::ofstream& stream)
		{
			stream.write(reinterpret_cast<char*>(
				&shared->load<T>(shared_id)), sizeof(T));
		}
	};

	struct flow
	{
		std::ofstream file;

		int64 freq;

		std::vector< Handle<stream_element> >
			params;
	};

public:

	const size_t max_freq = 100;

	Telemetry();

	~Telemetry();

	bool init(Handle<SharedData> shared, // timestamp_xHz.telem
		const std::string& config);

	int64 dispatch(int64 t_now);

private:

	Handle<stream_element>
		_create_element(Handle<SharedData> shared,
						const std::string& path,
						const std::string& type);

	bool _read_config(Handle<SharedData> shared,
					  const std::string& name);

	std::vector<flow> _flows;
};
