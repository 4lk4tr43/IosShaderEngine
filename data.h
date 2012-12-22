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
			i.seekg(start, ios::beg);
			return real_count;
		}
		else
		{
			i.open(path, ios::in | ios::binary | ios::beg);
			i.seekg(start, ios::beg);
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
#include <typeindex>
class ObjectFile
{
    static ios::pos_type get_index_position(string path, unsigned long index)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        size_t size[1];        
        ios::pos_type position = 0;
        for (unsigned long i = 0; i < index; ++i)
        {
            File::read(path, (char*)size, position, sizeof(size_t));
            position += header_size + size[0];
        }
        return position;
    }
    
public:
	static void append(string path, const char *data, type_index typeinfo, size_t size)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        char header[header_size];
        memcpy(header, &size, sizeof(size_t));
        memcpy(&header[sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        File::append(path, header, header_size);
        File::append(path, data, size);
    }
    
    static unsigned long count(string path)
    {
        unsigned long count = 0;
        auto file_size = File::size(path);        
        auto header_size = sizeof(size_t) + sizeof(type_index);
        size_t size[1];
        ios::pos_type position = 0;
        while (position < file_size)
        {
            File::read(path, (char*)size, position, sizeof(size_t));
            position += header_size + size[0];
            count++;
        }        
        return count;
    }
    
    static void get(string path, char *data, unsigned long index = 0, type_index *typeinfo = nullptr)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = get_index_position(path, index);
        char header[header_size];
        File::read(path, header, index_position, header_size);
        if (typeinfo)
            memcpy((void*)typeinfo, (const void*)&header[sizeof(size_t)], sizeof(type_index));
        File::read(path, data, (size_t)index_position + header_size, ((size_t*)header)[0]);
    }
	static char* get_new(string path, unsigned long index = 0, type_index *typeinfo = nullptr)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = get_index_position(path, index);
        char header[header_size];
        File::read(path, header, index_position, header_size);
        if (typeinfo)
            memcpy((void*)typeinfo, (const void*)&header[sizeof(size_t)], sizeof(type_index));
        return File::read_new(path, (size_t)index_position + header_size, ((size_t*)header)[0]);
    }
	
    static void push(string path, const char *data, type_index typeinfo, size_t size)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto buffer_size = header_size + size + File::size(path);
        auto buffer = new char[buffer_size];
        memcpy(buffer, (const void*)&size, sizeof(size_t));
        memcpy(&buffer[sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        memcpy(&buffer[header_size], (const void *)data, size);
        File::read(path, &buffer[header_size + size]);
        File::write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
	static void insert(string path, const char *data, unsigned long index, type_index typeinfo, size_t size)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto buffer_size = header_size + size + File::size(path);
        auto buffer = new char[buffer_size];
        auto index_position = get_index_position(path, index);
        File::read(path, buffer, 0, index_position);
        memcpy(&buffer[index_position], (const void*)&size, sizeof(size_t));
        memcpy(&buffer[(size_t)index_position + sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        File::read(path, &buffer[(size_t)index_position + header_size + size], index_position);
        File::write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
	static void remove(string path, unsigned long index = 0)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = get_index_position(path, index);
        char header[header_size];
        File::read(path, header, index_position, header_size);
        auto buffer_size = File::size(path) - header_size - ((size_t*)header)[0];
        auto buffer = new char[buffer_size];
        File::read(path, buffer, 0, index_position);
        File::read(path, &buffer[index_position], (size_t)index_position + header_size + ((size_t*)header)[0]);
        File::write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
    static type_index* type_index_new()
    {
        return (type_index*)new char[sizeof(type_index)];
    }
};

#endif
