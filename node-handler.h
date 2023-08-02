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


class node_handler : public osmium::handler::Handler
{
	std::ostream &outfile_;
	const filter &filter_;
	uint64_t counter_ = 0;

public:
	explicit node_handler(std::ostream &outfile, const filter &filter);
	void node(const osmium::Node &node);
	uint64_t counter() const;
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

class way_handler : public osmium::handler::Handler
{
	std::ostream &outfile_;
	poly_map &poly_map_;
	const filter &filter_;
	uint64_t counter_ = 0;

public:
	explicit way_handler(std::ostream &outfile, poly_map &poly_map, const filter &filter);
	void way(const osmium::Way &way);
	uint64_t counter() const;
};
