#ifndef AIC_TACTICS_PATROLS_HPP
#define AIC_TACTICS_PATROLS_HPP

#include "aic_tactics/patrol_geometry.hpp"

namespace AicTactics {
namespace SHC141 {

// Attack patrol settings, indexed by installed AIC character 1..16. Each
// value is 0 (use the module setting) or the character's choice + 1:
// patrolOrders 1 native (slot 15 only), 2 both native patrol slots;
// patrolFormation 1 native leader search, 2 wing, 3 mirror;
// patrolSpacing the wing distance in tiles.
extern int patrolOrders[17];
extern int patrolFormation[17];
extern int patrolSpacing[17];
// Module settings: orders 0/1, formation 0/1/2 (PatrolFormation), spacing.
extern int patrolOrdersFallback;
extern int patrolFormationFallback;
extern int patrolSpacingFallback;

// Replaces the three native calls to the attack patrol dispatcher inside the
// AI state update. Runs the unchanged native dispatcher first, then the
// second patrol slot duty when it is on for that character. No state is
// kept: every decision uses saved native slot, tribe and unit records.
void __fastcall dispatchAttackPatrols(void* aic, void*, int player);

// The checks above, shared with other duties: a slot occupant validated like
// the native dispatcher's (UID, owner, lifecycle, a living leader linked back
// to it), a tribe's living linked leader, and the native move instruction's
// playable-map test for a tile (0 when it would refuse it).
int validSlotTribe(int player, int slot, TilePoint& leader);
bool tribeLeaderTile(int player, int tribe, TilePoint& position);
int movableTile(TilePoint point);

} // namespace SHC141
} // namespace AicTactics
#endif
