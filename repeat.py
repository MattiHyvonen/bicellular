import os
import sys

run = True
i = int(sys.argv[1])
seedfile = open("goodseeds", "ap")

while run:
	os.system("./bicellular " + str(i) )
	userInput = raw_input("continue? (Y|n|s) ")
	if userInput == "n":
		run = False
	if userInput == "s":
		seedfile.write(str(i)+"\n")
	i = i + 1