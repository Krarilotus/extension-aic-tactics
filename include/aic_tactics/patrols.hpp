#ifndef AIC_TACTICS_PATROLS_HPP
#define AIC_TACTICS_PATROLS_HPP

#include "aic_tactics/groups.hpp"

namespace AicTactics {
namespace SHC141 {

// Attack patrol settings, indexed by installed AIC character 1..16. Each
// value is 0 (use the module setting) or the character's choice + 1:
// patrolOrders 1 native (slot 15 only), 2 every patrol group;
// patrolFormation 1 native leader search, 2 wing, 3 mirror, 4 routes;
// patrolSpacing the wing distance in tiles
// (stored as the value itself); patrolExtraGroups the detached groups split off
// the native patrol slots; patrolMinSize the smallest group a split may leave;
// patrolRadius the ring's walking distance from the target castle;
// siegeScreen 1 native rally, 2 engine approach; siegeScreenForward tiles.
extern int patrolOrders[17];
extern int patrolFormation[17];
extern int patrolSpacing[17];
extern int patrolExtraGroups[17];
extern int patrolMinSize[17];
extern int patrolRadius[17];
extern int siegeScreen[17];
extern int siegeScreenForward[17];
// Module settings: orders 0/1, formation 0..3 (PatrolFormation), spacing,
// extra groups 0..6, minimum size 1..50, radius 4..100, screen 0/1, forward 0..10.
extern int patrolOrdersFallback;
extern int patrolFormationFallback;
extern int patrolSpacingFallback;
extern int patrolExtraGroupsFallback;
extern int patrolMinSizeFallback;
extern int patrolRadiusFallback;
extern int siegeScreenFallback;
extern int siegeScreenForwardFallback;

// Ring route of one patrol group, saved so a reload continues the same route.
// Entry 0 of a player is native slot 16, entry 1 + i the detached group record i.
struct PatrolRoute {
    unsigned int uid;   // tribe UID the route belongs to; 0 = none
    int target;         // player whose keep the route circles
    int point;          // ring direction, or front sweep position
    int step;           // +1 or -1
    int x;              // issued route point; 0 = none yet
    int y;
    unsigned int tick;  // last progress, order or native hand-over
    int distance;       // leader's best distance to the route point
    int stalls;         // 0 on route, 1 alternate taken, 2 native search until tick + PatrolStepTicks
};
const int PatrolRoutesPerPlayer = 1 + DetachedGroupsPerPlayer;
extern PatrolRoute patrolRoutes[9][PatrolRoutesPerPlayer];

// Replaces the three native calls to the attack patrol dispatcher inside the
// AI state update. Runs the unchanged native dispatcher for slot 15 first,
// then splits and orders the other patrol groups when that is on for the
// character. Decisions use saved native records, the saved detached group
// records and the saved route records only.
void __fastcall dispatchAttackPatrols(void* aic, void*, int player);
// Hands detached patrol groups back to the native slots once the attack has
// left states 3-5. Called at the start of every AI attack update.
void __cdecl finishAttackPatrols(void* aic, int player);
// Hands every detached patrol group back before the native retreat orders the
// slots home; members left over are sent home as a group.
void __cdecl retreatAttackPatrols(void* aic, int player);
// Replaces the move order inside the native siege-defence rally routine
// (attack states 3-4). Opted-in characters station their siege-defence groups
// on the engines' approach; the native wall assault in state 5 is unchanged.
int __fastcall screenSiegeDefenceMove(void* tribes, void*, int tribe, int x, int y,
    int rally, int store, int speed);

} // namespace SHC141
} // namespace AicTactics
#endif
