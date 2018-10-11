def emissionPoint(row):
    den = row['t1']+row['t2']
    return { 
        'x':(row['x1']*row['t2']+row['x2']*row['t1'])/den,
        'y':(row['y1']*row['t2']+row['y2']*row['t1'])/den,
        'z':(row['z1']*row['t2']+row['z2']*row['t1'])/den,
    }