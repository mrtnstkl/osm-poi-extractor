#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>

#include <iostream>
#include <fstream>

#include "node-handler.h"
#include "poi.h"
#include "filter.h"

template <typename... T>
void process(osmium::io::File &file, T &...handlers)
{
	osmium::io::Reader reader(file);
	osmium::apply(reader, handlers...);
	reader.close();
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " INFILE OUTFILE [FILTERS...]\n"
				  << "Example: ./poi_extract austria-latest.osm.pbf out.txt tourism.alpine_hut + phone natural.peak + summit:cross.yes\n";
		return 1;
	}

	std::ofstream outfile(argv[2]);
	if (!outfile.is_open())
	{
		std::cerr << "Failed to open output file" << std::endl;
		return 1;
	}
	osmium::io::File infile(argv[1]);
	try
	{
		infile.check();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to open input file: " << e.what() << '\n';
		return 1;
	}

	auto filter = filter::parse_args(argc - 3, argv + 3);
	filter.print(std::cout);

	poly_map poly_map;

	way_preprocessor way_preprocessor(poly_map, filter);
	poly_node_handler poly_node_handler(poly_map);
	poi_handler poi_handler(outfile, poly_map, filter);

	std::cout << "Building polygon index. " << std::flush;
	process(infile, way_preprocessor);
	std::cout << "done\n"
			  << "Processing polygon nodes. " << std::flush;
	process(infile, poly_node_handler);
	std::cout << "done\n"
			  << "Processing way and node POIs. " << std::flush;
	process(infile, poi_handler);
	std::cout << "done\n"
			  << std::flush;

	outfile.close();
	std::cout << "Successfully extracted " << poi_handler.count() << " POIs: " 
			  << poi_handler.node_count() << " point feature and " << poi_handler.way_count() << " area features.\n";
}
