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
	if (argc <= 3)
	{
		std::cerr << "Usage: " << argv[0] << " INFILE OUTFILE [FILTERS...]\n"
				  << "Example: ./poi_extract austria-latest.osm.pbf out.txt tourism.alpine_hut + phone natural.peak + summit:cross.yes\n";
		return 1;
	}

	auto filter = filter::parse_args(argc - 3, argv + 3);
	filter.print(std::cout);

	std::ofstream outfile(argv[2]);
	osmium::io::File infile(argv[1]);

	poly_map poly_map;

	way_preprocessor way_preprocessor(poly_map, filter);
	poly_node_handler poly_node_handler(poly_map);
	way_handler way_handler(outfile, poly_map, filter);
	node_handler node_handler(outfile, filter);

	std::cout << "Building polygon index. " << std::flush;
	process(infile, way_preprocessor);
	std::cout << "done\n"
			  << "Processing polygon nodes. " << std::flush;
	process(infile, poly_node_handler);
	std::cout << "done\n"
			  << "Processing way and node POIs . " << std::flush;
	process(infile, way_handler, node_handler);
	std::cout << "done\n"
			  << std::flush;

	outfile.close();
	std::cout << "Successfully processed " << way_handler.counter() << " way POIs and " << node_handler.counter() << " node POIs.\n";
}
