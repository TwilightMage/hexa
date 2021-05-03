import os

sources = ["Engine", "HexaGame"]
dir_path = os.path.dirname(os.path.realpath(__file__))

for source in sources:
    for name in os.listdir("./Build"):
        out_dir = dir_path + "/Build/" + name;
        if os.path.isdir(out_dir):
            os.system(" ".join(["python.exe", '"' + dir_path + "/move_resources.py" + '"', '"' + dir_path + '"', source, '"' + out_dir + '"']))
