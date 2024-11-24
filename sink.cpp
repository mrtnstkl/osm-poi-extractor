#include "sink.h"

poi_sink::poi_sink(std::ostream &out, std::vector<tagger> taggers)
	: out_(out), taggers_(std::move(taggers))
{
}

poi_sink& poi_sink::operator<<(poi &poi)
{
	for (const auto &tagger : taggers_)
		tagger.tag(poi);
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
	for (const auto& [key, value] : poi.custom_tags)
		json[key] = value;
	return json.dump();
}
