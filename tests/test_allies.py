"""Ally help: AIC fields, module settings, inheritance and recorded state."""
import json
from pathlib import Path

import yaml

from test_backend import backend
from test_state import state

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ['AICTactics_AllyRelief', 'AICTactics_AllyGoods', 'AICTactics_AllyGoldKeep',
          'AICTactics_AllyRecruitGoldBuffer', 'AICTactics_AllyGoldSharePercent',
          'AICTactics_AllyMonthlyGivePercent', 'AICTactics_AllyArmySharePercent']


def test_fields_store_inherit_or_explicit_values_per_ai():
    backend().execute('''
      local function words(ai)
        local result={}
        for word=0,6 do result[#result+1]=memory[native.allySettings+(ai*7+word)*4] or 0 end
        return result
      end
      prepare(4,{}).commit()
      for _,value in ipairs(words(4)) do assert(value==0) end
      assert(reliefPreflights==nil and goodsPreflights==nil)
      prepare(4,{AICTactics_AllyRelief=true,AICTactics_AllyGoldKeep=2500,
        AICTactics_AllyArmySharePercent=0}).commit()
      local stored=words(4)
      assert(stored[1]==2 and stored[2]==0 and stored[3]==2501 and stored[7]==1)
      assert(reliefPreflights==1 and reliefActivations==1 and goodsActivations==nil)
      -- The save-admission record stays identical to a native character's.
      for address=10000+4*352,10000+4*352+348,4 do assert((memory[address] or 0)==0) end
      -- Explicit off is distinct from absent and activates nothing.
      prepare(5,{AICTactics_AllyGoods=false,AICTactics_AllyGoldSharePercent=0}).commit()
      stored=words(5)
      assert(stored[2]==1 and stored[5]==1 and goodsActivations==nil)
      prepare(6,{AICTactics_AllyGoods=true}).commit()
      assert(goodsPreflights==1 and goodsActivations==1)
      -- A rollback restores every earlier word.
      local op=prepare(4,{AICTactics_AllyGoldKeep=0})
      op.commit()
      stored=words(4)
      assert(stored[1]==0 and stored[3]==1 and stored[7]==0)
      op.rollback()
      stored=words(4)
      assert(stored[1]==2 and stored[3]==2501 and stored[7]==1)
    ''')


def test_field_values_are_validated():
    backend().execute('''
      local bounds={AICTactics_AllyGoldKeep={0,100000},AICTactics_AllyRecruitGoldBuffer={0,5000},
        AICTactics_AllyGoldSharePercent={0,100},AICTactics_AllyMonthlyGivePercent={0,100},
        AICTactics_AllyArmySharePercent={0,100}}
      for field,range in pairs(bounds) do
        assert(pcall(prepare,4,{[field]=range[1]}) and pcall(prepare,4,{[field]=range[2]}))
        for _,value in ipairs({range[1]-1,range[2]+1,1.5,'10',true}) do
          assert(not pcall(prepare,4,{[field]=value}))
        end
      end
      for _,field in ipairs({'AICTactics_AllyRelief','AICTactics_AllyGoods'}) do
        assert(not pcall(prepare,4,{[field]=1}) and not pcall(prepare,4,{[field]='On'}))
      end
    ''')


def test_module_settings_default_on_and_validate():
    backend().execute('''
      local fallbacks=require('config.allies').fallbacks
      local words=fallbacks(nil)
      assert(#words==7 and words[1]==1 and words[2]==1 and words[3]==1000 and words[4]==300
        and words[5]==25 and words[6]==30 and words[7]==75)
      words=fallbacks({allyRelief=false,allyGoods=false,allyGoldKeep=0,allyArmySharePercent=100})
      assert(words[1]==0 and words[2]==0 and words[3]==0 and words[7]==100)
      assert(not pcall(fallbacks,{allyGoldKeep=-1}) and not pcall(fallbacks,{allyArmySharePercent=101}))
      assert(not pcall(fallbacks,{allyRelief='yes'}))
    ''')


def test_ally_policy_requires_a_fresh_process_and_never_changes_the_record():
    backend().execute('''
      local schema=require('config.personality')
      local candidate=schema.defaults()
      assert(not schema.active(candidate))
      candidate.AICTactics_AllyRelief=true
      assert(not schema.recordActive(candidate) and schema.active(candidate))
      prepare(4,{AICTactics_AllyMonthlyGivePercent=50}).commit()
      assert(backend.policyActive())
      memory[native.game.gameTick]=500
      assert(not pcall(prepare,5,{AICTactics_AllyGoods=true}))
    ''')


def test_module_switches_make_the_policy_active():
    backend().execute('''
      assert(not backend.policyActive())
      memory[native.allyFallback]=1
      assert(backend.policyActive())
      memory[native.allyFallback]=0
      memory[native.allyFallback+4]=1
      assert(backend.policyActive())
    ''')


def test_recorded_gifts_and_relief_timers_survive_a_cold_load():
    state().execute('''
      local function record(player,word) return native.allyStates+(player*65+word)*4 end
      memory[record(2,0)]=7               -- month 7
      memory[record(2,1+4)]=40            -- 40 stone given this month
      memory[record(2,1+15)]=500          -- 500 gold given this month
      memory[record(3,26+4)]=8            -- stone last received in month 7
      memory[record(3,51+1)]=2;memory[record(3,56+1)]=1   -- a barracks lost
      memory[record(2,61)]=6400           -- next ask
      memory[record(2,62)]=8000           -- relief pause
      memory[record(2,63)]=5600           -- last threat seen
      memory[record(2,64)]=1              -- serves a help request
      local bytes=state.capture()
      core.setMemory(native.allyStates,0,9*65*4)
      state.restore(bytes)
      assert(memory[record(2,0)]==7 and memory[record(2,5)]==40 and memory[record(2,16)]==500)
      assert(memory[record(3,30)]==8 and memory[record(3,52)]==2 and memory[record(3,57)]==1)
      assert(memory[record(2,61)]==6400 and memory[record(2,62)]==8000 and memory[record(2,63)]==5600)
      assert(memory[record(2,64)]==1 and state.capture()==bytes)
      -- A fresh match starts without gifts or timers.
      state.callbacks:initialize({kind='map'})
      assert(memory[record(2,5)]==0 and memory[record(2,62)]==0)
    ''')


def test_invalid_ally_state_is_rejected_before_any_write():
    state().execute('''
      local function record(player,word) return native.allyStates+(player*65+word)*4 end
      local good=state.capture()
      for _,case in ipairs({{2,1+4,-5},{3,56+1,3},{2,64,2},{0,5,1}}) do
        memory[record(case[1],case[2])]=case[3]
        if case[2]==57 then memory[record(case[1],52)]=2 end
        local bad=state.capture()
        core.setMemory(native.allyStates,0,9*65*4)
        assert(not pcall(state.restore,bad))
        memory[record(case[1],case[2])]=0
        memory[record(case[1],52)]=0
      end
      state.restore(good)
    ''')


def test_settings_enter_match_identity_but_not_save_admission():
    state().execute('''
      local saveIdentity,matchIdentity=state.identity(),state.matchIdentity()
      local bytes=state.capture()
      memory[native.allySettings+(4*7+2)*4]=2501
      memory[native.allyFallback+6*4]=75
      assert(state.identity()==saveIdentity and state.matchIdentity()~=matchIdentity)
      state.restore(bytes)
      assert(state.capture()==bytes)
    ''')


def test_ally_settings_require_the_saved_section():
    state().execute('''
      memory[native.configuration+4*352]=0
      assert(not state.callbacks.isRequired())
      memory[native.allyFallback]=1
      assert(state.callbacks.isRequired())
      memory[native.allyFallback]=0
      memory[native.allySettings+(5*7+6)*4]=1
      assert(state.callbacks.isRequired())
    ''')


def test_schema_options_and_locales_match_the_lua_bounds():
    schema = json.loads((ROOT / 'schema/aic-tactics.schema.json').read_text(encoding='utf-8'))
    options = {option['name']: option for option in
               yaml.safe_load((ROOT / 'options.yml').read_text(encoding='utf-8'))['options']}
    bounds = {'AICTactics_AllyGoldKeep': (0, 100000, 'allyGoldKeep', 1000),
              'AICTactics_AllyRecruitGoldBuffer': (0, 5000, 'allyRecruitGoldBuffer', 300),
              'AICTactics_AllyGoldSharePercent': (0, 100, 'allyGoldSharePercent', 25),
              'AICTactics_AllyMonthlyGivePercent': (0, 100, 'allyMonthlyGivePercent', 30),
              'AICTactics_AllyArmySharePercent': (0, 100, 'allyArmySharePercent', 75)}
    for field in FIELDS:
        assert field in schema['properties']
    for field, (low, high, option, default) in bounds.items():
        entry = schema['properties'][field]
        assert entry['type'] == 'integer' and entry['minimum'] == low and entry['maximum'] == high
        contents = options[option]['contents']
        assert contents['min'] == low and contents['max'] == high and contents['value'] == default
    for option in ('allyRelief', 'allyGoods'):
        assert options[option]['contents']['value'] is True
        assert options[option]['category'] == ['{{ai}}', '{{improvements}}']
    locales = sorted((ROOT / 'locale').glob('*.yml'))
    assert len(locales) == 9
    for path in locales:
        labels = yaml.safe_load(path.read_text(encoding='utf-8'))
        for option in ['allyRelief', 'allyGoods'] + [value[2] for value in bounds.values()]:
            assert labels[option] and labels[option + '_descr']
