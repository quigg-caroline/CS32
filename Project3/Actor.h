#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

/**
 *Abstract ACTOR class*
 base class for all actor objects: Frackman, Dirt etc.
 derived from GraphObject,
 makes self visible and alive
**/

//HELPER FUNCTIONS//
//returns the x and y val of where you want to move given a direction
bool moveInDir(int& x, int &y, GraphObject::Direction dir);

class Actor:public GraphObject
{
public:
    Actor(int imageID, int startX, int startY,StudentWorld* world,Direction dir=right, double size = 1.0, unsigned int depth = 0,bool sharespace= true)
    :GraphObject(imageID,startX,startY,dir,size,depth)
    {
        //make self visible, alive and able to shar space
        m_world = world;
        share = sharespace;
        setVisible(true);
        alive = true;
        hidden = false;
        pickup = false;
    }
    virtual ~Actor() {};
    
    //accessors
    bool isAlive() const {return alive;}
    StudentWorld* getWorld() const{return m_world;}
    bool canShare() const {return share;}
    bool isHidden() const {return hidden;}
    bool canPick()const{return pickup;}
    
    //setting functions
    void canPickUp(bool val){pickup = val;}
    void setAlive(bool set) {alive = set;}
    void setHidden(bool set){hidden = set;}
    
    //doStuff functions
    virtual void doSomething()=0;
    virtual void recieveGold(){};
    virtual bool getAnnoyed(int val){return false;}
private:
    bool pickup;
    bool alive;
    bool share;
    bool hidden;
    StudentWorld* m_world;
};

//*BOULDER**//
class Boulder:public Actor
{
public:
    Boulder(int x, int y, StudentWorld* w)
    :Actor(IID_BOULDER,x,y,w,down,1.0,1.0,false), stable(true), waiting(0), falling(false){};
    virtual ~Boulder(){};
    virtual void doSomething();
private:
    bool stable;
    int waiting;
    bool falling;
};


//**DIRT**//
/*
 -is constructed at a specified position
 -doesn't really do anything
 **/
class Dirt:public Actor
{
public:
    Dirt(int startX, int startY, StudentWorld* world)
    :Actor(IID_DIRT,startX,startY,world,right,0.25,3)
    {}
    virtual ~Dirt(){};
    //dirt doesn't really do anything but sit there
    virtual void doSomething(){};
};

//**WATER SQUIRT**//
class Squirt:public Actor
{
public:
    Squirt(int x, int y, StudentWorld*w, Direction dir): Actor(IID_WATER_SPURT,x,y,w, dir), distance(0){}
    virtual ~Squirt(){};
    virtual void doSomething();
private:
    int distance;
    
};

//** PEOPLE BASE CLASS**//
//all people are derived from Actors and have a number of hits
//all people can get annoyed
class People:public Actor
{
public:
    People(int imageID, int startX, int startY,StudentWorld* world, int health, Direction dir)
    :Actor(imageID,startX,startY,world,dir),m_hits(health){}
    
    virtual ~People(){};
    
    //accessor functions
    int getHits() {return m_hits;}
    void updateHits(int val){m_hits-=val;}
    
    //modify functions
    virtual bool getAnnoyed(int val)
    {
        return true;
    }
    
    //virtual functions
private:
    int m_hits;
    
};

//**GOODIES**//
class Goodies:public Actor
{
public:
    Goodies(int imageID, int startX,int startY, StudentWorld* world, int sound = SOUND_GOT_GOODIE, Direction dir = right, double size = 1.0, unsigned int depth = 2.0 ) : Actor (imageID, startX, startY, world, dir, size, depth), m_sound(sound),pickedup(false){};
    int sound() {return m_sound;}
    virtual ~Goodies(){};
    bool wasPicked()const{return pickedup;}
    virtual void doSomething();
private:
    int m_sound;
    bool pickedup;
};

//**SPAWNABLE GOODIES**//
class Spawn:public Goodies
{
public:
    Spawn(int imageID,int x, int y, StudentWorld* w);
    virtual ~Spawn(){};
    virtual void doSomething();
private:
    int time;
};

//**BURIED GOODIES**//
class Buried:public Goodies
{
public:
    Buried(int imageID, int x, int y, StudentWorld*w, int sound = SOUND_GOT_GOODIE);
    virtual ~Buried(){};
    virtual void doSomething();
    
};

//**SONAR KIT**//
class Sonar:public Spawn
{
public:
    Sonar(int x, int y, StudentWorld*w):Spawn(IID_SONAR,x,y,w){};
    virtual ~Sonar(){};
    virtual void doSomething();
    
};

//**WATER POOL**//
class WaterPool:public Spawn
{
public:
    WaterPool(int x, int y,StudentWorld* world);
    virtual void doSomething();
    virtual ~WaterPool(){};
private:
    int time;
};

//**BARREL**//
class Barrel:public Buried
{
public:
    Barrel(int x, int y, StudentWorld*w);
    virtual ~Barrel(){};
    virtual void doSomething();
};

//**GOLD NUGGETS**//
class Nugget:public Buried
{
public:
    Nugget(int x, int y, StudentWorld*w, bool pickup);
    virtual void doSomething();
    virtual ~Nugget(){};
private:
    bool frack_pick;
    int time;
};

//**FRACKMAN**//
class FrackMan:public People
{
public:
    FrackMan(StudentWorld* w):People(IID_PLAYER,30,60,w,10,right), water(5), sonar(1), nugget(0){};
    virtual void doSomething();
    void addWater() {water+=5;}
    void addSonar() {sonar+=2;}
    void addNugget() {nugget++;}
    
    int getWater(){return water;}
    int getSonar(){return sonar;}
    int getNugget(){return nugget;}
    int getHealth(){return getHits()*10;}
    
    virtual bool getAnnoyed(int val);
    
    virtual ~FrackMan(){};
    
    //do stuffs
private:
    int water;
    int sonar;
    int nugget;

};

//**PROTESTER BASE CLASS**//
class Protester:public People
{
public:
    Protester(int id, StudentWorld* w,int hits,int annoy);
    virtual void doSomething();
    virtual bool getAnnoyed(int hits);
    virtual ~Protester(){};
    bool isLeaving()const{return leaveTheField;}
    int getResting()const {return ticksToWaitBetweenMoves;}
    void setResting(int val){ticksToWaitBetweenMoves = val;}
    void leave(){leaveTheField=true;}
private:
    bool leaveTheField;
    int ticksToWaitBetweenMoves;
    int restTicks;
    int toMoveInDir;
    int movedPerp;
    int annoyPoints;
};

//**REGULAR PROTESTER**//
class RegProtest:public Protester
{
public:
    RegProtest(StudentWorld*w):Protester(IID_PROTESTER,w,5,100){};
    virtual void recieveGold();
    virtual ~RegProtest(){};
};

//**HARDCORE PROTESTER**//
class HardCore:public Protester
{
public:
    HardCore(StudentWorld*w);
    virtual void recieveGold();
    virtual void doSomething();
    virtual ~HardCore(){};
private:
    int stare_ticks;
    int signal;
};

#endif // ACTOR_H_


