"""Licensed-binary check for the attack patrol dispatch bindings.

Resolves the real layout, group-action and patrol owners against every local
reference executable, then repeats the patrol resolution on copies where another
module replaced the dispatcher entry, a native caller or the patrol count read.
Usage: check_patrol_bindings.py [projects-root]
"""
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

import pefile
from lupa import LuaRuntime

ROOT = Path(__file__).resolve().parents[1]
PROJECTS = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT.parent
FIXTURES = PROJECTS / 'Roadmap/Investigations/Tunnelers-Engineers/binary-identities.json'
STATE_UPDATE = '53 56 8B 74 24 0C 57 8B FE 69 FF F4 39 00 00 8B 87 ? ? ? ? 85 C0 8B D9 0F 84'
# Same identifying context as native-siege-bindings.lua's group movement owner.
GROUP_MOVE = '83 EC 34 33 C0 55 8B 6C 24 44 57 8B 7C 24 44 3B F8 89 4C 24 08 89 44 24 18'


def runtime(image, base):
    def aob(pattern):
        expression = b''.join(b'.' if token == '?' else re.escape(bytes([int(token, 16)]))
                              for token in pattern.split())
        matches = list(re.finditer(expression, bytes(image), re.S))
        if len(matches) != 1:
            raise RuntimeError('pattern matched %d times' % len(matches))
        return base + matches[0].start()

    lua = LuaRuntime()
    lua.globals().root = ROOT.as_posix()
    lua.globals().aob = aob
    lua.globals().read_byte = lambda address: image[address - base]
    lua.globals().read_integer = lambda address: struct.unpack_from('<i', image, address - base)[0]
    lua.globals().state_update = STATE_UPDATE
    lua.globals().group_move = GROUP_MOVE
    lua.execute('''
      package.path=root..'/?.lua;'..package.path
      core={AOBScan=aob,readByte=read_byte,readInteger=read_integer}
      context=require('native-context')
      game=require('native-layout').resolve()
      for key,value in pairs(require('native-group-actions').resolve(game)) do game[key]=value end
      game.updateAIPlayerState=context.find('AI state update',state_update)
      game.siegeGroupMove=context.find('group move',group_move)
      patrol=require('native-patrol-bindings')
    ''')
    return lua


def check(identity):
    path = Path(identity['path']) if 'path' in identity else PROJECTS / identity['file']
    raw = path.read_bytes()
    assert hashlib.sha256(raw).hexdigest() == identity['sha256']
    pe = pefile.PE(data=raw)
    base = pe.OPTIONAL_HEADER.ImageBase
    image = bytearray(pe.get_memory_mapped_image())
    lua = runtime(image, base)
    lua.execute('''
      sites=patrol.resolve(game)
      assert(sites.attackPatrolUnavailable==nil, sites.attackPatrolUnavailable)
      assert(#sites.attackPatrolCalls==3)
      for _,site in ipairs(sites.attackPatrolCalls) do
        assert(context.call(site,'patrol caller')==sites.attackPatrolDispatch)
        assert(site-game.updateAIPlayerState<0xA00)
      end
      context.verify(game.findAttackGroup+patrol.patrolCountOffset,'patrol count',patrol.patrolCountSignature)
    ''')
    sites = lua.globals().sites
    game = lua.globals().game
    dispatch = sites['attackPatrolDispatch']
    calls = [sites['attackPatrolCalls'][index] for index in (1, 2, 3)]
    result = {'variant': path.parent.parent.name + '/' + path.name if 'file' in identity else path.name,
              'sha256': identity['sha256'][:8], 'dispatch': hex(dispatch),
              'destination': hex(sites['attackPatrolDestination']),
              'troopValueState': hex(sites['troopValueState']),
              'patrolStance': hex(game['tribeStance']),
              'playableTiles': hex(sites['playableTiles']),
              'tribeStride': hex(game['tribeStride']),
              'callOffsets': [hex(call - game['updateAIPlayerState']) for call in calls]}

    # UCP loads every module before enabling any, so AIC Tactics resolves on
    # original bytes. Another module enabled later then changes the dispatcher
    # entry, one native caller or the patrol count; the preflight must see it.
    def patch(address, data):
        offset = address - base
        saved = bytes(image[offset:offset + len(data)])
        image[offset:offset + len(data)] = data
        return lambda: image.__setitem__(slice(offset, offset + len(data)), saved)

    def write_code(address, values):
        # The module only writes E8 rel32 here: one opcode byte, then a dword.
        patch(address, bytes([values[1]]) + struct.pack('<i', values[2]))

    lua.globals().write_code = write_code
    # The production preflight and install, run on this executable's image.
    lua.execute('''
      core.writeCode=function(address, values) write_code(address, values) end
      for key,value in pairs(sites) do game[key]=value end
      function preflight(expected)
        return (pcall(patrol.preflight, game, expected or game.attackPatrolDispatch))
      end
    ''')
    assert lua.execute('return preflight()') is True
    for address, data in ((dispatch, b'\xE9' + struct.pack('<i', 0x1000)),
                          (calls[1] + 1, struct.pack('<i', 0x1000)),
                          (sites['attackPatrolDestination'], b'\xE9' + struct.pack('<i', 0x1000)),
                          (game['findAttackGroup'] + 0x65, bytes([0xBA, 1, 0, 0, 0, 0x90, 0x90]))):
        undo = patch(address, data)
        assert lua.execute('return preflight()') is False
        undo()
    assert lua.execute('return preflight()') is True
    # Install redirects all three callers to the handler; afterwards the
    # preflight expects the handler, and a second install is not needed.
    saved = bytes(image)
    lua.execute('''
      local handler = game.updateAIPlayerState + 0x40000
      patrol.install(game, handler)
      for _, site in ipairs(game.attackPatrolCalls) do
        assert(context.call(site, 'redirected caller') == handler)
      end
      assert(preflight(handler) and not preflight())
      local ok, message = pcall(patrol.preflight, {attackPatrolDispatch=0,
        attackPatrolUnavailable='AIC Tactics: missing native attack patrol dispatch'}, 0)
      assert(not ok and message:find('unavailable: missing native attack patrol dispatch', 1, true)
        and not message:find('AIC Tactics: missing', 1, true))
    ''')
    image[:] = saved

    # A module that patched before AIC Tactics resolved: AIC Tactics still
    # starts and reports patrol orders as unavailable.
    occupied = bytearray(image)
    offset = dispatch - base
    occupied[offset:offset + 5] = b'\xE9' + struct.pack('<i', 0x1000)
    lua = runtime(occupied, base)
    lua.execute('assert(patrol.resolve(game).attackPatrolUnavailable~=nil)')
    redirected = bytearray(image)
    offset = calls[1] - base
    redirected[offset + 1:offset + 5] = struct.pack('<i', 0x1000)
    lua = runtime(redirected, base)
    lua.execute('assert(patrol.resolve(game).attackPatrolUnavailable:find("callers changed"))')
    # A module that forced one patrol group before AIC Tactics resolved: the
    # module still starts (findAttackGroup no longer covers that read) and the
    # patrol preflight refuses.
    forced = bytearray(image)
    offset = game['findAttackGroup'] + 0x65 - base
    forced[offset:offset + 7] = bytes([0xBA, 1, 0, 0, 0, 0x90, 0x90])
    lua = runtime(forced, base)
    lua.execute('''
      assert(patrol.resolve(game).attackPatrolUnavailable==nil)
      assert(not pcall(context.verify,game.findAttackGroup+patrol.patrolCountOffset,
        'count',patrol.patrolCountSignature))
    ''')
    result['negativeChecks'] = ('later entry/caller/destination/count patches refused; earlier entry/caller '
                                'patches reported unavailable; earlier count patch starts and is refused')
    return result


if __name__ == '__main__':
    identities = json.loads(FIXTURES.read_text())
    identities += json.loads((FIXTURES.parent / 'official-patches/executable-matrix.json').read_text())
    print(json.dumps([check(identity) for identity in identities], indent=2))
