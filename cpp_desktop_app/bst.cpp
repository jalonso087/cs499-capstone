#include "bst.h"

TripBST::TripBST() {
    root = nullptr;
}

TripBST::~TripBST() {
    clear();
}

void TripBST::clear() {
    destroyTree(root);
    root = nullptr;
}

void TripBST::destroyTree(TripNode* node) {
    if (node!=nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void TripBST::insert(double price, QJsonObject tripData) {
    insertRecursive(root, price, tripData);
}

void TripBST::insertRecursive(TripNode*& node, double price, QJsonObject tripData) {
    //insert new node if found empty
    if (node == nullptr) {
        node = new TripNode(price, tripData);
        return;
    }

    //smaller left, larger or equal to the right
    if (price < node->price) {
        insertRecursive(node->left, price, tripData);
    } else {
        insertRecursive(node->right, price, tripData);
    }
}

QList<QJsonObject> TripBST::filterByPrice(double minPrice, double maxPrice) {
    QList<QJsonObject> results;
    filterRecursive(root, minPrice, maxPrice, results);
    return results;
}

void TripBST::filterRecursive(TripNode* node, double minPrice, double maxPrice, QList<QJsonObject>& results) {
    if (node == nullptr) {
        return;
    }

    //if current price is greater than the minimum, valid nodes might exist on the left
    if (node->price > minPrice) {
        filterRecursive(node->left, minPrice, maxPrice, results);
    }

    //if current price is within our requested range, add it to results
    if (node->price >= minPrice && node->price <= maxPrice) {
        results.append(node->tripData);
    }

    //if current price is less than max, valid nodes might exist on the right
    if (node->price < maxPrice) {
        filterRecursive(node->right, minPrice, maxPrice, results);
    }
}