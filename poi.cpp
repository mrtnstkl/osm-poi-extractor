#include "poi.h"


void poly_coord::operator+=(coord pos)
{
	lat_ += pos.lat;
	lon_ += pos.lon;
	++count_;
}
void poly_coord::add(float lat, float lon)
{
	lat_ += lat;
	lon_ += lon;
	++count_;
}
coord poly_coord::get()
{
	return {lat_ / count_, lon_ / count_};
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
