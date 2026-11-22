-- Attack group duties that reuse native AI slots. Absent fields keep the
-- native behaviour: only the first attack patrol group receives orders.
local M = {fields = {'PatrolGroupOrders', 'PatrolFormation', 'PatrolSpacing'}}
local orders = {Native = 0, AllGroups = 1}
local formations = {Native = 0, Wing = 1, Mirror = 2}

function M.defaults()
  return {PatrolGroupOrders = 'Native', PatrolFormation = 'Native', PatrolSpacing = 12}
end

function M.prepare(previous, spec, resetting)
  local result = M.defaults()
  if not resetting then
    for _, field in ipairs(M.fields) do
      if previous and previous[field] ~= nil then result[field] = previous[field] end
      if spec[field] ~= nil then result[field] = spec[field] end
    end
  end
  assert(orders[result.PatrolGroupOrders] ~= nil, 'PatrolGroupOrders must be Native or AllGroups')
  assert(formations[result.PatrolFormation] ~= nil, 'PatrolFormation must be Native, Wing or Mirror')
  local spacing = result.PatrolSpacing
  assert(type(spacing) == 'number' and spacing == math.floor(spacing) and spacing >= 4 and spacing <= 40,
    'PatrolSpacing must be an integer from 4 to 40')
  if not resetting then
    assert(spec.PatrolFormation == nil or result.PatrolGroupOrders == 'AllGroups',
      'PatrolFormation requires PatrolGroupOrders AllGroups')
    assert(spec.PatrolSpacing == nil or result.PatrolFormation == 'Wing',
      'PatrolSpacing requires PatrolFormation Wing')
  end
  -- Authored values stay stored for later partial edits; only an active
  -- choice reaches the native record.
  local active = orders[result.PatrolGroupOrders]
  local formation = active == 1 and formations[result.PatrolFormation] or 0
  return result, {active, formation, formation == 1 and spacing or 0}
end

function M.active(candidate)
  return candidate.PatrolGroupOrders ~= nil and candidate.PatrolGroupOrders ~= 'Native'
end

function M.copy(candidate)
  local result = {}
  for _, field in ipairs(M.fields) do result[field] = candidate[field] end
  return result
end

return M
