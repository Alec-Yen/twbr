import time
from roboclaw import Roboclaw

rc = Roboclaw("/dev/serial0",9600)

rc.Open()

while 1:
	#Get version string
	version = rc.ReadVersion(0x80)
	if version[0]==False:
		print "GETVERSION Failed"
	else:
		print repr(version[1])
	time.sleep(1)
