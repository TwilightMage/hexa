import os
import shutil

sources = ["Engine", "HexaGame"]
dir_path = os.path.dirname(os.path.realpath(__file__))

for name in os.listdir("./Build"):
    out_dir = dir_path + "/Build/" + name;
    if os.path.isdir(out_dir):
        full_out_dir = out_dir + "/resources"
        if os.path.exists(full_out_dir):
            shutil.rmtree(full_out_dir)
        for source in sources:
            os.system(" ".join(["python.exe", '"' + dir_path + "/move_resources.py" + '"', '"' + dir_path + '"', source, '"' + full_out_dir + '"']))
