#include "tagger.h"

tagger::tagger(const std::string &key, const std::string &value, const filter &filter)
	: key_(key), key_cstr_(key_.c_str()), value_(value), filter_(filter)
{
}

void tagger::tag(poi &poi) const
{
	if (filter_.check(*poi.osm_taglist))
	{
		auto *field = poi.tags[key_];
		if (field != nullptr)
			*field = value_;
		else
			poi.tags.emplace_back(key_, value_);
	}
}

tagger tagger::parse_json(const nlohmann::json &tagger_json)
{
	if (!tagger_json.contains("key") || !tagger_json["key"].is_string())
		throw std::runtime_error("Config error: tagger must contain a string key field");
	std::string key = tagger_json["key"].get<std::string>();

	if (!tagger_json.contains("value") || !tagger_json["value"].is_string())
		throw std::runtime_error("Config error: tagger must contain a string value field");
	std::string value = tagger_json["value"].get<std::string>();

	if (!tagger_json.contains("rules") || !tagger_json["rules"].is_array())
		throw std::runtime_error("Config error: tagger must contain an array of rules");

	filter filter = filter::parse_json(tagger_json);
	if (!tagger_json.contains("allow_unnamed"))
		filter.allow_unnamed(true); // default to allowing unnamed for taggers

	return tagger(key, value, filter);
}

void tagger::print(std::ostream &out) const
{
	out << '"' << key_ << "\"=\"" << value_ << "\" ";
	filter_.print_rules(out);
}
