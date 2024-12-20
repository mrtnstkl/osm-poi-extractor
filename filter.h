#pragma once

#include "poi.h"
#include <osmium/osm/tag.hpp>
#include <vector>

class filter
{
	using key = std::string;
	using value = std::string;
	using tag = std::pair<key, value>;
	using rule = std::vector<tag>;

	std::vector<rule> rule_list_;
	bool allow_unnamed_ = false;

public:
	static filter parse_args(const std::vector<std::string>& args);
	void add_rule(const rule& rule);
	void add_rule(const key& k, const value& v = std::string());
	void allow_unnamed(bool allow_unnamed);

	bool check(const osmium::TagList &tags) const;
	bool check(poi::tag_list &tags) const;

	void print(std::ostream &out) const;
};
