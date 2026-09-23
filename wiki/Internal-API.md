# Internal API

Internal storage machinery owns raw slot lifetime, active-element bookkeeping, relocation/construction constraints, compact indices and recursive trait certification. It must preserve the rule that only active elements have object lifetime.
