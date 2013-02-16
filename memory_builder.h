#ifndef memory_builder_h__
#define memory_builder_h__

#include <iostream>

class MemoryBuilder
{
	bool _has_allocated;
	char *_memory;
	char *_last_pop;
	size_t _current_size;

	void ChangeObjectLast(bool last, char *object)
	{
		((bool*)&object[sizeof(size_t)])[0] = last;
	}

	char* GetLastObject(char *object)
	{
		auto position = object;
		while (!IsLastObject(position))
			position = GetNextObject(position);
		return position;
	}

	char* GetObjectData(char *object)
	{
		return &object[sizeof(size_t) + sizeof(bool)];
	}

	size_t GetObjectSize(char *object)
	{
		return ((size_t*)object)[0];
	}

	char* GetNextObject(char *object)
	{
		return &object[sizeof(size_t) + sizeof(bool) + ((size_t*)object)[0]];
	}

	bool IsLastObject(char *object)
	{
		return ((bool*)&object[sizeof(size_t)])[0];
	}

public:
	MemoryBuilder()
	{
		_has_allocated = false;
		_memory = nullptr;
		_last_pop = nullptr;
		_current_size = 0;
	}

	~MemoryBuilder()
	{
		if (_has_allocated)
			delete[] _memory;
	}

	template <class T>
	static vector<T> CopyMemoryToVector(T* memory, size_t element_count)
	{
		vector<T> result;
		for (size_t i = 0; i < element_count; ++i)
			result.push_back(memory[i]);
		return result;
	}

	char* GetMemory(size_t *size = nullptr)
	{
		if (size)
			*size = _current_size;
		return _memory;
	}

	char* GetMemoryNew(size_t *size = nullptr)
	{
		if (size)
			*size = _current_size;
		auto clone = new char[_current_size];
		memcpy(clone, _memory, _current_size);
		return clone;
	}

	void PointToSource(char *src)
	{
		if (_has_allocated)
			delete[] _memory;
		_has_allocated = false;
		_memory = src;
		_last_pop = _memory;
		size_t new_size = 0;
		char *position = _memory;
		while (true)
		{
			new_size += sizeof(size_t) + sizeof(bool) + GetObjectSize(position);
			if (IsLastObject(position))
				break;
			position = GetNextObject(position);
		}
		_current_size = new_size;
	}

	char* Pop(size_t *size = nullptr)
	{
		if (!_last_pop)
			return nullptr;
		auto object_size = GetObjectSize(_last_pop);
		if (size)
			*size = object_size;
		auto object = GetObjectData(_last_pop);
		if (IsLastObject(_last_pop))
			_last_pop = nullptr;
		else
			_last_pop = GetNextObject(_last_pop);
		return object;
	}

	template <class T>
	vector<T> PopVector()
	{
		auto element_count = *(size_t*)Pop();
		return CopyMemoryToVector((T*)Pop(), element_count);
	}

	void Push(void* src, size_t size)
	{
		auto new_size = _current_size + sizeof(size_t) + sizeof(bool) + size;
		auto new_memory = new char[new_size];
		auto new_object = &new_memory[_current_size];
		((size_t*)new_object)[0] = size;
		ChangeObjectLast(true, new_object);
		memcpy(GetObjectData(new_object), src, size);
		if (_memory)
		{
			ChangeObjectLast(false, GetLastObject(_memory));
			memcpy(new_memory, _memory, _current_size);
			if (_has_allocated)
				delete[] _memory;
		}
		_has_allocated = true;
		_current_size = new_size;
		_memory = new_memory;
		_last_pop = _memory;
	}

	template <class T>
	void PushVector(vector<T> &vec)
	{
		auto vector_size = vec.size();
		Push(&vector_size, sizeof(size_t));
		Push(&vec[0], sizeof(T) * vector_size);
	}
};

#endif
