#include "poi.h"
#include <osmium/osm/types.hpp>


void poly_coord::operator+=(coord pos)
{
	add(pos.lat, pos.lon);
}

void poly_coord::add(float lat, float lon)
{
	lat_min_ = std::min(lat_min_, lat);
	lat_max_ = std::max(lat_max_, lat);
	lon_min_ = std::min(lon_min_, lon);
	lon_max_ = std::max(lon_max_, lon);
}

coord poly_coord::get()
{
	if (lat_min_ == FLT_MAX)
		return {0, 0};
	return {(lat_min_ + (lat_max_ - lat_min_) * .5f), (lon_min_ + (lon_max_ - lon_min_) * .5f)};
}

poi::poi(float lat, float lon, nlohmann::json tags)
	: json_({{"lat", lat}, {"lon", lon}, {"tags", tags}})
{
}

poi::poi(float lat, float lon, osmium::object_id_type id, nlohmann::json tags)
	: json_({{"id", id}, {"lat", lat}, {"lon", lon}, {"tags", tags}})
{
	if (tags.contains("name"))
		json_["name"] = tags["name"];
}

void poi::set_tags(const osmium::TagList &tag_list)
{
	auto& tags = json_["tags"];
	for (const auto& tag : tag_list)
	{
		tags[tag.key()] = tag.value();
		if (tag.key() == std::string("name"))
			json_["name"] = tag.value();
	}
}

poi poi::parse_geoname(const std::string& line)
{
	std::stringstream ss(line);

    int geonameid;
    std::string name, asciiname, alternatenames;
    float latitude, longitude;

	ss >> geonameid;
	ss.ignore();
	std::getline(ss, name, '\t');
	std::getline(ss, asciiname, '\t');
	std::getline(ss, alternatenames, '\t');
	ss >> latitude;
	ss.ignore();
	ss >> longitude;
	ss.ignore();

	return poi(latitude, longitude, {"name", name});
}

float poi::lat() const
{
	return json_["lat"].template get<float>();
}

float poi::lon() const
{
	return json_["lon"].template get<float>();
}

std::string poi::string() const
{
	return json_.dump();
}
