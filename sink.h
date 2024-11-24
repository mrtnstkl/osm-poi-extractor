#pragma once

#include "poi.h"
#include "tagger.h"
#include <vector>

class poi_sink
{
	std::ostream &out_;

public:
	poi_sink(std::ostream &out);
	poi_sink& operator<<(poi &poi);

	static std::string format(const poi &poi);
};
