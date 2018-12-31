import os
import sys
from datetime import datetime

run = True
seedfile = open("goodseeds", "r")
video_n = 0

print "Started rendering at ", datetime.now().time()

for line in seedfile:
	framesPath = "data/frames" + str(video_n) + "/"
	
	os.system("mkdir -v " + framesPath )
	
	seed = int(line)
	os.system("./bicellular " + str(seed) + " " + framesPath)
	print "rendering ", video_n, " finished at ", datetime.now().time()
	video_n = video_n + 1

