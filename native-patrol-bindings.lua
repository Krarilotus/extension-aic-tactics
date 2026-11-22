local context = require('native-context')
local M = {}

-- sendAITribe_0xf_toAttackPatrol: validates the AI, slot 15 and its UID and
-- leader, marks the tribe, then jumps to the native destination routine.
M.dispatcherSignature = '8B 4C 24 04 8B C1 69 C0 F4 39 00 00 83 B8 ? ? ? ? 00 74 52 8B D1 69 D2 F4 39 00 00 '..
  '0F BF 92 ? ? ? ? 85 D2 74 3F 8B C2 69 C9 F4 39 00 00 69 C0 ? ? ? ? 56 8B B0 ? ? ? ? '..
  '3B B1 ? ? ? ? 5E 75 21 66 83 B8 ? ? ? ? 00 74 17 66 C7 80 ? ? ? ? 01 00 89 54 24 04 B9 ? ? ? ? E9'
-- sendAttackingPatrolTribeToComputedDestination(tribe): leader-based search.
local destinationSignature = '53 8B 5C 24 08 8B C3 69 C0 ? ? ? ? 56 0F BF B0 ? ? ? ? 57 6A 01 B9 ? ? ? ? E8 ? ? ? ? '..
  '69 F6 ? ? ? ? 0F BF 8E ? ? ? ? 0F BF 96 ? ? ? ? 0F BF 86 ? ? ? ? 6A 01 6A 0C'
-- addUnitToSmallestBehaviourTypeTribe reads AttUnitPatrolGroupsCount here.
M.patrolCountOffset = 0x65
M.patrolCountSignature = '8B 94 0A 64 02 00 00'
-- The native callers pass the player and the AI state object.
local callerPrefix = {0x56, 0x8B, 0xCB}
local scanLength = 0xA00

local function resolve(game)
  local read = core.readInteger
  local site = context.find('attack patrol dispatch', M.dispatcherSignature)
  assert(read(site + 0x0E) == game.players + 0x2300
      and read(site + 0x20) == game.players + 0x310C + 15 * 2
      and read(site + 0x32) == game.tribeStride
      and read(site + 0x39) == game.tribes + 0x34
      and read(site + 0x3F) == game.players + 0x329C + 15 * 4
      and read(site + 0x49) == game.tribes + 0x5A,
    'AIC Tactics: inconsistent native attack patrol dispatch')
  local flag = read(site + 0x53) - game.tribes
  assert(flag > 0x5C and flag < game.tribeStride - 1,
    'AIC Tactics: inconsistent native attack patrol flag')
  local troopValueState = read(site + 0x5E)
  local destination = site + 0x67 + read(site + 0x63)
  context.verify(destination, 'attack patrol destination', destinationSignature)
  assert(read(destination + 9) == game.tribeStride
      and read(destination + 17) == game.tribes + 0x5A
      and read(destination + 36) == 0x490
      and read(destination + 43) == game.unitRecords + 0x96
      and read(destination + 50) == game.unitRecords + 0xC6
      and read(destination + 57) == game.unitRecords + 0xC4
      and troopValueState > 0,
    'AIC Tactics: inconsistent native attack patrol destination')
  -- Every native caller lies inside the AI state update. Require all three.
  local calls = {}
  local owner = game.updateAIPlayerState
  for offset = 3, scanLength - 5 do
    local address = owner + offset
    if core.readByte(address) == 0xE8 and address + 5 + read(address + 1) == site then
      for index, byte in ipairs(callerPrefix) do
        assert(core.readByte(address - 4 + index) == byte,
          'AIC Tactics: unexpected native attack patrol caller')
      end
      calls[#calls + 1] = address
    end
  end
  assert(#calls == 3, 'AIC Tactics: native attack patrol callers changed')
  return {attackPatrolDispatch = site, attackPatrolDestination = destination,
    troopValueState = troopValueState, tribePatrolFlag = flag, attackPatrolCalls = calls}
end

-- Another module may already own the dispatcher (for example Smarter Patrols
-- replaces its entry). AIC Tactics still starts; only characters that opt in
-- to patrol orders are refused, with the reason.
function M.resolve(game)
  local ok, result = pcall(resolve, game)
  if ok then return result end
  return {attackPatrolDispatch = 0, attackPatrolDestination = 0, troopValueState = 0,
    tribePatrolFlag = 0, attackPatrolCalls = {}, attackPatrolUnavailable = tostring(result)}
end

return M
