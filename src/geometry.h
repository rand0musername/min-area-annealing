#pragma once
#include <vector>

namespace geometry {
    struct Point {double x, y;};

    // Returns euclidean distance
    double Dist(Point a, Point b);
    
    // Returns the turn given by (a, b, c)
    // 1=left, -1=right, 0=collinear
    int Turn(Point a, Point b, Point c);
    
    // Checks if a and b are on the same side of (c, d)
    // 1=same, -1=diff, 0=a/b on cd
    int SameSide(Point a, Point b, Point c, Point d);

    // Checks if line segments ab and cd intersect
    bool DoIntersect(Point a, Point b, Point c, Point d);

    // Checks if a polygon is self intersecting
    bool IsSelfIntersectingPoly(const std::vector<Point>& poly);

    // Calculates the area of a polygon by summing intersections
    double PolyArea(const std::vector<Point>& poly);
}