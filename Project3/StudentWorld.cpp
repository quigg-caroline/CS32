#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <cmath>
#include <queue>
using namespace std;

struct Coord
{
    int x;
    int y;
};

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

//fills in dirt for level by initializing 2-D array with Dirt pointers
//empty spots represented by nullptr
void StudentWorld::fillDirt()
{
    for (int i =0 ; i<VIEW_WIDTH ;i++)
    {
        for (int j = 0 ; j<VIEW_HEIGHT; j++)
        {
            //fill exit
            exit[i][j] = -1;
            if ((i>=VIEW_WIDTH/2-SPRITE_WIDTH/2 && i<VIEW_WIDTH/2 + SPRITE_WIDTH/2
                && j>=SPRITE_HEIGHT && j<VIEW_HEIGHT - SPRITE_HEIGHT) || j>=VIEW_HEIGHT-SPRITE_WIDTH)
            {
                oilfield [i][j] = nullptr;
            }
            else
                oilfield[i][j] = new Dirt(i,j,this);
            
        }
    }
}


//create a 2d array of ints based on x,y,pos
void StudentWorld::updateDataStruct(int x, int y, int arr[][VIEW_HEIGHT])
{
    //reset exit to empty
    for (int i =0 ; i<VIEW_WIDTH ; i ++)
    {
        for(int j=0; j<VIEW_HEIGHT; j++)
        {
            arr[i][j]=-1;
        }
    }
    queue<Coord> field;
    //start at exit
    Coord check;
    check.x = x;
    check.y = y;
    field.push(check);
    arr[check.x][check.y] = 0;
    while(!field.empty())
    {
        check = field.front();
        field.pop();
        //check north
        if(canMove(check.x,check.y,GraphObject::up) && !touching(check.x,check.y+1,3,frackBoi)&&arr[check.x][check.y+1]<0)
        {
            Coord good;
            good.x = check.x;
            good.y = check.y+1;
            field.push(good);
            arr[check.x][check.y+1] = arr[check.x][check.y] +1;
        }
        //check east
        if(canMove(check.x,check.y,GraphObject::right) && !touching(check.x+1,check.y,3,frackBoi)&&arr[check.x+1][check.y]<0)
        {
            Coord good;
            good.x = check.x+1;
            good.y = check.y;
            field.push(good);
            arr[check.x+1][check.y] = arr[check.x][check.y] +1;
        }
        //check south
        if(canMove(check.x,check.y,GraphObject::down) && !touching(check.x,check.y-1,3,frackBoi)&&arr[check.x][check.y-1]<0)
        {
            Coord good;
            good.x = check.x;
            good.y = check.y-1;
            field.push(good);
            arr[check.x][check.y-1] = arr[check.x][check.y] +1;
        }
        //check west
        if(canMove(check.x,check.y,GraphObject::left) && !touching(check.x-1,check.y,3,frackBoi)&&arr[check.x-1][check.y]<0)
        {
            Coord good;
            good.x = check.x-1;
            good.y = check.y;
            field.push(good);
            arr[check.x-1][check.y] = arr[check.x][check.y] +1;
        }
    }
    /*
    //print out exit array
    for(int i = 63; i>40; i --)
    {
        for (int j = 25; j<64 ; j++)
        {
            if(exit[j][i]<10 && exit[j][i]>0)cout<<" ";
            cout << exit[j][i] << " ";
        }
        cout << endl;
    }
    cout << "/////////////////////////"<<endl;*/
}

GraphObject::Direction StudentWorld::clearLineOfSight(int x, int y)
{
    GraphObject::Direction see = GraphObject::none;
    if (frackBoi->getY() == y)
    {
        //if frackman is onleft
        if (x>frackBoi->getX())
        {
            if (trackFrack[x][y]==x-frackBoi->getX())
                see=GraphObject::left;
        }
        //if frackman is on right
        else
        {
            if(trackFrack[x][y] == frackBoi->getX()-x)
                see = GraphObject::right;
        }
    }
    else if(frackBoi->getX()==x)
    {
        //if frackman is above
        if (y<frackBoi->getY())
        {
            if(trackFrack[x][y] == frackBoi->getY()-y)
            {
                see = GraphObject::up;
            }
        }
        //if frackman is below
        if(trackFrack[x][y] == y-frackBoi->getY())
            see = GraphObject::down;
    }
    return see;
}

GraphObject::Direction StudentWorld::getShortestDir(int x, int y)
{
    GraphObject::Direction dir = GraphObject::none;
    int shortest = -1;
    //check up
    if(y<60 && exit[x][y+1]>=0 && canMove(x,y,GraphObject::up))
    {
        shortest = exit[x][y+1];
        dir = GraphObject::up;
    }
    //check down
    if(y>0 && exit[x][y-1]>=0)
    {
        if(shortest<0 || (shortest>exit[x][y-1] && canMove(x,y,GraphObject::down)))
        {
            shortest = exit[x][y-1];
            dir = GraphObject::down;
        }
    }
    //check left
    if(x>0 && exit[x-1][y]>=0)
    {
        if(shortest<0 || (shortest>exit[x-1][y] && canMove(x,y,GraphObject::left)))
        {
            shortest=exit[x-1][y];
            dir = GraphObject::left;
        }
    }
    //check right
    if(x<60 && exit[x+1][y]>=0)
    {
        if(shortest<0||(shortest>exit[x+1][y]&&canMove(x,y,GraphObject::right)))
        {
            shortest = exit[x+1][y];
            dir = GraphObject::right;
        }
    }
    return dir;
}

GraphObject::Direction StudentWorld::getFrackDir(int x, int y)
{
    GraphObject::Direction dir = GraphObject::none;
    int shortest = -1;
    //check up
    if(y<60 && trackFrack[x][y+1]>=0 && canMove(x,y,GraphObject::up))
    {
        shortest = trackFrack[x][y+1];
        dir = GraphObject::up;
    }
    //check down
    if(y>0 && trackFrack[x][y-1]>=0)
    {
        if(shortest<0 || (shortest>trackFrack[x][y-1] && canMove(x,y,GraphObject::down)))
        {
            shortest = trackFrack[x][y-1];
            dir = GraphObject::down;
        }
    }
    //check left
    if(x>0 && trackFrack[x-1][y]>=0)
    {
        if(shortest<0 || (shortest>trackFrack[x-1][y] && canMove(x,y,GraphObject::left)))
        {
            shortest=trackFrack[x-1][y];
            dir = GraphObject::left;
        }
    }
    //check right
    if(x<60 && trackFrack[x+1][y]>=0)
    {
        if(shortest<0||(shortest>trackFrack[x+1][y]&&canMove(x,y,GraphObject::right)))
        {
            shortest = trackFrack[x+1][y];
            dir = GraphObject::right;
        }
    }
    return dir;
}

//delete the dirt that overlaps Frackman postioned at (x,y)
//play dig noise if dirt deleted
bool StudentWorld::deleteDirt(int x, int y)
{
    bool deleted=false;
    //delete dirt that is overlapping
    for(int i =x ; i<x+SPRITE_WIDTH; i++)
    {
        for(int j = y; j<y+SPRITE_HEIGHT; j++)
        {
            //check that there could be dirt there
            if (i<VIEW_WIDTH && j<VIEW_HEIGHT-SPRITE_HEIGHT)
            {
                if(oilfield[i][j]!=nullptr)deleted=true;
                delete oilfield[i][j];
                oilfield[i][j]=nullptr;
            }
        }
    }
    
    return deleted;
}

//check the radius between two objects
bool StudentWorld::radius(int x1, int y1, int x2,int y2,double r)
{
    double dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    return dist<=r;
}

//assigns x and y to the x,y position of frackman respectively
void StudentWorld::getFracked(int& x, int &y)
{
    x = frackBoi->getX();
    y = frackBoi->getY();
}

//check if an actor at position (x1,y1) is touching another actor that it is not allowed to touch within a certain radious
bool StudentWorld::touching(int x1, int y1, double r, Actor * check)
{
    vector<Actor*>::iterator x = objects.begin();
    bool touch = false;
    while (x!=objects.end())
    {
        if(radius(x1,y1,(*x)->getX(), (*x)->getY() , r) && !(*x)->canShare() && check != *x)
        {

            touch = true;
            break;
        }
        x++;
    }
    return touch;
}

//checks if there is dirt or boulder in the specified direction
bool StudentWorld::canMove(int x, int y, Actor::Direction dir)
{
    bool move = true;
    switch(dir)
    {
        case Actor::left:
            if(x<=0)
            {move = false;
                break;}
            else
            {
                for (int i=0;i<SPRITE_HEIGHT; i++)
                {
                    if(oilfield[x-1][y+i] != nullptr)
                    {
                        move=false;
                    }
                }
            }
            break;
        case Actor::right:
            if(x>=VIEW_WIDTH){move = false;break;}
            else
            {
                for (int i=0;i<SPRITE_HEIGHT; i++)
                {
                    if(oilfield[x+SPRITE_WIDTH][y+i] != nullptr)
                    {
                        move = false;
                    }
                }
            }
            break;
        case Actor::up:
            if(y>=VIEW_HEIGHT- SPRITE_HEIGHT){move = false;break;}
            else
            {
                for (int i=0;i<SPRITE_WIDTH; i++)
                {
                    if(oilfield[x+i][y+SPRITE_HEIGHT] != nullptr)
                    {
                        move=false;
                    }
                }
            }
            break;
        case Actor::down:
            if(y<=0){move = false;break;}
            else
            {
                for (int i=0;i<SPRITE_WIDTH; i++)
                {
                    if(oilfield[x+i][y-1] != nullptr)
                    {
                        move = false;
                    }
                }
            }
            break;
        default:
            break;
    }
    return move;
}

void appendToEnd(string& c, int x)
{
    if(x>=99) c+=to_string(99);
    else
    {
        //number is less than 10 need leading space
        if(x<10)
        {
            c+=" ";
            c+=to_string(x);
        }
        //no leading space needed
        else
        {
            c+=to_string(x);
        }
    }
}

void StudentWorld::updateText()
{
    int level = getLevel();
    int lives = getLives();
    int health = frackBoi->getHealth();
    int squirts = frackBoi->getWater();
    int gold = frackBoi->getNugget();
    int sonar = frackBoi->getSonar();
    string text = "Scr: ";
    //convert score to leading zero form
    int temp  = getScore();
    int digit=0;
    while (temp!=0)
    {
        temp/=10;
        digit++;
    }
    if (digit>6) text+=to_string(9999999);
    else
    {
        for (int i=0; i<6-digit;i++)
        {
            text+='0';
        }
        if(getScore()>0)text+=to_string(getScore());
    }
    //add level
    text+="  Lvl: ";
    appendToEnd(text,level);
    
    //add lives
    text+="  Lives: ";
    if(lives>9) text+=to_string(9);
    else
    {
        text+=to_string(lives);
    }
    
    //add health
    text+="  Hlth: ";
    if(health==100) text+=to_string(health);
    else
    {
        text+=" ";
        text+=to_string(health);
    }
    //add water
    text+="%  Wtr: ";
    appendToEnd(text,squirts);
    //add gold
    text+="  Gld: ";
    appendToEnd(text,gold);
    
    //add sonar
    text+="  Sonar: ";
    appendToEnd(text,sonar);
    //add oil
    text+="  Oil Left: ";
    appendToEnd(text,barrels);
    setGameStatText(text);
    
}



void StudentWorld::pickedUpWater()
{
    frackBoi->addWater();
    increaseScore(100);
}

void StudentWorld::pickedUpSonar()
{
    frackBoi->addSonar();
    increaseScore(75);
}

void StudentWorld::pickedUpBarrel()
{
    barrels--;
    increaseScore(1000);
}

void StudentWorld::pickedUpNugget()
{
    frackBoi->addNugget();
    increaseScore(10);
}
void StudentWorld::doSonar()
{
    vector<Actor*>::iterator x = objects.begin();
    while(x!=objects.end())
    {
        if (radius(frackBoi->getX(),frackBoi->getY(),(*x)->getX(), (*x)->getY(),12))
        {
            (*x)->setVisible(true);
            (*x)->setHidden(false);
        }
        x++;
    }
}

void StudentWorld::makeSquirt(Squirt*s)
{
    objects.push_back(s);
}

void StudentWorld::makeBribe()
{
    //create a new nugget at frackman's position that is NOT pickup-able by frackman
    objects.push_back(new Nugget(frackBoi->getX(),frackBoi->getY(),this, false));
}

bool StudentWorld::bribeActors(int x,int y)
{
    bool bribed = false;
    vector<Actor*>::iterator z = objects.begin();
    while(z!=objects.end())
    {
        if(radius(x,y,(*z)->getX(),(*z)->getY(),3))
        {
            if((*z)->canPick())
            {
                bribed = true;
                (*z)->recieveGold();
                break;
            }
        }
        z++;
    }
    return bribed;
}

int StudentWorld::annoyActors(int x, int y,int val )
{
    int annoyed = 0;
    vector<Actor*>::iterator z = objects.begin();
    while (z!=objects.end())
    {
        if(radius(x,y,(*z)->getX(), (*z)->getY(),3))
        {
        //if the actor is able to get annoyed
        if((*z)->getAnnoyed(val))
        {
            annoyed++;
        }
        }
        z++;
    }
    return annoyed;
}


int StudentWorld::init()
{
    fillDirt();
    frackBoi = new FrackMan(this);
    
    //make boulders
    int z = getLevel()/2 +2;
    int b = min(z, 6);
    while (b>0)
    {
        bool add = true;
        int x = rand()%60;
        int y =  rand() %37 + 20;
        if(x>=VIEW_WIDTH/2-SPRITE_WIDTH-2 && x<VIEW_WIDTH/2 + SPRITE_WIDTH+2 && y>=SPRITE_HEIGHT && y<VIEW_HEIGHT - SPRITE_HEIGHT ) continue;
        vector<Actor*>::iterator it = objects.begin();
        while (it!=objects.end())
        {
            if (radius(x,y,(*it) ->getX(), (*it) ->getY(), 6))
            {
                add = false;
                break;
            }
            it++;
        }
        if (add)
        {
            Boulder * bould = new Boulder (x,y,this);
            objects.push_back(bould);
            deleteDirt(x,y);
            b--;
        }
 
    }

    
    //make barrels
    int v = 2 + getLevel();
    barrels = min(v, 20);
    int  c =barrels;
    while (c>0)
    {
        bool add = true;
        int x = rand()%60;
        int y = rand()%57;
        vector<Actor*>::iterator it = objects.begin();
        if(x>=VIEW_WIDTH/2-SPRITE_WIDTH-2 && x<VIEW_WIDTH/2 + SPRITE_WIDTH+2 && y>=SPRITE_HEIGHT && y<VIEW_HEIGHT - SPRITE_HEIGHT ) continue;
        while (it!=objects.end())
        {
            if (radius(x,y,(*it) ->getX(), (*it) ->getY(), 6) )
            {
                add = false;
                break;
            }
            it++;
        }
        if (add)
        {
            objects.push_back(new Barrel(x,y,this));
            c--;
        }
    }
    
    //make nuggets
    int nug = 5 - getLevel()/2;
    nug = max(nug,2);
    while (nug>0)
    {
        bool add = true;
        int x = rand()%60;
        int y = rand()%57;
        if(x>=VIEW_WIDTH/2-SPRITE_WIDTH-2 && x<VIEW_WIDTH/2 + SPRITE_WIDTH+2 && y>=SPRITE_HEIGHT && y<VIEW_HEIGHT - SPRITE_HEIGHT ) continue;
        vector<Actor*>::iterator it = objects.begin();
        while (it!=objects.end())
        {
            if (radius(x,y,(*it) ->getX(), (*it) ->getY(), 6))
            {
                add = false;
                break;
            }
            it++;
        }
        if (add)
        {
            objects.push_back(new Nugget(x,y,this,true));
            nug--;
        }
    }
//make first protester and initialize the protester adding info
    int f = getLevel()*10 +30;
    add_protest = 0;
    num_protest = 0;
    hardcore_prob = std::min(90,f);
    int r  = rand()%100 +1;
    if (r<=hardcore_prob)
        objects.push_back(new HardCore(this));
    else
        objects.push_back(new RegProtest(this));
    num_protest++;
    
    return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move()
{

    updateText();
    updateDataStruct(frackBoi->getX(),frackBoi->getY(),trackFrack);
    updateDataStruct(60,60,exit);
    //have all other actors do stuff
    std::vector<Actor*>::iterator x = objects.begin();
    while (x!= objects.end())
    {
        (*x)->doSomething();
        x++;
    }
    
    //have frackMan move
    frackBoi->doSomething();
    //update shortest path
 
    
    //check if barrels have been picked up
    if (barrels<=0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    //generate goodies
    int g = getLevel()*25 + 300;
    int guess = rand()%g +1;
    //add goody
    if (guess == 1)
    {
        bool added = false;
        int guess2 = rand()%5 +1;
        //add a water pool
        if (guess2 !=1)
        {
            int x,y;
            while (!added)
            {
            x  = rand()%61;
            y = rand()%61;
            //check for empty space
            if (oilfield[x][y] == nullptr)
            {
                added = true;
                for (int i = 0 ; i<SPRITE_WIDTH ; i++)
                {
                    for (int j = 0 ; j<SPRITE_HEIGHT; j++)
                    {
                        if (oilfield[i+x][j+y] != nullptr)
                        {
                            added  =false;
                            break;
                        }
                    }
                    if(!added) break;
                }
            }
            } ;
            objects.push_back(new WaterPool(x,y,this));
        }
        //else add sonar
        else
        objects.push_back(new Sonar(0,60,this));
    }
    
    //add protesters if needed
    int h = 2 + getLevel()*1.5;
    int f = 200 - getLevel();
    int max_protest =std::min(15,h);
    int wait =std::max(25,f);
    add_protest++;
    //max isn't there and number of ticks has passed
    if(num_protest<max_protest && add_protest%wait == 0)
    {
        int r = rand()%101 +1;
        if (r<=hardcore_prob)
        {
            objects.push_back(new HardCore(this));
        }
        else
            objects.push_back(new RegProtest(this));
        num_protest++;
    }
    
    //remove all dead actors
    x = objects.begin();
    while (x!= objects.end())
    {
        if (!((*x)->isAlive()))
        {
            //if a person was deleted its a protester
            if ((*x)->getAnnoyed(0)) num_protest--;
            delete *x;
            x = objects.erase(x);
        }else x++;
    }
    
    //check if died
    if(!frackBoi->isAlive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}
