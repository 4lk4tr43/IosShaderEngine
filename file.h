#ifndef file_h__
#define file_h__

#include <cstdio>
#include <fstream>
#include <string>
using namespace std;

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
	static void Append(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::app);
		FileOutBinary(o, data, count);
	}
    
	static void Delete(string path)
	{
        remove(path.c_str());
	}
    
    static bool Exists(string path)
    {
        ifstream i(path);
        return i.good();
    }
    
	static void Read(string path, char *buffer, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		i.Read(buffer, OpenBinary(count, i, path, start));
		i.close();
	}

	static char* ReadNew(string path, ios::pos_type start = 0, streamsize count = 0)
	{
		ifstream i;
		auto real_count = OpenBinary(count, i, path, start);
		char *buffer = new char[(unsigned long)real_count];
		i.Read(buffer, real_count);
		i.close();
		return buffer;
	}
    
	static streamsize Size(string path)
	{
		ifstream i(path, ios::in | ios::binary | ios::ate);
		auto size = i.tellg();
		i.close();
		return size;
	}

	static void Write(string path, const char *data, streamsize count)
	{
		ofstream o(path, ios::out | ios::binary | ios::trunc);
		FileOutBinary(o, data, count);
	}
};

#endif
