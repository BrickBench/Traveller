#!/usr/bin/env python3
from sys import platform
import os
import shutil

if platform == "linux":
    os.system('cmake -B ./Build -S . -DCMAKE_TOOLCHAIN_FILE=MinGW.cmake')
    os.system('cmake --build ./Build')
    shutil.copyfile("./Out/dinput8.dll", "/home/javst/Documents/LEGO/LSWTCS/PC Edited/DINPUT8.dll")
    shutil.copyfile("./Out/ExampleMod.dll", "/home/javst/Documents/LEGO/LSWTCS/PC Edited/plugins/ExampleMod.dll")
elif platform == "win32":
    os.system('cmake -B ./Build -S .')
