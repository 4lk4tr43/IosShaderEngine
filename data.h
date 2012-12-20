#ifndef data_h__
#define data_h__

#include <cstdio>
#include <fstream>
#include <string>
using namespace std;

class Path
{
public:
	static string folder(string path)
	{
		size_t pos = path.find_last_of("/");
		if (pos != string::npos) return path.substr(0, pos + 1);
		throw invalid_argument("Couldn't determine folder of path string.");
	}
    
	static string file(string path)
	{
		size_t pos = path.find_last_of("/");
		if (pos != string::npos)
		{
			string fileName = path.substr(pos + 1);
			if (fileName.length() != 0)	return fileName;
		}
		throw invalid_argument("Couldn't determine file of path string.");
	}
};

class File
{
	static void file_out_binary(ofstream &o, const char * data, streamsize count)
	{
		o.write(data, count);
		o.close();
	}
    
	static streamsize open_binary(streamsize count, ifstream &i, string path, ios::pos_type start)
	{
		if (!count)
		{
			i.open(path, ios::in | ios::binary | ios::ate);
			auto real_count = i.tellg();
			if (!start) i.seekg(start, ios::beg);
			return real_count;
		}
		else
		{
			i.open(path, ios::in | ios::binary | ios::beg);
			if (!start) i.seekg(start, ios::beg);
			return count;
		}
	}
    
public:
	static void append(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::app);
		file_out_binary(o, data, count);
	}
    
	static void del(string path)
	{
        remove(path.c_str());
	}
    
    static bool exists(string path)
    {
        ifstream i(path);
        return i.good();
    }
    
	static void read(string path, char *buffer, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		i.read(buffer, open_binary(count, i, path, start));
		i.close();
	}

	static char* read_new(string path, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		auto real_count = open_binary(count, i, path, start);
		char *buffer = new char[(unsigned long)real_count];
		i.read(buffer, real_count);
		i.close();
		return buffer;
	}
    
	static streamsize size(string path)
	{
		ifstream i(path, ios::in | ios::binary | ios::ate);
		auto size = i.tellg();
		i.close();
		return size;
	}

	static void write(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::trunc);
		file_out_binary(o, data, count);
	}
};

#include <memory>
#include <typeinfo>

class ObjectFile
{
public:
	void append(string path, const char *data, type_info typeinfo, size_t size)
    {
        size_t header_bytes = sizeof(size_t) + sizeof(type_info);
        size_t total_bytes =  header_bytes + size;
        
        char header[header_bytes];
        memcpy((void*)header, (const void*)&total_bytes, sizeof(size_t));
        memcpy((void*)&header[sizeof(size_t)], (const void*)&typeinfo, sizeof(type_info));
        
        File::append(path, header, header_bytes);
        File::append(path, data, size);
    }
    
    unsigned long count(string path) { return 0; }
    void get(string path, const char *data, unsigned long index = 0) {}
	char* get_new(string path, unsigned long index = 0) { return nullptr; }
	void push(string path, const char *data) {}
	void insert(string path, const char *data, unsigned long index) {}
	void remove(string path, unsigned long index = 0) {}
};

#endif
