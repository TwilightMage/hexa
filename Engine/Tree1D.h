#pragma once

#include <functional>
#include <map>
#include <stack>


#include "Engine/Math.h"

template<typename KeyType, typename ValueType>
class Tree1D
{
public:
    struct Point
    {
        const KeyType x;
        ValueType value;
    };

    struct Node
    {
        Point point;
        Node* left = nullptr;
        Node* right = nullptr;
        int h = 1;

        Node(const Node& rhs)
            : point(rhs.point)
            , left(rhs.left)
            , right(rhs.right)
            , h(rhs.h)
        {
        }
        
        explicit Node(const Point& point)
            : point(point)
        {
        }

        void update_h()
        {
            h = Math::max(left ? left->h : 0, right ? right->h : 0) + 1;
        }
    };

    class Iterator
    {
        friend Tree1D;
    
    public:
        bool operator==(const Iterator& rhs) const
        {
            return tree_ == rhs.tree_ && i_ == rhs.i_;
        }

        Iterator& operator++()
        {
            i_++;
            if (stack_.top()->right)
            {
                stack_.push(stack_.top()->right);
                while (stack_.top()->left != nullptr)
                {
                    stack_.push(stack_.top()->left);
                }
            }
            else
            {
                Node* prev;
                do
                {
                    prev = stack_.top();
                    stack_.pop();
                }
                while (stack_.size() > 0 && prev == stack_.top()->right);
            }

            return *this;
        }

        Point& operator*() const { return stack_.top()->point; }
        Point& operator->() { return stack_.top()->point; }
   
        explicit Iterator(const Tree1D* tree)
            : Iterator(tree, 0)
        {
            stack_.push(tree->root_);
            while (stack_.top() != nullptr)
            {
                stack_.push(stack_.top()->left);
            }
            stack_.pop();
        }

        static Iterator get_end(const Tree1D* tree)
        {
            return Iterator(tree, tree->size_);
        }

    private:
        Iterator(const Tree1D* tree, uint i)
            : tree_(tree)
            , i_(i)
        {
        }

        const Tree1D* tree_;
        uint i_;
        std::stack<Node*> stack_;
    };

private:
    FORCEINLINE static void rotation_ll(Node*& node)
    {
        auto l_temp = node->left;
        auto lr_temp = l_temp->right;
        node->left->right = node;
        node->left = lr_temp;
        node = l_temp;

        if (node->left) node->left->update_h();
        if (node->right) node->right->update_h();
        node->update_h();
    }

    FORCEINLINE static void rotation_rr(Node*& node)
    {
        auto r_temp = node->right;
        auto rl_temp = node->right->left;
        node->right->left = node;
        node->right = rl_temp;
        node = r_temp;

        if (node->left) node->left->update_h();
        if (node->right) node->right->update_h();
        node->update_h();
    }

    FORCEINLINE static void rotation_rl(Node*& node)
    {
        auto rl_temp = node->right->left;
        auto rll_temp = node->right->left->left;

        node->right->left = node->right->left->right;
        rl_temp->right = node->right;
        node->right = rll_temp;
        rl_temp->left = node;
        node = rl_temp;

        if (node->left)
        {
            if (node->left->right) node->left->right->update_h();
            node->left->update_h();
        }
        if (node->right)
        {
            if (node->right->left) node->right->left->update_h();
            node->right->update_h();
        }
        node->update_h();
    }
    
    FORCEINLINE static void rotation_lr(Node*& node)
    {
        auto lr_temp = node->left->right;
        auto lrr_temp = node->left->right->right;

        node->left->right = node->left->right->left;
        lr_temp->left = node->left;
        node->left = lrr_temp;
        lr_temp->right = node;
        node = lr_temp;

        if (node->left)
        {
            if (node->left->right) node->left->right->update_h();
            node->left->update_h();
        }
        if (node->right)
        {
            if (node->right->left) node->right->left->update_h();
            node->right->update_h();
        }
        node->update_h();
    }

    FORCEINLINE static int bf(Node* node)
    {
        if (node)
        {
            return (node->left ? node->left->h : 0) - (node->right ? node->right->h : 0);
        }

        return 0;
    }
    
    FORCEINLINE static void balance(Node*& node)
    {
        auto bfs = bf(node);
        auto bfl = bf(node->left);
        auto bfr = bf(node->right);

        if (bfs == 2 && bfl == 1)
        {
            rotation_ll(node);
        }
        else if (bfs == -2 && bfr == -1)
        {
            rotation_rr(node);
        }
        else if (bfs == -2 && bfr == 1)
        {
            rotation_rl(node);
        }
        else if (bfs == 2 && bfl == -1)
        {
            rotation_lr(node);
        }
    }
    
    static int insert(const Point& point, Node*& node, uint& size)
    {
        if (node)
        {
            if (point.x == node->point.x)
            {
                node->point.value = point.value;
                return node->h;
            }

            node->h = insert(point, (point.x < node->point.x) ? node->left : node->right, size) + 1;
            balance(node);
            return node->h;
        }
        else
        {
            node = new Node(point);
            size++;
            return node->h;
        }
    }

    FORCEINLINE static Node* take_most_right(Node*& node)
    {
        if (!node->right)
        {
            Node* result = node;
            node = node->left;
            result->left = nullptr;
            return result;
        }
        
        return take_most_right(node->right);
    }

    FORCEINLINE static Node* take_most_left(Node*& node)
    {
        if (!node->left)
        {
            Node* result = node;
            node = node->right;
            result->right = nullptr;
            return result;
        }
        
        return take_most_right(node->left);
    }
    
    static Point* find(std::function<bool(const Point& point)> predicate, Node* node)
    {
        if (node)
        {
            if (auto left = find(predicate, node->left)) return left;
            if (predicate(node->point)) return &node->point;
            if (auto right = find(predicate, node->right)) return right;
        }

        return nullptr;
    }

    static int remove(const KeyType& x, Node*& node, uint& size)
    {
        if (node)
        {
            if (x == node->point.x)
            {
                size--;
                if (node->left)
                {
                    auto most_r = take_most_right(node->left);
                    most_r->left = node->left;
                    most_r->right = node->right;
                    delete node;
                    node = most_r;
                    return node->h;
                }
                else if (node->right)
                {
                    auto most_l = take_most_left(node->right);
                    most_l->right = node->right;
                    delete node;
                    node = most_l;
                    return node->h;
                }
                else
                {
                    delete node;
                    node = nullptr;
                    return 0;
                }
            }

            node->h = remove(x, (x < node->point.x) ? node->left : node->right, size) + 1;
            return node->h;
        }

        return 0;
    }

    static void for_each(std::function<void(const Point& point)> callback, Node* node)
    {
        if (node)
        {
            for_each(callback, node->left);
            callback(node->point);
            for_each(callback, node->right);
        }
    }

    static void fill_stack(std::stack<Node*> stack, Node* node)
    {
        if (node)
        {
            fill_stack(stack, node->left);
            stack.push(node);
            fill_stack(stack, node->right);
        }
    }

    static void destroy(Node*& node)
    {
        if (node)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    static void copy(Node* from, Node*& to)
    {
        if (from)
        {
            to = new Node(*from);
            copy(from->left, to->left);
            copy(from->right, to->right);
        }
    }

public:
    Tree1D()
        : root_(nullptr)
        , size_(0)
    {
    }

    Tree1D(const Tree1D& rhs)
        : size_(rhs.size_)
    {
        copy(rhs.root_, root_);
    }

    Tree1D& operator=(const Tree1D& rhs)
    {
        if (this == &rhs) return *this;

        destroy(root_);
        copy(rhs.root_, root_);

        size_ = rhs.size_;

        return *this;
    }
    
    ~Tree1D()
    {
        destroy(root_);
    }

    void clear()
    {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    void insert(const KeyType& x, const ValueType& value)
    {
        insert(Point(x, value), root_, size_);
    }

    ValueType* find(const KeyType& x) const
    {
        Node* current = root_;

        while (current != nullptr)
        {
            if (x == current->point.x)
            {
                return &current->point.value;
            }

            current = (x < current->point.x) ? current->left : current->right;
        }

        return nullptr;
    }

    Point* find(std::function<bool(const Point& point)> callback) const
    {
        return find(callback, root_);
    }

    void remove(const KeyType& x)
    {
        remove(x, root_, size_);
    }

    void for_each(std::function<void(const Point& point)> callback)
    {
        for_each(callback, root_);
    }

    Iterator begin()
    {
        Iterator result = Iterator(this);
        fill_stack(result.stack_, root_);
        return result;
    }

    const Iterator begin() const
    {
        Iterator result = Iterator(this);
        fill_stack(result.stack_, root_);
        return result;
    }

    Iterator end()
    {
        return Iterator::get_end(this);
    }
    
    const Iterator end() const
    {
        return Iterator::get_end(this);
    }

    uint size() const
    {
        return size_;
    }

private:
    Node* root_;
    uint size_;
};
