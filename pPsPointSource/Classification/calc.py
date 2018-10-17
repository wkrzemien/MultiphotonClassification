import pandas as pd
from sklearn.model_selection import train_test_split

def emissionPoint(row):
    den = row['t1']+row['t2']
    return { 
        'x':(row['x1']*row['t2']+row['x2']*row['t1'])/den,
        'y':(row['y1']*row['t2']+row['y2']*row['t1'])/den,
        'z':(row['z1']*row['t2']+row['z2']*row['t1'])/den,
    }

def loadDataFrames(filename):
    codes = {'detector1':1, 'detector2':2, 'detector3':3}
    df = pd.read_csv(filename,
        names = [
        "EventID1", "EventID2", "TrackID1", "TrackID2", "x1", "y1", "z1", "x2", "y2", "z2",
        "e1", "e2", "dt", "t1", "t2", "vol1", "vol2", "pPs"
        ])

    df['vol1'] = df['vol1'].map(codes)
    df['vol2'] = df['vol2'].map(codes)
    X = df.drop(['pPs'], axis=1)
    y = df[["pPs"]]
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2)
    X_test_with_times = X_test.copy()
    X_train.drop(['t1', 't2', "EventID1", "EventID2", "TrackID1", "TrackID2"], axis=1, inplace=True)
    X_test.drop(['t1', 't2', "EventID1", "EventID2", "TrackID1", "TrackID2"], axis=1, inplace=True)
    return df, X_train, X_test, y_train, y_test, X_test_with_times