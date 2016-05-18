//
//  Map.h
//  Project 2-CS32
//
//  Created by Caroline Quigg on 1/20/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#ifndef MAP_H
#define MAP_H

#include <string>



typedef std::string KeyType;
typedef double ValueType;
class Map
{
public:
    Map();
    Map(const Map&src);
    Map& operator= (const Map&src);
    ~Map();
    bool empty() const;
    int  size() const;
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    void swap(Map& other);
    void dump()const;
private:

    struct Node
    {
        KeyType m_key;
        ValueType m_value;
        Node * prev;
        Node* next;
    };
    Node* head;
    int m_size;
    
    //traverse list and return ptr to node if found
    //if not found,return nullptr
    Node* find(const KeyType& key)const
    {
        Node* ptr1 = head;
        while (ptr1!= nullptr)
        {
            if(ptr1->m_key==key)return ptr1;
            ptr1= ptr1->next;
            
        }
        return nullptr;
    }
    
    void destroy();

};

bool combine(const Map& m1, const Map& m2, Map& result);

void subtract(const Map& m1, const Map& m2, Map& result);


#endif