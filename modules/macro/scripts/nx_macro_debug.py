#!/usr/bin/env python3
"""
nx_macro_debug.py — Interactive C preprocessor macro expansion debugger.

Usage:
  python nx_macro_debug.py -I modules/macro/include -f modules/macro/include/nx/macro.hpp
  python nx_macro_debug.py -I modules/macro/include -f modules/macro/include/nx/macro.hpp "NX_INC(3)"
  python nx_macro_debug.py ... --all "NX_AND(1, 0)"
  python nx_macro_debug.py ... --list [pattern]
  python nx_macro_debug.py ... -D MY_MACRO=value
"""

import re
import sys
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

# ── Colors ────────────────────────────────────────────────────────────────────

USE_COLOR = sys.stdout.isatty()

def c(text, *codes):
    if not USE_COLOR or not codes:
        return str(text)
    return ''.join(f'\033[{x}m' for x in codes) + str(text) + '\033[0m'

BOLD=1; DIM=2; RED=31; GRN=32; YLW=33; BLU=34; MAG=35; CYN=36

# ── Tokenizer ─────────────────────────────────────────────────────────────────

_TOK_RE = re.compile(r'|'.join([
    r'(?P<PASTE>##)',
    r'(?P<STR>#)',
    r'(?P<DOTS>\.\.\.)',
    r'(?P<ID>[A-Za-z_]\w*)',
    r'(?P<NUM>\d[\d.]*[uUlLfF]*)',
    r'(?P<STRLIT>"(?:[^"\\]|\\.)*")',
    r'(?P<CHRLIT>\'(?:[^\'\\]|\\.)*\')',
    r'(?P<LP>\()',
    r'(?P<RP>\))',
    r'(?P<COMMA>,)',
    r'(?P<SP>[ \t]+)',
    r'(?P<NL>\n)',
    r'(?P<OTHER>.)',
]))

@dataclass
class Tok:
    k: str
    v: str
    def __repr__(self): return f'{self.k}:{self.v!r}'

def tokenize(text: str) -> list:
    out = []
    for m in _TOK_RE.finditer(text):
        k, v = m.lastgroup, m.group()
        if k == 'NL': continue
        if k == 'SP': v = ' '
        out.append(Tok(k, v))
    return out

def toks2str(toks: list) -> str:
    parts, prev_sp = [], True
    for t in toks:
        if t.k == 'SP':
            if not prev_sp:
                parts.append(' ')
                prev_sp = True
        else:
            parts.append(t.v)
            prev_sp = False
    return ''.join(parts).strip()

def tok_strip(toks: list) -> list:
    """Remove leading/trailing SP tokens."""
    s, e = 0, len(toks)
    while s < e and toks[s].k == 'SP': s += 1
    while e > s and toks[e-1].k == 'SP': e -= 1
    return toks[s:e]

# ── Macro definition ──────────────────────────────────────────────────────────

@dataclass
class MDef:
    name: str
    params: Optional[list]   # None → object-like
    variadic: bool
    body: list               # list[Tok]
    file: str = ''
    line: int = 0

    @property
    def func_like(self): return self.params is not None

    def signature(self):
        if not self.func_like: return self.name
        ps = list(self.params)
        if self.variadic: ps.append('...')
        return f"{self.name}({', '.join(ps)})"

    def __str__(self):
        return f"#define {self.signature()} {toks2str(self.body)}"

# ── Macro database ────────────────────────────────────────────────────────────

class MacroDB:
    def __init__(self):
        self.defs: dict = {}
        self.seen: set = set()
        self.inc_dirs: list = []
        self.warnings: list = []

    def add_inc(self, d: str):
        self.inc_dirs.append(Path(d))

    def define(self, spec: str):
        """Add a -D style definition: NAME or NAME=body."""
        if '=' in spec:
            name, body = spec.split('=', 1)
        else:
            name, body = spec, '1'
        m = self._parse_define(f'{name} {body}', '<cmdline>', 0)
        if m: self.defs[m.name] = m

    def load(self, path: str, follow_inc=True) -> int:
        rp = str(Path(path).resolve())
        if rp in self.seen: return 0
        self.seen.add(rp)
        try:
            text = Path(rp).read_text(encoding='utf-8', errors='replace')
        except FileNotFoundError:
            self.warnings.append(f'not found: {path}')
            return 0
        except PermissionError:
            self.warnings.append(f'permission denied: {path}')
            return 0
        n = 0
        for lineno, raw in enumerate(self._join_cont(text), 1):
            line = self._strip_comments(raw).strip()
            if not line.startswith('#'): continue
            rest = line[1:].lstrip()
            if re.match(r'define\b', rest):
                m = self._parse_define(rest[6:].lstrip(), rp, lineno)
                if m:
                    self.defs[m.name] = m
                    n += 1
            elif follow_inc and re.match(r'include\b', rest):
                self._do_include(rest[7:].lstrip(), rp)
        return n

    def _join_cont(self, text: str) -> list:
        lines, cur = [], ''
        for ln in text.splitlines():
            if ln.endswith('\\'):
                cur += ln[:-1] + ' '
            else:
                cur += ln
                lines.append(cur)
                cur = ''
        if cur: lines.append(cur)
        return lines

    def _strip_comments(self, s: str) -> str:
        s = re.sub(r'/\*.*?\*/', ' ', s)
        s = re.sub(r'//.*$', '', s)
        return s

    def _parse_define(self, text: str, file: str, lineno: int) -> Optional[MDef]:
        # name optionally followed immediately by '(' for function-like
        m = re.match(r'([A-Za-z_]\w*)(\(([^)]*)\))?\s*(.*)', text, re.DOTALL)
        if not m: return None
        name = m.group(1)
        has_params = m.group(2) is not None
        params_raw = m.group(3) or ''
        body_str = m.group(4).strip()

        params, variadic = None, False
        if has_params:
            if params_raw.strip() == '':
                params = []
            else:
                parts = [p.strip() for p in params_raw.split(',')]
                params = []
                for p in parts:
                    if p == '...':
                        variadic = True
                    elif p.endswith('...'):
                        params.append(p[:-3].rstrip())
                        variadic = True
                    else:
                        params.append(p)

        return MDef(name=name, params=params, variadic=variadic,
                    body=tokenize(body_str), file=file, line=lineno)

    def _do_include(self, spec: str, from_file: str):
        spec = spec.strip()
        if spec.startswith('"') and spec.endswith('"'):
            fname = spec[1:-1]
            candidates = [Path(from_file).parent / fname]
        elif spec.startswith('<') and spec.endswith('>'):
            fname = spec[1:-1]
            candidates = []
        else:
            return
        for d in self.inc_dirs:
            candidates.append(d / fname)
        for cand in candidates:
            if cand.exists():
                self.load(str(cand))
                return

# ── Expansion steps ───────────────────────────────────────────────────────────

@dataclass
class Step:
    num: int
    name: str
    mdef: MDef
    call: str
    args: dict
    result: list
    pastes: list = field(default_factory=list)   # [(left, right, pasted)]
    notes: list = field(default_factory=list)

@dataclass
class StepErr:
    num: int
    name: str
    call: str
    msg: str

# ── Expander ──────────────────────────────────────────────────────────────────

class Expander:
    def __init__(self, db: MacroDB, max_steps=500):
        self.db = db
        self.max_steps = max_steps
        self.steps: list = []
        self._n = 0

    def run(self, text: str) -> list:
        self.steps = []
        self._n = 0
        return self._expand(tokenize(text), frozenset())

    # ── core expansion ───────────────────────────────────────────────────────

    def _expand(self, toks: list, dis: frozenset) -> list:
        # Process tokens left to right.  When a macro fires we splice its
        # expansion back in front of the remaining input and rescan — this is
        # the correct C preprocessor rescan rule and fixes "open-paren carry":
        # an object-like macro that expands to  FOO(  picks up its arguments
        # from the tokens that follow in the *original* stream.
        out = []
        i = 0
        while i < len(toks):
            t = toks[i]
            if (t.k == 'ID'
                    and t.v in self.db.defs
                    and t.v not in dis
                    and self._n < self.max_steps):
                md = self.db.defs[t.v]
                if not md.func_like:
                    # Object-like: expand, then rescan (expansion + rest-of-stream)
                    self._n += 1
                    res = list(md.body)
                    self.steps.append(Step(self._n, t.v, md, t.v, {}, res))
                    # KEY FIX: splice expansion before the remaining tokens
                    out.extend(self._expand(res + toks[i+1:], dis | {t.v}))
                    return out   # remainder already consumed above
                else:
                    # Function-like: find opening paren, which may come from
                    # the *current* token stream or after a prior expansion —
                    # either way it must be the very next non-space token.
                    j = i + 1
                    while j < len(toks) and toks[j].k == 'SP': j += 1
                    if j < len(toks) and toks[j].k == 'LP':
                        raw_args, end, err = self._collect_args(toks, j)
                        call = t.v + '(' + ', '.join(toks2str(a) for a in raw_args) + ')'
                        if err:
                            self._n += 1
                            self.steps.append(StepErr(self._n, t.v, call, err))
                            i = end + 1
                            continue
                        # arity check
                        nparams = len(md.params) if md.params else 0
                        nargs = len(raw_args)
                        # f() with 0 params: one empty arg → 0 args
                        if nargs == 1 and not toks2str(raw_args[0]) and nparams == 0 and not md.variadic:
                            nargs = 0; raw_args = []
                        if not md.variadic and nargs != nparams:
                            self._n += 1
                            self.steps.append(StepErr(
                                self._n, t.v, call,
                                f'expected {nparams} arg(s), got {nargs}'))
                            i = end + 1
                            continue
                        # pre-expand each named arg (in isolation, standard rule)
                        raw_map, exp_map = {}, {}
                        for pi, pname in enumerate(md.params or []):
                            atoks = tok_strip(raw_args[pi]) if pi < len(raw_args) else []
                            raw_map[pname] = atoks
                            exp_map[pname] = self._expand(atoks, dis)
                        if md.variadic:
                            va: list = []
                            base = len(md.params or [])
                            for vi in range(base, len(raw_args)):
                                if vi > base: va.append(Tok('COMMA', ','))
                                va.extend(tok_strip(raw_args[vi]))
                            raw_map['__VA_ARGS__'] = va
                            exp_map['__VA_ARGS__'] = self._expand(va, dis)
                        self._n += 1
                        res, pastes = self._subst(md, raw_map, exp_map)
                        self.steps.append(Step(self._n, t.v, md, call, exp_map, res, pastes))
                        # KEY FIX: splice expansion before the remaining tokens
                        out.extend(self._expand(res + toks[end+1:], dis | {t.v}))
                        return out   # remainder already consumed above
                    else:
                        out.append(t); i += 1
            else:
                if t.k != 'SP' or (out and out[-1].k != 'SP'):
                    out.append(t)
                i += 1
        return out

    # ── collect function-like macro arguments ─────────────────────────────────

    def _collect_args(self, toks: list, lp: int):
        """Return (list_of_arg_lists, closing_paren_idx, error_or_None)."""
        args = [[]]
        depth = 1
        i = lp + 1
        while i < len(toks):
            t = toks[i]
            if t.k == 'LP':
                depth += 1; args[-1].append(t)
            elif t.k == 'RP':
                depth -= 1
                if depth == 0: return args, i, None
                args[-1].append(t)
            elif t.k == 'COMMA' and depth == 1:
                args.append([])
            elif t.k == 'SP':
                if args[-1] and args[-1][-1].k != 'SP':
                    args[-1].append(t)
            else:
                args[-1].append(t)
            i += 1
        return [], i, 'unterminated argument list'

    # ── substitute arguments into body ────────────────────────────────────────

    def _subst(self, md: MDef, raw: dict, exp: dict):
        """Substitute raw/expanded args into macro body. Handles # and ##."""
        body = md.body
        out: list = []
        pastes: list = []
        i = 0
        while i < len(body):
            t = body[i]

            # ── stringification: #param ──────────────────────────────────────
            if t.k == 'STR':
                i += 1
                while i < len(body) and body[i].k == 'SP': i += 1
                if i < len(body) and body[i].k == 'ID' and body[i].v in raw:
                    s = toks2str(raw[body[i].v])
                    s = s.replace('\\', '\\\\').replace('"', '\\"')
                    out.append(Tok('STRLIT', f'"{s}"'))
                    i += 1
                else:
                    out.append(t)   # bare #, leave as-is
                continue

            # ── token pasting: left ## right (## chains supported) ───────────
            j = i + 1
            while j < len(body) and body[j].k == 'SP': j += 1
            if j < len(body) and body[j].k == 'PASTE':
                # get left operand string
                if t.k == 'ID' and t.v in raw:
                    left = toks2str(raw[t.v])
                else:
                    left = t.v

                curr = j  # index of ##
                while curr < len(body) and body[curr].k == 'PASTE':
                    k = curr + 1
                    while k < len(body) and body[k].k == 'SP': k += 1
                    if k >= len(body):
                        pastes.append((left, '', left)); i = k; break
                    rt = body[k]
                    if rt.k == 'ID' and rt.v in raw:
                        right = toks2str(raw[rt.v])
                    else:
                        right = rt.v
                    pasted = left + right
                    pastes.append((left, right, pasted))
                    left = pasted
                    i = k + 1
                    curr = i
                    while curr < len(body) and body[curr].k == 'SP': curr += 1

                if left:
                    out.extend(tokenize(left))
                continue

            # ── regular substitution ──────────────────────────────────────────
            if t.k == 'ID' and t.v in exp:
                ext = exp[t.v]
                if ext:
                    # avoid double-spaces
                    if out and out[-1].k == 'SP' and ext and ext[0].k == 'SP':
                        out.extend(ext[1:])
                    else:
                        out.extend(ext)
            elif t.k == 'SP':
                if out and out[-1].k != 'SP':
                    out.append(t)
            else:
                out.append(t)
            i += 1

        return out, pastes

# ── Display ───────────────────────────────────────────────────────────────────

SEP = '─' * 64

def _short_path(p: str) -> str:
    try: return str(Path(p).relative_to(Path.cwd()))
    except ValueError: return p

def print_step(s, total):
    if isinstance(s, StepErr):
        print(f"{c(f'Step {s.num}/{total}', BOLD)} {c('ERROR', RED, BOLD)}: "
              f"{c(s.name, MAG, BOLD)}")
        print(f"  {c('Call:', DIM)}   {c(s.call, YLW)}")
        print(f"  {c('Error:', RED)}  {s.msg}")
        print()
        return
    print(f"{c(f'Step {s.num}/{total}', BOLD)}: "
          f"expanding {c(s.name, CYN, BOLD)}")
    print(f"  {c('Call:', DIM)}   {c(s.call, YLW)}")
    print(f"  {c('Def:', DIM)}    {c(str(s.mdef), BLU)}")
    if s.mdef.file:
        loc = _short_path(s.mdef.file) + ':' + str(s.mdef.line)
        print(f"  {c('File:', DIM)}   {c(loc, DIM)}")
    if s.args:
        print(f"  {c('Args:', DIM)}")
        for p, v in s.args.items():
            vs = toks2str(v) if v else c('(empty)', DIM)
            print(f"    {c(p, YLW)} = {vs}")
    if s.pastes:
        print(f"  {c('Paste:', DIM)}")
        for l, r, res in s.pastes:
            ls = c(l, YLW) if l else c('∅', DIM)
            rs = c(r, YLW) if r else c('∅', DIM)
            print(f"    {ls} ## {rs}  →  {c(res, GRN)}")
    res_str = toks2str(s.result) if s.result else c('(empty)', DIM)
    print(f"  {c('→', GRN, BOLD)}      {c(res_str, GRN)}")
    print()

# ── Interactive expansion session ─────────────────────────────────────────────

def run_expansion(expr: str, db: MacroDB, max_steps=500, auto=False):
    print(f"\n{c('Expanding:', BOLD)} {c(expr, YLW)}\n")
    exp = Expander(db, max_steps)
    final = exp.run(expr)
    steps = exp.steps

    if not steps:
        print(f"  {c('(no macro expansions — expression unchanged)', DIM)}")
    else:
        print(f"{c(str(len(steps)) + ' expansion step(s)', DIM)}  "
              f"{c('Enter=next  a=all  s=skip  q=quit', DIM)}\n")
        i = 0
        while i < len(steps):
            print_step(steps[i], len(steps))
            if auto:
                i += 1
                continue
            try:
                cmd = input(c('  [↵/a/s/q] > ', DIM)).strip().lower()
            except (EOFError, KeyboardInterrupt):
                print(); return
            if cmd == 'q': return
            elif cmd == 'a': auto = True; i += 1
            elif cmd == 's': break
            else: i += 1

    final_str = toks2str(final) if final else c('(empty)', DIM)
    print(SEP)
    label = c('Result:', BOLD, GRN)
    print(f"{label} {c(final_str, GRN, BOLD)}")
    print(SEP)

# ── REPL commands ─────────────────────────────────────────────────────────────

def cmd_list(db: MacroDB, pat: str):
    names = sorted(db.defs.keys())
    if pat:
        try: names = [n for n in names if re.search(pat, n, re.I)]
        except re.error as e: print(c(f'Bad regex: {e}', RED)); return
    print(f"\n{c(str(len(names)) + ' macros' + (f' matching {pat!r}' if pat else ''), BOLD)}:")
    cap = 80 if pat else 60
    for i, name in enumerate(names):
        if i >= cap and not pat:
            print(c(f'  ... {len(names)-i} more (add a pattern to filter)', DIM))
            break
        md = db.defs[name]
        body_preview = toks2str(md.body)
        if len(body_preview) > 50: body_preview = body_preview[:47] + '…'
        print(f"  {c(md.signature(), CYN)}  {c(body_preview, DIM)}")

def cmd_def(db: MacroDB, name: str):
    if name not in db.defs:
        # try partial match
        matches = [k for k in db.defs if name.lower() in k.lower()]
        if not matches:
            print(c(f"\n'{name}' not found.", RED)); return
        if len(matches) == 1:
            name = matches[0]
        else:
            print(f"\n{c('Multiple matches:', YLW)}")
            for m in matches[:20]: print(f"  {m}")
            if len(matches) > 20: print(c(f'  ... {len(matches)-20} more', DIM))
            return
    md = db.defs[name]
    print(f"\n{c(str(md), BLU)}")
    if md.func_like:
        print(f"  params:   {md.params}")
        print(f"  variadic: {md.variadic}")
    if md.file:
        print(f"  {c(_short_path(md.file) + ':' + str(md.line), DIM)}")

HELP_TEXT = """
COMMANDS
  list [pattern]   list macros (optional regex filter)
  def NAME         show macro definition (supports partial name)
  help             show this help
  q / quit         exit

DURING EXPANSION
  Enter            next step
  a                show all remaining steps without pausing
  s                skip to final result
  q                abort expansion

EXAMPLES
  NX_INC(3)
  NX_AND(1, 0)
  NX_CONCAT(foo_, bar)
  NX_ARGS_COUNT(a, b, c)
  _nx_bool(5)
  NX_WHILE_D(0)(_nx_bool, NX_INC, NX_DEC, 3)
"""

# ── REPL ──────────────────────────────────────────────────────────────────────

def repl(db: MacroDB, max_steps: int):
    try:
        import readline
        hist = Path.home() / '.nx_macro_debug_history'
        try: readline.read_history_file(str(hist))
        except: pass
        import atexit
        atexit.register(lambda: readline.write_history_file(str(hist)))
    except ImportError:
        pass

    print('═' * 64)
    print(c('  nx::macro expansion debugger', CYN, BOLD))
    print('═' * 64)
    print(f"  {c(str(len(db.defs)), GRN)} macro definitions loaded")
    if db.warnings:
        print(f"  {c(str(len(db.warnings)) + ' load warning(s):', YLW)}")
        for w in db.warnings[:5]: print(f"    {c(w, YLW)}")
    print(f"\n  Type a C macro expression to expand, or {c('help', CYN)}.\n")

    while True:
        try:
            line = input(c('nx> ', CYN, BOLD)).strip()
        except (EOFError, KeyboardInterrupt):
            print(); break
        if not line: continue
        if line in ('q', 'quit', 'exit'): break
        if line in ('help', '?'): print(HELP_TEXT); continue
        if line.startswith('list'):
            cmd_list(db, line[4:].strip()); continue
        if line.startswith('def '):
            cmd_def(db, line[4:].strip()); continue
        run_expansion(line, db, max_steps)

# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    p = argparse.ArgumentParser(
        description='Interactive C preprocessor macro expansion debugger',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__)
    p.add_argument('-I', dest='incdirs', action='append', default=[], metavar='DIR',
                   help='add include directory (can repeat)')
    p.add_argument('-f', dest='files', action='append', default=[], metavar='FILE',
                   help='load macros from file, follows #include (can repeat)')
    p.add_argument('-D', dest='defines', action='append', default=[], metavar='NAME[=val]',
                   help='define a macro (like compiler -D)')
    p.add_argument('--scan', dest='scandirs', action='append', default=[], metavar='DIR',
                   help='recursively load ALL .hpp/.h files from DIR (can repeat)')
    p.add_argument('--nx', action='store_true',
                   help='shortcut: load all nx::macro headers from -I dirs automatically')
    p.add_argument('--max-steps', type=int, default=500,
                   help='max expansion steps per expression (default: 500)')
    p.add_argument('--all', action='store_true',
                   help='print all steps without pausing')
    p.add_argument('--list', metavar='PATTERN', nargs='?', const='',
                   help='list loaded macros (optional filter) and exit')
    p.add_argument('--no-color', action='store_true', help='disable colors')
    p.add_argument('expression', nargs='?',
                   help='expression to expand (omit for interactive REPL)')
    args = p.parse_args()

    global USE_COLOR
    if args.no_color: USE_COLOR = False

    db = MacroDB()
    for d in args.incdirs: db.add_inc(d)

    print(c('Loading...', DIM), end=' ', flush=True)

    for f in args.files:
        db.load(f)

    # --scan: load every header under the given directory
    for scan_dir in args.scandirs:
        for hp in sorted(list(Path(scan_dir).rglob('*.hpp')) + list(Path(scan_dir).rglob('*.h'))):
            db.load(str(hp))

    # --nx: auto-discover nx::macro headers from the first -I dir that has nx/macro/
    if args.nx:
        found = False
        for inc_dir in ([Path(d) for d in args.incdirs] or [Path('.')]):
            nx_macro = inc_dir / 'nx' / 'macro'
            if nx_macro.is_dir():
                for hp in sorted(nx_macro.rglob('*.hpp')):
                    db.load(str(hp))
                # also load the top-level nx/macro.hpp if present
                top = inc_dir / 'nx' / 'macro.hpp'
                if top.exists(): db.load(str(top))
                found = True
                break
        if not found:
            print(c('\n  --nx: could not find nx/macro/ under any -I dir', YLW))

    total = len(db.defs)
    if total:
        print(c(f'{total} macros loaded.', GRN))
    elif not args.files and not args.scandirs and not args.nx:
        print(c('0 macros (no -f/--scan/--nx specified).', YLW))
    else:
        print(c('0 macros loaded.', YLW))

    for w in db.warnings:
        print(c(f'  warning: {w}', YLW))

    for d in args.defines:
        db.define(d)

    if args.list is not None:
        cmd_list(db, args.list)
        return

    if args.expression:
        run_expansion(args.expression, db, args.max_steps, auto=args.all)
    else:
        repl(db, args.max_steps)

if __name__ == '__main__':
    main()
