#include "filter.h"

#include <iostream>
#include <osmium/osm/tag.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "poi.h"

filter filter::parse_args(const std::vector<std::string>& args)
{
	filter filter;
	rule rule;
	bool join = true;

	for (const auto& arg : args)
	{
		if (arg == "+")
		{
			join = true;
			continue;
		}
		if (!rule.empty() && !join)
		{
			filter.rule_list_.push_back(std::move(rule));
			rule = filter::rule();
		}
		join = false;

		const auto dot = arg.find('.');
		if (dot == std::string::npos)
		{
			rule.push_back({arg, std::string()});
		}
		else
		{
			rule.push_back({arg.substr(0, dot), arg.substr(dot + 1)});
		}
	}

	if (!rule.empty())
	{
		filter.rule_list_.push_back(std::move(rule));
	}

	return filter;
}

filter filter::parse_json(const nlohmann::json &filter_json)
{
	filter filter;
	if (filter_json.contains("allow_unnamed"))
	{
		if (!filter_json["allow_unnamed"].is_boolean())
			throw std::runtime_error("Config error: allow_unnamed must be a boolean");
		filter.allow_unnamed(filter_json["allow_unnamed"].get<bool>());
	}

	if (!filter_json.contains("rules"))
		throw std::runtime_error("Config error: missing rules array in " + filter_json.dump());

	if (!filter_json["rules"].is_array())
		throw std::runtime_error("Config error: rules must be an array");

	for (const auto& rule_json : filter_json["rules"])
	{
		std::vector<std::string> conditions;
		if (!rule_json.is_array() || rule_json.empty())
			throw std::runtime_error("Config error: each rule must be a non-empty array");
		for (const auto& condition_json : rule_json)
		{
			if (!condition_json.is_string())
				throw std::runtime_error("Config error: each condition must be a string");
			conditions.push_back(condition_json.get<std::string>());
		}
		filter.add_rule(conditions);
	}
	return filter;
}

void filter::add_rule(const rule& rule)
{
	rule_list_.push_back(rule);
}
void filter::add_rule(const key& k, const value& v)
{
	rule_list_.push_back({{k, v}});
}

void filter::add_rule(const std::vector<std::string>& conditions)
{
	rule rule;
	for (const auto& condition : conditions)
	{
		const auto dot = condition.find('.');
		if (dot == std::string::npos)
			rule.push_back({condition, std::string()});
		else
			rule.push_back({condition.substr(0, dot), condition.substr(dot + 1)});
	}
	rule_list_.push_back(std::move(rule));
}

void filter::allow_unnamed(bool allow_unnamed)
{
	allow_unnamed_ = allow_unnamed;
}

filter filter::operator+(const filter& other) const
{
	filter result = *this;
	result.rule_list_.insert(result.rule_list_.end(), other.rule_list_.begin(), other.rule_list_.end());
	result.allow_unnamed_ = allow_unnamed_ || other.allow_unnamed_;
	return result;
}

bool filter::check(const osmium::TagList &tags) const
{
	if (!allow_unnamed_ && !tags["name"])
		return false;

	if (rule_list_.empty())
		return true;

	for (const auto &rule : rule_list_)
	{
		uint satisfied = 0;
		for (const auto &tag : rule)
		{
			const char *tag_value = tags[tag.first.c_str()];
			if (tag_value == nullptr)
				continue;
			if (!tag.second.empty() && tag.second != tag_value)
				continue;
			++satisfied;
		}
		if (satisfied == rule.size())
			return true;
	}
	return false;
}

bool filter::check(poi::tag_list &tags) const
{
	if (!allow_unnamed_ && !tags["name"])
		return false;

	if (rule_list_.empty())
		return true;

	for (const auto &rule : rule_list_)
	{
		uint satisfied = 0;
		for (const auto &tag : rule)
		{
			std::string *tag_value = tags[tag.first.c_str()];
			if (tag_value == nullptr)
				continue;
			if (!tag.second.empty() && tag.second != *tag_value)
				continue;
			++satisfied;
		}
		if (satisfied == rule.size())
			return true;
	}
	return false;
}

void filter::print(std::ostream &out) const
{
	if (allow_unnamed_)
		out << "Unnamed POIs are allowed\n";
	if (rule_list_.empty())
	{
		out << "Using empty filter" << std::endl;
		return;
	}
	out << "At least one of the following rules must be satisfied:\n";
	int i = 1;
	for (const auto &rule : rule_list_)
	{
		out << "   Rule " << i++ << ':';
		for (const auto &kv : rule)
			out << ' ' << kv.first << '=' << (kv.second.empty() ? "*" : kv.second);
		out << std::endl;
	}
}
