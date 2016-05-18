//
//  IntelMap.h
//  Project 4-CS32
//
//  Created by Caroline Quigg on 3/7/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//
#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <unordered_set>

class IntelWeb
{
public:
    IntelWeb() {}
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
                       );
    bool purge(const std::string& entity);
    
private:
    // Your private member declarations will go here
    bool failsPrev(const string& entity , unsigned int minPrev);
    DiskMultiMap initiate;
    DiskMultiMap target;
    unordered_set<string> goodVals;
};

#endif // INTELWEB_H_

