local M = {}

-- UnitType.hpp. Slaves, slingers and fire throwers fill remaining seats.
local priority = {
  [22]=1, [23]=1, [24]=1, [25]=1, [26]=1, [27]=1, [37]=1,
  [70]=1, [71]=2, [72]=2, [73]=1, [75]=1, [76]=2,
}

local function board(riders, towers, units)
  local nextTower = 1
  for _, group in ipairs(units) do
    for _, unit in ipairs(group) do
      if #towers == 0 then return end
      local tower = towers[nextTower]
      if riders:boardUnit(tower.id, unit) then
        tower.seats = tower.seats - 1
        if tower.seats == 0 then
          table.remove(towers, nextTower)
        else
          nextTower = nextTower + 1
        end
        if nextTower > #towers then nextTower = 1 end
      end
    end
  end
end

function M.register(riders, native, game)
  if not riders or type(riders.registerTowerCrewPolicy) ~= 'function' then return end
  local short, integer = core.readSmallInteger, core.readInteger
  local function inWave(record, player, wave)
    if short(record + 0x8C) ~= 2 or short(record + 0x96) ~= player then return false end
    local id = short(record + 0x2D8)
    if id <= 0 or id >= 1250 then return false end
    local tribe = game.tribes + id * game.tribeStride
    return short(tribe + 0x40) == 2 and integer(tribe + 0x2C) == player
      and integer(tribe + 0x34) == integer(record + 0x2E4)
      and short(tribe + game.tribeAttackWave) == wave
  end
  local function wallBound(record)
    return short(record + 0x39C) == 0x17 and short(record + 0x2C0) == 0x65
  end
  riders:registerTowerCrewPolicy(function(tower)
    if not riders:isClimbingReady() then return false end
    local record = game.unitRecords + tower * 0x490
    local player = short(record + 0x96)
    if player < 1 or player > 8 then return false end
    local owner = game.players + player * 0x39F4
    local character = integer(owner + 0x2300) - 1
    if character < 1 or character > 16
        or integer(native.configuration + character * native.configurationSize + 348) == 1 then
      return false
    end
    local wave = integer(owner + 0x3924)
    if wave <= 0 or not inWave(record, player, wave) then return false end
    -- Keep seats available before departure. The owning module checks both
    -- native and replacement tower orders here, before its proximity fill.
    if not wallBound(record) or riders:getFreeSeats(tower) == 0 then return true end
    local towers, units = {}, {{}, {}}
    for id = 1, game.unitCapacity - 1 do
      local candidate = game.unitRecords + id * 0x490
      local kind = short(candidate + 0x8E)
      if (kind == 58 or priority[kind]) and inWave(candidate, player, wave) then
        if kind == 58 then
          if wallBound(candidate) then
            local seats = riders:getFreeSeats(id)
            if seats > 0 then towers[#towers + 1] = {id=id, seats=seats} end
          end
        else
          local group = units[priority[kind]]
          group[#group + 1] = id
        end
      end
    end
    board(riders, towers, units)
    return true
  end)
end

return M
