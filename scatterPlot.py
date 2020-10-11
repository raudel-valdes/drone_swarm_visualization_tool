import plotly.io as pio
import plotly.express as px
import plotly.graph_objects as go
import pandas as pd

fig_dict = {
    "data": [],
    "layout": {},
    "frames": []
}

fig_dict["layout"]["xaxis"] = {'range': [0, 85], 'title': {'text': 'Life Expectancy'}}
fig_dict["layout"]["yaxis"] = {'title': {'text': 'GDP per Capita'}, 'type': 'log'}
#fig_dict["layout"]["zaxis"] = {"range": [30, 85], "title": "Life Expectancy"}
fig_dict["layout"]["hovermode"] = "closest"
fig_dict["layout"]["updatemenus"] = [
    {
        "buttons": [
            {
                "args": [None, {"frame": {"duration": 500, "redraw": False},
                                "fromcurrent": True, "transition": {"duration": 300,
                                                                    "easing": "quadratic-in-out"}}],
                "label": "Play",
                "method": "animate"
            },
            {
                "args": [[None], {"frame": {"duration": 0, "redraw": False},
                                  "mode": "immediate",
                                  "transition": {"duration": 0}}],
                "label": "Pause",
                "method": "animate"
            }
        ],
        "direction": "left",
        "pad": {"r": 10, "t": 87},
        "showactive": False,
        "type": "buttons",
        "x": 0.1,
        "xanchor": "right",
        "y": 0,
        "yanchor": "top"
    }
]

sliders_dict = {
    "active": 0,
    "yanchor": "top",
    "xanchor": "left",
    "currentvalue": {
        "font": {"size": 20},
        "prefix": "year",
        "visible": True,
        "xanchor": "right"
    },
    "transition": {"duration": 300, "easing": "cubic-in-out"},
    "pad": {"b": 10, "t": 50},
    "len": 0.9,
    "x": 0.1,
    "y": 0,
    "steps": []
}

# mock data location
filePath= './droneData_xAxis_newFormat.txt'

# Initializing variables for drone details
xAxis = []
yAxis = []
zAxis = []
batteryLife = []
droneLocation = []
droneId = []
numberOfDrones = 0

last_time = -1
with open(filePath) as openFile:

	line = openFile.readline()
	splitString = line.split('|', 6)

	numberOfDrones = splitString[1]
	frameCounter = 0

	# Every Formation of Drones
	while line:
		frameCounter += 1

		frame = {"data": [], "name": "Frame: " + str(frameCounter)}


		for i in range(0, int(numberOfDrones)):
			line = openFile.readline()

			if line:
				splitString = line.split('|', 6)

				# data_dict["x"].append(float(splitString[0]))
				# data_dict["y"].append(float(splitString[1]))
				# data_dict["z"].append(float(splitString[2]))

				data_dict = {
						"x": list([float(splitString[0])]),
						"y": list([float(splitString[1])]),
						"mode": "markers",
						"text": list(["cuba"]),
						"marker": {
								"sizemode": "area",
								"sizeref": 200000,
								"size": list([200]),
						},
						"name": "test",
				}
				frame["data"].append(data_dict)
			else: 
				break
		if line:
			fig_dict["frames"].append(frame)

			slider_step = {
				"args": 
				[
					["test"],
					{
						"frame": {"duration": 300, "redraw": False},
						"mode": "immediate",
						"transition": {"duration": 300}
					}
				],
				"label": "test",
				"method": "animate"
			}

			slider_step["label"] = "Step: " + str(frameCounter)
			sliders_dict["steps"].append(slider_step)

		# Reinitializing the array and saving the last element which belongs to the new upcoming frame
		xAxis = []
		yAxis = []
		zAxis = []

fig_dict["layout"]["sliders"] = [sliders_dict]

fig_dict["data"]=fig_dict["frames"][0]["data"]
fig = go.Figure({
    'data': [{'marker': {'size': [8425333], 'sizemode': 'area', 'sizeref': 200000},
              'mode': 'markers',
              'name': 'Asia',
              'text': [Afghanistan],
              'type': 'scatter',
              'x': [28.801],
              'y': [779.4453145]},
             {'marker': {'size': [1282697], 'sizemode': 'area', 'sizeref': 200000},
              'mode': 'markers',
              'name': 'Europe',
              'text': [Albania],
              'type': 'scatter',
              'x': [55.23],
              'y': [1601.056136]},
             {'marker': {'size': [9279525], 'sizemode': 'area', 'sizeref': 200000},
              'mode': 'markers',
              'name': 'Africa',
              'text': [Algeria],
              'type': 'scatter',
              'x': [43.077],
              'y': [2449.008185]}],
    'frames': [{'data': [{'marker': {'size': [8425333], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [28.801],
                          'y': [779.4453145]},
                         {'marker': {'size': [1282697], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [55.23],
                          'y': [1601.056136]},
                         {'marker': {'size': [9279525], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [Algeria],
                          'type': 'scatter',
                          'x': [43.077],
                          'y': [2449.008185]}],
                'name': '1952'},
               {'data': [{'marker': {'size': [10267083], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [31.997],
                          'y': [853.1007099999999]},
                         {'marker': {'size': [1728137], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [64.82],
                          'y': [2312.888958]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1962'},
               {'data': [{'marker': {'size': [11537966], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [34.02],
                          'y': [836.1971382]},
                         {'marker': {'size': [1984060], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [66.22],
                          'y': [2760.196931]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1967'},
               {'data': [{'marker': {'size': [13079460], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [36.088],
                          'y': [739.9811057999999]},
                         {'marker': {'size': [2263554], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [67.69],
                          'y': [3313.422188]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1972'},
               {'data': [{'marker': {'size': [14880372], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [38.438],
                          'y': [786.11336]},
                         {'marker': {'size': [2509048], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [68.93],
                          'y': [3533.0039100000004]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1977'},
               {'data': [{'marker': {'size': [12881816], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [39.854],
                          'y': [978.0114388000001]},
                         {'marker': {'size': [2780097], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [70.42],
                          'y': [3630.880722]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1982'},
               {'data': [{'marker': {'size': [13867957], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [40.821999999999996],
                          'y': [852.3959447999999]},
                         {'marker': {'size': [3075321], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [72.0],
                          'y': [3738.9327350000003]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1987'},
               {'data': [{'marker': {'size': [16317921], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [41.674],
                          'y': [649.3413952000001]},
                         {'marker': {'size': [3326498], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [71.581],
                          'y': [2497.4379010000002]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1992'},
               {'data': [{'marker': {'size': [22227415], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [41.763000000000005],
                          'y': [635.341351]},
                         {'marker': {'size': [3428038], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [72.95],
                          'y': [3193.054604]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '1997'},
               {'data': [{'marker': {'size': [25268405], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [42.129],
                          'y': [726.7340548]},
                         {'marker': {'size': [3508512], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [75.65100000000001],
                          'y': [4604.211737]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '2002'},
               {'data': [{'marker': {'size': [31889923], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Asia',
                          'text': [Afghanistan],
                          'type': 'scatter',
                          'x': [43.828],
                          'y': [974.5803384]},
                         {'marker': {'size': [3600523], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Europe',
                          'text': [Albania],
                          'type': 'scatter',
                          'x': [76.423],
                          'y': [5937.029525999999]},
                         {'marker': {'size': [], 'sizemode': 'area', 'sizeref': 200000},
                          'mode': 'markers',
                          'name': 'Africa',
                          'text': [],
                          'type': 'scatter',
                          'x': [],
                          'y': []}],
                'name': '2007'}],
    'layout': {'hovermode': 'closest',
               'sliders': [{'active': 0,
                            'currentvalue': {'font': {'size': 20}, 'prefix': 'Year:', 'visible': True, 'xanchor': 'right'},
                            'len': 0.9,
                            'pad': {'b': 10, 't': 50},
                            'steps': [{'args': [['1952'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1952',
                                       'method': 'animate'},
                                      {'args': [['1962'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1962',
                                       'method': 'animate'},
                                      {'args': [['1967'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1967',
                                       'method': 'animate'},
                                      {'args': [['1972'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1972',
                                       'method': 'animate'},
                                      {'args': [['1977'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1977',
                                       'method': 'animate'},
                                      {'args': [['1982'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1982',
                                       'method': 'animate'},
                                      {'args': [['1987'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1987',
                                       'method': 'animate'},
                                      {'args': [['1992'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1992',
                                       'method': 'animate'},
                                      {'args': [['1997'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '1997',
                                       'method': 'animate'},
                                      {'args': [['2002'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '2002',
                                       'method': 'animate'},
                                      {'args': [['2007'], {'frame': {'duration':
                                                300, 'redraw': False}, 'mode':
                                                'immediate', 'transition':
                                                {'duration': 300}}],
                                       'label': '2007',
                                       'method': 'animate'}],
                            'transition': {'duration': 300, 'easing': 'cubic-in-out'},
                            'x': 0.1,
                            'xanchor': 'left',
                            'y': 0,
                            'yanchor': 'top'}],
               'template': '...',
               'updatemenus': [{'buttons': [{'args': [None, {'frame': {'duration':
                                                      500, 'redraw': False},
                                                      'fromcurrent': True,
                                                      'transition': {'duration':
                                                      300, 'easing': 'quadratic-in-
                                                      out'}}],
                                             'label': 'Play',
                                             'method': 'animate'},
                                            {'args': [[None], {'frame':
                                                      {'duration': 0, 'redraw':
                                                      False}, 'mode': 'immediate',
                                                      'transition': {'duration':
                                                      0}}],
                                             'label': 'Pause',
                                             'method': 'animate'}],
                                'direction': 'left',
                                'pad': {'r': 10, 't': 87},
                                'showactive': False,
                                'type': 'buttons',
                                'x': 0.1,
                                'xanchor': 'right',
                                'y': 0,
                                'yanchor': 'top'}],
               'xaxis': {'range': [30, 85], 'title': {'text': 'Life Expectancy'}},
               'yaxis': {'title': {'text': 'GDP per Capita'}, 'type': 'log'}}
})

fig.show()
print(fig)

# Outputs the html file to the specified file
#pio.write_html(fig, file='./drone_simulation.html', auto_open=True)
