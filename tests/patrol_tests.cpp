#include <cassert>
#include "aic_tactics/patrol_geometry.hpp"

using namespace AicTactics;

static TilePoint at(int x, int y)
{
    TilePoint point;
    point.x = x;
    point.y = y;
    return point;
}

static bool same(TilePoint a, int x, int y) { return a.x == x && a.y == y; }

static void octants()
{
    assert(patrolOctant(0, 0) == -1);
    assert(patrolOctant(10, 0) == 0 && patrolOctant(10, 4) == 0 && patrolOctant(10, 5) == 1);
    assert(patrolOctant(0, 10) == 2 && patrolOctant(-10, 10) == 3 && patrolOctant(-10, 0) == 4);
    assert(patrolOctant(-7, -7) == 5 && patrolOctant(0, -10) == 6 && patrolOctant(7, -7) == 7);
    assert(patrolOctant(4, 10) == 2 && patrolOctant(5, 10) == 1);
    int ux = 9, uy = 9;
    patrolOctantVector(8, ux, uy);
    assert(ux == 0 && uy == 0);
    patrolOctantVector(3, ux, uy);
    assert(ux == -1 && uy == 1);
    assert(patrolDistance(at(1, 1), at(4, -2)) == 3);
}

static void wing()
{
    TilePoint out[2];
    // Target east of the first group: stations to the north and south.
    assert(wingPatrolStations(at(100, 100), at(150, 100), at(98, 110), 12, out) == 2);
    assert(same(out[0], 100, 112) && same(out[1], 100, 88));
    // Equal distance keeps the left (north) station first.
    assert(wingPatrolStations(at(100, 100), at(150, 100), at(100, 100), 12, out) == 2);
    assert(same(out[0], 100, 88) && same(out[1], 100, 112));
    // Diagonal approach uses diagonal flanks.
    assert(wingPatrolStations(at(100, 100), at(150, 150), at(90, 90), 12, out) == 2);
    assert(same(out[0], 112, 88) && same(out[1], 88, 112));
    assert(wingPatrolStations(at(100, 100), at(150, 100), at(1, 1), 0, out) == 0);
    assert(wingPatrolStations(at(100, 100), at(100, 100), at(1, 1), 12, out) == 0);
}

static void mirror()
{
    TilePoint out[2];
    // Home south of the keep: reflect across the vertical axis.
    assert(mirrorPatrolStations(at(170, 190), at(200, 200), at(200, 300), out) == 2);
    assert(same(out[0], 230, 190) && same(out[1], 230, 210));
    // Home east: horizontal axis.
    assert(mirrorPatrolStations(at(170, 190), at(200, 200), at(300, 200), out) == 2);
    assert(same(out[0], 170, 210));
    // Home south-east: diagonal y = x.
    assert(mirrorPatrolStations(at(170, 190), at(200, 200), at(300, 300), out) == 2);
    assert(same(out[0], 190, 170));
    // Home south-west: diagonal y = -x.
    assert(mirrorPatrolStations(at(170, 190), at(200, 200), at(100, 300), out) == 2);
    assert(same(out[0], 210, 230));
    // Approaching along the axis: use both flanks of the keep.
    assert(mirrorPatrolStations(at(200, 170), at(200, 200), at(200, 300), out) == 2);
    assert(same(out[0], 230, 200) && same(out[1], 170, 200));
    // Too close to the keep, or no home direction.
    assert(mirrorPatrolStations(at(202, 201), at(200, 200), at(200, 300), out) == 0);
    assert(mirrorPatrolStations(at(170, 190), at(200, 200), at(200, 200), out) == 0);
    // Still marching from far away: no mirror yet.
    assert(mirrorPatrolStations(at(151, 200), at(200, 200), at(200, 300), out) == 0);
    assert(mirrorPatrolStations(at(152, 200), at(200, 200), at(200, 300), out) == 2);
}

int main()
{
    octants();
    wing();
    mirror();
    return 0;
}
