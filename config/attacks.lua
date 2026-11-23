-- Per-character attack patrol settings. An absent field uses the module
-- setting from Customizations; an explicit value switches the feature for
-- this character only. Stored per AI as 0 (use the module setting) or the
-- chosen value + 1, like the siege fields.
local M = {fields = {'AICTactics_PatrolOrders', 'AICTactics_PatrolFormation', 'AICTactics_PatrolSpacing'}}
local orders = {Native = 0, AllGroups = 1}
local formations = {Native = 0, Wing = 1, Mirror = 2}

M.orders, M.formations = orders, formations

function M.defaults() return {} end

function M.prepare(previous, spec, resetting)
  local result = {}
  if not resetting then
    for _, field in ipairs(M.fields) do
      if previous and previous[field] ~= nil then result[field] = previous[field] end
      if spec[field] ~= nil then result[field] = spec[field] end
    end
  end
  local order, formation, spacing = result.AICTactics_PatrolOrders, result.AICTactics_PatrolFormation,
    result.AICTactics_PatrolSpacing
  assert(order == nil or orders[order] ~= nil, 'AICTactics_PatrolOrders must be Native or AllGroups')
  assert(formation == nil or formations[formation] ~= nil,
    'AICTactics_PatrolFormation must be Native, Wing or Mirror')
  assert(spacing == nil or (type(spacing) == 'number' and spacing == math.floor(spacing)
      and spacing >= 4 and spacing <= 40), 'AICTactics_PatrolSpacing must be an integer from 4 to 40')
  return result, {order == nil and 0 or orders[order] + 1, formation == nil and 0 or formations[formation] + 1,
    spacing or 0}
end

function M.active(candidate)
  for _, field in ipairs(M.fields) do if candidate[field] ~= nil then return true end end
  return false
end

function M.copy(candidate)
  local result = {}
  for _, field in ipairs(M.fields) do result[field] = candidate[field] end
  return result
end

-- Module settings: the defaults for every AI.
function M.fallbacks(config)
  local order = config and config.patrolOrders or 'AllGroups'
  local formation = config and config.patrolFormation or 'Wing'
  local spacing = config and config.patrolSpacing or 12
  assert(orders[order] ~= nil, 'AIC Tactics: patrolOrders must be Native or AllGroups')
  assert(formations[formation] ~= nil, 'AIC Tactics: patrolFormation must be Native, Wing or Mirror')
  assert(type(spacing) == 'number' and spacing == math.floor(spacing) and spacing >= 4 and spacing <= 40,
    'AIC Tactics: patrolSpacing must be an integer from 4 to 40')
  return orders[order], formations[formation], spacing
end

return M
