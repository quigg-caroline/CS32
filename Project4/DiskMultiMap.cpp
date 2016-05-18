//
//  BinaryFile.cpp
//  Project 4-CS32
//
//  Created by Caroline Quigg on 3/5/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#include "DiskMultiMap.h"
#include <functional>
#include <string>
#include <cstring>

DiskMultiMap::DiskMultiMap()
{
    
}

DiskMultiMap::~DiskMultiMap()
{
    close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
    //close the open file
    if (m_bf.isOpen())
    {
        close();
    }
    //attempt to create a new file the name (overwrite if possible)
    if(!m_bf.createNew(filename)) return false;
    
    //create some header information (# of buckets, "head" for list of empty positions)
    BinaryFile::Offset empty = 0;
    header head;
    head.numBuckets = numBuckets;
    head.empty =empty;
    
    m_bf.write(head,0);
    
    //create an "array" of B buckets, each contains a pointer to a linked list
    for (int i =0 ; i<numBuckets ; i++)
    {
        if(!m_bf.write(empty,m_bf.fileLength())) return false;
    }
    
    //successfully created an empty hash table
    return true;
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
    return m_bf.openExisting(filename);
}

void DiskMultiMap::close()
{
    //if the binary file is open, close it
    m_bf.close();
}

unsigned int DiskMultiMap::hash(const std::string & hashed)
{
    std::hash<std::string> str_hash;
    unsigned int hashVal = str_hash(hashed);
    unsigned int numBuck;
    m_bf.read(numBuck,0);
    unsigned int cool = hashVal%numBuck;
    return cool;
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    if (!m_bf.isOpen()) return false;
    //check if any of the info is longer than the max len
    if (key.length()>MAX_LEN || value.length()>MAX_LEN || context.length() > MAX_LEN) return false;
    
    //get the binary offset of corresponding hash value
    unsigned int pos = hash(key);
    pos*=sizeof(BinaryFile::Offset);
    pos = pos + sizeof(header);
    
    //create new node to add
    DiskNode add;
    strcpy(add.key,key.c_str());
    strcpy(add.value,value.c_str());
    strcpy(add.context,context.c_str());
    m_bf.read(add.next, pos);
    
    
    //check if there are any empty spaces to add to
    header head;
    BinaryFile::Offset empty;
    m_bf.read(head,0);
    empty = head.empty;
    if (empty != 0)
    {
        BinaryFile::Offset newpos=empty;
        //unlink the position from the empty position linked list
        DiskNode f;
        //m_bf.read(empty, empty);
        m_bf.read(f,empty);
        //head.empty = empty;
        head.empty = f.next;
        m_bf.write(head,0);
        
        //update bucket and insert
        m_bf.write(newpos,pos);
        m_bf.write(add,newpos);
    }
    //else increase file
    else
    {
        //update bucket
        m_bf.write(m_bf.fileLength(),pos);
        m_bf.read(pos,pos);
        m_bf.write(add,m_bf.fileLength());
    }
    
    return true;
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    if (!m_bf.isOpen()) return false;
    int count =0;
    
    //get the bucket to look through
    unsigned int pos = hash(key);
    pos*=sizeof(BinaryFile::Offset);
    pos = pos + sizeof(unsigned int) + sizeof(BinaryFile::Offset);
    
    BinaryFile::Offset look;
    m_bf.read(look,pos);
    BinaryFile::Offset prev =0;
    DiskNode prevnode;
    
    while (look !=0)
    {
        DiskNode stuff;
        m_bf.read(stuff,look);
        //found node to delete
        if (strcmp(stuff.key,key.c_str())==0 && strcmp(stuff.value,value.c_str())==0 && strcmp(stuff.context,context.c_str()) ==0)
        {
            count++;
            //deleting the first node in a list
            if (prev ==0)
            {
                //unlink from list by updating head
                m_bf.write(stuff.next,pos);
            }
            //deleting some other node
            else
            {
                prevnode.next = stuff.next;
                m_bf.write(prevnode,prev);
            }
            
            //add the empty position to the linked list of empty positions
            BinaryFile::Offset empty;
            header head;
            m_bf.read(head, 0);
            empty = head.empty;
            head.empty = look;
            DiskNode addit = stuff;
            addit.next = empty;
            m_bf.write(head,0);
            //m_bf.write(look,sizeof(unsigned int));
            m_bf.write(addit,look);
        }
        else
        {
            m_bf.read(prevnode,look);
            prev = look;
        }
        //advance to next node
        look = stuff.next;
    }
    return count;
}


DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key)
{
    if (!m_bf.isOpen()) return Iterator();
    //find appropriate bucket
    unsigned int pos = hash(key);
    pos*=sizeof(BinaryFile::Offset);
    pos = pos + sizeof(unsigned int) + sizeof(BinaryFile::Offset);
    
    
    BinaryFile::Offset p;
    m_bf.read(p,pos);
    string m_key;
    DiskNode n;
    
    
    bool found = false;
    //iterate through bucket
    while (p!=0)
    {
        m_bf.read(n,p);
        if (strcmp(n.key,key.c_str()) == 0)
            {
                found = true;
                break;
            }
            p = n.next;
            }
            
            if(found)
            {
                return Iterator(this,p);
            }
            else
            {
                return Iterator();
            }
            }
            
            
            //**ITERATOR**//
            
            
            //default construction
            DiskMultiMap::Iterator::Iterator()
        {
            valid = false;
            m_file = nullptr;
            cur =0;
        }
            
            DiskMultiMap::Iterator::Iterator(DiskMultiMap* src, BinaryFile::Offset pos)
        {
            valid = true;
            m_file = src;
            cur = pos;
            DiskNode n;
            m_file->m_bf.read(n,cur);
            data.key = n.key;
            data.context = n.context;
            data.value = n.value;
        }
            
            DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
        {
            
            //search for the next node that contains the key
            if(isValid())
            {
                //search for the next node with the key
                DiskNode n;
                m_file->m_bf.read(n,cur);
                string m_key = n.key;
                
                
                BinaryFile::Offset p;
                p = n.next;
                
                
                bool found = false;
                //iterate through bucket
                while (p!=0)
                {
                    m_file->m_bf.read(n,p);
                    if (strcmp(n.key,m_key.c_str()) == 0)
                    {
                        cur = p;
                        found = true;
                        data.key = n.key;
                        data.context = n.context;
                        data.value = n.value;
                        break;
                    }
                    p = n.next;
                }
                
                if(!found)
                {
                    valid = false;
                }
            }
            return *this;
        }
            
            MultiMapTuple DiskMultiMap::Iterator::operator*()
        {
            MultiMapTuple info;
            if(isValid())
            {
                info = data;
            }
            return info;
        }
            
            
            
            
