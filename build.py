#!/usr/bin/env python3
from sys import platform
import os
import shutil

if platform == "linux":
    os.system('cmake -B ./Build -S . -DCMAKE_TOOLCHAIN_FILE=MinGW.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1')
    shutil.copyfile("./Build/compile_commands.json", "./compile_commands.json")
    os.system('cmake --build ./Build')
    shutil.copyfile("./Out/DINPUT8.dll", "/var/home/javst/Documents/LEGO/TCS_Edited/DINPUT8.dll")
    shutil.copyfile("./Out/ExampleMod.dll", "/var/home/javst/Documents/LEGO/TCS_Edited/plugins/ExampleMod.dll")
    shutil.copyfile("./Out/CheatMenu.dll", "/var/home/javst/Documents/LEGO/TCS_Edited/plugins/CheatMenu.dll")
elif platform == "win32":
    os.system('cmake -B ./Build -S .')
