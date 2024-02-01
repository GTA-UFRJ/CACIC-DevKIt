# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: perform security checks before calling task

from sqlite3 import connect
from sqlite3 import Error as Sqlite_error
from scacic_macros import DATABASE_PATH
from scacic_errors import Server_error
from scacic_utils import *

# TODO: expand database columns
def create_db():
    connection = connect(DATABASE_PATH)
    cursor = connection.cursor()
    cursor.execute(
        """
        CREATE TABLE TACIOT (
        ID          INT PRIMARY KEY     NOT NULL,
        TIME        CHAR(20)            NOT NULL,
        TYPE        CHAR(6)             NOT NULL,
        PK          CHAR(8)             NOT NULL,
        SIZE        INT                 NOT NULL,
        ENCRYPTED   TEXT                NOT NULL
        );
        """)
    connection.close()

def db_publish(time, pk, type, result):
    id = get_random_id()
    enc_text = convert_bytes_to_text(result)
    #enc_text = "a5-23-12-f1-ab-"

    command = f"""
    INSERT INTO TACIOT (ID, TIME, TYPE, PK, SIZE, ENCRYPTED)
    VALUES ({id},'{time}','{type}','{pk}',{len(result)},'{enc_text}')
    """

    error = Server_error.OK
    try:
        connection = connect(DATABASE_PATH)
        cursor = connection.cursor()
        cursor.execute(command)
        connection.commit()
    except Sqlite_error as e:
        print("Database error:", e)
        error = Server_error.print_error(Server_error.DB_INSERT_EXECUTION_ERROR)
    finally:
        if connection:
            connection.close()

    return error

def correct_command(command):
    if ";" in command:
        return command, Server_error.print_error(Server_error.INVALID_DB_STATEMENT_ERROR)
    else:
        return command.replace("_", " "), Server_error.OK

def convert_data_tuple_in_dict(tuple):
    return {"time":tuple[1],"type":tuple[2],"pk":tuple[3],"size":tuple[4],"encrypted":convert_text_to_bytes(tuple[5])}

def db_query(command, index):
    data_list, error = db_multi_query(command)
    if error != Server_error.OK:
        return None, error
    
    if(index > len(data_list)-1):
        return None, Server_error.print_error(Server_error.OUT_OF_BOUND_INDEX)

    return data_list[index], Server_error.OK

def db_multi_query(command):
    command, error = correct_command(command)
    if error != Server_error.OK:
        return None, error

    try:
        connection = connect(DATABASE_PATH)
        cursor = connection.cursor()
        cursor.execute(command)
        data_list = cursor.fetchall()
    except Sqlite_error as e:
        print("Database error:", e)
        error = Server_error.print_error(Server_error.DB_SELECT_EXECUTION_ERROR)
    finally:
        if connection:
            connection.close()
    
    result = []
    for data in data_list:
        result.append(convert_data_tuple_in_dict(data))
    
    return result, Server_error.OK

# Test code
if __name__ == "__main__":
    #create_db()
    db_publish('horario', '72d41281', 123456, "a5-23-12-f1-ab-")
    data, error = db_query('select * from taciot where type=123456',0)
    #data, error = db_multi_query('select * from taciot where type=123456')
    if(error == Server_error.OK):
        print(data)
    else:
        Server_error.print_error(error)