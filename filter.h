#pragma once

#include <osmium/osm/tag.hpp>

class filter
{
	using key = std::string;
	using value = std::string;
	using kv = std::pair<key, value>;
	using conjunct = std::vector<kv>;

	std::vector<conjunct> dnf_;

public:
	static filter parse_args(int argc, char *argv[]);
	bool check(const osmium::TagList &tags) const;
	void print(std::ostream &out) const;
};
