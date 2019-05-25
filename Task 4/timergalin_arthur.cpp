#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <new>
using namespace std; 
  
class Node{  
public: 
    int val;  
    Node* left;  
    Node* right;  
    int height;  
};  

class AVLTree{
    Node* root = 0;

    size_t _getHeight(Node* node)  
    {  
        if (node == 0)  
            return 0;  
        return node->height;  
    }

    size_t _getMaxNeighbourHeight(Node* node)  
    {
        size_t leftHeight =  _getHeight(node->left);
        size_t rightHeight = _getHeight(node->right);
          
        return (leftHeight > rightHeight)? leftHeight : rightHeight;  
    }

    Node* _getLowestNode(Node* node)  
    {  
        Node* _temp_node = node;  
        while (_temp_node->left != 0)  
            _temp_node = _temp_node->left;  
  
        return _temp_node;  
    }    

    int _checkBalance(Node* node)  
    {  
        if (node == 0)  
            return 0;  
        return _getHeight(node->left) - _getHeight(node->right);  
    }

    Node* _rotateLeft(Node* subroot)  
    {  
        Node* rightSubtree = subroot->right;  
        Node* tempSubtree = rightSubtree->left;  
        rightSubtree->left = subroot;  
        subroot->right = tempSubtree;  
    
        subroot->height = _getMaxNeighbourHeight(subroot) + 1;  
        rightSubtree->height = _getMaxNeighbourHeight(rightSubtree) + 1;  
  
        return rightSubtree;  
    }  

    Node* _rotateRight(Node* subroot)  
    {  
        Node* leftSubtree = subroot->left;  
        Node* tempSubtree = leftSubtree->right;  
        leftSubtree->right = subroot;  
        subroot->left = tempSubtree;  
  
        subroot->height = _getMaxNeighbourHeight(subroot) + 1;  
        leftSubtree->height = _getMaxNeighbourHeight(leftSubtree) + 1;  
  
        return leftSubtree;  
    }

    Node* _balanceNodeAfterInsert(Node* node, int val){
        int balance = _checkBalance(node);
        if (balance <= 1 && balance >= -1)
            return node;  
        if (balance > 1 && val < node->left->val)  
            return _rotateRight(node);    
        if (balance < -1 && val > node->right->val)  
            return _rotateLeft(node);  
        if (balance > 1 && val > node->left->val)  
        {  
            node->left = _rotateLeft(node->left);  
            return _rotateRight(node);  
        }  
        if (balance < -1 && val < node->right->val)  
        {  
            node->right = _rotateRight(node->right);  
            return _rotateLeft(node);  
        }    
        return node;  
    }

    Node* _balanceNodeAfterDelete(Node* node){
        int balance = _checkBalance(node);  
        if (balance > 1 && _checkBalance(node->left) >= 0)  
            return _rotateRight(node);  
        if (balance > 1 && _checkBalance(node->left) < 0)  
        {  
            node->left = _rotateLeft(node->left);  
            return _rotateRight(node);  
        }  
        if (balance < -1 && _checkBalance(node->right) <= 0)  
            return _rotateLeft(node);  
        if (balance < -1 && _checkBalance(node->right) > 0)  
        {  
            node->right = _rotateRight(node->right);  
            return _rotateLeft(node);  
        }

        return node;  
    }

    
    Node* _balance(Node* node){
        node = _balanceNodeAfterInsert(node, node->val);
        node = _balanceNodeAfterDelete(node);

        return node;
    }
 
    Node* _insertNode(Node* node, size_t val)  
    {  
        if (node == 0)  
        {
            node = new Node();
            node->height = 1;
            node->left = 0;  
            node->right = 0;  
            node->val = val; 

            return node;
        } 
        if (val < node->val)  
            node->left = _insertNode(node->left, val);  
        else if (val > node->val)  
            node->right = _insertNode(node->right, val);  
        else
            return node;    
        node->height = 1 + _getMaxNeighbourHeight(node);  
    
        return _balanceNodeAfterInsert(node, val);  
    }

    Node* _deleteNode(Node* subroot, int val)  
    {  
        if (subroot == 0)  
            return subroot;  

        if (val < subroot->val)  
            subroot->left = _deleteNode(subroot->left, val);  
        else if( val > subroot->val )  
            subroot->right = _deleteNode(subroot->right, val);  
        else if(subroot->left == 0 || subroot->right == 0)  
        {  
            Node *_temp_node = subroot->left ? subroot->left : subroot->right;  
            if (_temp_node == 0)  
            {  
                _temp_node = subroot;  
                subroot = 0;  
            }  
            else 
                *subroot = *_temp_node;
            free(_temp_node);  
        }  
        else
        {  
            Node* temp = _getLowestNode(subroot->right);  
            subroot->val = temp->val;  
            subroot->right = _deleteNode(subroot->right, temp->val);  
        }     
        if (subroot == 0)  
            return subroot;  
        subroot->height = 1 + _getMaxNeighbourHeight(subroot);  

        return _balanceNodeAfterDelete(subroot);  
    }

    void _print(Node* node, bool isInOrder){
        if(node != 0){
            if(!isInOrder){
                _print(node->left, isInOrder);
                printf("%d ", node->val);
            }
            else{
                printf("%d ", node->val);
               _print(node->left, isInOrder);  
            }
            _print(node->right, isInOrder);
        }
    }    
      
public:
    AVLTree(size_t initial_value){
        root = new Node();
        root->height = 1;
        root->left = 0;  
        root->right = 0;  
        root->val = initial_value;
    }
    
    void Insert(size_t val)  
    {  
        if(root == 0){
            root = new Node();
            root->height = 1;
            root->left = 0;  
            root->right = 0;  
            root->val = val; 
        }
        else
            root = _insertNode(root, val);
    }

    void Delete(size_t val){
        root = _deleteNode(root, val);
    }

    // баланс происходит автоматически при операциях вставки/удаления
    void Balance(){
        root = _balance(root);
    } 

    void PrintBalanced(){
        _print(root, false);
    }

    void PrintInOrder(){
        _print(root, true);
    } 
};

int main()  
{
    printf("Creating tree with 1 at root...\n");
    AVLTree tree = AVLTree(1);

    printf("One-node tree:\n");
    tree.PrintBalanced();

    printf("\n\nInserting 3, 4, 5, 2...\n");
    tree.Insert(3);  
    tree.Insert(4);  
    tree.Insert(5);  
    tree.Insert(2);
    tree.Balance();
    printf("Balance in tree:\n");
    tree.PrintBalanced();
    printf("\nOrder in tree:\n");
    tree.PrintInOrder();   
    
    printf("\n\nDeleting 1, 5...\n");
    tree.Delete(1);
    tree.Delete(5);
    tree.Balance();
    printf("Balance in tree:\n");
    tree.PrintBalanced();
    printf("\nOrder in tree:\n");
    tree.PrintInOrder();     
}  
