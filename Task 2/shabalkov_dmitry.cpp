#include <iostream>
#include <map>

using namespace std;

int GetClosiestPowOfTwo(int number)
{
	int pow = 0;
	int temp = 1;
	while (temp < number)
	{
		temp *= 2;
		pow++;
	}
	return pow;
}

int PowOfTwo(int pow)
{
	int result = 1;
	for (int i = 0; i < pow; i++)
		result *= 2;
	return result;
}

class Node
{
public:
	size_t Size;
	void* Pointer;
	bool IsEmpty = true;
	Node* LeftChild;
	Node* RightChild;
	Node* Parent;

	Node(size_t size)
	{
		Pointer = malloc(size);
		Size = size;
	}

	Node(Node* parent, bool isRightChild)
	{
		Parent = parent;
		Size = parent->Size / 2;
		if (isRightChild)
		{
			parent->RightChild = this;
			Pointer = reinterpret_cast<void*>((size_t)parent->Pointer + Size);
		}
		else
		{
			parent->LeftChild = this;
			Pointer = parent->Pointer;
		}
	}

	bool IsNoChilds()
	{
		return (LeftChild == nullptr || LeftChild->IsEmpty) && (RightChild == nullptr || RightChild->IsEmpty);
	}
};

class Alloc
{
private:
	Node * _root;
	map<size_t, map<void*, Node*>> _emptyCells;
	map<void*, Node*> _usingCells;

public:

	Alloc(size_t size)
	{
		size_t powOfTwoSize = PowOfTwo(GetClosiestPowOfTwo(size));
		_root = new Node(powOfTwoSize);
		_emptyCells[powOfTwoSize][_root->Pointer] = _root;
	}

	size_t GetCellSize(size_t size)
	{
		size_t min = 8;
		while (min < size)
		{
			min = min * 2;
		}
		return min;
	}

	Node* AllocCell(size_t size)
	{
		auto first = _emptyCells[size].begin();
		Node* node = (*first).second;
		_emptyCells[size].erase(node->Pointer);
		node->IsEmpty = false;
		return node;
	}

	void* Allocate(size_t size)
	{
		size_t sizeOfRightCell = GetCellSize(size);
		size_t closiestFreeCellSize = 0;
		for (auto cellSize = sizeOfRightCell; cellSize <= _root->Size; cellSize *= 2)
		{
			if (!_emptyCells[cellSize].empty())
			{
				closiestFreeCellSize = cellSize;
				break;
			}
		}
		if (closiestFreeCellSize == 0)
			throw "No empty cells";

		Node* cell = this->AllocCell(closiestFreeCellSize);
		while (closiestFreeCellSize > sizeOfRightCell)
		{
			Node* nextCell = new Node(cell, true);
			cell = new Node(cell, false);
			cell->IsEmpty = false;
			closiestFreeCellSize /= 2;
			_emptyCells[closiestFreeCellSize][nextCell->Pointer] = nextCell;
		}
		_usingCells[cell->Pointer] = cell;
		return cell->Pointer;
	}

	void Deallocate(void* pointer)
	{
		Node* cell = _usingCells[pointer];
		_usingCells.erase(pointer);
		cell->IsEmpty = true;
		_emptyCells[cell->Size][cell->Pointer] = cell;
		while (cell->Parent != nullptr && cell->Parent->IsNoChilds())
		{
			size_t size = cell->Size;
			cell = cell->Parent;
			_emptyCells[size].erase(cell->RightChild->Pointer);
			_emptyCells[size].erase(cell->LeftChild->Pointer);
			cell->IsEmpty = true;
			_emptyCells[cell->Size][cell->Pointer] = cell;
			cell->RightChild = nullptr;
			cell->LeftChild = nullptr;
		}
	}

	void Dump()
	{
		cout << "Allocator size : " << _root->Size << endl;
		cout << "Using cells : " << endl;
		for (auto first = _usingCells.begin(); first != _usingCells.end(); first++)
			cout << "	Pointer : " << (*first).first << " Size : " << (*first).second->Size << endl;
		cout << "Empty blocks : " << endl;
		for (auto first = _emptyCells.begin(); first != _emptyCells.end(); first++)
		{
			if (!_emptyCells[(*first).first].empty())
			{
				cout << "Size : " << (*first).first << endl;
				for (auto begin = _emptyCells[(*first).first].begin(); begin != _emptyCells[(*first).first].end(); begin++)
					cout << "	Pointer : " << (*begin).first << endl;
			}
		}
	}
};

int main()
{
	Alloc* alloc = new Alloc(1000);
	cout << "Empty allocator"<< '\n' << endl;
	alloc->Dump();
	void *pointer1 = alloc->Allocate(360);
	cout << "360 bytes allocated" << '\n' << endl;
	alloc->Dump();
	void *pointer2 = alloc->Allocate(124);
	cout << "124 more bytes allocated" << '\n' <<  endl;
	alloc->Dump();
	alloc->Deallocate(pointer1);
	cout << "Pointer1 deallocated" << '\n' << endl;
	alloc->Dump();
	system("pause");
}