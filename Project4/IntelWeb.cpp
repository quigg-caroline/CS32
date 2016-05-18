//
//  IntelWeb.cpp
//  Project 4-CS32
//
//  Created by Caroline Quigg on 3/7/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//
#include "IntelWeb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <queue>


bool operator< (const InteractionTuple& a, const InteractionTuple& b)
{
    if(a.context<b.context) return true;
    if(a.context == b.context && a.from<b.from) return true;
    if(a.context==b.context && a.from == b.from && a.to < b.to) return true;
    return false;
}

bool IntelWeb::failsPrev(const string& entity , unsigned int minPrev)
{
    if (goodVals.find(entity) != goodVals.end())return false;
    int count=0;
    //look how many times the entity appears as a key
    DiskMultiMap::Iterator find = initiate.search(entity);
    while(find.isValid())
    {
        count++;
        if (count >=minPrev)
        {
            goodVals.insert(entity);
            return false;
        }
        ++find;
    }
    //look how many times the entity appears as a value
    find = target.search(entity);
    while(find.isValid())
    {
        count++;
        if (count >=minPrev)
        {
            goodVals.insert(entity);
            return false;
        }
        ++find;
    }
    
    return true;
}

IntelWeb::~IntelWeb()
{
    close();
}

void IntelWeb::close()
{
    initiate.close();
    target.close();
}

bool IntelWeb::openExisting(const std::string &filePrefix)
{
    close();
    if (!initiate.openExisting(filePrefix+"_init"))
        return false;
    if(!target.openExisting(filePrefix+"_targ"))
    {
        initiate.close();
        return false;
    }
    return true;
}

bool IntelWeb::createNew(const std::string &filePrefix, unsigned int maxDataItems)
{
    close();
    unsigned int buckets = maxDataItems/.75;
    if (!initiate.createNew(filePrefix+"_init",buckets)) return false;
    if (!target.createNew(filePrefix+"_targ",buckets))
    {
        initiate.close();
        return false;
    }
    return true;
}

bool IntelWeb::ingest(const std::string& telemetryFile)
{
    ifstream inf(telemetryFile);
		  // Test for failure to open
    if (!inf)
    {
        cout << "Cannot open file!" << endl;
        return false;
    }
    

    string line;
    while (getline(inf, line))
    {
        // To extract the information from the line, we'll
        // create an input stringstream from it, which acts
        // like a source of input for operator>>
        istringstream iss(line);
        string context;
        string init;
        string targ;
        // The return value of operator>> acts like false
        // if we can't extract a word followed by a number
        if ( ! (iss >> context >> init >> targ) )
        {
            return false;
        }
        // If we want to be sure there are no other non-whitespace
        // characters on the line, we can try to continue reading
        // from the stringstream; if it succeeds, extra stuff
        // is after the double.
        
        // Add data to expenses map
        initiate.insert(init,targ,context);
        target.insert(targ,init,context);
    }
    return true;
}

bool IntelWeb::purge(const std::string& entity)
{
    bool removed = false;
    
    DiskMultiMap::Iterator purge1 = initiate.search(entity);
    DiskMultiMap::Iterator purge2 = target.search(entity);
    MultiMapTuple stuff;
    //go through initiate map
    while (purge1.isValid())
    {
        removed  = true;
        stuff = *purge1;
        ++purge1;
        initiate.erase(stuff.key,stuff.value,stuff.context);
        target.erase(stuff.value,stuff.key,stuff.context);
    }
    
    //go through target map
    while(purge2.isValid())
    {
        removed = true;
        stuff = *purge2;
        ++purge2;
        target.erase(stuff.key,stuff.value,stuff.context);
        initiate.erase(stuff.value,stuff.key,stuff.context);
    }
    
    return removed;
}


unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,unsigned int minPrevalenceToBeGood,
std::vector<std::string>& badEntitiesFound,std::vector<InteractionTuple>& interactions)
{
    set<string> uniqueBad;
    set<InteractionTuple> uniqueInteract;
    
    badEntitiesFound.clear();
    interactions.clear();
    
    //for each item in the vector of indicators
    for (int i =0 ; i<indicators.size() ;i++)
    {
        //haven't discovered the entity on a previous crawl
        if(uniqueBad.find(indicators[i]) == uniqueBad.end())
        {
            //do the crawl
            queue<string> q;
            
            //lookup in maps
            
            //lookup in first map
            DiskMultiMap::Iterator find = initiate.search(indicators[i]);
            
            //entity appears as an initiator
            if(find.isValid())
            {
                //push the item
                q.push(indicators[i]);
                
                //while you discover new entities
                while (!q.empty())
                {
                    string key = q.front();
                    q.pop();
                    if (uniqueBad.find(key) !=uniqueBad.end())continue;
                    uniqueBad.insert(key);

                    //lookup entity
                    find = initiate.search(key);
                    
                    
                    //while there is a key to find
                    while (find.isValid())
                    {
                        MultiMapTuple info = *find;
                        
                        //log the interaction
                        uniqueInteract.insert(InteractionTuple(info.key, info.value, info.context));
                        
                        //if the value does not meet prevalence req AND it had already not been found
                        //value is a bad entity
                        if (uniqueBad.find(info.value) == uniqueBad.end() && failsPrev(info.value,minPrevalenceToBeGood))
                        q.push(info.value);
                        ++find;
                    }
                    
                    find = target.search(key);
                    while (find.isValid())
                    {
                        MultiMapTuple info = *find;
                        
                        //log the interaction
                        uniqueInteract.insert(InteractionTuple(info.value, info.key, info.context));
                        
                        //if the value does not meet prevalence req AND it had already not been found
                        //value is a bad entity
                        if (uniqueBad.find(info.value) == uniqueBad.end()&& failsPrev(info.value,minPrevalenceToBeGood))
                            q.push(info.value);
                        ++find;
                    }
                    
                }
            }
            
            //lookup in second map if not found during the first map crawl
            if (uniqueBad.find(indicators[i]) == uniqueBad.end())
            {
            find = target.search(indicators[i]);
            if (find.isValid())
            {
                //push the item
                q.push(indicators[i]);
                
                //while you discover new entities
                while (!q.empty())
                {

                string key = q.front();
                if (uniqueBad.find(key) !=uniqueBad.end())continue;
                uniqueBad.insert(key);
                q.pop();
                
                //lookup entity
                find = target.search(key);
                
                
                //while there is a key to find
                while (find.isValid())
                {
                    MultiMapTuple info = *find;
                    
                    //log the interaction
                    uniqueInteract.insert(InteractionTuple(info.value, info.key, info.context));
                    
                    //if the value does not meet prevalence req AND it had already not been found
                    //value is a bad entity
                    if (uniqueBad.find(info.value) == uniqueBad.end() && failsPrev(info.value,minPrevalenceToBeGood))
                        q.push(info.value);
                    ++find;
                }
                    find = initiate.search(key);
                    while (find.isValid())
                    {
                        MultiMapTuple info = *find;
                        
                        //log the interaction
                        uniqueInteract.insert(InteractionTuple(info.key, info.value, info.context));
                        
                        //if the value does not meet prevalence req AND it had already not been found
                        //value is a bad entity
                        if (uniqueBad.find(info.value) == uniqueBad.end() && failsPrev(info.value,minPrevalenceToBeGood))
                            q.push(info.value);
                        ++find;
                    }
                
            }
            }
        }
    }
        
    
    
    }
    //convert sets to vectors
    set<string>::iterator it = uniqueBad.begin();
    while (it != uniqueBad.end())
    {
        badEntitiesFound.push_back(*it);
        it++;
    }
    
    set<InteractionTuple>::iterator it2 = uniqueInteract.begin();
    while (it2 != uniqueInteract.end())
    {
        interactions.push_back(*it2);
        it2++;
    }
    

    return badEntitiesFound.size();

}


