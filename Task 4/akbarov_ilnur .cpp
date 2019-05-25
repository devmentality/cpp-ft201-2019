#include <iostream>
#include <functional>

using namespace std;


struct Node
{
public:
    int value;
    Node* left = nullptr;
    Node* right = nullptr;
    int height = 1;

    Node(int value)
    {
        this->value = value;
    }
};


class AVL
{
    Node* root;

    int GetHeight(Node *p)
    {
        return p ? p->height : 0;
    }

    int GetBalance(Node *p)
    {
        return GetHeight(p->right) - GetHeight(p->left);
    }

    void SetActualHeight(Node *p)
    {
        int leftHeight = GetHeight(p->left);
        int rightHeight = GetHeight(p->right);
        p->height = max(leftHeight, rightHeight) + 1;
    }

    Node* GetBalancedTree(Node *p)
    {
        SetActualHeight(p);
        if (GetBalance(p)==2)
        {
            if (GetBalance(p->right) < 0)
                p->right = RotateRight(p->right);
            return RotateLeft(p);
        }
        if (GetBalance(p) == -2)
        {
            if (GetBalance(p->left) > 0)
                p->left = RotateLeft(p->left);
            return RotateRight(p);
        }
        return p;
    }

    Node* RotateRight(Node *p)
    {
        Node* q = p->left;
        p->left = q->right;
        q->right = p;
        SetActualHeight(p);
        SetActualHeight(q);
        return q;
    }

    Node* RotateLeft(Node *q)
    {
        Node* p = q->right;
        q->right = p->left;
        p->left = q;
        SetActualHeight(q);
        SetActualHeight(p);
        return p;
    }

    Node* AddNode(Node *node, int value)
    {
        if (node == nullptr)
            return new Node(value);
        if (value < node->value)
            node->left = AddNode(node->left, value);
        else
            node->right = AddNode(node->right, value);
        return GetBalancedTree(node);
    }

    Node* RemoveMinValue(Node *p)
    {
        if (p->left == nullptr)
            return p->right;
        p->left = RemoveMinValue(p->left);
        return GetBalancedTree(p);
    }

    Node* RemoveNode(Node *node, int value)
    {
        if (node == nullptr)
            return nullptr;
        if (value < node->value)
            node->left = RemoveNode(node->left, value);
        else if (value > node->value)
            node->right = RemoveNode(node->right, value);
        else
        {
            Node* left = node->left;
            Node* right = node->right;
            delete node;
            if (right == nullptr)
                return left;
            Node* min = right;
            while (min->left != nullptr)
                min = min->left;
            min->right = RemoveMinValue(right);
            min->left = left;
            return GetBalancedTree(min);
        }
        return GetBalancedTree(node);
    }

    static void PrintTree(Node* tree, int r) {
        if (tree != nullptr)
        {
            PrintTree(tree->right, r+5);
            for (int i=0; i < r ; i++)
                printf(" ");
            printf("%d\n",tree->value);
            PrintTree(tree->left, r+5);
        }
    }

public:
    void Add(int value)
    {
        root = AddNode(root, value);
    }

    void Remove(int value)
    {
        root = RemoveNode(root, value);
    }

    AVL(int value){
        root = new Node(value);
    }

    void Print() // дерево слево направо
    {
        PrintTree(root, 0);
    }
};



int main() {
    cout << "Balance(left rotate)" << endl;
    AVL* tree = new AVL(3);
    tree->Add(4);
    tree->Add(5);
    tree->Print();
    cout << endl;

    cout << "Balance(big left rotate)" << endl;
    tree = new AVL(3);
    tree->Add(5);
    tree->Add(4);
    tree->Print();
    cout << endl;

    cout << "Balance(right rotate)" << endl;
    tree = new AVL(5);
    tree->Add(4);
    tree->Add(3);
    tree->Print();
    cout << endl;

    cout << "Balance(big right rotate)" << endl;
    tree = new AVL(5);
    tree->Add(3);
    tree->Add(4);
    tree->Print();
    cout << endl;

    cout << "Remove" << endl;
    tree = new AVL(5);
    tree->Add(3);
    tree->Add(4);
    tree->Add(2);
    tree->Print();
    cout << "-------------" << endl;
    tree->Remove(5);
    tree->Print();
    cout << endl;
    return 0;
}