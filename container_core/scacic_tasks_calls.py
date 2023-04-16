# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: function called by publciation code. Programmed by DevKit user

from scacic_macros import * 
import pandas as pd
from scacic_errors import Server_error
import pmdarima as pm 
import pickle
from scacic_utils import *
from scacic_db_secure_interface import *
import base64

# ds_name=sample_dataset_energy,start_p=1,start_q=1,test=kpss,...
def get_params_payload_fields_model(payload):
    dict = {'ds_name':'sample_dataset_energy.csv','start_p':1,'start_q':1,'test':'kpss','max_p':3,'max_q':3,'m':24,'d':None}
    pairs = [pair.split('=') for pair in payload.split(',')]
    new_values = {k: v.strip("'") if v.startswith("'") else int(v) for k, v in pairs}
    dict.update(new_values)
    return dict

def get_dataset(csv_name):
    try:
        df = pd.read_csv(DATASETS_ROOT+csv_name)
    except FileNotFoundError:
        return None, Server_error.print_error(Server_error.OPEN_DATABASE_ERROR)
    return df, Server_error.OK

def perform_inference(model, points):
    result = {}
    try:
        preds, conf_int = model.predict(n_periods = points, return_conf_int=True)
    except Exception as e:
        print("Inference error: ", e)
        return None,  Server_error.print_error(Server_error.INFERENCE_TASK_ERROR)
    result['predicted_series'] =  preds.to_dict().values()
    result['lower_series'] = conf_int[:, 0]
    result['upper_series'] = conf_int[:, 0]
    return result, Server_error.OK

def generate_energy_dataframe(time, id, received_payload, ck):
    # payload will contain a command to locate the datas and the dataset name
    # we will query, decrypt the data and identify fields
    # then, we will generate a table as follows:
    # {'Datetime': ['xxx', 'xxx', 'xxx'],
    #  'PJME_MW': ['xxx', 'xxx', 'xxx']}
    # lastly, this will be written into use_case/database as a CSV
    print_if_debug("Called generate_dataframe()")
    path = DATASETS_ROOT + received_payload + time
    return path.replace(' ', '_').replace('.csv') + '.csv'

def serialize_to_string(content):
    return base64.b64encode(pickle.dumps(content)).decode('ascii')

def deserialize_from_string(content):
    return pickle.loads(base64.b64decode(content.encode('ascii')))
    
def build_energy_sarima_model(time, id, received_payload, ck):
    error = Server_error.OK
    parameters = get_params_payload_fields_model(received_payload)
    df, error = get_dataset(parameters['ds_name'])
    if(error != Server_error.OK):
        return None, error
    model = pm.auto_arima(df['PJME_MW'], 
        start_p=parameters['start_p'], 
        start_q=parameters['start_q'], 
        test=parameters['test'], 
        max_p=parameters['max_p'], max_q=parameters['max_q'],
        m=parameters['m'], 
        d=parameters['d'],
        seasonal=True, 
        trace=True, 
        error_action='warn', 
        suppress_warnings=True,
        stepwise=True)
    print_if_debug("Built SARIMA model: ", model)
    return serialize_to_string(model), Server_error.OK

def predict_energy_sarima(time, id, received_payload, ck):
    # command|index|24
    command, index, num_points = received_payload.split('|')
    
    payload, error = secure_query_db(command, index, id)
    if(error != Server_error.OK):
        return None, error
        
    model = deserialize_from_string(payload)
    print_if_debug("Queried model from database: ", model)

    result, error = perform_inference(model, num_points)
    if(error != Server_error.OK):
        return None, error
    print_if_debug("Infered with SARIMA model: ", result)

    # https://stackoverflow.com/questions/1296162/how-can-i-read-a-python-pickle-database-file-from-c
    return serialize_to_string(result), Server_error.OK
    
def get_server_functions():
    return {
        '222222' : generate_energy_dataframe, 
        '333333' : build_energy_sarima_model,
        '444444' : predict_energy_sarima}