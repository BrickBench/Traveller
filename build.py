#!/usr/bin/env python3
from sys import platform
import os
import shutil

if platform == "linux":
    os.system('cmake -B ./Build -S . -DCMAKE_TOOLCHAIN_FILE=MinGW.cmake')
    os.system('cmake --build ./Build')
    shutil.copyfile("./Out/dinput8.dll", "/home/javst/Documents/LEGO/tcs_clean/dinput8.dll")

elif platform == "win32":
    os.system('cmake -B ./Build -S .')
