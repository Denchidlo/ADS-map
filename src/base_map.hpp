#include "memory.h"
#include "stdlib.h"
#include "stdint.h"
#include <exception>

#include <iostream>

namespace base_map
{
    constexpr uint8_t RB_BLACK = 1;
    constexpr uint8_t RB_RED = 0;

    template <class _ktype, class _vtype>
    class map
    {
    private:
        typedef struct rb_bits_t
        {
            uint8_t color : 1;
            uint8_t visited : 1;
            uint8_t free_space : 6;
        } rb_bits;

        typedef struct rb_node_t
        {
            _ktype key;
            _vtype value;

            struct rb_node_t *p;
            struct rb_node_t *l;
            struct rb_node_t *r;

            rb_bits state;
        } rb_node;
    public:
        class map_iterator
        {
        public:
            map_iterator(map* _map, rb_node* _ptr)
            {
                this->current_ptr = _ptr;
                this->_map= _map;
            }

            _vtype &operator*()
            {
                return current_ptr->value;
            }

            _vtype *operator->()
            {
                return &current_ptr->value;
            }

            map_iterator &operator++()
            {
                this->current_ptr = _map->rb_next(current_ptr);
                return *this;
            }

            map_iterator &operator--()
            {
                this->current_ptr = _map->rb_prev(current_ptr);
                return *this;
            }

        private:
            map* _map;
            rb_node *current_ptr;
        };

    public: 
        map()
        {
            this->nil = (rb_node *)malloc(sizeof(rb_node));
            this->nil->p = this->nil;
            this->nil->l = this->nil;
            this->nil->r = this->nil;
            this->nil->state.visited = 0;
            this->nil->state.color = RB_BLACK;
            this->root = this->nil;
            this->begin_ptr = this->end_ptr = this->nil;
        }

        ~map()
        {
            free(this->nil);
        }

        map_iterator begin()
        {
            map_iterator iter = map_iterator(this, this->begin_ptr);
            return iter;
        }

        map_iterator end()
        {
            map_iterator iter = map_iterator(this, this->end_ptr);
            return iter;
        }

        void insert(_ktype key, _vtype value)
        {
            rb_node *new_node = allocate_node(key, value);
            rb_insert(new_node);
            if (this->begin_ptr == this->nil || this->begin_ptr->value > value)
                this->begin_ptr = new_node;
            if (this->end_ptr == this->nil || this->end_ptr->value > value)
                this->end_ptr = new_node;
        }

        bool erase(_ktype key)
        {
            rb_node *to_remove = rb_search(this->root, key);
            if (to_remove == this->nil)
                return false;
            if (to_remove == this->begin_ptr)
                this->begin_ptr = to_remove->p;
            if (to_remove == this->end_ptr)
                this->end_ptr = to_remove->p;
            rb_remove(to_remove);
            free(to_remove);
            return true;
        }

        _vtype &operator[](const _ktype &key)
        {
            rb_node *responce = rb_search(this->root, key);
            if (responce != this->nil)
                return responce->value;
            throw new std::logic_error("Key failure");
        }

    private:
        friend class map_iterator;

        rb_node *allocate_node(_ktype key, _vtype value)
        {
            rb_node *responce = (rb_node *)malloc(sizeof(rb_node));
            responce->p = this->nil;
            responce->l = this->nil;
            responce->r = this->nil;
            responce->state.color = RB_RED;
            responce->state.visited = 0;
            responce->key = key;
            responce->value = value;
            return responce;
        }

        void rb_right_rotation(rb_node *node)
        {
            rb_node *lchild = node->l;

            node->l = lchild->r;
            if (lchild->r != this->nil)
                lchild->r->p = node;

            lchild->p = node->p;
            if (node->p == this->nil)
                this->root = lchild;
            else if (node->p->l == node)
                node->p->l = lchild;
            else
                node->p->r = lchild;

            lchild->r = node;
            node->p = lchild;
        }

        void rb_left_rotation(rb_node *node)
        {
            rb_node *rchild = node->r;

            node->r = rchild->l;
            if (rchild->l != this->nil)
                rchild->l->p = node;

            rchild->p = node->p;
            if (node->p == this->nil)
                this->root = rchild;
            else if (node->p->l == node)
                node->p->l = rchild;
            else
                node->p->r = rchild;

            rchild->l = node;
            node->p = rchild;
        }

        void rb_insert(rb_node *node)
        {
            // Position search and default insert
            rb_node *parent = this->nil;
            rb_node *current = this->root;
            while (current != this->nil)
            {
                parent = current;
                if (node->key < current->key)
                    current = current->l;
                else
                    current = current->r;
            }
            node->p = parent;

            if (parent == this->nil)
                this->root = node;
            else if (node->key < parent->key)
                parent->l = node;
            else
                parent->r = node;

            // Node RB fixup
            int counter = 0;
            while (node->p->state.color == RB_RED)
            {
                parent = node->p;
                rb_node *g = parent->p;
                // Parent is a left child
                if (parent == g->l)
                {
                    rb_node *u = g->r;
                    // Case 1
                    // Uncle and dad both red:
                    // we set them both black while grandad is inverted to red
                    if (u->state.color == RB_RED)
                    {
                        parent->state.color = RB_BLACK;
                        u->state.color = RB_BLACK;
                        g->state.color = RB_RED;
                        node = g;
                    }
                    else
                    {
                        // Case 2
                        // "node" is a right child so we need
                        // left rotation to get case 3
                        if (node == parent->r)
                        {
                            node = node->p;
                            rb_left_rotation(node);
                        }
                        // Case 3
                        // we simply need to make right rotation and
                        // set our new grandad as black, while all the others
                        // (except uncle) gonna be red
                        node->p->state.color = RB_BLACK;
                        node->p->p->state.color = RB_RED;
                        rb_right_rotation(node->p->p);
                    }
                }
                else // Symmetric situation, just invert right and left
                {
                    rb_node *u = g->l;
                    if (u->state.color == RB_RED)
                    {
                        parent->state.color = RB_BLACK;
                        u->state.color = RB_BLACK;
                        g->state.color = RB_RED;
                        node = g;
                    }
                    else
                    {
                        if (node == parent->l)
                        {
                            node = node->p;
                            rb_right_rotation(node);
                        }
                        node->p->state.color = RB_BLACK;
                        node->p->p->state.color = RB_RED;
                        rb_left_rotation(node->p->p);
                    }
                }
            }
            this->root->state.color = RB_BLACK;
        }

        void rb_transplant(rb_node *to_remove, rb_node *displace_with)
        {
            if (to_remove->p == this->nil)
                this->root = displace_with;
            else if (to_remove == to_remove->p->l)
                to_remove->p->l = displace_with;
            else
                to_remove->p->r = displace_with;
            displace_with->p = to_remove->p;
        }

        rb_node *rb_search(rb_node *root, _ktype key)
        {
            if (root == this->nil || root->key == key)
                return root;

            if (root->key < key)
                return rb_search(root->r, key);

            return rb_search(root->l, key);
        }

        rb_node *rb_find(_ktype key)
        {
            return rb_search(this->root, key);
        }

        void rb_remove(rb_node *node)
        {
            rb_node *BB_point;
            rb_node *displacer = node;
            uint8_t orig_color = node->state.color;
            if (node->l == this->nil)
            {
                BB_point = node->r;
                rb_transplant(node, BB_point);
            }
            else if (node->r == this->nil)
            {
                BB_point = node->l;
                rb_transplant(node, BB_point);
            }
            else
            {
                displacer = rb_min(node->r);
                orig_color = displacer->state.color;
                BB_point = displacer->r;
                if (displacer->p == node)
                    BB_point->p = displacer;
                else
                {
                    rb_transplant(displacer, displacer->r);
                    displacer->r = node->r;
                    displacer->r->p = displacer;
                }
                rb_transplant(node, displacer);
                displacer->l = node->l;
                displacer->l->p = displacer;
                displacer->state.color = node->state.color;
            }
            rb_node *to_fix = BB_point;
            if (orig_color == RB_BLACK)
            {
                while (to_fix != this->root && to_fix->state.color == RB_BLACK)
                {
                    if (to_fix == to_fix->p->l)
                    {
                        rb_node *brother = to_fix->p->r;
                        if (brother->state.color == RB_RED)
                        {
                            brother->state.color = RB_BLACK;
                            to_fix->p->state.color = RB_RED;
                            rb_left_rotation(to_fix->p);
                            brother = to_fix->p->r;
                        }
                        if (brother->l->state.color == RB_BLACK && brother->r->state.color == RB_BLACK)
                        {
                            brother->state.color = RB_RED;
                            to_fix = to_fix->p;
                        }
                        else
                        {
                            if (brother->r->state.color == RB_BLACK)
                            {
                                brother->l->state.color = RB_BLACK;
                                brother->state.color = RB_RED;
                                rb_right_rotation(brother);
                                brother = to_fix->p->r;
                            }
                            brother->state.color = to_fix->p->state.color;
                            to_fix->p->state.color = RB_BLACK;
                            brother->r->state.color = RB_BLACK;
                            rb_left_rotation(to_fix->p);
                            to_fix = this->root;
                        }
                    }
                    else
                    {
                        rb_node *brother = to_fix->p->l;
                        if (brother->state.color == RB_RED)
                        {
                            brother->state.color = RB_BLACK;
                            to_fix->p->state.color = RB_RED;
                            rb_right_rotation(to_fix->p);
                            brother = to_fix->p->l;
                        }
                        if (brother->r->state.color == RB_BLACK && brother->l->state.color == RB_BLACK)
                        {
                            brother->state.color = RB_RED;
                            to_fix = to_fix->p;
                        }
                        else
                        {
                            if (brother->l->state.color == RB_BLACK)
                            {
                                brother->r->state.color = RB_BLACK;
                                brother->state.color = RB_RED;
                                rb_left_rotation(brother);
                                brother = to_fix->p->l;
                            }
                            brother->state.color = to_fix->p->state.color;
                            to_fix->p->state.color = RB_BLACK;
                            brother->l->state.color = RB_BLACK;
                            rb_right_rotation(to_fix->p);
                            to_fix = this->root;
                        }
                    }
                }
            }
        }

        rb_node *rb_min(rb_node *root)
        {
            while (root->l != this->nil)
                root = root->l;
            return root;
        }

        rb_node *rb_max(rb_node *root)
        {
            while (root->r != this->nil)
                root = root->r;
            return root;
        }

        rb_node *rb_next(rb_node *current)
        {
            if (current->r != this->nil)
                return rb_min(current->r);
            rb_node *y = current->p;
            while (y != this->nil && current == y->r)
            {
                current = y;
                y = y->p;
            }
            return y;
        }

        rb_node *rb_prev(rb_node *current)
        {
            if (current->l != this->nil)
                return rb_max(current->l);
            rb_node *y = current->p;
            while (y != this->nil && current == y->l)
            {
                current = y;
                y = y->p;
            }
            return y;
        }

        rb_node *begin_ptr;
        rb_node *end_ptr;

        rb_node *nil;
        rb_node *root;
    };

}