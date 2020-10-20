import time

t = 0
for i in range(0, 100):
	t += 10
	#time.sleep()

	
	f = open("updateData.txt", 'a')
	for id in [1, 2, 3]:
		f.write(str(t) + "|")
		f.write(str(id) + "|")
		f.write("form|")
		f.write("20|"*3)  
		f.write("20\n")  

	f.close()
			
