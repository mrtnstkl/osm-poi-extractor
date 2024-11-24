#include "sink.h"

poi_sink::poi_sink(std::ostream &out)
	: out_(out)
{
}

poi_sink& poi_sink::operator<<(poi &poi)
{
	out_ << format(poi) << '\n';
	return *this;
}

std::string poi_sink::format(const poi &poi)
{
	auto json = nlohmann::json{
		{"id", poi.id},
		{"lat", poi.lat},
		{"lon", poi.lon},
		{"tags", nlohmann::json::object()},
	};
	auto &tags = json["tags"];
	for (const auto& [key, value] : poi.tags)
	{
		if (key == "name")
			json["name"] = value;
		else
			tags[key] = value;
	}
	return json.dump();
}
