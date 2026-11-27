local M = {}

-- Bind during module enable or personality admission, before installing hooks.
function M.find(name, signature)
  assert(type(core.AOBScanUnique) == 'function',
    'AIC Tactics: unique game code scanning is unavailable')
  local site, reason = core.AOBScanUnique(signature)
  assert(type(site) == 'number' and site > 0 and site % 1 == 0,
    'AIC Tactics: native ' .. name .. ' ' .. (reason or 'has an invalid address'))
  return site
end

function M.call(site, name)
  assert(core.readByte(site) == 0xE8, 'AIC Tactics: modified native '..name..' call')
  local target = site + 5 + core.readInteger(site + 1)
  assert(target > 0, 'AIC Tactics: invalid native '..name..' call')
  return target
end

-- Validate context at an already resolved owner without scanning again.
function M.verify(site, name, signature)
  local offset=0
  for token in signature:gmatch('%S+') do
    assert(token=='?' or core.readByte(site+offset)==tonumber(token,16),
      'AIC Tactics: modified native '..name)
    offset=offset+1
  end
end

return M
