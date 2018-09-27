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
df = pd.read_csv('data.csv',
    names = ["EventID", "x1", "y1", "z1", "x2", "y2", "z2",
     "Energy1", "Energy2", "dt", "t1", "t2", "pPs"])
X = df.iloc[:, 1:12]
y = df.iloc[:, 12]
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2)
X_test_with_times = X_test.iloc[:, 0:11]
X_test = X_test.iloc[:, 0:9]
X_train = X_train.iloc[:, 0:9]

# Initializing Neural Network
classifier = Sequential()

# Adding the first hidden layer
classifier.add(
    Dense(output_dim = 12, init = 'uniform', activation = 'relu', input_dim = 9)
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

# Add data to the plot
for index, row in TP.iterrows():
    ax.scatter(
        xs=[(row[0]*row[10]+row[3]*row[9])/(row[10]+row[9])],
        ys=[(row[1]*row[10]+row[4]*row[9])/(row[10]+row[9])], 
        zs=[(row[2]*row[10]+row[5]*row[9])/(row[10]+row[9])],
        c="green",
        label='TP' if index == TP.first_valid_index() else ""
    )

for index, row in FP.iterrows():
    ax.scatter(
        xs=[(row[0]*row[10]+row[3]*row[9])/(row[10]+row[9])],
        ys=[(row[1]*row[10]+row[4]*row[9])/(row[10]+row[9])], 
        zs=[(row[2]*row[10]+row[5]*row[9])/(row[10]+row[9])],
        c="red",
        label='FP' if index == FP.first_valid_index() else ""
    )

for index, row in FN.iterrows():
    ax.scatter(
        xs=[(row[0]*row[10]+row[3]*row[9])/(row[10]+row[9])],
        ys=[(row[1]*row[10]+row[4]*row[9])/(row[10]+row[9])], 
        zs=[(row[2]*row[10]+row[5]*row[9])/(row[10]+row[9])],
        c="blue",
        label='FN' if index == FN.first_valid_index() else ""
    )

for index, row in TN.iterrows():
    ax.scatter(
        xs=[(row[0]*row[10]+row[3]*row[9])/(row[10]+row[9])],
        ys=[(row[1]*row[10]+row[4]*row[9])/(row[10]+row[9])], 
        zs=[(row[2]*row[10]+row[5]*row[9])/(row[10]+row[9])],
        c="yellow",
        label='TN' if index == TN.first_valid_index() else ""
    )

ax.set_xlabel('x [mm]')
ax.set_ylabel('y [mm]')
ax.set_zlabel('z [mm]')
ax.legend(loc='lower left')
plt.title('pPs point source - JPET simulation recostrucion')

plt.show()