import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
import keras
from keras.models import Sequential
from keras.layers import Dense
from sklearn.metrics import confusion_matrix
from sklearn.preprocessing import StandardScaler
import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Load and transform data into sets 
codes = {'detector1':1, 'detector2':2, 'detector3':3}
df = pd.read_csv('data.csv',
    names = ["EventID", "x1", "y1", "z1", "x2", "y2", "z2",
     "Energy1", "Energy2", "dt", "t1", "t2", "vol1", "vol2", "pPs"])

df['vol1'] = df['vol1'].map(codes)
df['vol2'] = df['vol2'].map(codes)

print(df.head())
X = df.iloc[:, 1:14]
y = df.iloc[:, 14]
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2)
X_test_with_times = X_test.iloc[:, 0:13]
X_train.drop(['t1', 't2'], axis=1, inplace=True)
X_test.drop(['t1', 't2'], axis=1, inplace=True)

# Initializing Neural Network
classifier = Sequential()

# Adding the first hidden layer
classifier.add(
    Dense(output_dim = 12, init = 'uniform', activation = 'relu', input_dim = 11)
)

# Adding the second hidden layer
classifier.add(
    Dense(output_dim = 6, init = 'uniform', activation = 'relu', )
)

# Adding the output layer
classifier.add(
    Dense(output_dim = 1, init = 'uniform', activation = 'sigmoid')
)

# Compiling Neural Network
classifier.compile(
    optimizer = 'adam', loss = 'binary_crossentropy', metrics = ['accuracy']
)

# Fitting our model 
classifier.fit(X_train, y_train, batch_size = 1000, nb_epoch = 1000)

# Predicting the Test set results
y_pred = classifier.predict(X_test)
y_pred = (y_pred > 0.5)

# Creating the Confusion Matrix
cm = confusion_matrix(y_test, y_pred)
print(cm)

# Create sets for visualization
pPsOrginalPositive = X_test_with_times[y_test.values > 0]
pPsOrginalNegative = X_test_with_times[y_test.values == 0]
pPsPredictedPositive = X_test_with_times[y_pred]
pPsPredictedNegative = X_test_with_times[y_pred == 0]

FP = pd.merge(pPsPredictedPositive,pPsOrginalNegative, how='inner')
TP = pd.merge(pPsPredictedPositive,pPsOrginalPositive, how='inner')
TN = pd.merge(pPsPredictedNegative,pPsOrginalNegative, how='inner')
FN = pd.merge(pPsPredictedNegative,pPsOrginalPositive, how='inner')

# Initialize plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

def emissionPoint(row):
    den = row['t1']+row['t2']
    return { 
        'x':(row['x1']*row['t2']+row['x2']*row['t1'])/den,
        'y':(row['y1']*row['t2']+row['y2']*row['t1'])/den,
        'z':(row['z1']*row['t2']+row['z2']*row['t1'])/den,
    }

# Add data to the plot
for index, row in TP.iterrows():
    point = emissionPoint(row)
    ax.scatter(
        xs=point['x'], ys=point['y'], zs=point['z'], c="green",
        label='TP' if index == TP.first_valid_index() else ""
    )
    if (row['vol1'] == 1 and row['vol2'] == 1):
        print(point)

for index, row in FP.iterrows():
    point = emissionPoint(row)
    ax.scatter(
        xs=point['x'], ys=point['y'], zs=point['z'], c="red",
        label='FP' if index == FP.first_valid_index() else ""
    )


for index, row in FN.iterrows():
    point = emissionPoint(row)
    ax.scatter(
        xs=point['x'], ys=point['y'], zs=point['z'], c="blue",
        label='FN' if index == FN.first_valid_index() else ""
    )

for index, row in TN.iterrows():
    point = emissionPoint(row)
    ax.scatter(
        xs=point['x'], ys=point['y'], zs=point['z'], c="yellow",
        label='TN' if index == TN.first_valid_index() else ""
    )

ax.set_xlabel('x [mm]')
ax.set_ylabel('y [mm]')
ax.set_zlabel('z [mm]')
ax.legend(loc='lower left')
plt.title('pPs point source - JPET simulation recostrucion')

plt.show()