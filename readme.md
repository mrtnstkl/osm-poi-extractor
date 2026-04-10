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
./poi-extract <INFILE> <OUTFILE> [-p | -a] [-c <FILTER_FILE>] [-un] [FILTERS...]
```

- `INFILE`: The path to the PBF file.
- `OUTFILE`: The file to where you want the extracted POIs saved. Will overwrite if file exists.
- `-p | -a` (optional): Only process point or area features.
- `-un` (optional): Don't exclude unnamed features.
- `-c` (optional): Reads filters and taggers from a JSON file.
- `FILTERS`: The criteria for filtering the POIs. Multiple filters can be specified.

### Filter Syntax

Each filter is a combination of OpenStreetMap tags, consisting of a key and value. If only a key is specified, any value will be accepted. To find the tag you're looking for, search [this](https://wiki.openstreetmap.org/wiki/Map_features) page.

- `tourism.alpine_hut`: Will extract all alpine huts.
- `tourism.alpine_hut + phone + website`: Will extract all alpine huts which have a phone number and website.
- `natural.peak + summit:cross.yes`: Will extract all mountain peaks which have a summit cross.

If multiple filters are given, all POIs will be extracted that pass at least one of them.

- `amenity.restaurant + cuisine.japanese amenity.pub`: Will extract all Japanese restaurants and all pubs. 

### Configuration File

Alternatively (or in addition) to command line filters, you can specify filters in a JSON file and provide it using the `-c` option. This is useful if you have a lot of filters or want to specify taggers.

**Taggers** are applied to all POIs in the filter output. They add a tag with a specified key and value to the POI if it matches a given set of rules. The rules for taggers follow the same logic as filters.

Refer to the below example for the structure of the JSON file. It extracts all alpine huts and mountain peaks with a summit cross, and adds a tag `has_contact_info=yes` to all elements which have a phone number or email address.

```json
{
  "filter": {
    "allow_unnamed": false,
    "rules": [
      ["tourism.alpine_hut"]
      ["natural.peak", "summit:cross.yes"]
    ]
  },
  "taggers": [
    {
      "key": "has_contact_info",
      "value": "yes",
      "rules": [
        ["phone"],
        ["contact:phone"],
        ["email"],
        ["contact:email"]
      ]
    }
  ]
}
```

- both `filter` and `taggers` are optional
- `allow_unnamed` in `filter` is optional and behaves the same as the `-un` command line option
- if both command line filters and config file filters are given, a POI will be included in the output if it matches at least one filter from either source
- later taggers will overwrite tags set by earlier ones if they have the same key
- tags set by taggers are kept separately from the original OSM tags, meaning that they can't be read by other taggers


### Output

By default, the output file will have a JSON object on every line, each representing a POI. Tags added by taggers are promoted to the top level of the object, while tags from the original OSM data are stored in a `tags` object.

```
{"lat":<number>, "lon":<number>, "name":<string>, "tags": {<key1>: <value1>, <key2>: <value2>, ...}}
...
```

You can change this behaviour by modifying the `poi_sink` implementation in `sink.cpp`.
