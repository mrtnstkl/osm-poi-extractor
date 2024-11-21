#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "node-handler.h"
#include "filter.h"

#define PRINT_USAGE(argv0) std::cerr << "Usage: " << argv0 << " INFILE OUTFILE [-p | -a] [-un] [FILTERS...]" << std::endl

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
		PRINT_USAGE(argv[0]);
		std::cerr << "Example: ./poi-extract austria-latest.osm.pbf out.txt tourism.alpine_hut + phone natural.peak + summit:cross.yes\n";
		return 1;
	}

	std::vector<std::string> filter_args;
	bool process_ways = false, process_nodes = false;
	bool allow_unnamed = false;
	for (int i = 3; i < argc; ++i)
	{
		std::string arg(argv[i]);
		if (arg == "-a")
			process_ways = true;
		else if (arg == "-p")
			process_nodes = true;
		else if (arg == "-un")
			allow_unnamed = true;
		else if (arg[0] == '-')
		{
			std::cerr << "Invalid argument \"" << arg << '"' << std::endl;
			PRINT_USAGE(argv[0]);
			return 1;
		}
		else
			filter_args.push_back(arg);
	}
	if (!process_ways && !process_nodes)
		process_nodes = process_ways = true;

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

	auto filter = filter::parse_args(filter_args);
	if (!allow_unnamed)
		filter.add_rule("name");
	filter.print(std::cout);

	poly_map poly_map;

	way_preprocessor way_preprocessor(poly_map, filter);
	poly_node_handler poly_node_handler(poly_map);
	way_handler way_handler(outfile, poly_map, filter);
	node_handler node_handler(outfile, filter);

	if (process_ways)
	{
		std::cout << "Building polygon index. " << std::flush;
		process(infile, way_preprocessor);
		std::cout << "done\n";
		std::cout << "Processing polygon nodes. " << std::flush;
		process(infile, poly_node_handler);
		std::cout << "done\n";
	}
	if (process_nodes && process_ways)
	{
		std::cout << "Processing way and node POIs. " << std::flush;
		process(infile, way_handler, node_handler);
	}
	else if (process_nodes)
	{
		std::cout << "Processing node POIs. " << std::flush;
		process(infile, node_handler);
	}
	else if (process_ways)
	{
		std::cout << "Processing way POIs. " << std::flush;
		process(infile, way_handler);
	}
	std::cout << "done" << std::endl;

	outfile.close();
	std::cout << "Successfully extracted " << node_handler.counter() + way_handler.counter() << " POIs: " 
			  << node_handler.counter() << " point features and " << way_handler.counter() << " area features.\n";
}
