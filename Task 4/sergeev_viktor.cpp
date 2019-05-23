#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>


template <class T>
class AVLTree
{
private:
    struct Node {
        Node* left_child;
        Node* right_child;
        Node* parent;
        T value;
        explicit Node (T _value): value(_value) {}
        void Tie(Node* child)
        {
            if (child->value < value)
                left_child = child;
            else
                right_child = child;
            child->parent = this;
        }
        void Untie(Node* child)
        {
            if (child->value < value)
                left_child = 0;
            else
                right_child = 0;
            child->parent = 0;
        }
    };

    Node* root;
    Node* Successor(Node* node)
    {
        if (node->right_child)
        {
            node = node->right_child;
            while (node->left_child)
                node = node->left_child;
            return node;
        } else {
            while (node->parent) {
                if (node == node->parent->left_child)
                    return node;
                else
                    node = node->parent;
            }
            return 0;
        }
    }
    void InOrderPrint(Node* node)
    {
        if (node)
        {
            InOrderPrint(node->left_child);
            std::cout << node->value << std::endl;
            InOrderPrint(node->right_child);
        }
    }
    void AVLRight(Node* node)
    {
        auto left = node->left_child;
        auto left_right_child = left->right_child;
        SetParent(node, left);
        node->left_child  = left_right_child;
        if (left_right_child)
            left_right_child->parent = node;
        left->right_child = node;
    }
    void AVLLeft(Node* node)
    {
        auto right = node->right_child;
        auto right_left_child = right->left_child;
        SetParent(node, right);
        node->right_child = right_left_child;
        if (right_left_child)
            right_left_child->parent = node;
        right->left_child = node;
    }
    void DoubleAVLLeftKink(Node* node)
    {
        auto left = node->left_child;
        AVLLeft(left);
        AVLRight(node);
    }
    void DoubleAVLRightKink(Node* node)
    {
        auto right = node->right_child;
        AVLRight(right);
        AVLLeft(node);
    }
    void SetParent(Node* a, Node* b)
    {
        auto parent = a->parent;
        a->parent = b;
        if (root == a)
            root = b;
        b->parent = parent;
        if (parent) {
            if (parent->right_child == a)
                parent->right_child = b;
            else
                parent->left_child = b;
        }
    }
    uint8_t Height(Node* node)
    {
        if (node == 0)
            return 0;
        return static_cast<uint8_t>(1 + std::max(
                        node->left_child ? Height(node->left_child) : 0,
                        node->right_child ? Height(node->right_child) : 0));
    }
    void TraverseUp(Node* node)
    {
        auto last_bf = 0;
        while (node) {
            auto bf = Height(node->right_child) - Height(node->left_child);
            if (bf * last_bf == -4) {
                // have found kink
                if (bf < -1) {
                    DoubleAVLLeftKink(node);
                }
                else {
                    DoubleAVLRightKink(node);
                }
            }
            else if (bf < -1)
                AVLRight(node);
            else if (bf > 1)
                AVLLeft(node);
            last_bf = bf;
            node = node->parent;
        }
    }

public :
    AVLTree() = default;

    bool Insert(T value) {
        auto node = new Node(value);

        if (root == 0) {
            root = node;
            return true;
        }
        auto current = root;
        while (current->value != node->value)
        {
            if (node->value < current->value) {
                if (current->left_child == 0) {
                    current->Tie(node);
                    TraverseUp(node);
                    return true;
                }
                else
                    current = current->left_child;
            }else if (node->value > current->value) {
                if (current->right_child == 0)
                {
                    current->Tie(node);
                    TraverseUp(node);
                    return true;
                } else
                    current = current->right_child;
            }
        }
        return false;
    }

    bool Remove(T value) {
        if (root == 0)
            return false;
        auto current = root;
        while(current->value != value)
        {
            if (current->value > value)
                current = current->left_child;
            else if (current->value < value)
                current = current->right_child;
            if (current == 0)
                return false;
        }
        // case 1 (no children)
        if (current->left_child == 0 and current->right_child == 0) {
            if (current->parent) {
                current->parent->Untie(current);
            }
        }
        // case 2 (one children)
        else if (current->left_child == 0 or current->right_child == 0)
        {
            if (current->parent) {
                auto new_node = current->left_child ? current->left_child : current->right_child;
                current->parent->Tie(new_node);
            }
        }
        // case 3 (two children)
        else {
            auto s = Successor(current);
            current->value = s->value;
            s->parent->Untie(s);
        }
        return true;
    }

    bool Contains(T value)
    {
        auto current = root;
        while (current)
        {
            if (current->value == value)
                return true;
            if (current->value < value)
                current = current->left_child;
            else
                current = current->right_child;
        }
        return false;
    }

    T Pop() {
        auto node = root;
        if (node == 0) {
            throw "Try 'pop' from empty tree";
        }
        while (node->left_child)
            node = node->left_child;
        Remove(node->value);
        return node->value;
    }

    T Peek() {
        auto node = root;
        if (node == 0) {
            throw "Try 'peek' from empty tree";
        }
        while (node->left_child)
            node = node->left_child;
        return node->value;
    }

    void InOrderPrint() {
        InOrderPrint(root);
    }

    int Height() {
        return Height(root);
    }
};

void test0_simple_balancing()
{
    auto tree = new AVLTree<int>();
    for (int i=0; i<10; ++i) {
        tree->Insert(i);
    }

    std::cout << "-----------------" << std::endl;
    tree->InOrderPrint();
    std::cout << "Pop Head: " << tree->Pop() << std::endl;

    std::cout << "-----------------" << std::endl;
    tree->InOrderPrint();
    std::cout << "Head: " << tree->Peek() << std::endl;

    std::cout << "-----------------" << std::endl
              << "Height: " << tree->Height() << std::endl;
}

void test2_simple_kink_balancing()
{
    auto tree = new AVLTree<int>();
    std::cout << "Left kink: " << std::endl;
    tree->Insert(3);
    tree->Insert(1);
    tree->Insert(2);

    tree->InOrderPrint();

    tree->Pop();
    tree->Pop();
    tree->Pop();

    std::cout << "Right kink:" << std::endl;
    tree->Insert(1);
    tree->Insert(3);
    tree->Insert(2);

    tree->InOrderPrint();
}

void test3_random_generated_values(int amount)
{
    auto tree = new AVLTree<int>();
    std::srand(std::time(NULL));

    std::cout << "Generating: " << amount << " random values" << std::endl;
    for (int i=0; i<amount*amount; ++i) {
        auto j = std::rand() % amount;
        tree->Insert(j);
    }
    std::cout << "Expected height in ("
              << std::log2(amount) << ", " << 2 * std::log2(amount) << ")"
              << std::endl
              << "Actual height: " << tree->Height() << std::endl;
//              << "Tree:" << std::endl;
//    tree->InOrderPrint();
}

void test1_complex_tree_balancing()
{
    auto tree = new AVLTree<int>();
    tree->Insert(15);
    tree->Insert(5);
    tree->Insert(1);
    tree->Insert(8);
    tree->Insert(20);

    tree->Insert(10);

    tree->InOrderPrint();
}

void test4_only_one_occurence()
{
    auto tree = new AVLTree<std::string>();
    
    tree->Insert("hello");
    tree->Insert("world!");

    std::cout << "Tree:" << std::endl;
    tree->InOrderPrint();

    std::cout << "Result of 'tree.Contains('hello')': " << tree->Contains("hello") << std::endl
              << "Result of 'tree.Contains('ehlo')': "  << tree->Contains("ehlo") << std::endl;
}



int main() {


    std::cout << "-----------------" << std::endl
              << "Test 0: simple line balancing" << std::endl
              << "-----------------" << std::endl;
    test0_simple_balancing();


    std::cout << "-----------------" << std::endl
              << "Test 1: complex tree balancing" << std::endl
              << "-----------------" << std::endl;
    test1_complex_tree_balancing();

    std::cout << "-----------------" << std::endl
              << "Test 2: simple king balancing" << std::endl
              << "-----------------" << std::endl;
    test2_simple_kink_balancing();

    std::cout << "-----------------" << std::endl
              << "Test 3: balance random generated values" << std::endl
              << "-----------------" << std::endl;
    test3_random_generated_values(1000);


    std::cout << "-----------------" << std::endl
              << "Test 4: finding elements" << std::endl
              << "-----------------" << std::endl;
    test4_only_one_occurence();
    return 0;
}
