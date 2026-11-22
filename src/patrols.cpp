#include "aic_tactics/patrols.hpp"
#include "aic_tactics/patrol_geometry.hpp"
#include "aic_tactics/native_bindings.hpp"

namespace AicTactics {
namespace SHC141 {

int patrolOrders[17];
int patrolFormation[17];
int patrolSpacing[17];

namespace {
const unsigned int PlayerStride = 0x39F4;
const int FirstPatrolSlot = 15;
const int SecondPatrolSlot = 16;
const int TribeCapacity = 1250;
template<class T> T& at(unsigned int address) { return *reinterpret_cast<T*>(address); }
typedef void (__thiscall *PlayerAction)(void*, int);
typedef void (__thiscall *TribeAction)(void*, int);
typedef int (__thiscall *TribeQuery)(void*, int, int);
typedef int (__thiscall *TribeMove)(void*, int, int, int, int, int, int);

unsigned int playerAddress(int player) { return nativeBindings.players + player * PlayerStride; }
unsigned int tribeAddress(int tribe) { return nativeBindings.tribes + tribe * nativeBindings.tribeStride; }

// The native dispatcher's own checks for slot 15, applied to any patrol slot:
// a live slot entry whose UID still matches and whose tribe has a leader.
// Owner and lifecycle are checked as well so a recycled record is never used.
int patrolTribe(int player, int slot)
{
    const int tribe = at<short>(playerAddress(player) + 0x310C + slot * 2);
    if (tribe <= 0 || tribe >= TribeCapacity) return 0;
    const unsigned int address = tribeAddress(tribe);
    if (at<unsigned int>(address + 0x34) != at<unsigned int>(playerAddress(player) + 0x329C + slot * 4)
        || at<int>(address + 0x2C) != player || at<short>(address + 0x40) != 2
        || at<short>(address + 0x5A) == 0) return 0;
    return tribe;
}

// A leader is used for geometry only if its unit record links back to the tribe.
bool leaderPosition(int tribe, TilePoint& position)
{
    const unsigned int address = tribeAddress(tribe);
    const int leader = at<short>(address + 0x5A);
    if (leader <= 0 || leader >= static_cast<int>(nativeBindings.unitCapacity)) return false;
    const unsigned int unit = nativeBindings.unitRecords + leader * 0x490;
    if (at<short>(unit + 0x2D8) != tribe || at<unsigned int>(unit + 0x2E4) != at<unsigned int>(address + 0x34)
        || at<short>(unit + 0x2A0) != 0) return false;
    position.x = at<short>(unit + 0xC4);
    position.y = at<short>(unit + 0xC6);
    return position.x > 0 && position.x < 399 && position.y > 0 && position.y < 399;
}

bool keepPosition(int player, TilePoint& position)
{
    if (player < 1 || player > 8 || at<int>(playerAddress(player) + 0x94) == 0) return false;
    position.x = at<int>(playerAddress(player) + 0x98);
    position.y = at<int>(playerAddress(player) + 0x9C);
    return position.x > 0 && position.x < 399 && position.y > 0 && position.y < 399;
}

void nativeDestination(int tribe)
{
    reinterpret_cast<TribeAction>(nativeBindings.attackPatrolDestination)(
        reinterpret_cast<void*>(nativeBindings.troopValueState), tribe);
}

// Formation station for the second patrol. Returns true when a station was
// handled (ordered, or already reached); false asks for the native search.
bool orderStation(void* aic, int player, int character, int tribe)
{
    const int formation = patrolFormation[character];
    if (formation != WingPatrolFormation && formation != MirrorPatrolFormation) return false;
    const int first = patrolTribe(player, FirstPatrolSlot);
    TilePoint firstLeader, secondLeader, keep, home;
    const int target = at<int>(playerAddress(player) + 0x2BD8);
    if (!first || target == player || !leaderPosition(first, firstLeader)
        || !leaderPosition(tribe, secondLeader) || !keepPosition(target, keep)) return false;
    TilePoint stations[2];
    int count = 0;
    if (formation == WingPatrolFormation) {
        count = wingPatrolStations(firstLeader, keep, secondLeader, patrolSpacing[character], stations);
    } else {
        if (!keepPosition(player, home)) return false;
        count = mirrorPatrolStations(firstLeader, keep, home, stations);
    }
    // Do not restart a march that has already reached its station.
    for (int index = 0; index < count; ++index)
        if (patrolDistance(stations[index], secondLeader) <= 3) return true;
    // At most two reachability queries and one order per AI update.
    for (int index = 0; index < count; ++index) {
        const TilePoint station = stations[index];
        if (station.x <= 0 || station.x >= 399 || station.y <= 0 || station.y >= 399) continue;
        const int tile = at<int>(nativeBindings.mapRows + station.y * 12) + station.x;
        if (tile <= 0 || tile >= 160000) continue;
        if (!reinterpret_cast<TribeQuery>(nativeBindings.tribePath)(aic, tribe, tile)) continue;
        reinterpret_cast<TribeMove>(nativeBindings.siegeGroupMove)(
            reinterpret_cast<void*>(nativeBindings.tribes), tribe, station.x, station.y, 0, 0, 0);
        return true;
    }
    return false;
}
} // namespace

void __fastcall dispatchAttackPatrols(void* aic, void*, int player)
{
    // Native slot 15 keeps its original order, timing and searches.
    reinterpret_cast<PlayerAction>(nativeBindings.attackPatrolDispatch)(aic, player);
    if (player < 1 || player > 8) return;
    const int character = at<int>(playerAddress(player) + 0x2300) - 1;
    if (character < 1 || character > 16 || patrolOrders[character] != 1) return;
    const int tribe = patrolTribe(player, SecondPatrolSlot);
    if (!tribe) return;
    // The native dispatcher marks its patrol tribe before ordering it.
    at<short>(tribeAddress(tribe) + nativeBindings.tribePatrolFlag) = 1;
    if (!orderStation(aic, player, character, tribe)) nativeDestination(tribe);
}

} // namespace SHC141
} // namespace AicTactics
