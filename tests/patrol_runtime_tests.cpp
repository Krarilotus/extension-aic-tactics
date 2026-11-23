// Executes dispatchAttackPatrols against mock game data and mock native
// routines on both tribe layouts (Crusader 0x334, Extreme 0x688).
#include "aic_tactics/runtime.hpp"
#include "aic_tactics/patrols.hpp"
#include <cassert>
#include <cstring>

namespace AicTactics { namespace SHC141 {
NativeBindings nativeBindings;
int configurationLocked;
}}

using namespace AicTactics::SHC141;

namespace {
unsigned char players[9 * 0x39F4];
unsigned char units[8 * 0x490];
unsigned char tribes[8 * 0x688];
unsigned char playable[400 * 400];
int rows[400 * 3];
void* const AIC = reinterpret_cast<void*>(0x1234);
void* const TROOP_VALUES = reinterpret_cast<void*>(0x5678);
int dispatchCalls, destinationCalls, destinationTribe, pathCalls, moveCalls, moveX, moveY;
int failingMoves, blockedTile;

template<class T> void put(unsigned char* base, unsigned int offset, T value)
{
    *reinterpret_cast<T*>(base + offset) = value;
}
template<class T> T get(unsigned char* base, unsigned int offset)
{
    return *reinterpret_cast<T*>(base + offset);
}

// Native thiscall routines, called through __thiscall pointers: __fastcall with
// an ignored edx has the same register use and callee stack cleanup.
void __fastcall dispatch(void* aic, void*, int player)
{
    assert(aic == AIC && player >= 1 && player <= 8);
    ++dispatchCalls;
}
void __fastcall destination(void* state, void*, int tribe)
{
    assert(state == TROOP_VALUES);
    ++destinationCalls;
    destinationTribe = tribe;
}
int __fastcall path(void* aic, void*, int tribe, int tile)
{
    assert(aic == AIC && tribe == 6 && tile > 0 && tile < 80400);
    ++pathCalls;
    return tile != blockedTile;
}
int __fastcall move(void* owner, void*, int tribe, int x, int y, int rally, int store, int speed)
{
    assert(owner == static_cast<void*>(tribes) && tribe == 6 && !rally && !store && !speed);
    ++moveCalls;
    if (failingMoves > 0) { --failingMoves; return 0; }
    moveX = x;
    moveY = y;
    return 1;
}

unsigned char* player(int id) { return players + id * 0x39F4; }
unsigned char* unit(int id) { return units + id * 0x490; }
unsigned char* tribe(int id) { return tribes + id * nativeBindings.tribeStride; }

void group(int slot, int id, unsigned int uid, int leader, int x, int y)
{
    put<short>(player(1), 0x310C + slot * 2, static_cast<short>(id));
    put<unsigned int>(player(1), 0x329C + slot * 4, uid);
    put<int>(tribe(id), 0x2C, 1);
    put<unsigned int>(tribe(id), 0x34, uid);
    put<short>(tribe(id), 0x40, 2);
    put<short>(tribe(id), 0x5A, static_cast<short>(leader));
    put<short>(unit(leader), 0x2D8, static_cast<short>(id));
    put<unsigned int>(unit(leader), 0x2E4, uid);
    put<short>(unit(leader), 0x96, 1);
    put<short>(unit(leader), 0x8C, 2);
    put<short>(unit(leader), 0x2A0, 0);
    put<int>(unit(leader), 0x3C8, 100);
    put<short>(unit(leader), 0xC4, static_cast<short>(x));
    put<short>(unit(leader), 0xC6, static_cast<short>(y));
}

void setup(unsigned int stride, unsigned int stance, int orders, int formation, int spacing)
{
    std::memset(players, 0, sizeof(players));
    std::memset(units, 0, sizeof(units));
    std::memset(tribes, 0, sizeof(tribes));
    std::memset(playable, 1, sizeof(playable));
    std::memset(patrolOrders, 0, sizeof(patrolOrders));
    std::memset(patrolFormation, 0, sizeof(patrolFormation));
    std::memset(patrolSpacing, 0, sizeof(patrolSpacing));
    patrolOrdersFallback = 0;
    patrolFormationFallback = 0;
    patrolSpacingFallback = 12;
    configurationLocked = 0;
    dispatchCalls = destinationCalls = destinationTribe = pathCalls = moveCalls = moveX = moveY = 0;
    failingMoves = 0;
    blockedTile = -1;
    nativeBindings.players = reinterpret_cast<unsigned int>(players);
    nativeBindings.unitRecords = reinterpret_cast<unsigned int>(units);
    nativeBindings.unitCapacity = 8;
    nativeBindings.tribes = reinterpret_cast<unsigned int>(tribes);
    nativeBindings.tribeStride = stride;
    nativeBindings.tribeStance = stance;
    nativeBindings.mapRows = reinterpret_cast<unsigned int>(rows);
    nativeBindings.playableTiles = reinterpret_cast<unsigned int>(playable);
    nativeBindings.attackPatrolDispatch = reinterpret_cast<unsigned int>(&dispatch);
    nativeBindings.attackPatrolDestination = reinterpret_cast<unsigned int>(&destination);
    nativeBindings.troopValueState = reinterpret_cast<unsigned int>(TROOP_VALUES);
    nativeBindings.tribePath = reinterpret_cast<unsigned int>(&path);
    nativeBindings.siegeGroupMove = reinterpret_cast<unsigned int>(&move);
    for (int row = 0; row < 400; ++row) rows[row * 3] = row * 200;
    // Player 1 is AI character 2 attacking player 2; keeps at (100,300) and (100,100).
    put<int>(player(1), 0x2300, 3);
    put<int>(player(1), 0x2BD8, 2);
    put<int>(player(1), 0x94, 1);
    put<int>(player(1), 0x98, 100);
    put<int>(player(1), 0x9C, 300);
    put<int>(player(2), 0x94, 1);
    put<int>(player(2), 0x98, 100);
    put<int>(player(2), 0x9C, 100);
    patrolOrders[2] = orders;
    patrolFormation[2] = formation;
    patrolSpacing[2] = spacing;
    group(15, 5, 505, 2, 100, 160);
    group(16, 6, 606, 3, 110, 170);
}

void run(unsigned int stride, unsigned int stance)
{
    // Absent fields: only the native dispatcher runs.
    setup(stride, stance, 0, 0, 0);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(dispatchCalls == 1 && destinationCalls == 0 && pathCalls == 0 && moveCalls == 0);
    assert(get<short>(tribe(6), stance) == 0 && configurationLocked == 0);

    // AllGroups with the native formation: the native destination routine for slot 16.
    setup(stride, stance, 2, 1, 0);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(dispatchCalls == 1 && destinationCalls == 1 && destinationTribe == 6);
    assert(get<short>(tribe(6), stance) == 1 && get<short>(tribe(5), stance) == 0);
    assert(configurationLocked == 1 && pathCalls == 0 && moveCalls == 0);

    // Wing 14: the first group heads north; the east station is nearer the second leader.
    setup(stride, stance, 2, 2, 14);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 1 && moveCalls == 1 && moveX == 114 && moveY == 160 && destinationCalls == 0);

    // A rejected move falls back to the other station, then to the native search.
    setup(stride, stance, 2, 2, 14);
    failingMoves = 1;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 2 && moveCalls == 2 && moveX == 86 && moveY == 160 && destinationCalls == 0);
    setup(stride, stance, 2, 2, 14);
    failingMoves = 2;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 2 && moveCalls == 2 && destinationCalls == 1);

    // An unreachable station is skipped.
    setup(stride, stance, 2, 2, 14);
    blockedTile = 160 * 200 + 114;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 2 && moveCalls == 1 && moveX == 86);

    // A station outside the playable map is not even queried.
    setup(stride, stance, 2, 2, 14);
    playable[160 * 400 + 114] = 0;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 1 && moveCalls == 1 && moveX == 86);

    // Already at the station: no new order, no native search.
    setup(stride, stance, 2, 2, 14);
    put<short>(unit(3), 0xC4, 113);
    put<short>(unit(3), 0xC6, 161);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 0 && moveCalls == 0 && destinationCalls == 0);

    // A leader that no longer links back, a dying leader or a recycled UID: no order at all.
    setup(stride, stance, 2, 2, 14);
    put<unsigned int>(unit(3), 0x2E4, 1);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(dispatchCalls == 1 && destinationCalls == 0 && moveCalls == 0 && get<short>(tribe(6), stance) == 0);
    setup(stride, stance, 2, 1, 0);
    put<short>(unit(3), 0x2A0, 1);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(destinationCalls == 0 && get<short>(tribe(6), stance) == 0);
    setup(stride, stance, 2, 1, 0);
    put<unsigned int>(player(1), 0x329C + 16 * 4, 999);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(destinationCalls == 0);

    // Without a target keep the native search is used.
    setup(stride, stance, 2, 2, 14);
    put<int>(player(2), 0x94, 0);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 0 && destinationCalls == 1);

    // Mirror on the home axis uses the keep's flanks; far away it waits for the native search.
    setup(stride, stance, 2, 3, 0);
    put<short>(unit(2), 0xC6, 140);
    put<short>(unit(3), 0xC4, 120);
    put<short>(unit(3), 0xC6, 150);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 1 && moveCalls == 1 && moveX == 60 && moveY == 100);
    setup(stride, stance, 2, 3, 0);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 0 && destinationCalls == 1);

    // An absent field uses the module setting; an explicit Native switches it off.
    setup(stride, stance, 0, 0, 0);
    patrolOrdersFallback = 1;
    patrolFormationFallback = 1;
    patrolSpacingFallback = 14;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(pathCalls == 1 && moveCalls == 1 && moveX == 114 && moveY == 160 && destinationCalls == 0);
    setup(stride, stance, 1, 0, 0);
    patrolOrdersFallback = 1;
    dispatchAttackPatrols(AIC, 0, 1);
    assert(dispatchCalls == 1 && destinationCalls == 0 && moveCalls == 0 && configurationLocked == 0);

    // Human players are never ordered.
    setup(stride, stance, 2, 2, 14);
    put<int>(player(1), 0x2300, 0);
    dispatchAttackPatrols(AIC, 0, 1);
    assert(dispatchCalls == 1 && destinationCalls == 0 && moveCalls == 0 && configurationLocked == 0);
}
}

int main()
{
    run(0x334, 0x2E0);
    run(0x688, 0x634);
    return 0;
}
