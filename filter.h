#pragma once

#include "poi.h"
#include <osmium/osm/tag.hpp>
#include "json.hpp"
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
	static filter parse_json(const nlohmann::json& filter_json);
	void add_rule(const rule& rule);
	void add_rule(const key& k, const value& v = std::string());
	void add_rule(const std::vector<std::string>& conditions);
	void allow_unnamed(bool allow_unnamed);

	filter operator+(const filter& other) const;

	bool check(const osmium::TagList &tags) const;
	bool check(poi::tag_list &tags) const;

	void print(std::ostream &out) const;
	void print_rules(std::ostream &out) const;
};
