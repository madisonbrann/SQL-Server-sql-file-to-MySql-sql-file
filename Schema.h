#ifndef SCHEMA
#define SCHEMA

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "Column.h"

using namespace std;

class Schema
{
private:
	string table;
	vector<Column> schema_columns;


public:
	Schema(string table_in)
	{
		table = table_in;
		clean_table();
	}
	virtual ~Schema()
	{

	}

	string get_table()
	{
		return table;
	}

	void clean_table() //remove brackets and the inital ( from the table name
	{
		table.erase(remove(table.begin(), table.end(), '['), table.end());
		table.erase(remove(table.begin(), table.end(), ']'), table.end());
		table.erase(remove(table.begin(), table.end(), '('), table.end());
	}

	void add_column(Column column)
	{
		schema_columns.push_back(column);
	}

	Column access_column(int index)
	{
		return schema_columns.at(index);
	}

	int num_of_columns()
	{
		return schema_columns.size();
	}

};

#endif