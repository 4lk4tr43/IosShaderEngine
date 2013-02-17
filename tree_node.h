#ifndef tree_node_h__
#define tree_node_h__

#include <algorithm>
#include <vector>
#include <functional>
using namespace std;

template <class T>
class TreeNode
{
public:
	class InvalideFringeException : public exception {};

private:
	void AddChildrenToFringe(vector<TreeNode<T>*> &fringe)
	{
		for (auto iter = children.begin(); iter != children.end(); iter++)
			fringe.push_back(*iter);
		for (auto iter = children.begin(); iter != children.end(); iter++)
			(*iter)->AddChildrenToFringe(fringe);
	}

	void AddChildrenToFringe(vector<TreeNode<T>*> &fringe, unsigned int *remaining_elements_to_add)
	{
		if (*remaining_elements_to_add == 0)
			return;

		for (auto iter = children.begin(); iter != children.end(); iter++)
		{
			if (*remaining_elements_to_add == 0)
				return;
			fringe.push_back(*iter);
			--*remaining_elements_to_add;
		}
		for (auto iter = children.begin(); iter != children.end(); iter++)
			(*iter)->AddChildrenToFringe(fringe, remaining_elements_to_add);
	}

public:
	T value;
	TreeNode<T> *parent;
	vector<TreeNode<T>*> children;

	TreeNode<T>(T value)
	{
		parent = nullptr;
		this->value = value;
	}

	void AddChildNode(TreeNode<T>* child)
	{
		child->parent = this;
		children.push_back(child);
	}

	void AddChildValueNew(T value)
	{
		AddChildNode(new TreeNode<T>(value));
	}

	template <class C>
	TreeNode<C>* CloneTreeNew(function<C(T)> cast_function, TreeNode<C>* root = nullptr)
	{
		auto node = new TreeNode<C>(cast_function(this->value));
		for (auto iter = children.begin(); iter != children.end(); iter++)
			(*iter)->template CloneTreeNew<C>(cast_function, node);
		if (root)
			root->AddChildNode(node);
		else
			return node;
		return nullptr;
	}

	static TreeNode<T>* CreateTreeFromStructureNew(vector<unsigned int> &structure, vector<T> &breadth_first_values)
	{
		unsigned int node_count = 0;
		for (auto iter = structure.begin(); iter != structure.end(); iter++)
			node_count = std::max(*iter, node_count);
		++node_count;
		vector<TreeNode<T>*> nodes;
		for (unsigned int i = 0; i < node_count; ++i)
			nodes.push_back(new TreeNode(breadth_first_values[i]));

		for (unsigned int i = 0; i < structure.size(); ++i)
		{
			for (unsigned int j = i + 1; j < structure.size(); ++j)
			{
				if (structure[i] == structure[j])
				{
					i = j;
					break;
				}
				nodes[structure[i]]->AddChildNode(nodes[structure[j]]);
			}
		}

		return nodes[0];
	}

	vector<unsigned int> GetStructure()
	{
		vector<unsigned int> result; 
		auto fringe = GetBreadthFirstSearchFringe();
		for (unsigned int i = 0; i < fringe.size(); ++i)
		{
			auto node = fringe[i];
			result.push_back(i);
			for (auto iter = node->children.begin(); iter != node->children.end(); iter++)
			{
				for (unsigned int j = 0; j < fringe.size(); ++j)
				{
					if (*iter == fringe[j])
					{
						result.push_back(j);
						break;
					}
				}
			}
			result.push_back(i);
		}
		return result;
	}

	unsigned int GetIndex(vector<TreeNode<T>*> *fringe = nullptr)
	{
		auto fringe_reference = (fringe) ? *fringe : this->GetRoot()->GetBreadthFirstSearchFringe();

		for (unsigned int i = 0; i < fringe_reference.size(); ++i)
		{
			if (this == fringe_reference[i])
				return i;
		}
		throw InvalideFringeException();
	}

	void ForEach(function<void(TreeNode<T>*)> method, bool depth_first = false, TreeNode<T>* recursion_parameter = nullptr)
	{
		if (!recursion_parameter)
			recursion_parameter = this->GetRoot();

		if (depth_first)
		{
			method(recursion_parameter);
			for (auto iter = recursion_parameter->children.begin(); iter != recursion_parameter->children.end(); iter++)
				ForEach(method, depth_first, *iter);
		}
		else
		{
			for (auto iter = recursion_parameter->children.begin(); iter != recursion_parameter->children.end(); iter++)
				ForEach(method, depth_first, *iter);
			method(recursion_parameter);
		}
	}

	void ForEachInSubTree(function<void(TreeNode<T>*)> method, bool depth_first = false, TreeNode<T>* recursion_parameter = nullptr)
	{
		ForEach(method, depth_first, this);
	}

	vector<TreeNode<T>*> GetBreadthFirstSearchFringe()
	{
		vector<TreeNode<T>*> fringe;
		fringe.push_back(this);
		AddChildrenToFringe(fringe);
		return fringe;
	}

	vector<TreeNode<T>*> GetBreadthFirstSearchFringe(unsigned int last_element)
	{
		vector<TreeNode<T>*> fringe;
		fringe.push_back(this);
		auto counter = new unsigned int();
		*counter = last_element;
		AddChildrenToFringe(fringe, counter);
		delete counter;
		return fringe;
	}

	vector<T> GetBreadthFirstSearchValues()
	{
		auto fringe = GetBreadthFirstSearchFringe();
		vector<T> result;
		for (auto iter = fringe.begin(); iter != fringe.end(); iter++)
			result.push_back((*iter)->value);
		return result;
	}

	TreeNode<T>* GetNode(unsigned int index, unsigned int recursion_value_counter = 0)
	{
		return GetRoot()->GetBreadthFirstSearchFringe(index)[index];
	}

	TreeNode<T>* GetRoot()
	{
		TreeNode<T>* parent = this;
		while (parent->parent)
			parent = parent->parent;
		return parent;
	}

	bool IsRoot()
	{
		return parent == nullptr;
	}

	T& operator [](unsigned int index)
	{
		return this->GetNode(index)->value;
	}

	void RemoveFromTree()
	{
		if (parent)
		{
			for (unsigned int i = 0; i < parent->children.size(); ++i)
			{
				if(parent->children[i] == this)
				{
					parent->children.erase(parent->children.begin() + i);
					break;
				}
			}

			for (auto iter = children.begin(); iter != children.end(); iter++)
			{
				parent->children.push_back(*iter);
				(*iter)->parent = this->parent;
			}
		}
		else
		{
			for (auto iter = children.begin(); iter != children.end(); iter++)
			{
				(*iter)->parent = nullptr;
			}
		}
	}

	// values are not deleted
	void ReleaseTree()
	{
		ForEach([](TreeNode<T>* node){delete node;});
	}

	void ReleaseSubTree()
	{
		parent = nullptr;
		ReleaseTree();
	}

	size_t Size()
	{
		return GetBreadthFirstSearchFringe().size();
	}
};

#endif
