import sys
sys.path.append('../my-build-tools/')

from builder import builder
tools = builder.tools

# get inputs
flag = "#include"
inputs = []
with open("./test.c", 'r') as file:
    lines = file.readlines()
for line in lines:
    if line.startswith("//") or "files" not in line:
        continue
    elif flag in line:
        line = line.replace(flag, "")
        line = line.replace('"', "")
        line = line.strip()
        inputs.append(line)

# on macro
def _onMacro():
    def onMacro(code, command, argument = None):
        return code
    return onMacro

# on line
def _onLine():
    def onLine(line):
        return line
    return onLine

# languages
bldr = builder.code()
bldr.setInput(inputs)
bldr.setComment("//")
bldr.setOutput("tools.h")
bldr.onMacro(_onMacro())
bldr.onLine(_onLine())
bldr.start()
