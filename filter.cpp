#include "filter.h"

#include <iostream>
#include <vector>

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

void filter::add_rule(const rule& rule)
{
	rule_list_.push_back(rule);
}
void filter::add_rule(const key& k, const value& v)
{
	rule_list_.push_back({{k, v}});
}

void filter::allow_unnamed(bool allow_unnamed)
{
	allow_unnamed_ = allow_unnamed;
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
