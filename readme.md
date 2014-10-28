#10/24/14
-Note that upon entering the asteroid field, the old thread for launching is still blocking on accept() and gets the next connect(),
	so I need to handle that somehow, because a probes data could be lost.
	-Maybe move all TFC to one thread and set a boolean for launching probes or not.