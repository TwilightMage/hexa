#pragma once

 /*
 * Code for a k-d tree implementaion for a 2-dimensional space.
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
#include <utility>

template<typename KeyType, typename ValueType>
struct point{
    KeyType x, y;
    ValueType value;
};

template<typename KeyType, typename ValueType>
struct Node{
    point<KeyType, ValueType> *p;
    Node *left;
    Node *right;

    explicit Node(point<KeyType, ValueType> &p_){
        p = new point<KeyType, ValueType>{p_.x, p_.y, p_.value};
        left = nullptr;
        right = nullptr;
    }

    void for_each(std::function<void(const point<KeyType, ValueType>& point)> callback)
    {
        callback(*p);
        if (left) left->for_each(callback);
        if (right) right->for_each(callback);
    }

    const point<KeyType, ValueType>* find(std::function<bool(const point<KeyType, ValueType>& point)> callback)
    {
        const point<KeyType, ValueType>* result = nullptr;
        
        if (callback(*p)) result = p;
        
        if (left && result == nullptr) result = left->find(callback);
        if (right && result == nullptr) result = right->find(callback);

        return result;
    }
};

template<typename KeyType, typename ValueType>
class Tree2D{
private:
    Node<KeyType, ValueType> *root;

    Node<KeyType, ValueType> * insert(point<KeyType, ValueType> &p, Node<KeyType, ValueType> *node){
        if(node == nullptr){
            node = new Node(p);
            return node;
        }

        if(node->p->x > p.x){
            node->left = insert(p, node->left);
        }
        else{
            node->right = insert(p, node->right);
        }
        return node;
    }

    void traverse(Node<KeyType, ValueType> *node){
        if (node == nullptr)
            return;
        std::cout << node->p->x << ' ' << node->p->y << '\n';
        traverse(node->left);
        traverse(node->right);
    }

    int getMedian(std::vector<point<KeyType, ValueType>> &points, int l, int r, bool x){
        if(l == r){
            return l;
        }

        if(x){
            return (l + r) >> 1;
        }

        std::priority_queue<std::pair<int, int>> p1;
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                            std::greater<std::pair<int, int>>> p2;
        int mid = points[l].y;
        p1.push(std::make_pair(mid, l));
        for(int i = l + 1; i <= r; ++i){
            int temp = points[i].y;
            if(p1.size() > p2.size()){
                if(temp < mid){
                    p2.push(p1.top());
                    p1.pop();
                    p1.push(std::make_pair(temp, i));
                }
                else{
                    p2.push(std::make_pair(temp, i));
                }
                mid = p1.top().first;
            }
            else if(p1.size() == p2.size()){
                if(temp < mid){
                    p1.push(std::make_pair(temp, i));
                    mid = p1.top().first;
                }
                else{
                    p2.push(std::make_pair(temp, i));
                    mid = p2.top().first;
                }
            }
            else{
                if(temp < mid){
                    p1.push(std::make_pair(temp, i));
                }
                else{
                    p1.push(p2.top());
                    p2.pop();
                    p2.push(std::make_pair(temp, i));
                }
                mid = p2.top().first;
            }
        }
        if(p2.size() > p1.size()){
            return p2.top().second;
        }
        else{
            return p1.top().second;
        }
    }

    void insert(std::vector<point<KeyType, ValueType>> &points, int l, int r, bool x){
        // std::cout << l << ' ' << r << '\n';
        if(l > r)
            return;
        int median = getMedian(points, l, r, x);
        insert(points[median]);
        insert(points, l, median - 1, x ^ 1);
        insert(points, median + 1, r,  x ^ 1);
    }

public:
    Tree2D(){
        root = nullptr;
    }

    void insert(point<KeyType, ValueType> p){
        if(root == nullptr){
            root = new Node(p);
            return;
        }
        insert(p, root);
    }

    void insert(std::vector<point<KeyType, ValueType>> &points){
        // function to insert medians.
        // proper implementation would require use of a data 
        // structure like segment tree

        // sort the points with respect to x
        std::sort(points.begin(), points.end(),
                  [](const point<KeyType, ValueType> &l, const point<KeyType, ValueType> &r) -> bool {
                      if (l.x == r.x)
                          return l.y < r.y;
                      else
                          return l.x < r.x;
                  });
        
        insert(points, 0, points.size() - 1, true);

    }

    void traverse(){
        // preorder traversal
        traverse(root);
    }

    void for_each(std::function<void(const point<KeyType, ValueType>& point)> callback)
    {
        if (root) root->for_each(callback);
    }

    const point<KeyType, ValueType>* find(std::function<bool(const point<KeyType, ValueType>& point)> callback)
    {
        if (root) return root->find(callback);

        return nullptr;
    }
};
