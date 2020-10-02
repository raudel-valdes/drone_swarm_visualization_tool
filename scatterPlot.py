import plotly.io as pio
import plotly.express as px
import pandas as pd


filePath= './droneData_xAxis_newFormat.txt'

xAxis = []
yAxis = []
zAxis = []
batteryLife = []
droneLocation = []
droneId = []

with open(filePath) as openFile:
   line = openFile.readline()
   while line:
      splitString = line.split('|', 5)

      xAxis.append(splitString[0])
      yAxis.append(splitString[1])
      zAxis.append(splitString[2])
      batteryLife.append(splitString[3])
      droneLocation.append(splitString[4])
      droneId.append(splitString[5].rstrip('\n'))
       
      line = openFile.readline()


dataForDataFrame = {
  'x_axis':  xAxis,
  'y_axis': yAxis,
  'z_axis': zAxis,
  'battery_life': batteryLife,
  'drone_location': droneLocation,
  'drone_id': droneId
}

dataFrame = pd.DataFrame (dataForDataFrame, columns = [  
  'x_axis',
  'y_axis',
  'z_axis',
  'battery_life',
  'drone_location',
  'drone_id'
])

fig = px.scatter_3d(dataFrame, x='x_axis', y='y_axis', z='z_axis', color='drone_id')
fig.update_layout(title_text='Drone Data Visualization')
pio.write_html(fig, file='./drone_simulation.html', auto_open=True)