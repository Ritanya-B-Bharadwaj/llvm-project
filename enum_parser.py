import re

def parse_enum_block(enum_name, body):
    symbols = []
    current_value = None
    for line in body.split(","):
        line = line.strip()
        if not line or line.startswith("//"):
            continue
        match = re.match(r'(\w+)(\s*=\s*[^,]+)?', line)
        if match:
            symbol = match.group(1)
            symbols.append(symbol)
    return enum_name, symbols

def generate_symbol_map(enums):
    output = []
    output.append("// Auto-generated file. Do not modify directly.")
    output.append("#pragma once\n")
    for enum_name, symbols in enums:
        # Forward map
        output.append(f'static const char* {enum_name}ToString[] = {{')
        for sym in symbols:
            output.append(f'    "{sym}",')
        output.append("};\n")

        # Reverse map using std::map
        output.append(f'#include <map>')
        output.append(f'static std::map<std::string, {enum_name}> StringTo{enum_name} = {{')
        for index, sym in enumerate(symbols):
            output.append(f'    {{"{sym}", {enum_name}::{sym}}},')
        output.append("};\n")
    return "\n".join(output)


def main():
    with open("enums.hpp", "r") as f:
        content = f.read()

    pattern = re.compile(r'enum\s+(\w+)\s*\{([^}]*)\}', re.MULTILINE | re.DOTALL)
    enums = []
    for match in pattern.finditer(content):
        enum_name = match.group(1)
        body = match.group(2)
        enums.append(parse_enum_block(enum_name, body))

    output = generate_symbol_map(enums)

    with open("generated_maps.hpp", "w") as f:
        f.write(output)

if __name__ == "__main__":
    main()
