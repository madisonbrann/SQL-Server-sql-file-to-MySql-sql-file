/*
SQL Server to My SQL Exporter
Created by Madison Brann

How it works: This takes in a sql server .sql file, and converts it to a MySQL .sql file. 
			  The sql server.sql files are generate through ssms by generating a script to
			  create a .sql file. The resulting MySQl .sql file is imported via MySQL workbench
			  and adds the resulting tables.
*/

/*
THINGS TO CHECK:
-having a value after the int type (sql server does not define but MySQL might need it
-extra commands at end of attribte like "auto incremental" at the end of the int type
-casting time as a data entry
*/


#include<iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Schema.h"
#include "Column.h"

using namespace std;


void remove_char(string& input, char type)
{
	input.erase(remove(input.begin(), input.end(), type), input.end());
}


void writeFile(Schema my_schema, ofstream& out, vector<vector<string>> all_data)
{
	stringstream ss;
	string output;
	//BEGIN WRITING SCHEMA
	ss << "CREATE TABLE IF NOT EXISTS `" << my_schema.get_table() << "` (" << endl;

	for (unsigned int i = 0; i < my_schema.num_of_columns(); i++)
	{
		//cout << "Name: " << my_schema.access_column(i).get_name() << " Type: " << my_schema.access_column(i).get_type() << " Type_Value: " << my_schema.access_column(i).get_type_value() << " Required: " << my_schema.access_column(i).get_required() << " " << endl;
		ss << "`" << my_schema.access_column(i).get_name() << "` " << my_schema.access_column(i).get_type();

		if (my_schema.access_column(i).get_type_value() != "uninitialized")
		{
			ss << my_schema.access_column(i).get_type_value();
		}
		if (i != my_schema.num_of_columns() - 1) //account for last trailing comma
		{
			ss << " " << my_schema.access_column(i).get_required() << "," << endl;
		}
		else
		{

			ss << " " << my_schema.access_column(i).get_required() << endl;
		}
	}
	ss << ");" << endl;
	cout << ss.str();
	out << ss.str();
	//END WRITING SCHEMA

	//BEGING WRITING DATA
	cout << endl << "INSERT INTO `" << my_schema.get_table() << "` (";
	for (unsigned int i = 0; i < my_schema.num_of_columns(); i++)
	{
		cout << "`" << my_schema.access_column(i).get_name() << "`,";
		if (i == my_schema.num_of_columns - 1) //remove trailing comma and add ')'
		{
			cout << "`" << my_schema.access_column(i).get_name() << "`)";
		}
	}
	
}


void readFile(ifstream& in, ofstream& out)
{
	vector<string> data_entries;
	vector<vector<string>> all_data;
	string word;
	string table_name;
	bool end_schema;

	in >> word; //skip "create"
	in >> word; //skip "table"
	in >> table_name; //table name

	Schema my_schema(table_name);
	//BEGIN ACCESSING SCHEMA INFO
	while (getline(in,word)) //go through Schema attributes line at a time
	{
		stringstream ss;
		string name;
		string type;
		string required;
		vector<string> holder;
		if (word != "")
		{
			if (word.at(0) == ')')
			{
				end_schema = true;
				break;
			}

			ss << word;

			while (ss >> word) //go through each word in the line
			{
				holder.push_back(word); //stores each word of line in vector
			}
			
			name = holder.at(0);
			type = holder.at(1);
			required = holder.at(2);
			
			Column my_column(name, type, required);			
			my_schema.add_column(my_column);
		}
	}

	//END ACCESSING SCHEMA INFO
	//BEGIN ACCESSING DATA INFO

	while (getline(in, word)) //go through data entries line at a time
	{
		stringstream ss;
		bool data_begin = false;
		ss << word;

		while (ss >> word)
		{
			if (word == "VALUES") //go past unneeded information to where data actually is
			{
				data_begin = true;
				ss >> word; //iterate to word after VALUE
				word = word.substr(1, word.size() - 1); //remove ( from first data entry
			}

			if (data_begin == true)
			{
				string total_word = word;
				if (total_word.at(0) == 'N' && total_word.at(1) == '\'') //if N' occurs
				{
					total_word = total_word.substr(2, total_word.size()); 	
				}
				if (total_word.at(total_word.size() - 1) != ',') //more then one word in entry, go through words until hits comma
				{
					while (ss >> word)
					{
						total_word += " ";
						total_word += word;
						if (total_word.at(total_word.size() - 1) == ',')
						{
							break;
						}
					}
				}

				remove_char(total_word, ',');
				remove_char(total_word, '\'');
				word = total_word;
				data_entries.push_back(word);
			}
		}
		if (data_entries.size() > 0)
		{
			remove_char(data_entries.at(data_entries.size() - 1), ')'); //remove ) at end of entry line
		}
		all_data.push_back(data_entries);
	}

	//END ACCESSING DATA INFO
	writeFile(my_schema, out, all_data);
}


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cerr << "Please provide name of input and output files" << argc << endl;
		system("pause");
		return 0;
	}
	ifstream in(argv[1]);
	ofstream out(argv[2]);
	readFile(in, out);
	system("pause");
	return 0;
}