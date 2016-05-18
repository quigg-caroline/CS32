//
//  History.hpp
//  Project 1 - CS32
//
//  Created by Caroline Quigg on 1/6/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#ifndef HISTORY_H
#define HISTORY_H

#include "globals.h"
class History
{
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;
private:
    int m_nRows;
    int m_nCols;
    char hist [MAXROWS][MAXCOLS];
};

#endif
