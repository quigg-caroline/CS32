


#include <iostream>
#include "DiskMultiMap.h"
#include "IntelWeb.h"
 int main()
{
    IntelWeb test;
    test.createNew("version1",25);
    test.ingest("/Users/carolinequigg/Google Drive/Project 4-CS32/Project 4-CS32/testlog1.dat");
    //test.printAll();
    //test.purge("zut.exe");
    //test.close();
    //test.openExisting("version1");
    //test.ingest("/Users/carolinequigg/Google Drive/Project 4-CS32/Project 4-CS32/testlog1.dat");
    vector<string> badThings;
    badThings.push_back("p.exe");
    badThings.push_back("qqqqqqq.x");
    vector<string> badThingsFound;
    vector<InteractionTuple> badInteract;
    test.crawl(badThings,5,badThingsFound,badInteract);
    
    cout << "BAD THINGS"<<endl;
    for(int i =0 ; i<badThingsFound.size(); i++)
    {
        cout<<badThingsFound[i]<<endl;
    }
    cout <<endl;
    
    for (int i =0 ; i<badInteract.size(); i++)
    {
        InteractionTuple c = badInteract[i];
        cout <<c.context << "   "<<c.from << "\t" << c.to <<endl;
    }
    
  
}
