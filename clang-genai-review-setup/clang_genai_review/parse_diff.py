import re

def parse_patch_string(patch_text):
    """Parse a unified diff patch and extract modified lines per file."""
    files = []
    current_file = None
    current_lines = []

    for line in patch_text.splitlines():
        if line.startswith("diff --git"):
            if current_file:
                files.append({
                    "filename": current_file,
                    "lines": current_lines
                })
                current_file = None
                current_lines = []

        elif line.startswith("+++ b/"):
            current_file = line[6:]  # Remove '+++ b/'

        elif line.startswith("@@"):
            # Match lines like: @@ -159,3 +159,3 @@
            match = re.search(r"\+(\d+)(?:,(\d+))?", line)
            if match:
                start_line = int(match.group(1))
                line_count = int(match.group(2) or "1")
                current_lines.extend(range(start_line, start_line + line_count))

    if current_file:
        files.append({
            "filename": current_file,
            "lines": current_lines
        })

    return files


def extract_lines_from_patch(patch_text):
    """Extract added lines from a patch, per file."""
    parsed = parse_patch_string(patch_text)
    result = {}

    for diff in parsed:
        filename = diff["filename"]
        result[filename] = []

    collecting = False
    current_file = None

    for line in patch_text.splitlines():
        if line.startswith("diff --git"):
            collecting = False
        elif line.startswith("+++ b/"):
            current_file = line[6:]
            collecting = True
        elif collecting and line.startswith("+") and not line.startswith("+++"):
            if current_file and current_file in result:
                result[current_file].append(line[1:])  # remove '+'

    return result
