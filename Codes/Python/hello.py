#%%
print("test")
# %%
from IPython.display import display
import os
import pandas as pd
from pathlib import Path
from scipy import stats
import matplotlib.pyplot as plt
import seaborn as sns
path=Path('/Users/ziyutian/Downloads')
#path = Path("C:/Users/c0090836/Downloads")
electric = pd.read_csv(path / "electric.csv")
electric
# %%
gas = pd.read_csv(path / "gas.csv")
water = pd.read_csv(path / "water.csv")
pd.set_option('display.float_format', lambda x: '%.2f' % x)
electric['Date'] = pd.to_datetime(electric['Date'])
gas['Date'] = pd.to_datetime(gas['Date'])
water['Date'] = pd.to_datetime(water['Date'])
electric = electric.sort_values(by='Date')
gas = gas.sort_values(by='Date')
water = water.sort_values(by='Date')
df = pd.merge(gas, electric, suffixes=("_gas", "_electric"), on="Date", how="outer")
df = pd.merge(
    df,
    water.rename( #Add the suffix to the water columns
        columns={col: col + "_water" for col in water.columns if col != "Date"}
    ),
    on="Date",
    how="outer",
)
df
# %%
electric.Unit.plot.hist(figsize=(8,8), color="orange", ec="white", bins=range(0,300000,1000))
plt.title('Histogram of electricity meter readings (Unit)')
plt.xlabel('Unit')
plt.ylabel('Count')
plt.xlim(0,30000)
plt.show()
# %%
electric['reading_gaps'] = electric.Date.diff().dt.total_seconds().divide(24*3600)
electric.reading_gaps.plot.hist(figsize=(8,8))
plt.show()
# %%
electric.reading_gaps.value_counts().sort_values(ascending=False)
electric.reading_gaps.plot.hist(figsize=(8,8), color='red', ec='white',bins=range(0,300,1))
plt.xlim(0,21)
plt.show()
# %%
electric['cost_per_unit'] = electric['Calculated Cost'].divide(electric['Unit'])
electric['carbon_per_unit'] = electric['Carbon'].divide(electric['Unit'])
electric.describe()
# %%
electric.plot(x='Date', y='Reading', figsize=(8,8))
plt.title('Electricity meter readings over time')
plt.xlabel('Date')
plt.ylabel('Reading')
plt.show()
# %%
ax = df.groupby(df.Date.dt.month)[['Unit_gas', 'Unit_electric']].mean().plot.bar(figsize=(8,8))
plt.show()
# %%
electric.corr()

# %%
# Set Seaborn figure size
sns.set(rc={"figure.figsize":(12, 8)})

# get coeffs of linear fit
slope, intercept, r_value, p_value, std_err = stats.linregress(electric['Unit'], electric['Carbon'])

# Regression plot of 'Carbon' vs. 'Unit' for 'electric' dataframe
# use line_kws to set line label for legend
ax = sns.regplot(x=electric['Unit'],y=electric['Carbon'], line_kws={'label':"y={0:.1f}x+{1:.1f}, R-value={2:.2f}".format(slope,intercept,r_value)})

# plot legend
ax.legend(loc='upper left')

# Show the plot
plt.show()
# %%
electric_correlations = electric.corr()
sns.set(rc={"figure.figsize":(12, 8)})
sns.heatmap(electric_correlations, annot = True) 
plt.show()


# %%
