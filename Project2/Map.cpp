//
//  Map.cpp
//  Project 2-CS32
//
//  Created by Caroline Quigg on 1/21/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#include "Map.h"
#include <iostream>
Map::Map()
{
    //default is empty
    head = nullptr;
    m_size =0;
   
}

Map::Map(const Map&src)
{
    //create an empty map
    m_size=0;
    head = nullptr;
    if(src.head==nullptr)
    {
        return;
    }
    KeyType k;
    ValueType v;
    //because insert() puts at beginning
    //need to insert starting from last node in src
    for (int i=src.size()-1; i>=0; i--)
    {
        src.get(i,k ,v);
        insert(k,v);
    }
    /*
    head = new Node;
    m_size++;
    head->prev = nullptr;
    Node* ptr1 = head;
    Node* ptr2 = src.head;
    while (ptr2->next!=nullptr)
    {
        //fill node with info
        ptr1->m_value = ptr2->m_value;
        ptr1->m_key = ptr2->m_key;
        //add new node
        ptr1->next = new Node;
        m_size++;
        ptr1->next->prev = ptr1;
        //advance to next pos
        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
        
    }
    ptr1->next = nullptr;
    ptr1->m_value = ptr2->m_value;
    ptr1->m_key = ptr2->m_key;
    */
}

Map& Map::operator= (const Map&src)
{
    //check for aliasing
    if (this != &src)
    {
        //destroy();
        //m_size = 0;
        Map temp = src;
        swap(temp);
    }
    //temp will be destructed due to scope
    return *this;
}

Map::~Map()
{
    destroy();
}

bool Map::empty() const
{
    return (m_size==0);
}

int Map::size() const
{
    return m_size;
}

bool Map::contains (const KeyType& key)const
{
    //check to see if key found
    return find(key)!=nullptr;
}

bool Map::get(const KeyType& key, ValueType& value)const
{
    Node* ptr = find(key);
    //key not found
    if (ptr == nullptr)return false;
    //key found
    value = ptr->m_value;
    return true;
}

bool Map::get(int i, KeyType& key, ValueType& value)const
{
    //not valid arguments
    if (i<0 || i>=m_size) return false;
    
    //traverse through array a certain number of times
    int count = 0;
    Node* ptr = head;
    while(ptr!=nullptr && count<i)
    {
        ptr = ptr->next;
        count++;
    }
    key = ptr->m_key;
    value = ptr->m_value;
    return true;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
    if(find(key) != nullptr) return false;
    //insert at the beginning of the list
    
    //create new node
    Node* add = new Node;
    add->m_key = key;
    add->m_value = value;
    add->next = head;
    add->prev = nullptr;
    
    //update head
    //if not the first element added, update ptrs
    if(head!=nullptr)
        head->prev = add;
    head = add;
    
    //update size
    m_size++;
    return true;
    
}

bool Map::update(const KeyType& key, const ValueType& value)
{
    //check for node, get ptr if found
    Node* check = find(key);
    if (check == nullptr) return false;
    
    //update node
    check->m_value = value;
    
    return true;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
    //check to see if key exists to be updated, else insert
    if (update(key,value))return true;
    else return insert(key,value);
}

bool Map::erase(const KeyType& key)
{
    //see if there is a key to erase
    Node* kill = find(key);
    if (kill == nullptr) return false;
    
    //update surrounding nodes
    Node* before = kill->prev;
    Node* after = kill->next;
    //the first element in the array deleted?
    if (before!=nullptr)
        before->next = after;
    else
        head = after;
    //the last element deleted?
    if (after!= nullptr)
        after->prev = before;
    

    delete kill;
    
    m_size--;
    return true;
}

void Map::swap(Map& other)
{
    //declare temps
    int temp_s = size();
    Node* temp_p = head;
    
    //swap
    m_size = other.m_size;
    head = other.head;
    other.m_size = temp_s;
    other.head = temp_p;
    
}

void Map::destroy()
{
    Node* ptr = head;
    while (ptr!= nullptr)
    {
        Node* temp = ptr->next;
        delete ptr;
        ptr = temp;
    }
    head = nullptr;
}

void Map::dump() const
{
    Node* ptr = head;
    std::cerr<<"Size: "<<m_size<<std::endl;
    while (ptr!=nullptr)
    {
        std::cerr<<ptr->m_key<<" ";
        std::cerr<<ptr->m_value<<std::endl;
        ptr = ptr->next;
    }
            std::cerr<<std::endl;
}

bool combine (const Map& m1, const Map& m2, Map& result)
{
    //check to see if either maps are empty
    if (m1.empty())
    {
        result = m2;
        return true;
    }else if (m2.empty())
    {
        result = m1;
        return true;
    }else
    {
        bool diffVal=false;
        Map temp = m1;
        for (int i = 0 ;i<m2.size() ; i++)
        {
            KeyType key;
            ValueType value;
            m2.get(i,key,value);
            //m1 does not contain m2 key value
            if (!temp.contains(key))
            {
                //add this key to the temp
                temp.insert(key,value);
            }
            else
            {
                //check if m1 has the same value stored
                ValueType v;
                m1.get(key,v);
                //same key,diff values so remove key from temp
                if (v!=value)
                {
                    temp.erase(key);
                    diffVal=true;
                }
            }
        }
        //update result so that it contains the correct info
        result.swap(temp);
        return !diffVal;
    }
}

void subtract(const Map&m1, const Map& m2, Map& result)
{
    //if either is empty, return m1
    if (m1.empty() || m2.empty())
    {
        result=m1;
        return;
    }
    
    //create temp variable to update
    Map temp = m1;
    
    for(int i =0; i<m1.size(); i++)
    {
        //iterate through linked list m1 and get vals
        KeyType key;
        ValueType value;
        m1.get(i,key,value);
        
        //delete any keys also in m2
        if (m2.contains(key)) temp.erase(key);
    }
    //update result
    result.swap(temp);
}

