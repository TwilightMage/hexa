import sys

if len(sys.argv) != 2:
    sys.exit(1)
    
path = sys.argv[1]

with open(path, "w") as mod_meta:
    mod_meta.write("Example Mod\n1.0.0\n0.1.0")