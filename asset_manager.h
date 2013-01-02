#ifndef asset_manager_h__
#define asset_manager_h__

#include <exception>
#include <map>
#include <tuple>
using namespace std;

#include "object_file.h"
#include "string_tokenizer.h"

class AssetManager
{
public:
	class FileNotFoundException : public exception {};
	class UnknownAssetType : public exception {};

	enum AssetType
	{
		DATA,
		MESH,
		POSTSHADER,
		SHADER,
		STRING
	};

private:
    map<AssetType, map<string,  tuple<char*, unsigned int>>*> _loaded_assets;

    string FindFolderForFile(string file_name)
    {
        size_t folder_index = -1;
        for (size_t i = 0; i < base_folders->LineCount(); ++i)
        {
            if (File::Exists(base_folders->operator[](i) + file_name))
            {
                folder_index = i;
                break;
            }
        }
        if (folder_index == -1)
            throw FileNotFoundException();
        return base_folders->operator[](folder_index);
    }
    
	string GenerateAssetKey(string file_name_or_absolute_path) 
	{
		string key;
		if (File::Exists(file_name_or_absolute_path))
			return file_name_or_absolute_path;
		else
			return FindFolderForFile(file_name_or_absolute_path) + file_name_or_absolute_path;
	}

	map<string,  tuple<char*, unsigned int>>* GetAssetTypeEntry(AssetType asset_type)
	{
		if (!_loaded_assets.count(asset_type))
			_loaded_assets[asset_type] = new map<string,  tuple<char*, unsigned int>>();
		return _loaded_assets[asset_type];
	}

	char* IncrementAssetReferenceCount(string key, map<string,  tuple<char*, unsigned int>> *asset_type_map) 
	{
		auto value = asset_type_map->operator[](key);
		auto result = get<0>(value);
		asset_type_map->operator[](key) = make_tuple(result, get<1>(value) + 1);
		return result;
	}

    char* LoadDataNew(string absolute_file_path)
    {
		return File::ReadNew(absolute_file_path);
    }

	char * LoadMeshNew(string absolute_file_path, GLenum vertex_usage, GLenum index_usage) 
	{
		auto vertex_array_object = new VertexArrayObject();
		auto vertex_count = (GLsizei*)ObjectFile::GetNew(absolute_file_path, 0);
		auto vertex_description = (VertexDescription*)ObjectFile::GetNew(absolute_file_path, 1);
		auto primitive_mode = (GLenum*)ObjectFile::GetNew(absolute_file_path, 2);
		auto vertices = (GLvoid*)ObjectFile::GetNew(absolute_file_path, 3);
		vertex_array_object->AddPackedVertices(vertex_usage, *primitive_mode, *vertex_description, vertices, *vertex_count);
		delete[] vertex_count;
		delete[] vertex_description;
		delete[] primitive_mode;
		delete[] vertices;
		auto index_count = (GLsizei*)ObjectFile::GetNew(absolute_file_path, 4);
		if (!*index_count)
			return (char*)vertex_array_object;
		auto index_type = (GLenum*)ObjectFile::GetNew(absolute_file_path, 5);
		auto indices = (GLvoid*)ObjectFile::GetNew(absolute_file_path, 6);
		vertex_array_object->AddIndices(index_usage, *index_type, indices, *index_count);
		delete[] index_count;
		delete[] index_type;		
		delete[] vertices;
		return (char*)vertex_array_object;
	}

	char* LoadPostShaderNew(string absolute_file_path, GLchar *uniform_names_seperated_by_comma)
	{
		string error_log;
		auto shader_string = File::ReadString(absolute_file_path);
		auto result = new PostShader((GLchar*)shader_string.c_str(), uniform_names_seperated_by_comma, &error_log);
		if (error_log.length())
			cout << error_log << endl;
		return (char*)result;
	}

	char * LoadShaderNew(string absolute_file_path_vertex, string absolute_file_path_fragment, GLchar* attribute_names_seperated_by_comma, GLchar * uniform_names_seperated_by_comma) 
	{
		string error_log;
		auto shader_string_vertex = File::ReadString(absolute_file_path_vertex);
		auto shader_string_fragment = File::ReadString(absolute_file_path_fragment);
		auto result = new Shader((GLchar*)shader_string_vertex.c_str(), (GLchar*)shader_string_fragment.c_str(), attribute_names_seperated_by_comma, uniform_names_seperated_by_comma, &error_log);
		if (error_log.length())
			cout << error_log << endl;
		return (char*)result;
	}

	char* LoadStringNew(string absolute_file_path)
	{
		return (char*)new string(File::ReadString(absolute_file_path));
	}

	void Unload(string key, map<string,  tuple<char*, unsigned int>> *asset_type_map, AssetType asset_type, bool erase = true) 
	{
		auto value = asset_type_map->operator[](key);
		switch (asset_type)
		{
		case AssetType::DATA:
			delete[] get<0>(value);
			break;
		case AssetType::MESH:
			delete (VertexArrayObject*)get<0>(value);
			break;
		case AssetType::POSTSHADER:
			delete (PostShader*)get<0>(value);
			break;
		case AssetType::SHADER:
			delete (Shader*)get<0>(value);
			break;
		case AssetType::STRING:
			delete (string*)get<0>(value);
			break;
		default:
			throw UnknownAssetType();
		}
		if (erase)
			asset_type_map->erase(key);
	}

public:
	StringTokenizer *base_folders;
    unsigned int root_folder_index;

	AssetManager(string folders_seperated_by_comma = "")
	{
		base_folders = new StringTokenizer(folders_seperated_by_comma, ",");
		root_folder_index = 0;
	}
    
    ~AssetManager()
    {
        for(auto iter = _loaded_assets.begin(); iter != _loaded_assets.end(); ++iter)
		{
			auto asset_type_map = iter->second;
			for(auto iter_asset_type = asset_type_map->begin(); iter_asset_type != asset_type_map->end(); ++iter_asset_type)
				Unload(iter_asset_type->first, asset_type_map, iter->first, false);
			asset_type_map->clear();
			delete asset_type_map;
		}
		_loaded_assets.clear();
        delete base_folders;
    }

    char* GetData(string file_name_or_absolute_path)
    {
		auto asset_type_map = GetAssetTypeEntry(AssetType::DATA);
		auto key = GenerateAssetKey(file_name_or_absolute_path);
		char *result;
        if (!asset_type_map->count(key))
		{
			result = LoadDataNew(key);
            asset_type_map->operator[](key) = make_tuple(result, 1);
		}
		else
			result = IncrementAssetReferenceCount(key, asset_type_map);
		return result;
    }

	VertexArrayObject* GetMesh(string file_name_or_absolute_path, GLenum vertex_usage, GLenum index_usage)
	{
		auto asset_type_map = GetAssetTypeEntry(AssetType::MESH);
		auto key = GenerateAssetKey(file_name_or_absolute_path);
		char *result;
		if (!asset_type_map->count(key))
		{
			result = LoadMeshNew(file_name_or_absolute_path, vertex_usage, index_usage);
			asset_type_map->operator[](key) = make_tuple(result, 1);
		}
		else
			result = IncrementAssetReferenceCount(key, asset_type_map);
		return (VertexArrayObject*)result;
	}

	PostShader* GetPostShader(string file_name_or_absolute_path, GLchar *uniform_names_seperated_by_comma = nullptr)
	{
		auto asset_type_map = GetAssetTypeEntry(AssetType::POSTSHADER);
		auto key = GenerateAssetKey(file_name_or_absolute_path);
		char *result;
		if (!asset_type_map->count(key))
		{
			result = LoadPostShaderNew(key, uniform_names_seperated_by_comma);
			asset_type_map->operator[](key) = make_tuple(result, 1);
		}
		else
			result = IncrementAssetReferenceCount(key, asset_type_map);
		return (PostShader*)result;
	}

	Shader* GetShader(string file_name_or_absolute_path_vertex, string file_name_or_absolute_path_fragment, GLchar* attribute_names_seperated_by_comma, GLchar *uniform_names_seperated_by_comma = nullptr)
	{
		auto asset_type_map = GetAssetTypeEntry(AssetType::POSTSHADER);
		auto key_vertex = GenerateAssetKey(file_name_or_absolute_path_vertex);
		auto key_fragment = GenerateAssetKey(file_name_or_absolute_path_fragment);
		auto key = key_vertex + key_fragment;
		char *result;
		if (!asset_type_map->count(key))
		{
			result = LoadShaderNew(key_vertex, key_fragment, attribute_names_seperated_by_comma, uniform_names_seperated_by_comma);
			asset_type_map->operator[](key) = make_tuple(result, 1);
		}
		else
			result = IncrementAssetReferenceCount(key, asset_type_map);
		return (Shader*)result;
	}

	string* GetString(string file_name_or_absolute_path) 
	{
		auto asset_type_map = GetAssetTypeEntry(AssetType::POSTSHADER);
		auto key = GenerateAssetKey(file_name_or_absolute_path);
		char *result;
		if (!asset_type_map->count(key))
		{
			result = LoadStringNew(key);
			asset_type_map->operator[](key) = make_tuple(result, 1);
		}
		else
			result = IncrementAssetReferenceCount(key, asset_type_map);
		return (string*)result;
	}

    void Release(string file_name_or_absolute_path, AssetType asset_type)
    {        
		if (!_loaded_assets.count(asset_type))
			return;
		auto asset_type_map = _loaded_assets[asset_type];
		auto key = GenerateAssetKey(file_name_or_absolute_path);
        if (!asset_type_map->count(key))
			return;
		auto value = asset_type_map->operator[](key);
		if (get<1>(value) == 1)
		{
			Unload(key, asset_type_map, asset_type);
			if (!asset_type_map->size())
			{
				delete asset_type_map;
				_loaded_assets.erase(asset_type);
			}
		}
		else
			asset_type_map->operator[](key) = make_tuple(get<0>(value), get<1>(value) - 1); 
    }
};

#endif
