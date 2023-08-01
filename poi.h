#pragma once

#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>

#include "json.hpp"

struct coord
{
	float lat = 0.f, lon = 0.f;
};

struct poly_coord
{
private:
	float lat_ = 0.f, lon_ = 0.f;
	int count_ = 0;

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
