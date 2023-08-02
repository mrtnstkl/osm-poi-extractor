#include "filter.h"

#include <iostream>

filter filter::parse_args(int argc, char *argv[])
{
	filter filter;
	conjunct conj;
	bool join = true;

	for (int i = 0; i < argc; i++)
	{
		std::string arg(argv[i]);

		if (arg == "+")
		{
			join = true;
			continue;
		}
		if (!conj.empty() && !join)
		{
			filter.dnf_.push_back(std::move(conj));
			conj = conjunct();
		}
		join = false;

		auto dot = arg.find('.');
		if (dot == std::string::npos)
		{
			conj.push_back({arg, std::string()});
		}
		else
		{
			conj.push_back({arg.substr(0, dot), arg.substr(dot + 1)});
		}
	}

	if (!conj.empty())
	{
		filter.dnf_.push_back(std::move(conj));
	}

	return filter;
}

bool filter::check(const osmium::TagList &tags) const
{
	if (!tags["name"])
		return false;

	for (const auto &conj : dnf_)
	{
		uint satisfied = 0;
		for (const auto &kv : conj)
		{
			const char *tag_value = tags[kv.first.c_str()];
			if (tag_value == nullptr)
				continue;
			if (!kv.second.empty() && kv.second != tag_value)
				continue;
			++satisfied;
		}
		if (satisfied == conj.size())
			return true;
	}
	return false;
}

void filter::print(std::ostream &out) const
{
	out << "At least one of the following rules must be satisfied:\n";
	int i = 1;
	for (const auto &conj : dnf_)
	{
		out << "   Rule " << i++ << ':';
		for (const auto &kv : conj)
			out << ' ' << kv.first << '=' << (kv.second.empty() ? "*" : kv.second);
		out << '\n';
	}
}
