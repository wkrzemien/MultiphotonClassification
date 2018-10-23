import pandas as pd
import numpy as np
import keras
from keras.models import Sequential
from keras.layers import Dense
from sklearn.metrics import confusion_matrix
from sklearn.preprocessing import StandardScaler
import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from calc import emissionPoint
from calc import loadDataFrames
from calc import reconstruction

# Load and transform data into sets 
df, X_train, X_test, y_train, y_test, X_test_with_times = loadDataFrames('data.csv')

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

reconstruction(FP, TP, TN, FN)

# Stats for all particles considered
allStatsFrame = df[["EventID1","TrackID1","e1","x1", "y1", "z1", "t1"]].drop_duplicates()

# All particles energy stats
figAll1 = plt.figure()
plt.hist(allStatsFrame[["e1"]].transpose(), bins=20)
plt.title('Energy loss - all particles')
plt.xlabel('Energy [keV]')
plt.ylabel('#')
plt.savefig('allEnergy.png')
# All particles t stats
figAll2 = plt.figure()
plt.hist(allStatsFrame[["t1"]].transpose(), bins=20)
plt.title('Detection time - all particles')
plt.xlabel('time [ns]')
plt.ylabel('#')
plt.savefig('allTime.png')
# All particles x stats
figAll3 = plt.figure()
plt.hist(allStatsFrame[["x1"]].transpose(), bins=20)
plt.title('X position - all particles')
plt.xlabel('Position [mm]')
plt.ylabel('#')
plt.savefig('allX.png')
# All particles y stats
figAll4 = plt.figure()
plt.hist(allStatsFrame[["y1"]].transpose(), bins=20)
plt.title('Y position - all particles')
plt.xlabel('Position [mm]')
plt.ylabel('#')
plt.savefig('allY.png')
# All particles z stats
figAll5 = plt.figure()
plt.hist(allStatsFrame[["e1"]].transpose(), bins=20)
plt.title('Z position - all particles')
plt.xlabel('Position [mm]')
plt.ylabel('#')
plt.savefig('allZ.png')
