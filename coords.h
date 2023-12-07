#ifndef COORDS_H
#define COORDS_H

/**
 * Class that represents a coordinate on the map terrain. 
 * [x][y] -> coords(x,y)
*/
class coords
{
private:
    int row;
    int col;
public:
    coords()
    {
        row = -1;
        col = -1;
    }

    coords(int x, int y)
    {
        row = x;
        col = y;
    }

    int getRow() const {return row;}
    int getCol() const {return col;}
    void setRow(int v) {row = v;}
    void setCol(int v) {col = v;}
};

#endif