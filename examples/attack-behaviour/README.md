# Attack behaviour examples

These fragments are documentation and test fixtures, not presets. Merge one into
the `aic` object of a copied personality whose `AttUnitPatrol` names a troop
type and whose `AttUnitPatrolMax` is at least 16. Every other value keeps its
authored or native meaning.

| File | What a player should see |
| --- | --- |
| `two-patrols.json` | Both attack patrol groups leave camp during an assault and each picks its own steps. |
| `wing-patrol.json` | The two patrols advance side by side, about 14 tiles apart. |
| `pincer-patrol.json` | The second patrol works the other side of the target keep. |

Without these fields the native game orders only the first patrol group; the
second waits at its staging point. Named combinations for an AI pack belong in
that pack's own files.
