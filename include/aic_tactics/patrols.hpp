#ifndef AIC_TACTICS_PATROLS_HPP
#define AIC_TACTICS_PATROLS_HPP

namespace AicTactics {
namespace SHC141 {

// Attack patrol slot duties, indexed by installed AIC character 1..16.
// patrolOrders: 0 native (slot 15 only), 1 both native patrol slots.
// patrolFormation: 0 native leader search, 1 wing, 2 mirror (PatrolFormation).
// patrolSpacing: wing distance in tiles, 0 unless the formation is wing.
extern int patrolOrders[17];
extern int patrolFormation[17];
extern int patrolSpacing[17];

// Replaces the three native calls to the attack patrol dispatcher inside the
// AI state update. Runs the unchanged native dispatcher first, then the
// second patrol slot duty for opted-in characters. No state is kept here:
// every decision uses saved native slot, tribe and unit records.
void __fastcall dispatchAttackPatrols(void* aic, void*, int player);

} // namespace SHC141
} // namespace AicTactics
#endif
