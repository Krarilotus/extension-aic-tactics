#ifndef AIC_TACTICS_PATROL_GEOMETRY_HPP
#define AIC_TACTICS_PATROL_GEOMETRY_HPP

namespace AicTactics {

// Integer-only station geometry for the second attack patrol. Directions are
// quantized to the eight compass octants so that every peer computes the same
// tiles without floating point or trigonometry.
struct TilePoint { int x; int y; };

enum PatrolFormation { NativePatrolFormation = 0, WingPatrolFormation = 1, MirrorPatrolFormation = 2 };

// 0=E, 1=SE, 2=S, 3=SW, 4=W, 5=NW, 6=N, 7=NE (map y grows southward).
// Returns -1 for a zero vector.
int patrolOctant(int dx, int dy);
void patrolOctantVector(int octant, int& ux, int& uy);
int patrolDistance(TilePoint a, TilePoint b);

// Wing: two stations `spacing` tiles to either side of the first group's
// leader, perpendicular to its direction toward the target keep. The side
// nearer the second group's leader comes first; ties keep the left side first.
int wingPatrolStations(TilePoint first, TilePoint keep, TilePoint second, int spacing, TilePoint out[2]);

// Mirror: the first group's offset from the target keep reflected across the
// compass axis nearest the target-to-home direction; the point reflection
// through the keep is the alternative. A group within four tiles of the keep
// has no meaningful mirror; one farther than MirrorPatrolRadius is still
// marching, and keeps the native search until it closes in.
const int MirrorPatrolRadius = 48;
int mirrorPatrolStations(TilePoint first, TilePoint keep, TilePoint home, TilePoint out[2]);

} // namespace AicTactics
#endif
