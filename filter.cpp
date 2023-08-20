#include "filter.h"

#include <iostream>

filter filter::parse_args(int argc, char *argv[])
{
	filter filter;
	rule rule;
	bool join = true;

	for (int i = 0; i < argc; i++)
	{
		std::string arg(argv[i]);

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

		auto dot = arg.find('.');
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

bool filter::check(const osmium::TagList &tags) const
{
	if (!tags["name"])
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
	out << "At least one of the following rules must be satisfied:\n";
	int i = 1;
	for (const auto &rule : rule_list_)
	{
		out << "   Rule " << i++ << ':';
		for (const auto &kv : rule)
			out << ' ' << kv.first << '=' << (kv.second.empty() ? "*" : kv.second);
		out << '\n';
	}
}
