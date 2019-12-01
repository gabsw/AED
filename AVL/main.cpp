#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int findMax(int a, int b) {
    return (a > b) ? a : b;
}

class Node {
public:

    string word;
    long counter;
    int height;
    Node *left;
    Node *right;

    Node(string word) : word(std::move(word)), counter(1), height(1) {
        left = nullptr;
        right = nullptr;
    }

    virtual ~Node() {
        free(left);
        free(right);
    }

    void updateHeight() {
        int leftHeight = left == nullptr ? 0 : left->height;
        int rightHeight = right == nullptr ? 0 : right->height;

        height = findMax(leftHeight, rightHeight) + 1;
    }

    int calcBalance() {
        int leftHeight = left == nullptr ? 0 : left->height;
        int rightHeight = right == nullptr ? 0 : right->height;
        return leftHeight - rightHeight;
    }

    void print() {
        cout << "Word: " << word << endl;
        cout << "Height: " << height << endl;
        cout << "Counter: " << counter << endl;
        cout << endl;
        cout << "Left: " << endl;
        if (left != nullptr) {
            left->print();
        }
        cout << "Right: " << endl;
        if (right != nullptr) {
            right->print();
        }
        cout << endl;

    }
};

class Info {
public:
    string word;
    long counter;

    Info(const string &word, long counter) : word(word), counter(counter) {}

    Info(Node *node) {
        word = node->word;
        counter = node->counter;
    }

    friend ostream &operator<<(ostream &os, const Info &info) {
        os << "word: " << info.word << " counter: " << info.counter;
        return os;
    }
};

class Tree {
public:
    Node *root;

    Tree() {
        root = nullptr;
    }

    ~Tree() {
        free(root);
    }

    // position of the word as well
    void addWord(const string &word) {
        root = addWord(root, word);
    }

    Node *findNode(string word) {
        return findNode(root, word);
    }

    void getAllInfo(vector<Info> &resultVector) {
        getInfo(resultVector, root);
    }

private:
    void getInfo(vector<Info> &resultVector, Node *node) {
        if (node == nullptr) {
            return;
        }

        Info info = Info(node);
        resultVector.push_back(info);

        getInfo(resultVector, node->left);
        getInfo(resultVector, node->right);
    }

    Node *findNode(Node *node, string word) {
        if (root == nullptr) {
            return nullptr;
        }

        if (word < node->word) {
            return findNode(node->left, word);
        }

        if (word > node->word) {
            return findNode(node->right, word);
        }

        return node;
    }

    Node *addWord(Node *node, const string &word) {
        if (node == nullptr) {
            Node *newNode = new Node(word);
            return newNode;
        }

        // Compare words to maintain order and find where to insert new word
        // Equal words will not be stored, just increase counter
        int compWords = word.compare(node->word);

        if (compWords < 0)
            node->left = addWord(node->left, word);
        else if (compWords > 0)
            node->right = addWord(node->right, word);
        else {
            node->counter++;
            return node;
        }

        // Update node height
        node->updateHeight();

        // Evaluate if tree has become unbalanced with new insertion
        // Rotate to fix
        int balance = node->calcBalance();

        // Left Left Case
        if (balance > 1 && word < node->left->word)
            return rotateRightSide(node);

        // Right Right Case
        if (balance < -1 && word > node->right->word)
            return rotateLeftSide(node);

        // Left Right Case
        if (balance > 1 && word > node->left->word) {
            node->left = rotateLeftSide(node->left);
            return rotateRightSide(node);
        }

        // Right Left Case
        if (balance < -1 && word < node->right->word) {
            node->right = rotateRightSide(node->right);
            return rotateLeftSide(node);
        }

        // Tree is now balanced
        return node;
    }

    Node *rotateRightSide(Node *oldRoot) {
        Node *newRoot = oldRoot->left;
        Node *rotatedChild = newRoot->right;

        // Rotation procedure
        newRoot->right = oldRoot;
        oldRoot->left = rotatedChild;

        // Height needs to be updated
        oldRoot->updateHeight();
        newRoot->updateHeight();

        return newRoot;
    }

    // This rotation will be basically the reverse of the last one
    Node *rotateLeftSide(Node *oldRoot) {
        Node *newRoot = oldRoot->right;
        Node *rotatedChild = newRoot->left;

        // Rotation procedure
        newRoot->left = oldRoot;
        oldRoot->right = rotatedChild;

        // Height needs to be updated
        oldRoot->updateHeight();
        newRoot->updateHeight();

        return newRoot;
    }
};


int main() {
    Tree tree = Tree();
    tree.addWord("blue");
    tree.addWord("green");
    tree.addWord("yellow");
    tree.addWord("red");
    tree.addWord("orange");
    tree.addWord("brown");
    tree.addWord("blue");
    tree.addWord("green");
    tree.addWord("yellow");
    tree.addWord("red");
    tree.addWord("orange");
    tree.addWord("brown");
    tree.addWord("blue");
    tree.addWord("green");
    tree.addWord("yellow");
    tree.addWord("red");
    tree.addWord("orange");
    tree.addWord("brown");

    Node *blue = tree.findNode("blue");
    blue->print();

    vector<Info> infoVector;
    tree.getAllInfo(infoVector);

    for (int i = 0; i < infoVector.size(); i++) {
        cout << infoVector[i] << endl;
    }

    return 0;
}