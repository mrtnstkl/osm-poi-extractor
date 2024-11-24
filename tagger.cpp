#include "tagger.h"

tagger::tagger(const std::string &key, const std::string &value, const filter &filter)
	: key_(key), key_cstr_(key_.c_str()), value_(value), filter_(filter)
{
}

void tagger::tag(poi &poi) const
{
	if (filter_.check(poi.custom_tags))
	{
		auto *field = poi.custom_tags[key_];
		if (field != nullptr)
			*field = value_;
		else
			poi.custom_tags.emplace_back(key_, value_);
	}
}
