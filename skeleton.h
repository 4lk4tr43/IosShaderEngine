#ifndef skeleton_h__
#define skeleton_h__

#include "transform.h"
#include "tree_node.h"

class Skeleton
{
	vector<TreeNode<Transform>*> _bones;
	vector<Transform> _bone_offsets;

public:
	Skeleton(vector<TreeNode<Transform>*> bones, vector<Transform> bone_offsets)
	{
		_bones = bones;
		_bone_offsets = bone_offsets;
	}

	~Skeleton()
	{
		_bones[0]->ReleaseTree();
	}

	static Skeleton* DeserializeNew(char *serialized_data)
	{
		MemoryBuilder memory_builder;
		memory_builder.PointToSource(serialized_data);
		auto offsets_vector = memory_builder.PopVector<Transform>();
		auto structure_vector = memory_builder.PopVector<unsigned int>();
		auto values_vector = memory_builder.PopVector<Transform>();
		auto bones_tree = TreeNode<Transform>::CreateTreeFromStructureNew(structure_vector, values_vector);
		auto indices_vector = memory_builder.PopVector<unsigned int>();
		vector<TreeNode<Transform>*> bones_vector;
		for (auto iter = indices_vector.begin(); iter != indices_vector.end(); iter++)
			bones_vector.push_back(bones_tree->GetNode(*iter));
		return new Skeleton(bones_vector, offsets_vector);
	}

	TreeNode<Transform>* GetBone(unsigned int index)
	{
		return _bones[index];
	}

	Transform GetAbsoluteTransformation(unsigned int index)
	{
		auto bone = GetBone(index);
		auto parent = bone->parent;
		auto current_transform = _bone_offsets[index] * bone->value;
		while (parent)
		{
			current_transform = current_transform * parent->value;
			parent = parent->parent;
		}
		return current_transform;
	}

	vector<Transform> GetAbsoluteTransformations()
	{
		vector<Transform> result;
		for (unsigned int i = 0; i < _bones.size(); ++i)
			result.push_back(this->GetAbsoluteTransformation(i));
		return result;
	}

	Transform& operator[](unsigned int index)
	{
		return GetBone(index)->value;
	}

	char* SerializeNew(size_t* size = nullptr)
	{
		MemoryBuilder memory_builder;
		memory_builder.PushVector(_bone_offsets);
		memory_builder.PushVector(_bones[0]->GetStructure());
		memory_builder.PushVector(_bones[0]->GetBreadthFirstSearchValues());
		vector<unsigned int> bone_indices;
		for (auto iter = _bones.begin(); iter != _bones.end(); iter++)
			bone_indices.push_back((*iter)->GetIndex());
		memory_builder.PushVector(bone_indices);
		return memory_builder.GetMemoryNew(size);
	}
};

#endif