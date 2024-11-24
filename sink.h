#pragma once

#include "poi.h"
#include "tagger.h"
#include <vector>

class poi_sink
{
	std::ostream &out_;
	std::vector<tagger> taggers_;

public:
	poi_sink(std::ostream &out, std::vector<tagger> taggers = {});
	poi_sink& operator<<(poi &poi);

	static std::string format(const poi &poi);
};
