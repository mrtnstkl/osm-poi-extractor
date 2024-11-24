#pragma once

#include "filter.h"
#include "poi.h"

class tagger
{
	const std::string key_;
	const char *key_cstr_;
	const std::string value_;
	filter filter_;
public:
	tagger(const std::string &key, const std::string &value, const filter &filter);
	void tag(poi &poi) const;
};
