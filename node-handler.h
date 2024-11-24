#pragma once

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/geom/coordinates.hpp>

#include <ostream>
#include <atomic>

#include "poi.h"
#include "filter.h"

class poly_map
{
public:
	using poly_id = uint64_t;

private:
	poly_id poly_counter_ = 0;

	std::unordered_map<osmium::object_id_type, poly_id> polys_;
	std::unordered_map<poly_id, poly_coord> coordinates_;

public:
	poly_id new_poly(const osmium::WayNodeList &node_ref_list);
	void process_node(const osmium::Node &node);
	poly_id get_poly_id(osmium::object_id_type node_id);
	coord poly_position(poly_id poly_id);
};

template <typename Sink>
class node_handler : public osmium::handler::Handler
{
	Sink &sink_;
	const filter &filter_;
	uint64_t counter_ = 0;

public:
	explicit node_handler(Sink &sink, const filter &filter)
		: sink_(sink), filter_(filter)
	{
	}

	void node(const osmium::Node &node)
	{
		if (!filter_.check(node.tags()))
			return;
		poi poi(node.location().lat(), node.location().lon(), node.id());
		poi.set_tags(node.tags());
		sink_ << poi;
		++counter_;
	}

	uint64_t counter() const
	{
		return counter_;
	}
};

class poly_node_handler : public osmium::handler::Handler
{
	poly_map &poly_map_;

public:
	explicit poly_node_handler(poly_map &poly_map);
	void node(const osmium::Node &node);
};

class way_preprocessor : public osmium::handler::Handler
{
	poly_map &poly_map_;
	const filter &filter_;

public:
	explicit way_preprocessor(poly_map &poly_map, const filter &filter);
	void way(const osmium::Way &way);
};

template <typename Sink>
class way_handler : public osmium::handler::Handler
{
	Sink &sink_;
	poly_map &poly_map_;
	const filter &filter_;
	uint64_t counter_ = 0;

public:
	explicit way_handler(Sink& sink, poly_map &poly_map, const filter &filter)
		: sink_(sink), poly_map_(poly_map), filter_(filter)
	{
	}

	void way(const osmium::Way &way)
	{
		if (!filter_.check(way.tags()))
			return;
		const auto coord = poly_map_.poly_position(poly_map_.get_poly_id(way.nodes().front().ref()));
		poi poi(coord.lat, coord.lon, way.id());
		poi.set_tags(way.tags());
		sink_ << poi;
		++counter_;
	}

	uint64_t counter() const
	{
		return counter_;
	}
};
