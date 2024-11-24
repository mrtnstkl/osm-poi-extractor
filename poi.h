#pragma once

#include <cfloat>
#include <osmium/osm/node.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/osm/way.hpp>
#include <string>
#include <utility>
#include <vector>

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


struct poi
{
	using tag = std::pair<std::string, std::string>;

	class tag_list : public std::vector<tag>
	{
	public:
		using std::vector<tag>::vector;
		std::string* operator[](const std::string &key);
	};

	float lat, lon;
	osmium::object_id_type id;
	tag_list tags;
	tag_list custom_tags;

	poi(float lat, float lon, tag_list tags = {});
	poi(float lat, float lon, osmium::object_id_type id, tag_list tags = {});

	void set_tags(const osmium::TagList &tag_list);

	static poi parse_geoname(const std::string& line);
};
