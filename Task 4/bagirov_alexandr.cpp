#include <iostream>
using namespace std;


struct Node
{
    int value;
    unsigned char height;

    Node* left;
    Node* right;

    explicit Node(int k)
    {
        value = k;
        left = right = nullptr;
        height = 1;
    }
};


class AVLTree {
private:
    Node* root;

    static unsigned char GetHeight(Node* p)
    {
        return p ? p->height : 0;
    }

    static int GetFactor(Node *p)
    {
        return GetHeight(p->right) - GetHeight(p->left);
    }

    static void UpdateHeight(Node *p)
    {
        unsigned char hl = GetHeight(p->left);
        unsigned char hr = GetHeight(p->right);
        p->height = (hl > hr ? hl : hr) + 1;
    }

    static Node* RightRotate(Node* p)
    {
        Node* q = p->left;
        p->left = q->right;
        q->right = p;
        UpdateHeight(p);
        UpdateHeight(q);
        return q;
    }

    static Node* LeftRotate(Node* q)
    {
        Node* p = q->right;
        q->right = p->left;
        p->left = q;
        UpdateHeight(q);
        UpdateHeight(p);
        return p;
    }

    static Node* Balance(Node* p)
    {
        UpdateHeight(p);
        if (GetFactor(p) == 2)
        {
            if (GetFactor(p->right) < 0)
                p->right = RightRotate(p->right);
            return LeftRotate(p);
        }
        if (GetFactor(p) == -2)
        {
            if (GetFactor(p->left) > 0)
                p->left = LeftRotate(p->left);
            return RightRotate(p);
        }
        return p;
    }

    static Node* InsertNode(Node* p, int k)
    {
        if (!p)
        {
            return new Node(k);
        }
        if (k < p->value)
        {
            p->left = InsertNode(p->left, k);
        }
        else
        {
            p->right = InsertNode(p->right, k);
        }

        return Balance(p);
    }

    static Node* FindMinElement(Node* p)
    {
        return p->left ? FindMinElement(p->left) : p;
    }

    static Node* RemoveMinElement(Node* p)
    {
        if (p->left == nullptr)
            return p->right;
        p->left = RemoveMinElement(p->left);
        return Balance(p);
    }

    static Node* RemoveNode(Node* p, int k)
    {
        if (!p)
        {
            return nullptr;
        }
        if (k < p->value)
        {
            p->left = RemoveNode(p->left, k);
        }
        else if (k > p->value)
        {
            p->right = RemoveNode(p->right, k);
        }
        else
        {
            Node* q = p->left;
            Node* r = p->right;
            delete p;
            if (!r)
            {
                return q;
            }
            Node* min = FindMinElement(r);
            min->right = RemoveMinElement(r);
            min->left = q;
            return Balance(min);
        }
        return Balance(p);
    }

    static void PrintTree(Node* p)
    {
        if (p)
        {
            PrintTree(p->left);
            cout << p->value << "|";
            PrintTree(p->right);
        }
    }

public:
    AVLTree() {
        root = nullptr;
    }

    void Insert(int value)
    {
        auto node = new Node(value);
        root = InsertNode(root, value);
    }

    void Remove(int value)
    {
        root = RemoveNode(root, value);
    }

    void Print()
    {
        cout << "Элементы: ";
        PrintTree(root);
        cout << endl;
    }
};



int main()
{
    auto tree = AVLTree();

    cout << "Пустое дерево" << endl;
    tree.Print();

    cout << "Добавлен элемент" << endl;
    tree.Insert(1);
    tree.Print();

    cout << "Удалён несуществующий элемент" << endl;
    tree.Remove(100500);
    tree.Print();

    cout << "Добавлены несколько элементов разных знаков" << endl;
    tree.Insert(100);
    tree.Insert(-100);
    tree.Insert(25);
    tree.Insert(99);
    tree.Insert(-5);
    tree.Print();

    cout << "Добавлены и удалены несколько элементов" << endl;
    tree.Remove(25);
    tree.Insert(68);
    tree.Insert(-68);
    tree.Remove(99);
    tree.Remove(-5);
    tree.Insert(3);
    tree.Print();
}
