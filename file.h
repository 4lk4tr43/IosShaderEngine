#ifndef file_h__
#define file_h__

#include <cstdio>
#include <fstream>
#include <string>

#include <iostream>
using namespace std;

#ifndef FILE_WRITE_SERIALIZED
#define FILE_WRITE_SERIALIZED(path, object_ptr) {size_t _file_serialized_size; \
	char *_file_data_serialized = (object_ptr)->SerializeNew(&_file_serialized_size); \
	File::Write(path, _file_data_serialized, _file_serialized_size); delete[] _file_data_serialized;}
#endif

#ifndef FILE_READ_SERIALIZED_NEW
#define FILE_READ_SERIALIZED(path, variable, class_name) {char *_file_data_serialized = File::ReadNew(path); \
	variable = class_name::Deserialize(_file_data_serialized); \
	delete[] _file_data_serialized;}
#endif

class File
{
	static void FileOutBinary(ofstream &o, const char *data, streamsize count)
	{
		o.write(data, count);
		o.close();
	}
    
	static streamsize OpenBinary(streamsize count, ifstream &i, string path, ios::pos_type start)
	{
		if (!count)
		{
			i.open(path.c_str(), ios::in | ios::binary | ios::ate);
			auto real_count = i.tellg();
			i.seekg(start, ios::beg);
			return real_count;
		}
		else
		{
			i.open(path.c_str(), ios::in | ios::binary);
			i.seekg(start, ios::beg);
			return count;
		}
	}
    
public:
	static void Append(string path, const char *data, streamsize count)
	{
		ofstream o(path.c_str(), ios::out | ios::binary | ios::app);
		FileOutBinary(o, data, count);
	}
    
	static void Delete(string path)
	{
        remove(path.c_str());
	}
    
    static bool Exists(string path)
    {
        ifstream i(path.c_str());
        return i.good();
    }
    
	static void Read(string path, char *buffer, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		i.read(buffer, OpenBinary(count, i, path, start));
		i.close();
	}

	static char* ReadNew(string path, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		auto real_count = OpenBinary(count, i, path, start);
		char *buffer = new char[(unsigned long)real_count];
		i.read(buffer, real_count);
		i.close();
		return buffer;
	}
    
    static string ReadString(string path)
    {
        ifstream i(path.c_str());
        string content_string;
        content_string.assign(istreambuf_iterator<char>(i), istreambuf_iterator<char>());
        i.close();
        return content_string;
    }
    
	static streamsize Size(string path)
	{
		ifstream i(path.c_str(), ios::in | ios::binary | ios::ate);
		auto size = i.tellg();
		i.close();
		return size;
	}

	static void Write(string path, const char *data, streamsize count)
	{
		ofstream o(path.c_str(), ios::out | ios::binary | ios::trunc);
		FileOutBinary(o, data, count);
	}
};

#endif
