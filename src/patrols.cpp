#include "aic_tactics/patrols.hpp"
#include "aic_tactics/patrol_geometry.hpp"
#include "aic_tactics/runtime.hpp"

namespace AicTactics {
namespace SHC141 {

int patrolOrders[17];
int patrolFormation[17];
int patrolSpacing[17];
int patrolOrdersFallback = 1;
int patrolFormationFallback = WingPatrolFormation;
int patrolSpacingFallback = 12;

namespace {
const unsigned int PlayerStride = 0x39F4;
const int FirstPatrolSlot = 15;
const int SecondPatrolSlot = 16;
const int TribeCapacity = 1250;
// Packed isometric tile count of a 400x400 map (OpenSHC Map/TileMapState.hpp).
const int TileCapacity = 80400;
template<class T> T& at(unsigned int address) { return *reinterpret_cast<T*>(address); }
typedef void (__thiscall *PlayerAction)(void*, int);
typedef void (__thiscall *TribeAction)(void*, int);
typedef int (__thiscall *TribeQuery)(void*, int, int);
typedef int (__thiscall *TribeMove)(void*, int, int, int, int, int, int);

int setting(const int values[17], int character, int fallback)
{
    return values[character] ? values[character] - 1 : fallback;
}

unsigned int playerAddress(int player) { return nativeBindings.players + player * PlayerStride; }
unsigned int tribeAddress(int tribe) { return nativeBindings.tribes + tribe * nativeBindings.tribeStride; }

// A living leader whose unit record links back to its tribe and player.
bool leaderPosition(int player, int tribe, TilePoint& position)
{
    const unsigned int address = tribeAddress(tribe);
    const int leader = at<short>(address + 0x5A);
    if (leader <= 0 || leader >= static_cast<int>(nativeBindings.unitCapacity)) return false;
    const unsigned int unit = nativeBindings.unitRecords + leader * 0x490;
    if (at<short>(unit + 0x2D8) != tribe || at<unsigned int>(unit + 0x2E4) != at<unsigned int>(address + 0x34)
        || at<short>(unit + 0x96) != player || at<short>(unit + 0x8C) != 2
        || at<short>(unit + 0x2A0) != 0 || at<int>(unit + 0x3C8) <= 0) return false;
    position.x = at<short>(unit + 0xC4);
    position.y = at<short>(unit + 0xC6);
    return position.x > 0 && position.x < 399 && position.y > 0 && position.y < 399;
}

// The native dispatcher's own checks for slot 15 (UID still matches, leader
// present), applied to any patrol slot, plus owner, lifecycle and a valid
// leader so a recycled record or a dying leader is never used.
int patrolTribe(int player, int slot, TilePoint& leader)
{
    const int tribe = at<short>(playerAddress(player) + 0x310C + slot * 2);
    if (tribe <= 0 || tribe >= TribeCapacity) return 0;
    const unsigned int address = tribeAddress(tribe);
    if (at<unsigned int>(address + 0x34) != at<unsigned int>(playerAddress(player) + 0x329C + slot * 4)
        || at<int>(address + 0x2C) != player || at<short>(address + 0x40) != 2
        || !leaderPosition(player, tribe, leader)) return 0;
    return tribe;
}

// The keep's entrance tile (BuildingEntryInfo x/y), as the AI's home checks use.
bool keepPosition(int player, TilePoint& position)
{
    if (player < 1 || player > 8 || at<int>(playerAddress(player) + 0x94) == 0) return false;
    position.x = at<int>(playerAddress(player) + 0x98);
    position.y = at<int>(playerAddress(player) + 0x9C);
    return position.x > 0 && position.x < 399 && position.y > 0 && position.y < 399;
}

// The same playable-map test the native move instruction applies first.
int stationTile(TilePoint station)
{
    if (station.x <= 0 || station.x >= 399 || station.y <= 0 || station.y >= 399) return 0;
    if (!at<unsigned char>(nativeBindings.playableTiles + station.y * 400 + station.x)) return 0;
    const int tile = at<int>(nativeBindings.mapRows + station.y * 12) + station.x;
    return tile > 0 && tile < TileCapacity ? tile : 0;
}

void nativeDestination(int tribe)
{
    reinterpret_cast<TribeAction>(nativeBindings.attackPatrolDestination)(
        reinterpret_cast<void*>(nativeBindings.troopValueState), tribe);
}

// Formation station for the second patrol. Returns true when a station was
// handled (ordered, or already reached); false asks for the native search.
bool orderStation(void* aic, int player, int character, int tribe, TilePoint secondLeader)
{
    const int formation = setting(patrolFormation, character, patrolFormationFallback);
    if (formation != WingPatrolFormation && formation != MirrorPatrolFormation) return false;
    TilePoint firstLeader, keep, home;
    const int target = at<int>(playerAddress(player) + 0x2BD8);
    if (target == player || !patrolTribe(player, FirstPatrolSlot, firstLeader)
        || !keepPosition(target, keep)) return false;
    TilePoint stations[2];
    int count = 0;
    if (formation == WingPatrolFormation) {
        const int spacing = patrolSpacing[character] ? patrolSpacing[character] : patrolSpacingFallback;
        count = wingPatrolStations(firstLeader, keep, secondLeader, spacing, stations);
    } else {
        if (!keepPosition(player, home)) return false;
        count = mirrorPatrolStations(firstLeader, keep, home, stations);
    }
    // Do not restart a march that has already reached its station.
    for (int index = 0; index < count; ++index)
        if (patrolDistance(stations[index], secondLeader) <= 3) return true;
    // At most two reachability queries and one accepted order per AI update.
    for (int index = 0; index < count; ++index) {
        const int tile = stationTile(stations[index]);
        if (!tile || !reinterpret_cast<TribeQuery>(nativeBindings.tribePath)(aic, tribe, tile)) continue;
        if (reinterpret_cast<TribeMove>(nativeBindings.siegeGroupMove)(
                reinterpret_cast<void*>(nativeBindings.tribes), tribe,
                stations[index].x, stations[index].y, 0, 0, 0)) return true;
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
    if (character < 1 || character > 16 || setting(patrolOrders, character, patrolOrdersFallback) != 1) return;
    // A running match fixes the configuration, as the census does for others.
    configurationLocked = 1;
    TilePoint leader;
    const int tribe = patrolTribe(player, SecondPatrolSlot, leader);
    if (!tribe) return;
    // The native dispatcher sets this stance on its patrol tribe before ordering it.
    at<short>(tribeAddress(tribe) + nativeBindings.tribeStance) = 1;
    if (!orderStation(aic, player, character, tribe, leader)) nativeDestination(tribe);
}

int validSlotTribe(int player, int slot, TilePoint& leader) { return patrolTribe(player, slot, leader); }
bool tribeLeaderTile(int player, int tribe, TilePoint& position) { return leaderPosition(player, tribe, position); }
int movableTile(TilePoint point) { return stationTile(point); }

} // namespace SHC141
} // namespace AicTactics
