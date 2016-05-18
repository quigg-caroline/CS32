//
//  Robot.h
//  Project 1 - CS32
//
//  Created by Caroline Quigg on 1/6/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.

#ifndef ROBOTS_H
#define ROBOTS_H

class Arena;

class Robot
{
public:
    // Constructor
    Robot(Arena* ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    
    // Mutators
    void move();
    bool getAttacked(int dir);
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_health;
};

#endif 