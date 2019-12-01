#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int findMax(int a, int b) {
    return (a > b) ? a : b;
}


class Info {
public:
    string word;
    long counter;
    vector<long> positions;

    Info(string word, long counter, long position) : word(std::move(word)), counter(counter) {
        positions.push_back(position);
    }

    friend ostream &operator<<(ostream &os, const Info &info) {
        os << "word: " << info.word << " counter: " << info.counter << " positions: [";
        for (long i = 0; i < info.positions.size(); i++)
            os << info.positions[i] << ", ";
        os << "]";
        return os;
    }
};

class Node {
public:
    Info info;
    int height;
    Node *left;
    Node *right;

    Node(Info info) : info(std::move(info)), height(1) {
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
        cout << "Info: " << info << endl;
        cout << "Height: " << height << endl;
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

class Tree {
public:
    Node *root;

    Tree() {
        root = nullptr;
    }

    ~Tree() {
        free(root);
    }

    void addWord(const string &word, long position) {
        root = addWord(root, Info(word, 1, position));
    }

    void addWord(Info &info) {
        root = addWord(root, info);
    }

    Info findInfo(string word) {
        return findNode(root, word)->info;
    }

    // Adds all Info objects from the tree into the resultVector.
    // If the vector already contains other Info, they will not be modified
    void addAllInfoToVector(vector<Info> &resultVector) {
        getInfo(resultVector, root);
    }

private:
    void getInfo(vector<Info> &resultVector, Node *node) {
        if (node == nullptr) {
            return;
        }

        Info info = node->info;
        resultVector.push_back(info);

        getInfo(resultVector, node->left);
        getInfo(resultVector, node->right);
    }

    Node *findNode(Node *node, string word) {
        if (root == nullptr) {
            return nullptr;
        }

        if (word < node->info.word) {
            return findNode(node->left, word);
        }

        if (word > node->info.word) {
            return findNode(node->right, word);
        }

        return node;
    }

    Node* addWord(Node *node, const Info& newInfo) {
        if (node == nullptr) {
            Node *newNode = new Node(newInfo);
            return newNode;
        }

        // Compare words to maintain order and find where to insert new word
        // Equal words will not be stored, just increase counter
        int compWords = newInfo.word.compare(node->info.word);

        if (compWords < 0)
            node->left = addWord(node->left, newInfo);
        else if (compWords > 0)
            node->right = addWord(node->right, newInfo);
        else {
            node->info.counter += newInfo.counter;
            node->info.positions.insert(end(node->info.positions), begin(newInfo.positions), end(newInfo.positions));
            return node;
        }

        // Update node height
        node->updateHeight();

        // Evaluate if tree has become unbalanced with new insertion
        // Rotate to fix
        int balance = node->calcBalance();

        // Left Left Case
        if (balance > 1 && newInfo.word < node->left->info.word)
            return rotateRightSide(node);

        // Right Right Case
        if (balance < -1 && newInfo.word > node->right->info.word)
            return rotateLeftSide(node);

        // Left Right Case
        if (balance > 1 && newInfo.word > node->left->info.word) {
            node->left = rotateLeftSide(node->left);
            return rotateRightSide(node);
        }

        // Right Left Case
        if (balance < -1 && newInfo.word < node->right->info.word) {
            node->right = rotateRightSide(node->right);
            return rotateLeftSide(node);
        }

        // Tree is now balanced
        return node;
    }

    Node* rotateRightSide(Node *oldRoot) {
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
    Node* rotateLeftSide(Node *oldRoot) {
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

unsigned int hash_function(const string &string,unsigned int s)
{
    static unsigned int table[256];
    unsigned int crc,i,j;
    if(table[1] == 0u)
        for(i = 0u;i < 256u;i++)
            for(table[i] = i,j = 0u;j < 8u;j++)
                if(table[i] & 1u)
                    table[i] = (table[i] >> 1) ^ 0xAED00022u;
                else
                    table[i] >>= 1;
    crc = 0xAED02019u;

    const char* str = string.c_str();
    while(*str !='\0')
        crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
    return crc % s;
}


int main() {
    Tree tree = Tree();
    tree.addWord("blue", 5);
    tree.addWord("brown", 18);
    tree.addWord("green", 6);
    tree.addWord("orange", 9);
    tree.addWord("red", 4);
    tree.addWord("yellow", 2);

    tree.addWord("blue", 20);
    tree.addWord("green", 24);
    tree.addWord("yellow", 25);
    tree.addWord("red", 28);
    tree.addWord("orange", 40);
    tree.addWord("brown", 41);
    tree.addWord("blue", 44);
    tree.addWord("green", 48);
    tree.addWord("yellow", 50);
    tree.addWord("red", 60);
    tree.addWord("orange", 62);
    tree.addWord("brown", 64);

    Info blueInfo = tree.findInfo("blue");
    cout << blueInfo << endl;

    vector<Info> infoVector;
    tree.addAllInfoToVector(infoVector);

    for (int i = 0; i < infoVector.size(); i++) {
        cout << infoVector[i] << endl;
    }

    string str = "adsfsadf";


    // Tests if destruction of original tree, copy of info vector and creation of new tree is working properly

    Node *t = new Node(Info("test", 1, 1));
    Info info = t->info;
    free(t);
    cout << endl << info << endl;


    Tree tree2 = Tree();
    for (int i = 0; i < infoVector.size(); i++) {
        tree2.addWord(infoVector[i]);
    }

    vector<Info> infoVector2;
    tree2.addAllInfoToVector(infoVector2);

    for (int i = 0; i < infoVector2.size(); i++) {
        cout << infoVector2[i] << endl;
    }

    // hash function test
    cout << endl << "String hash:" << endl;
    cout << hash_function("this is a test str", 10000) << endl;

    return 0;
}