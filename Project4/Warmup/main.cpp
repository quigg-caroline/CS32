//
//  main.cpp
//  Project 4 warmup
//
//  Created by Caroline Quigg on 3/1/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//




#include "BinaryFile.h"
#include <iostream>
#include <cstring>
#include <vector>

const int MAX_LENGTH = 256;
struct DiskNode
{
    char info [MAX_LENGTH];
    BinaryFile::Offset next;
};

class DiskList
{
public:
    DiskList(const std::string& filename);
    bool push_front(const char* data);
    bool remove(const char* data);
    void printAll();
private:
    BinaryFile m_bf;
    BinaryFile::Offset cur;
    BinaryFile::Offset head;
    BinaryFile::Offset empty;
};


DiskList::DiskList(const std::string& filename)
{
    m_bf.createNew(filename);
    //create head pointer and set to "null"
    head = 0;
    m_bf.write(head,0);
    cur = sizeof(BinaryFile::Offset);
    
    //create the head pointer for the list of empty nodes
    empty =0;
    m_bf.write(empty,cur);
    cur += sizeof(BinaryFile::Offset);
}

bool DiskList::push_front(const char* data)
{
    //check length
    if (strlen(data)>= MAX_LENGTH)
    {
        return false;
    }
    
    //add to front
    
    //create a new node
    DiskNode add;
    //read from head ( have add.next point to what head points to)
    m_bf.read(add.next,0);
    strcpy(add.info,data);
    
    //write to disk
    //check if there is a previously vacated spot open
    if (empty == 0)
    {
        m_bf.write(add,cur);
        head =cur;
        cur+=sizeof(DiskNode);
    }
    else
    {
        BinaryFile::Offset newpos = empty;
        //unlink the position from the list
        m_bf.read(empty, empty +MAX_LENGTH);
        
        //add the new node at the positons
        m_bf.write(add, newpos);
        head  = newpos;
        //m_bf.write(newpos,0);
    }
    //set head to point to new node

    m_bf.write(head, 0);
    //update location to add at


    return true;
}

void DiskList::printAll()
{
    //first read the string
    if (head==0) return; //nothing to print
    
    BinaryFile::Offset mr;
    //read in from head
    m_bf.read(mr,0);
    //while not empty
    while(mr !=0)
    {
        //read to an empty disknode
        DiskNode stuff;
        m_bf.read(stuff,mr);
        cout << stuff.info<<endl;
        mr = stuff.next;
    }
    
}

bool DiskList::remove(const char* data)
{

    bool removed = false;
    BinaryFile::Offset p ;
    //read in from head
    m_bf.read(p,0);
    BinaryFile::Offset prev = 0;
    //iterate through the linked list
    while (p !=0)
    {
        DiskNode stuff;
        m_bf.read(stuff,p);
        //if a matching value found
        if (strcmp(stuff.info,data)==0) 
        {
            //previous node should point to the next node's next
            //overwrite previous node's
            
            BinaryFile::Offset overwrite = stuff.next;
            
            
            //update the linked list of empty positions
            
            //add the new open position to the front of the list
            m_bf.write(empty,p + MAX_LENGTH);
            //update head
            empty = p;
            m_bf.write(empty,sizeof(BinaryFile::Offset));
            
            
            //deleting first thing in linked list
            if (prev ==0 )
            {
                head = overwrite;
                m_bf.write(overwrite,0);
                p = overwrite;
            }
            else
            {
            m_bf.write(overwrite, prev+MAX_LENGTH);
            p = stuff.next;
            }
            removed = true;
        }
        else
        {
            prev =p;
            p = stuff.next;
        }
        
    }
    return removed;
}

int main()
{
    DiskList x("mylist.dat");
    x.push_front("Fred");
    x.push_front("Lucy");
    x.push_front("Ethel");
    x.push_front("Ethel");
    x.push_front("Lucy");
    x.push_front("Fred");
    x.push_front("Ethel");
    x.push_front("Ricky");
    x.push_front("Lucy");
    x.push_front("Lucy");
    x.remove("Lucy");
    x.push_front("Fred");
    x.push_front("Ricky");
   // x.remove("Ethel");
    x.printAll();  // writes, one per line
 
}