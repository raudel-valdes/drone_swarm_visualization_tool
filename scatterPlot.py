import plotly.graph_objects as go
import plotly.io as pio

fig = go.Figure(go.Scatter(x=[1, 2, 3, 4], y=[4, 3, 2, 1]))
fig.update_layout(title_text='hello world')
pio.write_html(fig, file='hello_world.html', auto_open=True)
