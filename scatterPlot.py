import plotly.io as pio
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd

fig = go.Figure()

# mock data location
filePath= './droneData_xAxis_newFormat.txt'

# Initializing variables for drone details
xAxis = []
yAxis = []
zAxis = []
batteryLife = []
droneLocation = []
droneId = []

last_time = -1
with open(filePath) as openFile:
	line = openFile.readline()
	while line:
		if line == "END\n":
			last_time = False
			xAxis.append("END")
			yAxis.append("END")
			zAxis.append("END")
		else:
			splitString = line.split('|', 6)

			xAxis.append(splitString[0])
			yAxis.append(splitString[1])
			zAxis.append(splitString[2])
			batteryLife.append(splitString[3])
			droneLocation.append(splitString[4])
			droneId.append(splitString[5])
			current_time = splitString[6].rstrip('\n')

		if last_time == -1:
			last_time = current_time
		
		# save trace and move on to next time	
		if last_time != current_time:	
			last_time = current_time

			# Adding a new frame to the scatterplot slider
			fig.add_trace(go.Scatter3d(x=xAxis[:-1], y=yAxis[:-1], z=zAxis[:-1]))

			# Makes sure to only allow a previous frames to be seen rather than all future ones
			fig.data[-1].visible = False

			# Reinitializing the array and saving the last element which belongs to the new upcoming frame
			xAxis = [xAxis[-1]]
			yAxis = [yAxis[-1]]
			zAxis = [zAxis[-1]]
		
		line = openFile.readline()

print(fig.data)

# makes first trace frame visible
fig.data[0].visible = True

# Controls the Logic for the slider
steps = []
for i in range(0, len(fig.data)):

	# A dictionary that decides which frames are visble by having an array called visible with a boolean
	# index for each fram. True shows the frame, False hides the frame
	step = dict(
		method="update",
		args=[{"visible":[False] * len(fig.data)},
					{"title":"Frame" + str(i)}]
	)

	# This is done in order to see previous frames in the current frame
	for j in range(0, i + 1):
		step["args"][0]["visible"][j] = True

	# One step for every frame. Every frame has total number of frames in visible list 
	steps.append(step)

# Displays the slider bar in the html
# active is the current step
# pad is for spacing in the slider
# steps is all of the existing frames you can slide one
sliders = [dict(
    active = 0,
    currentvalue = { "prefix": "Time: "},
    pad = {"t": 50},
    steps=steps
)]

# Updates the layout of the graph
fig.update_layout(
	title_text='Drone Data Visualization', 
	sliders=sliders,
	scene = dict(
		xaxis = dict(nticks=5, range=[-30,30]),
    yaxis = dict(nticks=5, range=[-30,30]),
    zaxis = dict(nticks=5, range=[0,30])
	)
)

# Outputs the html file to the specified file
pio.write_html(fig, file='./drone_simulation.html', auto_open=True)
