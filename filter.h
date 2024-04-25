#pragma once

#include <osmium/osm/tag.hpp>
#include <vector>

class filter
{
	using key = std::string;
	using value = std::string;
	using tag = std::pair<key, value>;
	using rule = std::vector<tag>;

	std::vector<rule> rule_list_;

public:
	static filter parse_args(const std::vector<std::string>& args);
	bool check(const osmium::TagList &tags) const;
	void print(std::ostream &out) const;
};
