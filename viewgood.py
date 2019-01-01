import os
import sys
from datetime import datetime

run = True
seedfile = open("goodseeds", "r")
video_n = 0

for line in seedfile:
	seed = int(line)
	os.system("./bicellular " + str(seed))

