#ifndef BST_H
#define BST_H

#include <QJsonObject>
#include <QList>


//creating the struct for the trip node which will hold the price, trip data, and pointers to the left and right child nodes
struct TripNode {
    double price;
    QJsonObject tripData;
    TripNode* left;
    TripNode* right;

    TripNode(double p, QJsonObject data) {
        price = p;
        tripData = data;
        left = nullptr;
        right = nullptr;
    }
};

//class for the binary search tree that will hold the trip nodes and provide functions to insert, search, and traverse the tree

class TripBST {
    public:
        TripBST();
        ~TripBST();

        // core functions for the binary search tree
        void insert(double price, QJsonObject tripData);
        QList<QJsonObject> filterByPrice(double minPrice, double maxPrice);
        void clear();

    private:
        TripNode* root;

        //recursive helper functions for the binary search tree operations
        void insertRecursive(TripNode*& node, double price, QJsonObject tripData);
        void filterRecursive(TripNode* node, double minPrice, double maxPrice, QList<QJsonObject>& results);
        void destroyTree(TripNode* node);
};

#endif
