#include "tagger.h"

tagger::tagger(const std::string &key, const std::string &value, const filter &filter)
	: key_(key), key_cstr_(key_.c_str()), value_(value), filter_(filter)
{
}

void tagger::tag(poi &poi) const
{
	if (filter_.check(poi.osm_tags))
	{
		auto *field = poi.tags[key_];
		if (field != nullptr)
			*field = value_;
		else
			poi.tags.emplace_back(key_, value_);
	}
}
