#include "node-handler.h"
#include <iostream>

#include "json.hpp"

#include "poi.h"

poly_map::poly_id poly_map::new_poly(const osmium::WayNodeList &node_ref_list)
{
	const poly_id id = poly_counter_++;
	coordinates_[id] = poly_coord();
	for (const auto &node_ref : node_ref_list)
	{
		polys_[node_ref.ref()] = id;
	}
	return id;
}
void poly_map::process_node(const osmium::Node &node)
{
	const auto poly = polys_.find(node.id());
	if (poly != polys_.end())
		coordinates_.find(poly->second)->second.add(node.location().lat(), node.location().lon());
}
poly_map::poly_id poly_map::get_poly_id(osmium::object_id_type node_id)
{
	return polys_.find(node_id)->second;
}
coord poly_map::poly_position(poly_id poly_id)
{
	return coordinates_.find(poly_id)->second.get();
}

std::string default_formatter::node(const osmium::Node &node)
{
	poi poi(node.tags()["name"], node.location().lat(), node.location().lon());
	poi.set_tags(node.tags());
	return poi.string();
}

std::string default_formatter::way(const osmium::Way &way, coord coordinates)
{
	poi poi(way.tags()["name"], coordinates.lat, coordinates.lon);
	poi.set_tags(way.tags());
	return poi.string();
}

poly_node_handler::poly_node_handler(poly_map &poly_map)
	: poly_map_(poly_map)
{
}

void poly_node_handler::node(const osmium::Node &node)
{
	poly_map_.process_node(node);
}

way_preprocessor::way_preprocessor(poly_map &poly_map, const filter &filter)
	: poly_map_(poly_map), filter_(filter)
{
}

void way_preprocessor::way(const osmium::Way &way)
{
	if (!filter_.check(way.tags()))
		return;
	poly_map_.new_poly(way.nodes());
}
