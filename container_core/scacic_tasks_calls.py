# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: function called by publciation code. Programmed by DevKit user

from scacic_macros import DATABASE_PATH 
import pandas as pd
from scacic_errors import Server_error
import pmdarima as pm 
import pickle
from scacic_utils import secure_query_db

# ds_name=sample_dataset_energy,start_p=1,start_q=1,test=kpss,...
def get_payload_fields_model(payload):
    dict = {'ds_name':'sample_dataset_energy.csv','start_p':1,'start_q':1,'test':'kpss','max_p':3,'max_q':3,'m':24,'d':None}
    pairs = [pair.split('=') for pair in payload.split(',')]
    new_values = {k: v.strip("'") if v.startswith("'") else int(v) for k, v in pairs}
    dict.update(new_values)

def get_dataset(csv_name):
    try:
        df = pd.read_csv(DATABASE_PATH+csv_name)
    except FileNotFoundError:
        return None, Server_error.print_error(Server_error.OPEN_DATABASE_ERROR)
    return df, Server_error.OK

def predict_energy_sarima(model, points):
    result = {}
    preds, conf_int = model.predict(n_periods = points, return_conf_int=True)
    result['predicted_series'] =  preds.to_dict().values()
    result['lower_series'] = conf_int[:, 0]
    result['upper_series'] = conf_int[:, 0]
    return result

def generate_energy_dataframe(time, id, received_payload, ck):
    # payload will contain a command to locate the datas and the dataset name
    # we will query, decrypt the data and identify fields
    # then, we will generate a table as follows:
    # {'Datetime': ['xxx', 'xxx', 'xxx'],
    #  'PJME_MW': ['xxx', 'xxx', 'xxx']}
    # lastly, this will be written into use_case/database as a CSV
    print("generate_dataframe()")
    
def build_energy_sarima_model(time, id, received_payload, ck):
    error = Server_error.OK
    parameters = get_payload_fields_model(received_payload)
    df, error = get_dataset(parameters.ds_name)
    if(error != Server_error.OK):
        return None, error
    model = pm.auto_arima(df['PJME_MW'], 
        start_p=parameters.start_p, 
        start_q=parameters.start_q, 
        test=parameters.test, 
        max_p=parameters.max_p, max_q=parameters.max_q,
        m=parameters.m, 
        d=parameters.d,
        seasonal=True, 
        trace=True, 
        error_action='warn', 
        suppress_warnings=True,
        stepwise=True)
    model_bytes = pickle.dump(model)
    return model_bytes.decode('unicode-escape'), Server_error.OK

def predict_energy_sarima(time, id, received_payload, ck):
    # payload will contain a command to identify the model and the number of points
    command, index, num_points = received_payload.split('|')
    
    # we will query, decrypt and retrieve the model
    data, error = secure_query_db(command, index, id)
    if(error != Server_error.OK):
        return Server_error.print_error(Server_error.INVALID_PAYLOAD_ERROR)

    # then, we will predict the consumption 
    # finally, we will format the result as a string
    # https://stackoverflow.com/questions/1296162/how-can-i-read-a-python-pickle-database-file-from-c
    print("TASK 2")
    
server_functions = {
    '222222' : generate_energy_dataframe, 
    '333333' : build_energy_sarima_model,
    '444444' : predict_energy_sarima}