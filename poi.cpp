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

std::string* poi::tag_list::operator[](const std::string &key)
{
	for (auto &tag : *this)
	{
		if (tag.first == key)
			return &tag.second;
	}
	return nullptr;
}

poi::poi(float lat, float lon, tag_list tags)
	: lat(lat), lon(lon), tags(tags)
{
}

poi::poi(float lat, float lon, osmium::object_id_type id, tag_list tags)
	: lat(lat), lon(lon), id(id), tags(tags)
{
}

void poi::set_tags(const osmium::TagList &tag_list)
{
	tags.clear();
	for (const auto& tag : tag_list)
	{
		tags.emplace_back(tag.key(), tag.value());
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

	return poi(latitude, longitude, {{"name", name}});
}
