from os import listdir
import subprocess

for f in listdir("board_parser/"):
    filename = "board_parser/"+f
    print(f)
    result = subprocess.run([".././reversi", filename])#,
                            #stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if (("pass" in f and result.returncode == 0) or ("fail" in f and result.returncode != 0)):
        pass
    else:
        print("error on file "+f)
        exit(-1)
print("sucess !!")
