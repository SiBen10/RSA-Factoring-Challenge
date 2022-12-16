#!/usr/bin/python3
import sys

with open(sys.argv[1], 'r') as f:
        x = f.readline()
        while (x):
                i = 2
                while (int(x) % i != 0):
                        i += 1
                msg = x+"="+ str(int(x) // i) + "*" + str(i) if i != int(x) else x+"="+ x + "*" + "1"
                print(msg)
                x = f.readline()
