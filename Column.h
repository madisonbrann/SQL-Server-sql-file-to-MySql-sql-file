#ifndef COLUMN
#define COLUMN

#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Column
{
private:
	string name;
	string type;
	string type_value = "uninitialized";
	string required;
	
public:
	Column(string name_in, string type_in, string required_in)
	{
		name = name_in;
		type = type_in;
		required = required_in;

		remove_char(name, '[');
		remove_char(name, ']');
		remove_char(type, '[');
		remove_char(type, ']');
		remove_char(required, ',');

		check_for_type_value(); //checks if type value is attached to the type
		check_null(); //see if required is null or not and adjust required
	}
	virtual ~Column()
	{

	}


	void check_null()
	{
		if (required == "NOT")
		{
			required = "NOT NULL";
		}

		else if (required == "NULL")
		{
			required = "DEFAULT NULL";
		}
		else
		{
			required = "unexpected \"required\" type";
		}
		
	}

	void check_for_type_value()
	{
		int begin_type;
		bool type_exists = false;
		string creating_type_value;

		for (unsigned int i = 0; i < type.size(); i++) //goes through type string to find locations of ()
		{
			
			if (type.at(i) == '(')
			{
				begin_type = i;
				type_exists = true;
			}
			if (type_exists == true)
			{
				creating_type_value.push_back(type.at(i));
			}
		}
		if (type_exists == true)
		{
			type_value = creating_type_value;
			type.erase(type.begin() + begin_type, type.end());
		}

	}

	void remove_char(string& input, char type)
	{
		input.erase(remove(input.begin(), input.end(), type), input.end());
	}

	string get_name()
	{
		return name;
	}

	string get_type()
	{
		return type;
	}

	string get_type_value()
	{
		return type_value;
	}

	string get_required()
	{
		return required;
	}

};

#endif