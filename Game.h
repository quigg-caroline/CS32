//
//  Game.hpp
//  Project 1 - CS32
//
//  Created by Caroline Quigg on 1/6/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#ifndef GAME_H
#define GAME_H

class Arena;

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nRobots);
    ~Game();
    
    // Mutators
    void play();
    
private:
    Arena* m_arena;
};


#endif /* Game_hpp */
