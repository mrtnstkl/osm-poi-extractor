## osm-poi-extractor

### Overview

A POI extractor for OpenStreetMap data. It reads in a PBF file, filters it based on criteria given as command line arguments, and outputs the relevant POIs to a text file.

Requires [Osmium](https://osmcode.org/libosmium/) for extracting OSM data.  
Uses [nlohmann/json](https://github.com/nlohmann/json) for JSON serialization (included).

### Installation

1. install [libosmium](https://osmcode.org/libosmium/) (packages available for Debian, Ubuntu, Fedora, Arch)
2. clone this repo
3. run make

Debian/Ubuntu:

```bash
sudo apt-get update
sudo apt-get install -y libosmium2-dev
git clone https://github.com/mrtnstkl/osm-poi-extractor.git
cd osm-poi-extractor
make
```

### Usage

```
./poi-extract <INFILE> <OUTFILE> [-p | -a] [-un] [FILTERS...]
```

- `INFILE`: The path to the PBF file.
- `OUTFILE`: The file to where you want the extracted POIs saved. Will overwrite if file exists.
- `-p | -a` (optional): Only process point or area features.
- `-un` (optional): Don't exclude unnamed features.
- `FILTERS`: The criteria for filtering the POIs. Multiple filters can be specified.

### Filter Syntax

Each filter is a combination of OpenStreetMap tags, consisting of a key and value. If only a key is specified, any value will be accepted. To find the tag you're looking for, search [this](https://wiki.openstreetmap.org/wiki/Map_features) page.

- `tourism.alpine_hut`: Will extract all alpine huts.
- `tourism.alpine_hut + phone + website`: Will extract all alpine huts which have a phone number and website.
- `natural.peak + summit:cross.yes`: Will extract all mountain peaks which have a summit cross.

If multiple filters are given, all POIs will be extracted that pass at least one of them.

- `amenity.restaurant + cuisine.japanese amenity.pub`: Will extract all Japanese restaurants and all pubs. 

### Output

By default, the output file will have a JSON object on every line, each representing a POI:

```
{"lat":<number>, "lon":<number>, "name":<string>, "tags": {<key1>: <value1>, <key2>: <value2>, ...}}
...
```

You can change this by modifying the `default_formatter` implementation in `node-handler.cpp`, or by using a `custom_node_handler` and `custom_way_handler` in the main function.
