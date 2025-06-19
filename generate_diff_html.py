#!/usr/bin/env python3
import sys
import subprocess
import html
from pathlib import Path

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <IR-file.ll> <opt-flag (e.g. mem2reg or -O2)>")
        sys.exit(1)

    ir_file, opt_flag = sys.argv[1], sys.argv[2]
    tool = Path("bin/optviz")

    # 1) Run optviz and capture its diff from stderr
    proc = subprocess.run(
        [str(tool), ir_file, f"-opt={opt_flag}"],
        capture_output=True,
        text=True
    )
    diff_lines = proc.stderr.splitlines()

    # 2) Separate removed (“- ”) and added (“+ ”) instructions
    removed, added = [], []
    for line in diff_lines:
        if line.startswith("- "):
            removed.append(line[2:])
        elif line.startswith("+ "):
            added.append(line[2:])

    # 3) Build HTML table rows
    rows = []
    for i in range(max(len(removed), len(added))):
        rem = html.escape(removed[i]) if i < len(removed) else ""
        ad  = html.escape(added[i])   if i < len(added)   else ""
        rows.append(f"<tr><td class='removed'>{rem}</td><td class='added'>{ad}</td></tr>")

    # 4) Emit complete HTML document
    html_doc = f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>IR Diff: {ir_file} [{opt_flag}]</title>
  <style>
    body {{ font-family: monospace; padding: 1rem; }}
    table {{ width: 100%; border-collapse: collapse; }}
    th, td {{ border: 1px solid #ccc; padding: 4px; vertical-align: top; }}
    .removed {{ background: #fee; color: #900; }}
    .added   {{ background: #efe; color: #060; }}
    th {{ background: #f8f8f8; }}
  </style>
</head>
<body>
  <h2>IR Diff for <code>{ir_file}</code> (<code>{opt_flag}</code>)</h2>
  <table>
    <tr><th>Removed</th><th>Added</th></tr>
    {''.join(rows)}
  </table>
</body>
</html>
"""

    out_path = Path("diff.html")
    out_path.write_text(html_doc)
    print(f"HTML diff written to {out_path.resolve()}")

if __name__ == "__main__":
    main()