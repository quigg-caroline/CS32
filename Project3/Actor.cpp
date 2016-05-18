#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


//HELPER FUNCTIONS//
bool moveInDir(int& x, int &y, GraphObject::Direction dir)
{
    bool moved = true;
    switch(dir)
    {
        case GraphObject::left :
            if(x>0)
            x--;
            else moved = false;
            break;
        case GraphObject::right:
            if(x<VIEW_WIDTH)
            x++;
            else moved =false;
            break;
        case GraphObject::up:
            if(y<VIEW_HEIGHT-SPRITE_HEIGHT)
            y++;
            else moved = false;
            break;
        case GraphObject::down:
            if(y>0)
            y--;
            else moved = false;
            break;
        default:
            break;
    }
return moved;
}

//boulder do something
void Boulder::doSomething()
{
    //check if alive
    if(!isAlive()) return;
    if(stable)
    {
        //check if dirt is below
        if (getWorld()->canMove(getX(),getY(),getDirection()))
        {
            //enter a waiting state
            stable = false;
            
        }
        else return;
    }
    //wait for 30 ticks
    else if (waiting <30) waiting ++;
    // in falling
    else
    {
        //transition into falling
        if (!falling)
        {
            getWorld()->playSound(SOUND_FALLING_ROCK);
            falling = true;
            return;
        }
        int x,y;
        getWorld()->getFracked(x,y);

        //continue moving until possible and scanning for people to annoy
        if (getWorld()->canMove(getX(),getY(),getDirection()) && !getWorld()->touching(getX(),getY()-1,3,this))
        {
            moveTo(getX(),getY()-1);
        }
        else
            setAlive(false);
        //if touching frackman
        if(getWorld()->radius(x,y,getX(),getY(),3))
        {
            getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
            getWorld()->annoyFrack(100);
        }
        
        //if touching other actors to annoy
        int annoyed = getWorld()->annoyActors(getX(),getY(),100);
    }
    
}

//WATER SQUIRT//
void Squirt::doSomething()
{
    //check for alive
    if(!isAlive())return;
    
    //check if within radius of protestor
    int annoy = getWorld()->annoyActors(getX(),getY(),2);
    if (annoy>0)
    {
        setAlive(false);
    }
    
    //traveled full distance
    if (distance>=4)
    {
        setAlive(false);
        return;
    }
    
    int newx = getX();
    int newy = getY();
    moveInDir(newx,newy,getDirection());
    //check if can move
    if(!getWorld()->canMove(getX(),getY(),getDirection()) || getWorld()->touching(newx,newy,3,this))
    {
        setAlive(false);
        return;
    }
    
    //otherwise continue moving
    else
    {
        moveTo(newx,newy);
        distance++;
    }
}


//GOODIES//
void Goodies::doSomething()
{
    if (!isAlive())return;
    //goodies are constantly searching to see if frackman is near them
    int x,y;
    //get frackman's position
    getWorld()->getFracked(x,y);
    
    //check and see if frackman is touching item
    if(getWorld()->radius(getX(),getY(),x,y,3))
    {
        //update status
        getWorld()->playSound(sound());
        setAlive(false);
        pickedup=true;
    }
    
}

//SPAWNABLE GOODY//
Spawn::Spawn(int id, int x, int y, StudentWorld* w)
:Goodies(id,x,y,w)
{
    int z = 300 - 10*w->getLevel();
    time = std::max(100,z);
}

void Spawn::doSomething()
{
    Goodies::doSomething();
    if (time == 0 && !wasPicked())
    {
        setAlive(false);
        return;
    }
    else time--;
}

//BURIED GOODY//
//not created visible
Buried::Buried(int id, int x, int y, StudentWorld*w, int sound)
:Goodies(id,x,y,w,sound)
{
    setVisible(false);
    setHidden(true);
}


void Buried::doSomething()
{
    //check if within a radius of 4 if not visible
    if(isHidden())
    {
        int x,y;
        getWorld()->getFracked(x,y);
        if (getWorld()->radius(x,y,getX(), getY(), 4))
        {
            setVisible(true);
            setHidden(false);
            return;
        }
    }
    else
        Goodies::doSomething();
}

//WATERPOOL//

WaterPool::WaterPool(int x, int y,StudentWorld* world) :Spawn(IID_WATER_POOL, x,y, world){}

void WaterPool::doSomething()
{
    Spawn::doSomething();
    if (!isAlive() && wasPicked())
    {
        getWorld()->pickedUpWater();
        return;
    }

}


//SONAR KIT//
void Sonar::doSomething()
{
    Spawn::doSomething();
    if (!isAlive() && wasPicked())
    {
        getWorld()->pickedUpSonar();
        return;
    }
}

//BARREL//
Barrel::Barrel(int x, int y, StudentWorld* w):Buried(IID_BARREL,x,y,w,SOUND_FOUND_OIL){}

void Barrel::doSomething()
{
    Buried::doSomething();
    
    //has been picked up
    if (!isAlive())
    {
        getWorld() -> pickedUpBarrel();
    }
}

//NUGGET//
Nugget::Nugget(int x, int y, StudentWorld*w, bool pickup):Buried(IID_GOLD,x,y,w)
{
    time = 100;
    frack_pick = pickup;
    //object is now visible and not hidden
    if (!frack_pick)
    {
        setHidden(false);
        setVisible(true);
    }
};

void Nugget::doSomething()
{
    //object is a buried nugget
    if(frack_pick)
    {
        Buried::doSomething();

        //has been picked up
        if(!isAlive())
        {
            getWorld()->pickedUpNugget();
        }
    }
    //object is a bribe nugget
    else
    {
        //check if within radius of protesters
        if(getWorld()->bribeActors(getX(),getY()))
        {
            setAlive(false);
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        }
        //check if time has elapsed
        if (time<=0)
        {
            setAlive(false);
            return;
        }
        else
            time--;
    }
}

//FRACKMAN//

//getAnnoyed
bool FrackMan::getAnnoyed(int val)
{
    updateHits(val);
    if(getHits()<=0)
    {
        setAlive(false);
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
    return true;
}

//Frackman do something
void FrackMan::doSomething()
{
    int c;
    //check if alive
    if(!isAlive())return;
    StudentWorld* world = getWorld();
    
    //check if user input should move
    if (world->getKey(c)==true)
  {
      switch (c)
      {
        case KEY_PRESS_LEFT:
              if(getDirection() ==left)
              {
                  if(world->touching(getX()-1,getY(),3,this)) break;
              if(getX()>0 )
                moveTo(getX()-1,getY());
              else moveTo(getX(),getY());
              }
              else setDirection(left);
            break;
        case KEY_PRESS_RIGHT:
              if(getDirection()==right)
              {
                  if(world->touching(getX()+1,getY(),3,this))break;
              if(getX()+SPRITE_WIDTH-1<VIEW_WIDTH-1 )
                  moveTo(getX()+1,getY());
                  else moveTo(getX(),getY());
              }
              else setDirection(right);
              break;
        case KEY_PRESS_UP:
              if(getDirection()==up)
              {
                  if(world->touching(getX(),getY()+1,3,this))break;
              if(getY()+SPRITE_HEIGHT-1<VIEW_HEIGHT -1)
                  moveTo(getX(),getY()+1);
                  else moveTo(getX(),getY());
              }
              else setDirection(up);
              break;
        case KEY_PRESS_DOWN:
              if(getDirection()==down)
              {
                  if(world->touching(getX(),getY()-1,3,this))break;
              if(getY()>0 )
                  moveTo(getX(),getY()-1);
                  else moveTo(getX(),getY());
              }
              else setDirection(down);
              break;
        case 'Z':
        case 'z':
              if (sonar>0)
              {
                  sonar--;
                  getWorld()->doSonar();
                  getWorld()->playSound(SOUND_SONAR);
              }
              break;
          case KEY_PRESS_SPACE:
              if(water>0)
              {
                  water--;
                  getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                  //check if water can be spawned at location
                  int new_x = getX();
                  int new_y = getY();
                   bool place = true;
                  for ( int i =0 ; i<SPRITE_WIDTH; i++)
                  {
                      place = getWorld()->canMove(new_x,new_y,getDirection());
                      moveInDir(new_x,new_y,getDirection());
                      if(!place) break;
                  }
                  if (place && !getWorld()->touching(new_x,new_y,3,this))
                  {
                      Squirt* s = new Squirt(new_x,new_y, getWorld(),getDirection());
                      getWorld()->makeSquirt(s);
                  }
              }
              break;
          case KEY_PRESS_TAB:
              if(nugget>0)
              {
                  nugget--;
                  getWorld()->makeBribe();
              }
              break;
          case KEY_PRESS_ESCAPE:
              setAlive(false);
              break;
      default:
      break;
      }
  }
  //delete overlap dirt
   if( world->deleteDirt(getX(),getY()))
   {
       world->playSound(SOUND_DIG);
   }
}

//**PROTESTER BASE CLASS**//
//Protestor constructor
Protester::Protester(int id,StudentWorld* w, int hits,int annoy)
:People(id,60,60, w,hits ,left)
{
    //default state:
    //protestor is not leaving the field
    //protestor given a number of ticks to wait between moves
    //given a random number to move in a certain direction between [8,60]
    leaveTheField =false;
    int x = 3 - getWorld()->getLevel()/4;
    ticksToWaitBetweenMoves=std::max(0, x);
    toMoveInDir = rand()%53 +8;
    movedPerp =200;
    canPickUp(true);
    annoyPoints  = annoy;
}

//protester get annoyed
bool Protester::getAnnoyed(int val)
{
    updateHits(val);
    if(getHits()<=0 && !isLeaving())
    {
        if(val==2 && !isLeaving())
        {

            getWorld()->increaseScore(annoyPoints);
        }
        else
        {
            getWorld()->increaseScore(500);
        }
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        leave();
        ticksToWaitBetweenMoves = 0;
        
    }
    else if(!isLeaving())
    {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED); 
        int s= 100 - getWorld()->getLevel()*10;
        ticksToWaitBetweenMoves += std::max(50,s);
    }
    return true;
}

//Reg Protestor do something
void Protester::doSomething()
{
    if(!isAlive())return;
    //resting between shouts
    
    //resting between moves
    if (ticksToWaitBetweenMoves>1)
    {
        ticksToWaitBetweenMoves--;
        return;
    }
    //waited the number of ticks
    int q = 3 - getWorld()->getLevel()/4;
    ticksToWaitBetweenMoves = std::max(0, q);
    
    
    movedPerp--;
    //check if leaving field
    if (leaveTheField)
    {
        //change this
        if(getX() == 60 && getY()==60)
        {
            setAlive(false);
            return;
        }
        else
        {
            Direction dir = getWorld()->getShortestDir(getX(),getY());
            if(getDirection() != dir)
            {
                setDirection(dir);
            }
            int x = getX();
            int y = getY();
            moveInDir(x,y,getDirection());
            moveTo(x,y);
            return;
        }
    }
    
    //check if within distance of FrackMan and same direction
    int frackx=0;
    int fracky=0;
    getWorld()->getFracked(frackx,fracky);
    //if within radius of four
    if (getWorld()->radius(frackx,fracky,getX(),getY(),4))
    {
        //check direction
        int tempx = getX();
        int tempy= getY();
        moveInDir(tempx,tempy,getDirection());
        //if overlapping when moved in current direction (facing frackman)
        if (getWorld()->radius(frackx,fracky,tempx,tempy,3))
        {
            //annoy frack
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->annoyFrack(2);
            ticksToWaitBetweenMoves*=15;
            return;
        }
    }
    
    //check for line of sight and ability to move there
    if(getX() == frackx || getY() == fracky)
    {
        Direction new_d = getWorld()->clearLineOfSight(getX(),getY());
        if (new_d != none)
        {
            setDirection(new_d);
            int a = getX();
            int b = getY();
            moveInDir(a,b,getDirection());
            moveTo(a,b);
            toMoveInDir = 0;
            return;
        }
    }
    //otherwise not near frackman and can't attack so try to move in direction
    //if has finished walking pick a new direction
    toMoveInDir--;
    if(toMoveInDir<=0)
    {
        //pick new dir until an appropriate spot has been found
        int newdir = 0;
        bool not_changed = true;
        while (not_changed)
        {
            newdir = rand()%4;
            switch(newdir)
            {
                //move up
                case 0:
                    if(getWorld()->canMove(getX(), getY(),up) && !getWorld()->touching(getX(), getY()+1,3,this))
                    {
                        setDirection(up);
                        not_changed = false;
                    }
                    break;
                //move down
                case 1:
                    if(getWorld()->canMove(getX(), getY(),down) && !getWorld()->touching(getX(), getY()-1,3,this))
                    {
                        setDirection(down);
                        not_changed = false;
                    }
                    break;
                //move left
                case 2:
                    if(getWorld()->canMove(getX(), getY(),left) && !getWorld()->touching(getX()-1, getY(),3,this))
                    {
                        setDirection(left);
                        not_changed = false;
                    }
                    break;
                //move right
                case 3:
                    if(getWorld()->canMove(getX(), getY(),right) && !getWorld()->touching(getX()+1, getY(),3,this))
                    {
                        setDirection(right);
                        not_changed = false;
                    }
                    break;
                default:
                    break;
            }
        }
        //pick new toMoveinDir val
        toMoveInDir = rand()%53 +8;
    }
    
    //check for intersection
    if(movedPerp<=0)
    {
        //check for moving up/down
        if(getDirection() == right || getDirection() == left)
        {
            //can move up or down without touching a boulder
            if((getWorld()->canMove(getX(),getY(),up) && !getWorld()->touching(getX(),getY()+1,3,this)) || (getWorld()->canMove(getX(),getY(),down) && !getWorld()->touching(getX(),getY()-1,3,this)))
            {
                //pick viable direction
                int move_in=0;
                bool moveup = getWorld()->canMove(getX(),getY(),up);
                bool movedown = getWorld()->canMove(getX(),getY(),down);
                if (moveup && movedown)
                {
                    move_in = rand()%2;
                }else if (movedown) move_in = 1;
                
                
                //move in picked direction
                if(move_in ==0) setDirection(up);
                else setDirection(down);
                toMoveInDir = rand()%53 +8;
                movedPerp = 200;
            }
        }
        //check for moving left/down
        else if(getDirection() == up || getDirection() == down)
        {
            //can move left or right without touching a boulder
            if((getWorld()->canMove(getX(),getY(),left) && !getWorld()->touching(getX()-1,getY(),3,this)) || (getWorld()->canMove(getX(),getY(),right) && !getWorld()->touching(getX()+1,getY(),3,this)))
            {
                //pick viable direction
                int move_in=0;
                bool moveleft = getWorld()->canMove(getX(),getY(),left);
                bool moveright = getWorld()->canMove(getX(),getY(),right);
                if (moveleft && moveright)
                {
                    move_in = rand()%2;
                }else if (moveright) move_in = 1;
                
                
                //move in picked direction
                if(move_in ==0) setDirection(left);
                else setDirection(right);
                toMoveInDir = rand()%53 +8;
                movedPerp = 200;
            }
        }
            
    }
    
    //if frackman can move in direction
    if(getWorld()->canMove(getX(),getY(),getDirection()))
       {
           int x = getX() ;
           int y = getY();
           moveInDir(x,y,getDirection());
           if(!getWorld()->touching(x,y,3,this))
           moveTo(x,y);
       }
    else
    {
        toMoveInDir=0;
    }

    
}

//REGULAR PROTESTER//


//reg protester recieve bribe
void RegProtest::recieveGold()
{
    getWorld()->increaseScore(25);
    leave();
    
}

//HARDCORE//

HardCore::HardCore(StudentWorld *w):Protester(IID_HARD_CORE_PROTESTER,w,20,250),stare_ticks(0)
{
    signal = 16 + getWorld()->getLevel()*2;
}

void HardCore::doSomething()
{
    if(!isAlive())return;
    
    
    //check if staring at gold nugget
    if (stare_ticks>0)
    {
        stare_ticks--;
        return;
    }
    
    //if the protester is not leaving and is not within radius four of frackman and not in a line of sight and can move (not resting) then look to see if frackman signal found
    int frackx=0;
    int fracky=0;
    getWorld()->getFracked(frackx,fracky);
    if (!isLeaving() && !getWorld()->radius(getX(),getY(),frackx,fracky,4) &&getResting()<=1)
    {
        //if can pickup frackman's signal
        if (getWorld()->radius(getX(),getY(),frackx,fracky,signal))
        {
            //determine new direction
            Direction d = getWorld()->getFrackDir(getX(),getY());
            if(getDirection() != d)
            {
                setDirection(d);
            }
            int x = getX();
            int y = getY();
            moveInDir(x,y,getDirection());
            moveTo(x,y);
            int q = 3 - getWorld()->getLevel()/4;
            int val = std::max(0, q);
            setResting(val);
            return;

        }
    }
    Protester::doSomething();
}

void HardCore::recieveGold()
{
    getWorld()->increaseScore(50);
    int z = 100- getWorld()->getLevel()*10;
    stare_ticks = std::max(50, z);
}
