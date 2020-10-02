import plotly.io as pio
import plotly.express as px


df = px.data.iris()
fig = px.scatter_3d(df, x='sepal_length', y='sepal_width', z='petal_width',
              color='species')
fig.update_layout(title_text='hello world')
pio.write_html(fig, file='./drone_simulation.html', auto_open=True)