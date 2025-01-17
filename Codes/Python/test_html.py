import plotly.express as px
import pandas as pd


data = {
    "sepal_width": [3.1, 2.9, 3.3, 3.5, 3.0],
    "sepal_length": [4.9, 5.1, 5.8, 6.0, 6.3],
    "species": ["setosa", "versicolor", "virginica", "setosa", "versicolor"]
}


df = pd.DataFrame(data)


fig = px.scatter(
    df, 
    x="sepal_width", 
    y="sepal_length", 
    color="species", 
    title="Custom Iris-like Dataset"
)


fig.write_html("interactive_plot1.html")
print("Interactive plot saved as 'interactive_plot1.html'")
