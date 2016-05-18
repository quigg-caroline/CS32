//
//  DiskMultiMap.h
//  Project 4-CS32
//
//  Created by Caroline Quigg on 3/5/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

const int MAX_LEN = 120;

class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        Iterator(DiskMultiMap * src, BinaryFile::Offset pos);
        bool isValid() const {return valid;}
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        bool valid;
        DiskMultiMap* m_file;
        BinaryFile::Offset cur;
        MultiMapTuple data;
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);

    
private:
    // Your private member declarations will go here
    BinaryFile m_bf;
    
    unsigned int hash(const std::string &hashed);
    
    struct DiskNode
    {
        BinaryFile::Offset next;
        char key [MAX_LEN+1];
        char value [MAX_LEN+1];
        char context [MAX_LEN+1];
    };
    
    struct header
    {
        unsigned int numBuckets;
        BinaryFile::Offset empty;
    };
    
};

#endif // DISKMULTIMAP_H_


