import dash
from dash.dependencies import Output, Input
import dash_core_components as dcc
import dash_html_components as html
import plotly
import plotly.graph_objs as go
from collections import deque

X = deque(maxlen=20)
X.append(1)
Y = deque(maxlen=20)
Y.append(1)

TOTAL_SUM = 0


app = dash.Dash(__name__)
app.layout = html.Div(
    [
        dcc.Graph(id='live-graph', animate=True),
        dcc.Interval(
            id='graph-update',
            interval=1*1000
        ),
        html.Div(id="mean")
    ]
)

@app.callback([Output('live-graph', 'figure'), Output("mean", "children")],
              [Input('graph-update', 'n_intervals')])
def update_graph_scatter(input_data):
    global TOTAL_SUM
    with open("../result.txt", "r") as f:
        line = f.read().strip()
        try:
            nexxt = int(line)
            Y.append(nexxt)
            TOTAL_SUM += nexxt
            X.append(X[-1]+1)
        except Exception:
            print(line)
            print("-"*30)

    data = plotly.graph_objs.Scatter(
        x=list(X),
        y=list(Y),
        name='Scatter',
        mode= 'lines+markers'
    )
    fig = {'data': [data],'layout' : go.Layout(xaxis=dict(range=[min(X),max(X)]),
                                               yaxis=dict(range=[min(Y),max(Y)]),)}
    return fig, f"Current mean: {TOTAL_SUM / (X[-1] - 1)}"


if __name__ == '__main__':
    app.run_server(host='127.0.0.1', port=8080 ,debug=True)