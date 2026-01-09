#pragma once
#include <vector>

#include "ObjectSchemas.h"

class ObjectTypeSchemaBaker {
public:
	static void bakeSchemas();
	static std::vector<ObjectSchemaEntry> getSchemas();
	static std::vector<int> getSchemaSizes();
	static size_t getTotalSchemaSize();
private:
	static std::vector<ObjectSchemaEntry> schemas;
	static std::vector<int> schemaSizes;

	static int entrySize(int numFields);
};