#pragma once

#include <cfloat>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>

#include "json.hpp"

struct coord
{
	float lat = 0, lon = 0;
};

struct poly_coord
{
private:
	float lat_min_ = FLT_MAX, lat_max_ = FLT_MIN;
	float lon_min_ = FLT_MAX, lon_max_ = FLT_MIN;

public:
	void operator+=(coord pos);
	void add(float lat, float lon);
	coord get();
};


class poi
{
	nlohmann::json json_;

public:
	poi(const char* name, float lat, float lon, nlohmann::json tags = nlohmann::json::object());

	void set_tags(const osmium::TagList &tag_list);

	static poi parse_geoname(const std::string& line);

	const std::string& name() const;
	float lat() const;
	float lon() const;

	std::string string() const;
};
