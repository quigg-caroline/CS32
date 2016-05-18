#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
    : GameWorld(assetDir){}

    virtual int init();

    virtual int move();

	virtual void cleanUp()
    {
        delete frackBoi;
        frackBoi = nullptr;
        for (int  i=0 ; i<VIEW_WIDTH; i++)
        {
            for (int j =0; j<VIEW_HEIGHT-SPRITE_HEIGHT; j++)
            {
                delete oilfield[i][j];
                oilfield[i][j] = nullptr;
            }
        }
        
        std::vector<Actor*>::iterator x = objects.begin();
        while (x!= objects.end())
        {
            delete *x;
            x = objects.erase(x);
        }

	}
    
    bool deleteDirt(int x, int y);
    
    bool canMove(int x, int y, GraphObject::Direction dir);
    
    bool radius(int x1, int y1, int x2, int y2, double r);
    
    bool touching(int x1, int y1, double r, Actor* check);
    
    void getFracked(int& x1, int& y);
    
    void pickedUpWater();
    
    void pickedUpSonar();
    
    void pickedUpBarrel();
    
    void pickedUpNugget();
    
    void doSonar();
    
    void makeSquirt(Squirt* s);
    
    void makeBribe();
    
    void updateText();
    
    void annoyFrack(int val){frackBoi->getAnnoyed(val);}
    
    int annoyActors(int x, int y, int val);
    
    bool bribeActors(int x, int y);
    
    void updateDataStruct(int x,int y,int arr[][VIEW_HEIGHT]);
    
    GraphObject::Direction clearLineOfSight(int x, int y);
    
    GraphObject::Direction getShortestDir(int x, int y);
    
    GraphObject::Direction getFrackDir(int x, int y);
    virtual ~StudentWorld()
    {
        delete frackBoi;
        frackBoi = nullptr;
        for (int  i=0 ; i<VIEW_WIDTH; i++)
        {
            for (int j =0; j<VIEW_HEIGHT-SPRITE_HEIGHT; j++)
            {
                delete oilfield[i][j];
                oilfield[i][j] = nullptr;
            }
        }
        
        std::vector<Actor*>::iterator x = objects.begin();
        while (x!= objects.end())
        {
            delete *x;
            x = objects.erase(x);
        }


    }

private:
    void fillDirt();
    FrackMan* frackBoi;
    Dirt* oilfield [VIEW_WIDTH][VIEW_HEIGHT];
    std::vector<Actor*> objects;
    int barrels;
    int num_protest;
    int add_protest;
    int hardcore_prob;
    int exit[VIEW_WIDTH][VIEW_HEIGHT];
    int trackFrack[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_
