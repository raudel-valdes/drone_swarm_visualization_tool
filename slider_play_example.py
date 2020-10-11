import plotly.graph_objects as go

import pandas as pd

dataset = pd.read_csv('./droneData_xAxis_newFormat.txt')
print(dataset)
frames = ["0", "1", "2", "3", "4"]

# make list of drone sets
droneIds = []
for droneId in dataset["droneId"]:
    if droneId not in droneIds:
        droneIds.append(droneId)
# make figure
fig_dict = {
    "data": [],
    "layout": {},
    "frames": []
}

# fill in most of layout
fig_dict["layout"]["xaxis"] = {"range": [0, 85], "title": "Life Expectancy"}
fig_dict["layout"]["yaxis"] = {"title": "GDP per Capita", "type": "log"}
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
        "prefix": "Year:",
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

# make data
frame = 0
for droneId in droneIds:
    dataset_by_frame = dataset[dataset["frame"] == frame]
    dataset_by_frame_and_droneId = dataset_by_frame[
        dataset_by_frame["droneId"] == droneId]

    data_dict = {
        "x": list(dataset_by_frame_and_droneId["xAxis"]),
        "y": list(dataset_by_frame_and_droneId["yAxis"]),
        "mode": "markers",
        "text": list(dataset_by_frame_and_droneId["droneId"]),
        "marker": {
            "sizemode": "area",
            "sizeref": 200000,
        },
        "name": droneId
    }
    fig_dict["data"].append(data_dict)

# make frames
for frame in frames:
    frameTemp = {"data": [], "name": str(frame)}
    for droneId in droneIds:
        dataset_by_frame = dataset[dataset["frame"] == int(frame)]
        dataset_by_frame_and_droneId = dataset_by_frame[
            dataset_by_frame["droneId"] == droneId]

        data_dict = {
            "x": list(dataset_by_frame_and_droneId["xAxis"]),
            "y": list(dataset_by_frame_and_droneId["yAxis"]),
            "mode": "markers",
            "text": list(dataset_by_frame_and_droneId["droneId"]),
            "marker": {
                "sizemode": "area",
                "sizeref": 200000,
            },
            "name": droneId
        }
        frameTemp["data"].append(data_dict)

    fig_dict["frames"].append(frameTemp)
    slider_step = {"args": [
        [frame],
        {"frame": {"duration": 300, "redraw": False},
         "mode": "immediate",
         "transition": {"duration": 300}}
    ],
        "label": frame,
        "method": "animate"}
    sliders_dict["steps"].append(slider_step)


fig_dict["layout"]["sliders"] = [sliders_dict]

fig = go.Figure(fig_dict)

fig.show()