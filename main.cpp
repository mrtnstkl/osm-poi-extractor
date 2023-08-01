#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>

#include <iostream>
#include <fstream>

#include "node-handler.h"
#include "poi.h"

template <typename... T>
void process(osmium::io::File &file, T &...handlers)
{
	osmium::io::Reader reader(file);
	osmium::apply(reader, handlers...);
	reader.close();
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " INFILE OUTFILE\n";
		return 1;
	}

	std::ofstream outfile(argv[2]);
	osmium::io::File infile(argv[1]);

	poly_map poly_map;

	way_preprocessor way_preprocessor(poly_map);
	poly_node_handler poly_node_handler(poly_map);
	way_handler way_handler(outfile, poly_map);
	node_handler node_handler(outfile);

	std::cout << "Building polygon index. " << std::flush;
	process(infile, way_preprocessor);
	std::cout << "done\n" << "Processing polygon nodes. " << std::flush;
	process(infile, poly_node_handler);
	std::cout << "done\n" << "Processing way and node POIs . " << std::flush;
	process(infile, way_handler, node_handler);
	std::cout << "done\n" << std::flush;

	outfile.close();
	std::cout << "Successfully processed " << way_handler.counter() << " way POIs and " << node_handler.counter() << " node POIs.\n";
}
