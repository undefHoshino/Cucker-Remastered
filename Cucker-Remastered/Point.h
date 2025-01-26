#pragma once

template <class T>
struct Point {
    T x, y;
    Point(T _x, T _y) : x(_x), y(_y) {}

    void legal(T minX, T minY, T maxX, T maxY) {
        if (x < minX)
            x = minX;
        else if (x > maxX)
            x = maxX;

        if (y < minY)
            y = minY;
        else if (y > maxY)
            y = maxY;
    }
};
using PointU = Point<unsigned short>;
using PointI = Point<int>;