#!/bin/bash
# Run the simulator for Test 1
# **PLEASE NOTE: The first time you run this sh file it will give a permission denied message
chmod +x test1.sh
g++ -o interrupts interrupts.cpp
./interrupts trace1.txt external_files1.txt
