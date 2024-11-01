#!/bin/bash
# Run the simulator for Test 1
# **PLEASE NOTE: The first time you run this sh file it will give a permission denied message
chmod +x test.sh
g++ -o interrupts interrupts.cpp
./interrupts trace.txt external_files.txt
