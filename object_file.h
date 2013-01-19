#ifndef object_file_h__
#define object_file_h__

#include <memory>
//#include <typeindex>
#include <typeinfo>

#include "file.h"

typedef type_info type_index;

class ObjectFile
{
    static ios::pos_type GetIndexPosition(string path, unsigned long index)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        size_t size[1];
        ios::pos_type position = 0;
        for (unsigned long i = 0; i < index; ++i)
        {
            File::Read(path, (char*)size, position, sizeof(size_t));
            position += header_size + size[0];
        }
        return position;
    }
    
public:
	static void Append(string path, const char *data, type_index typeinfo, size_t size)
    {
        const size_t header_size = sizeof(size_t) + sizeof(type_index);
        char header[header_size];
        memcpy(header, &size, sizeof(size_t));
        memcpy(&header[sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        File::Append(path, header, header_size);
        File::Append(path, data, size);
    }
    
    static unsigned long Count(string path)
    {
        unsigned long count = 0;
        auto file_size = File::Size(path);
        auto header_size = sizeof(size_t) + sizeof(type_index);
        size_t size[1];
        ios::pos_type position = 0;
        while (position < file_size)
        {
            File::Read(path, (char*)size, position, sizeof(size_t));
            position += header_size + size[0];
            count++;
        }
        return count;
    }
    
    static void Get(string path, char *data, unsigned long index, type_index *typeinfo = nullptr)
    {
        const size_t header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = GetIndexPosition(path, index);
        char header[header_size];
        File::Read(path, header, index_position, header_size);
        if (typeinfo)
            memcpy((void*)typeinfo, (const void*)&header[sizeof(size_t)], sizeof(type_index));
        File::Read(path, data, (size_t)index_position + header_size, ((size_t*)header)[0]);
    }
    
	static char* GetNew(string path, unsigned long index, type_index *typeinfo = nullptr)
    {
        const size_t header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = GetIndexPosition(path, index);
        char header[header_size];
        File::Read(path, header, index_position, header_size);
        if (typeinfo)
            memcpy((void*)typeinfo, (const void*)&header[sizeof(size_t)], sizeof(type_index));
        return File::ReadNew(path, (size_t)index_position + header_size, ((size_t*)header)[0]);
    }
	
	static void Insert(string path, const char *data, unsigned long index, type_index typeinfo, size_t size)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto buffer_size = header_size + size + File::Size(path);
        auto buffer = new char[(unsigned int)buffer_size];
        auto index_position = GetIndexPosition(path, index);
        File::Read(path, buffer, 0, index_position);
        memcpy(&buffer[(unsigned int)index_position], (const void*)&size, sizeof(size_t));
        memcpy(&buffer[(size_t)index_position + sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        File::Read(path, &buffer[(size_t)index_position + header_size + size], index_position);
        File::Write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
    static void Push(string path, const char *data, type_index typeinfo, size_t size)
    {
        auto header_size = sizeof(size_t) + sizeof(type_index);
        auto buffer_size = header_size + size + File::Size(path);
        auto buffer = new char[(unsigned int)buffer_size];
        memcpy(buffer, (const void*)&size, sizeof(size_t));
        memcpy(&buffer[sizeof(size_t)], (const void*)&typeinfo, sizeof(type_index));
        memcpy(&buffer[header_size], (const void *)data, size);
        File::Read(path, &buffer[header_size + size]);
        File::Write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
	static void Remove(string path, unsigned long index)
    {
        const size_t header_size = sizeof(size_t) + sizeof(type_index);
        auto index_position = GetIndexPosition(path, index);
        char header[header_size];
        File::Read(path, header, index_position, header_size);
        auto buffer_size = File::Size(path) - header_size - ((size_t*)header)[0];
        auto buffer = new char[(unsigned int)buffer_size];
        File::Read(path, buffer, 0, index_position);
        File::Read(path, &buffer[(unsigned int)index_position], (size_t)index_position + header_size + ((size_t*)header)[0]);
        File::Write(path, buffer, buffer_size);
        delete[] buffer;
    }
    
    static type_index* TypeIndexNew()
    {
        return (type_index*)new char[sizeof(type_index)];
    }
};

#endif
