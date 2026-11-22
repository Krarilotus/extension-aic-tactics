#include "aic_tactics/patrol_geometry.hpp"

namespace AicTactics {

namespace {
int magnitude(int value) { return value < 0 ? -value : value; }
const int octantX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int octantY[8] = {0, 1, 1, 1, 0, -1, -1, -1};

TilePoint point(int x, int y)
{
    TilePoint result;
    result.x = x;
    result.y = y;
    return result;
}
} // namespace

int patrolOctant(int dx, int dy)
{
    if (dx == 0 && dy == 0) return -1;
    const int ax = magnitude(dx), ay = magnitude(dy);
    // tan(22.5 degrees) is about 0.414; 2/5 keeps the test integral.
    if (5 * ay <= 2 * ax) return dx > 0 ? 0 : 4;
    if (5 * ax <= 2 * ay) return dy > 0 ? 2 : 6;
    if (dx > 0) return dy > 0 ? 1 : 7;
    return dy > 0 ? 3 : 5;
}

void patrolOctantVector(int octant, int& ux, int& uy)
{
    if (octant < 0 || octant > 7) { ux = 0; uy = 0; return; }
    ux = octantX[octant];
    uy = octantY[octant];
}

int patrolDistance(TilePoint a, TilePoint b)
{
    const int dx = magnitude(a.x - b.x), dy = magnitude(a.y - b.y);
    return dx > dy ? dx : dy;
}

int wingPatrolStations(TilePoint first, TilePoint keep, TilePoint second, int spacing, TilePoint out[2])
{
    if (spacing <= 0) return 0;
    const int direction = patrolOctant(keep.x - first.x, keep.y - first.y);
    if (direction < 0) return 0;
    int lx, ly, rx, ry;
    patrolOctantVector((direction + 6) % 8, lx, ly);
    patrolOctantVector((direction + 2) % 8, rx, ry);
    const TilePoint left = point(first.x + lx * spacing, first.y + ly * spacing);
    const TilePoint right = point(first.x + rx * spacing, first.y + ry * spacing);
    const bool rightFirst = patrolDistance(right, second) < patrolDistance(left, second);
    out[0] = rightFirst ? right : left;
    out[1] = rightFirst ? left : right;
    return 2;
}

int mirrorPatrolStations(TilePoint first, TilePoint keep, TilePoint home, TilePoint out[2])
{
    if (patrolDistance(first, keep) < 4) return 0;
    const int axis = patrolOctant(home.x - keep.x, home.y - keep.y);
    if (axis < 0) return 0;
    const int vx = first.x - keep.x, vy = first.y - keep.y;
    int mx, my;
    switch (axis % 4) {
    case 0: mx = vx; my = -vy; break;   // horizontal axis
    case 2: mx = -vx; my = vy; break;   // vertical axis
    case 1: mx = vy; my = vx; break;    // diagonal y = x
    default: mx = -vy; my = -vx; break; // diagonal y = -x
    }
    if (patrolDistance(point(mx, my), point(vx, vy)) < 4) {
        // The first group approaches along the axis itself: use its flanks.
        out[0] = point(keep.x - vy, keep.y + vx);
        out[1] = point(keep.x + vy, keep.y - vx);
        return 2;
    }
    out[0] = point(keep.x + mx, keep.y + my);
    out[1] = point(keep.x - vx, keep.y - vy);
    return 2;
}

} // namespace AicTactics
