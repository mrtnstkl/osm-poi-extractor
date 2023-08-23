"""

This script extracts each of the POI categories defined
in the mapsforge "poi-mapping.xml" into a separate file.

Download the XML from https://github.com/mapsforge/mapsforge,
under mapsforge-poi-writer/src/main/config/poi-mapping.xml

Be warned that it is relatively slow, since it has to invoke
the poi-extractor once for each of the 20 categories.

Usage:
python mapsforge-poi-mappings.py <poi_mappings_xml> <input_osm_pbf_file> <output_folder>
(the output folder must already exist!)

"""

import xml.etree.ElementTree as ET
import json
import sys
import subprocess

def extract_filters_from_category(category, namespace):
    filters = [mapping.get('tag').replace('=', '.', 1) for mapping in category.findall(f"{{{namespace}}}mapping[@tag]")]
    # Recursively extract filters from the category
    for child_category in category.findall(f"{{{namespace}}}category"):
        filters.extend(extract_filters_from_category(child_category, namespace))
    return filters

def main(input_xml, input_pbf_file, output_folder):
    # Parse the XML content
    tree = ET.parse(input_xml)
    root = tree.getroot()
    namespace = root.tag.split('}')[0].strip('{')

    # Extract top-level categories and their associated filters
    for category in root.findall(f"{{{namespace}}}category"):
        title = category.attrib.get('title')
        if title: 
            filters = extract_filters_from_category(category, namespace)
            subprocess.run(["./poi_extract", input_pbf_file, f"{output_folder}/{title}.txt"] + filters)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: python {sys.argv[0]} <poi_mappings_xml> <input_osm_pbf_file> <output_folder>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2], sys.argv[3])
