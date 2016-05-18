//
//  Player.hpp
//  Project 1 - CS32
//
//  Created by Caroline Quigg on 1/6/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#ifndef PLAYER_H
#define PLAYER_H

class Arena;

class Player
{
public:
    // Constructor
    Player(Arena *ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;
    
    // Mutators
    void   stand();
    void   moveOrAttack(int dir);
    void   setDead();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_age;
    bool   m_dead;
};



#endif 
