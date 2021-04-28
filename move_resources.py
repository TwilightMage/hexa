import sys
import os
import shutil

if len(sys.argv) != 4:
    sys.exit(1)

project_dir = sys.argv[1]
category = sys.argv[2]
out_dir = sys.argv[3]

full_in_dir = project_dir + "/" + category + "/resources/" + category.lower()
full_out_dir = out_dir + "/resources/" + category.lower()

if os.path.exists(full_out_dir):
    shutil.rmtree(full_out_dir)
shutil.copytree(full_in_dir, full_out_dir, False, None)