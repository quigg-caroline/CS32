//
//  History.cpp
//  Project 1 - CS32
//
//  Created by Caroline Quigg on 1/6/16.
//  Copyright © 2016 Caroline Quigg. All rights reserved.
//

#include "History.h"
#include "globals.h"
#include <iostream>
using namespace std;
History::History(int nRows, int nCols)
{
    m_nRows=nRows;
    m_nCols=nCols;
    for (int i = 0 ; i<m_nRows; i++)
    {
        for (int j=0; j<m_nCols; j++)
        {
            hist[i][j]='.';
        }
    }
}
bool History::record(int r, int c)
{
    if (r<1 || r>m_nRows || c<1 || c>m_nCols)
    {
        return false;
    }
    char& histChar = hist[r-1][c-1];
    switch(histChar)
    {
        case '.':
        {
            histChar = 'A';
            break;
        }
        case 'Z':
        {
            break;
        }
        default:
            histChar++;
    }
    return true;
}
void History::display() const
{
    clearScreen();
    for (int i =0 ; i<m_nRows;i++)
    {
        for (int j=0 ; j<m_nCols; j++)
        {
            cout << hist[i][j];
        }
        cout <<endl;
    }
    cout<<endl;
}
