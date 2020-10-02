import plotly.io as pio
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd

fig = go.Figure()

filePath= './droneData_xAxis_newFormat.txt'

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
			'''dataForDataFrame = {
				'x_axis':  xAxis,
				'y_axis': yAxis,
				'z_axis': zAxis,
				'battery_life': batteryLife,
				'drone_location': droneLocation,
				'drone_id': droneId
			}'''

			'''dataFrame = pd.DataFrame (dataForDataFrame, columns = [  
				'x_axis',
				'y_axis',
				'z_axis',
				'battery_life',
				'drone_location',
				'drone_id'
			])'''
			#fig.add_trace(px.scatter_3d(dataFrame, x='x_axis', y='y_axis', z='z_axis', color='drone_id')) 
			fig.add_trace(go.Scatter3d(x=xAxis[:-1], y=yAxis[:-1], z=zAxis[:-1]))
			fig.data[-1].visible = False
			xAxis = [xAxis[-1]]
			yAxis = [yAxis[-1]]
			zAxis = [zAxis[-1]]
				
		line = openFile.readline()

# makes first trace frame visible
print(fig.data)
fig.data[0].visible = True


steps = []
for i in range(0, len(fig.data)):
	step = dict(
		method="update",
		args=[{"visible":[False] * len(fig.data)},
					{"title":"Should have switched to frame" + str(i)}]
	)
	step["args"][0]["visible"][i] = True
	steps.append(step)
sliders = [dict(
    active=0,
    currentvalue={"prefix": "Time: "},
    pad={"t": 50},
    steps=steps
)]


fig.update_layout(
	title_text='Drone Data Visualization', 
	sliders=sliders,
	scene = dict(
		xaxis = dict(nticks=5, range=[-30,30]),
    yaxis = dict(nticks=5, range=[-30,30]),
    zaxis = dict(nticks=5, range=[0,30])
	)
)
pio.write_html(fig, file='./drone_simulation.html', auto_open=True)
