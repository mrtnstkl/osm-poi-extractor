#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>

#include <iostream>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "node-handler.h"
#include "filter.h"
#include "sink.h"
#include "tagger.h"

#define PRINT_USAGE(argv0) std::cerr << "Usage: " << argv0 << " INFILE OUTFILE [-p | -a] [-un] [FILTERS...]" << std::endl

template <typename... T>
void process(osmium::io::File &file, T &...handlers)
{
	osmium::io::Reader reader(file);
	osmium::apply(reader, handlers...);
	reader.close();
}

void parse_config_file(const char* path, filter& f, std::vector<tagger>& ts)
{
	std::ifstream in(path);
	nlohmann::json config_json;
	if (!in.is_open())
		throw std::runtime_error("Failed to open config file");

	try
	{
		in >> config_json;
	}
	catch (const std::exception &e)
	{
		throw std::runtime_error("Failed to parse config file: " + std::string(e.what()));
	}

	if (config_json.contains("filter"))
	{
		if (!config_json["filter"].is_object())
			throw std::runtime_error("Config error: filter must be an object");
		f = f + filter::parse_json(config_json["filter"]);
	}
	if (config_json.contains("taggers"))
	{
		if (!config_json["taggers"].is_array())
			throw std::runtime_error("Config error: taggers must be an array");
		for (const auto& tagger_json : config_json["taggers"])
		{
			if (!tagger_json.is_object())
				throw std::runtime_error("Config error: each tagger must be an object");
			ts.emplace_back(tagger::parse_json(tagger_json));
		}
	}
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
	std::string config_path;
	bool process_ways = false, process_nodes = false;
	std::optional<bool> allow_unnamed = std::nullopt;
	for (int i = 3; i < argc; ++i)
	{
		std::string arg(argv[i]);
		if (arg == "-a")
			process_ways = true;
		else if (arg == "-p")
			process_nodes = true;
		else if (arg == "-un")
			allow_unnamed = true;
		else if (arg == "-c")
			config_path = argv[++i]; // unsafe if -c is the last argument
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
	std::vector<tagger> taggers;
	if (!config_path.empty())
	{
		try
		{
			parse_config_file(config_path.c_str(), filter, taggers);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Failed to parse config file!\n" << e.what() << '\n';
			return 1;
		}
	}

	if (allow_unnamed.has_value())
		filter.allow_unnamed(allow_unnamed.value());
	filter.print(std::cout);
	if (!taggers.empty())
		std::cout << "The following taggers will be applied to the output:\n";
	for (size_t i = 0; i < taggers.size(); ++i)
	{
		std::cout << "   Tagger " << i + 1 << ": ";
		taggers[i].print(std::cout);
		std::cout << std::endl;
	}

	poly_map poly_map;

	poi_sink result_sink(outfile, taggers);

	way_preprocessor way_preprocessor(poly_map, filter);
	poly_node_handler poly_node_handler(poly_map);
	way_handler way_handler(result_sink, poly_map, filter);
	node_handler node_handler(result_sink, filter);

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
