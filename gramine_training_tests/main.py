import base64
import pandas as pd
import pmdarima as pm 
import numpy as np
from sklearn.metrics import mean_squared_error
import pickle

DATASETS_ROOT = '/database/'

def get_dataset(csv_name):
    print("Dataset path: " + DATASETS_ROOT+csv_name)
    try:  
        df = pd.read_csv(DATASETS_ROOT+csv_name)
    except FileNotFoundError:
        print("Database not found")
        return None, -1
    return df, 0

def index_by_date(df):
    df.Datetime = pd.to_datetime(df.Datetime)
    df = df.sort_values(by ='Datetime')
    df.index = pd.to_datetime(df.Datetime)
    return df

def filter_time_interval(df, initial_date_str, final_date_str):
    return df.loc[(df.Datetime > initial_date_str) & (df.Datetime <= final_date_str)]

def generate_daframes():
    df, error = get_dataset('sample_dataset.csv')
    if(error != 0):
        return None, None, error
    df = index_by_date(df)
    train_df = filter_time_interval(df, '2010-01-01', '2010-01-07')
    test_df = filter_time_interval(df, '2010-01-07', '2010-01-08')
    return train_df, test_df, 0

def test_opening_dataframes():
    train, test, error = generate_daframes()
    if(error == 0):   
        print("-----------TRAIN-----------")
        print(train.head())
        print(train.tail())
        print("-----------TEST-----------")
        print(test.head())
        print(test.tail())

def serialize_to_string(content):
    return base64.b64encode(pickle.dumps(content)).decode('ascii')

def deserialize_from_string(content):
    return pickle.loads(base64.b64decode(content.encode('ascii')))

def store_model(model):
    
    string = serialize_to_string(model)
    try:
        with open(DATASETS_ROOT + 'saved_model', 'w') as f:
            f.write(string)
    except IOError as e:
        print("Error writing to file: {}".format(e))

def load_model():
    try:
        with open(DATASETS_ROOT + 'saved_model', 'r') as f:
            string = f.read()
    except IOError as e:
        print("Error reading to file: {}".format(e))
    return deserialize_from_string(string)

def build_energy_sarima_model(df,start_p=1,start_q=1,test='kpss',
                             max_p=3,max_q=3,m=24,d=None):
    try:
        model = pm.auto_arima(df['PJME_MW'], 
            start_p=start_p, # ordem da auto regressão (AR)
            start_q=start_q, # ordem da média móvel (MA)
            test=test, # teste de estacionaridade de Kwiatkowski–Phillips–Schmidt–Shin 
            max_p=max_p, max_q=max_q,
            m=m, # frequencia de sazonalidade (m=1 => sazobalidade desligada)
            d=d,# o modelo escolhe a ordem de derivação para alcançar estacionaridade (I)
            seasonal=True, # liga sazonalidade (S)
            trace=True, #logs 
            error_action='warn', # mostra erros (alternativa: 'ignore')
            suppress_warnings=True,
            stepwise=True)
    except:
        return None 
    return model
        
def predict_energy_sarima(model, points):
    result = {}
    preds, conf_int = model.predict(n_periods = points, return_conf_int=True)
    result['predicted_series'] =  preds.to_dict().values()
    result['lower_series'] = conf_int[:, 0]
    result['upper_series'] = conf_int[:, 0]
    return result

def test_energy_sarima(predictions, test_values):
    return np.sqrt(mean_squared_error(test_values, pd.Series(predictions)))

if __name__ == "__main__":
    train, test, error = generate_daframes()
    if(error != 0):
        exit()
    model = build_energy_sarima_model(train) # or, use load model() to get a saved model
    if model is None:
        print("No model trained")
        exit()
    store_model(model)
    result = predict_energy_sarima(model, test.shape[0])
    print("Test RMSE: %.3f" % test_energy_sarima(result['predicted_series'], test['PJME_MW']))
