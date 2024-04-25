#include "poi.h"


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

poi::poi(const char *name, float lat, float lon, nlohmann::json tags)
	: json_({{"name", name}, {"lat", lat}, {"lon", lon}, {"tags", tags}})
{
}

void poi::set_tags(const osmium::TagList &tag_list)
{
	auto& tags = json_["tags"];
	for (const auto& tag : tag_list)
		tags[tag.key()] = tag.value();
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

	return poi(name.c_str(), latitude, longitude);
}

const std::string& poi::name() const
{
	return json_["name"].template get_ref<const std::string&>();
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
