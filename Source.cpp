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

//HERE output for inserts are duplicating and missing stuff

#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include "Schema.h"
#include "Column.h"

using namespace std;

int counter = 0;

void remove_char(string& input, char type)
{
	input.erase(remove(input.begin(), input.end(), type), input.end());
}

void troubleshoot()
{
	// cout << "this happened" << endl;
}

bool detect_time_cast(string input)
{
	for (unsigned int i = 0; i < input.size(); i++)
	{
		if (input.at(i) == '(')
			return true;
	}
	return false;
}


void writeFile(vector<Schema> all_schemas, ofstream& out, vector<vector<string>> all_data)
{
	stringstream ss;
	string output;
	//BEGIN WRITING SCHEMA
	for (unsigned int i = 0; i < all_schemas.size(); i++)
	{
		Schema my_schema = all_schemas.at(i);

		ss << "CREATE TABLE IF NOT EXISTS `" << my_schema.get_table() << "` (" << endl;

		for (unsigned int i = 0; i < my_schema.num_of_columns(); i++)
		{
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
		//cout << ss.str();
		//cout << my_schema.num_of_columns();
		
	}
	out << ss.str();
	//END WRITING SCHEMA
	ss.clear();
	ss.str("");


	//BEGING WRITING DATA
	for (unsigned int i = 0; i < all_schemas.size(); i++)
	{
		Schema my_schema = all_schemas.at(i);
		ss << endl << "INSERT INTO `" << my_schema.get_table() << "` (";
		for (unsigned int i = 0; i < my_schema.num_of_columns(); i++)
		{
			if (i == my_schema.num_of_columns() - 1) //remove trailing comma and add ')'
			{
				ss << "`" << my_schema.access_column(i).get_name() << "`)" << endl << endl;
			}
			else
			{
				ss << "`" << my_schema.access_column(i).get_name() << "`,";
			}
		}

		ss << "VALUES" << endl << endl;
		vector<vector<string>> schema_data = my_schema.get_all_data();
		for (unsigned int i = 0; i < schema_data.size(); i++)
		{
			if (schema_data.at(i).size() > 0) //skip  empty entries
			{

				ss << "(";
				for (unsigned int j = 0; j < schema_data.at(i).size(); j++)
				{
					if (schema_data.at(i).at(j) == "NULL") //keep quotes off of NULL
					{
						if (j == schema_data.at(i).size() - 1) //once again remove trailing comma in case of NULL
						{
							ss << schema_data.at(i).at(j);
						}
						else
						{
							ss << schema_data.at(i).at(j) << ",";
						}
					}

					else if (j == schema_data.at(i).size() - 1)
					{
						ss << "'" << schema_data.at(i).at(j) << "'"; //remove trailing comma
					}

					else
					{

						ss << "'" << schema_data.at(i).at(j) << "',";
					}
				}
				if (i == schema_data.size() - 1) //remove trailing comma, add ';'
				{

					ss << ");" << endl << endl;
				}
				else
				{
					ss << ")," << endl << endl;
				}
			}

		}

		// cout << ss.str();
	}
	out << ss.str();
	
	cout << counter << " completed" << endl;
}


void multipleWords(stringstream& ss, string& word, string& total_word)
{

	while (ss >> word)
	{
		if (word == "AS") //strange cast to DateTime we still need to account for. All times will be the same for now
		{
			ss >> word; //skip DateTime word
			if (word == "DateTime),")
			{
				total_word = "1996-05-27 12:36:24";
				break;
			}
		}
		total_word += " ";
		total_word += word;
		if (total_word.at(total_word.size() - 1) == ',' && total_word.at(total_word.size() - 2) == '\'')
		{
			break;
		}
	}
}


void readFile(ifstream& in, ofstream& out)
{
	vector<string> data_entries;
	vector<vector<string>> all_data;
	vector<Schema> all_schemas;
	string word;
	string table_name;
	bool end_schema;
	bool begin_schema = false;
	bool end_all_schema = false;
	string current_table;

	while (in >> word) //skip stuff at beginning of sql server .sql file
	{
		if (word == "CREATE")
		{
			begin_schema = true;
		}


		if (begin_schema == true)
		{


			// in >> word; //skip "create"
			in >> word; //skip "table";
			in >> table_name; //table name


			remove_char(table_name, '.');

			Schema my_schema(table_name);
			//BEGIN ACCESSING SCHEMA INFO
			while (getline(in, word)) //go through Schema attributes line at a time
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
					// cout << name << endl;
					type = holder.at(1);
					//	cout << type << endl;
					required = holder.at(2);

					Column my_column(name, type, required);
					my_schema.add_column(my_column);
				}
			}
			all_schemas.push_back(my_schema);
		}
		begin_schema = false; //move onto next schema 
		if (word == "INSERT") //have arrived at data
		{
			in >> word; //access table name
			remove_char(word, '.');
			current_table = word;
			break;
		}
	}
		//END ACCESSING SCHEMA INFO
		//BEGIN ACCESSING DATA INFO
		
	int table_number = 0;

		while (getline(in, word)) //go through data entries line at a time
		{
			stringstream ss;
			bool data_begin = false;
			ss << word;

			while (ss >> word)
			{
				if (word == "INSERT")
				{
					ss >> word; //access table name
					remove_char(word, '.');
					if (current_table != word)
					{
						table_number++; //arrived at new table
					}
					current_table = word;
					data_begin = false;
				}

				if (word == "VALUES") //go past unneeded information to where data actually is
				{
					data_begin = true;
					ss >> word; //iterate to word after VALUE
					word = word.substr(1, word.size() - 1); //remove ( from first data entry
				}

				if (data_begin == true)
				{
					string total_word = word;
					bool single_word = false;
					if (total_word.at(0) == 'N' && total_word.at(1) == '\'') //if N' occurs
					{
						total_word = total_word.substr(2, total_word.size());
						if (total_word.size() == 0) //to account for any spaces between N' and the first word.
						{
							ss >> word;
							total_word = word;
						}
					}
					else
					{
						single_word = true;
						if (detect_time_cast(total_word) == true)
							multipleWords(ss, word, total_word);
					}

					if (total_word.at(total_word.size() - 1) == ',')// && total_word.at(total_word.size() - 2) != '\'') //more then one word in entry, go through words until hits comma
					{
						if (total_word.at(total_word.size() - 1) > 0)
						{
							if (total_word.at(total_word.size() - 2) != '\'' && single_word != true)
							{
								multipleWords(ss, word, total_word);
							}
						}

					}
					else
					{
						if (single_word != true)
							multipleWords(ss, word, total_word);
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
		all_schemas.at(table_number).add_data(data_entries);
			if (data_entries.size() > 0)
			{
				//cout << data_entries.at(0) << endl;

				if (data_entries.at(0) == "7766")
					troubleshoot();

			}
			//d	cout << counter << " completed" << endl;
			/*
			if (data_entries.size() != my_schema.num_of_columns() && data_entries.size() != 0)
			{
				for (unsigned int i = 0; i < data_entries.size(); i++)
				{
					cout << data_entries.at(i) << endl;
				}
				cout << "Entries" << data_entries.size() << endl << endl;
			}
			*/

			counter++;
			data_entries.clear();
		}

for (unsigned int i = 0; i < all_schemas.size(); i++)
{
	vector<vector<string>> all_data = all_schemas.at(i).get_all_data();
	cout << "NEW" << endl << endl;
	for (unsigned int j = 0; j < all_data.size(); j++)
	{
		vector<string> data = all_data.at(j);
		for (unsigned int k = 0; k < data.size(); k++)
		{
			cout << data.at(k) << " ";
		}
		cout << endl;
	}
}
		//END ACCESSING DATA INFO
		writeFile(all_schemas, out, all_data);
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