import plotly.graph_objects as go
import pandas as pd
import plotly.io as pio


dataset = pd.read_csv('./drone_simulation_data.txt')

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
fig_dict["layout"]["xaxis"] = {"range": [0, 85], "title": "xAxis"}
fig_dict["layout"]["yaxis"] = {"title": "yAxis"}
fig_dict["layout"]["hovermode"] = "closest"
fig_dict["layout"]["updatemenus"] = [
    {
        "buttons": [
            {
                "args": [None, {"frame": {"duration": 500, "redraw": True},
                                "fromcurrent": True, "transition": {"duration": 300,
                                                                    "easing": "quadratic-in-out"}}],
                "label": "Play",
                "method": "animate"
            },
            {
                "args": [[None], {"frame": {"duration": 0, "redraw": True},
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
        "z": list(dataset_by_frame_and_droneId["zAxis"]),
        "mode": "markers",
        "text": list(dataset_by_frame_and_droneId["droneId"]),
        "marker": {
            "sizemode": "area",
            "sizeref": 200000,
        },
        "name": droneId,
        "type": "scatter3d"
    }
    fig_dict["data"].append(data_dict)

# make frames
counter = 0
for frame in frames:
    frameTemp = {"data": [], "name": str(frame)}
    for droneId in droneIds:
        dataset_by_frame = dataset[dataset["frame"] == int(frame)]
        dataset_by_frame_and_droneId = dataset_by_frame[
            dataset_by_frame["droneId"] == droneId]

        data_dict = {
            "x": list(dataset_by_frame_and_droneId["xAxis"]),
            "y": list(dataset_by_frame_and_droneId["yAxis"]),
            "z": list(dataset_by_frame_and_droneId["zAxis"]),
            "mode": "markers",
            "text": list(dataset_by_frame_and_droneId["droneId"]),
            "marker": {
                "sizemode": "area",
                "sizeref": 200000,
            },
            "name": droneId,
            "type": "scatter3d"
        }

        frameTemp["data"].append(data_dict)

    # if counter > 0:
    #     print("first")
    #     prevFrame = fig_dict["frames"][counter - 1]["data"]
    #     for index in range(0, len(prevFrame)):
    #         frameTemp["data"].append(prevFrame[index])

    #     print(frameTemp["data"])
    #     fig_dict["frames"].append(frameTemp)
    # else:
    #     print("second")
        # fig_dict["frames"].append(frameTemp)
    
    fig_dict["frames"].append(frameTemp)
    
    slider_step = {"args": [
        [frame],
        {"frame": {"duration": 300, "redraw": True},
         "mode": "immediate",
         "transition": {"duration": 300}}
    ],
        "label": frame,
        "method": "animate"}
    sliders_dict["steps"].append(slider_step)

    counter += 1

fig_dict["layout"]["sliders"] = [sliders_dict]

fig = go.Figure(fig_dict)

# fig.add_trace(go.Scatter3d( 
#     x= dataset_by_frame_and_droneId["xAxis"],
#     y= dataset_by_frame_and_droneId["yAxis"],
#     z= dataset_by_frame_and_droneId["zAxis"]
# ))

pio.write_html(fig, file='./drone_simulation.html', auto_open=True)