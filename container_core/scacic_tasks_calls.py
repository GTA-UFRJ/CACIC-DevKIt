# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: function called by publciation code. Programmed by DevKit user

from scacic_macros import * 
from scacic_errors import Server_error
from scacic_utils import *

def do_nothing(time, id, received_payload, ck):
    return received_payload, Server_error.OK

def get_server_functions():
    return {
        '123456' : do_nothing,
        '555555' : do_nothing}