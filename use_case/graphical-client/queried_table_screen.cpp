#include "queried_table_screen.h"
#include "ui_queried_table_screen.h"
#include "result_screen.h"
#include "errors.h"
#include <string>
#include <vector>
#include <iostream>

#include <QFile>
#include <QTextStream>

void queried_table_screen::show_queried_table_screen(std::vector<std::string> datas) {
    queried_table_screen queried_table_screen_object(datas);
    queried_table_screen_object.setModal(true);
    queried_table_screen_object.exec();
}

queried_table_screen::queried_table_screen(std::vector<std::string> datas, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::queried_table_screen),
    datas(datas)
{
    ui->setupUi(this);

    config_queried_screen();

    connect(ui->return_button, SIGNAL(pressed()), this, SLOT(return_clicked()));
    connect(ui->gencsv_button, SIGNAL(pressed()), this, SLOT(gencsv_clicked()));
}

queried_table_screen::~queried_table_screen()
{
    std::cout << "Deleted queried_table_screen" << std::endl;
    delete ui;
}

void queried_table_screen::parse_data_fields(std::string data, queried_data_t* p_fields)
{
    std::vector<std::string> fields_vector;
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    while ((pos = data.find(delimiter)) != std::string::npos) {
        token = data.substr(0, pos);
        fields_vector.push_back(token);
        data.erase(0, pos + delimiter.length());
    }
    fields_vector.push_back(data);

    bool filled[4] = {false,false,false,false};
    for(unsigned i=0; i<fields_vector.size(); i++) {
        if(fields_vector[i] == "time") {
            p_fields->time = QString(fields_vector[i+1].c_str());
            filled[0] = true;
        }
        else if(fields_vector[i] == "pk") {
            p_fields->id = QString(fields_vector[i+1].c_str());
            filled[1] = true;
        }
        else if(fields_vector[i] == "type") {
            p_fields->type = QString(fields_vector[i+1].c_str());
            filled[2] = true;
        }
        else if(fields_vector[i] == "payload") {
            p_fields->payload = QString(fields_vector[i+1].c_str());
            filled[3] = true;
        }
    }

    if(filled[0] == false) p_fields->time = "-";
    if(filled[1] == false) p_fields->type = "-";
    if(filled[2] == false) p_fields->id = "-";
    if(filled[3] == false) p_fields->payload = "-";

    for(unsigned i=0; i<(p_fields->time).length(); i++)
        p_fields->time[i] = (p_fields->time[i] == '_' ? ' ' : p_fields->time.toLocal8Bit().data()[i]);

    if(p_fields->type == "123456") p_fields->type = "Smart Meter";
    if(p_fields->type == "555555") p_fields->type = "Aggregated";

/*
    std::cout << "TO DO: parse_data_fields()" << std::endl;
    p_fields->time = QString("09-01-2023 10h30m15s");
    p_fields->type = QString("Consumption");
    p_fields->id = QString("72d453f1");
    p_fields->payload = QString("150");
*/
}

void queried_table_screen::config_queried_screen()
{
    setWindowTitle("CACIC - Result");

    // Prepare table formatt
    ui->table->setColumnCount(4);
    QStringList column_names;
    column_names << "Time" << "Type" << "ID" << "Payload";
    ui->table->setHorizontalHeaderLabels(column_names);

    // Fill table
    for(auto data : datas) {

        std::cout << "Data: " << data << std::endl;

        queried_data_t data_fields;
        parse_data_fields(data, &data_fields);
        structured_datas.push_back(data_fields);

        int row = ui->table->rowCount();
        ui->table->insertRow(row);
        ui->table->setItem(row, TIME, new QTableWidgetItem(data_fields.time));
        ui->table->setItem(row, TYPE, new QTableWidgetItem(data_fields.type));
        ui->table->setItem(row, ID  , new QTableWidgetItem(data_fields.id));
        ui->table->setItem(row, PAYLOAD, new QTableWidgetItem(data_fields.payload));
    }
}

void queried_table_screen::return_clicked()
{
    hide();
}

void queried_table_screen::gencsv_clicked()
{
    QFile file("./out.csv");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cout << "Error opening CSV output file" << std::endl;
        result_screen::show_result_screen(OPEN_CSV_FILE_ERROR);
    }

    QTextStream out (&file);

    for(unsigned i=0; i<datas.size(); i++)
        out << structured_datas[i].time     << ',' <<
               structured_datas[i].type     << ',' <<
               structured_datas[i].id       << ',' <<
               structured_datas[i].payload  << '\n';

    file.flush();
    file.close();
    std::cout << "Success generation out.csv" << std::endl;
    result_screen::show_result_screen(OK);
}

