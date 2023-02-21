#include "publish_screen.h"
#include "ui_publish_screen.h"
#include "result_screen.h"
#include "client_generic_interface.h"
#include "config_macros.h"
#include <stdio.h>
#include <iostream>
#include <string>

#include <QComboBox>
#include <QCheckBox>

publish_screen::publish_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::publish_screen)
{
    ui->setupUi(this);

    connect(ui->send_button, SIGNAL(pressed()), this, SLOT(send_clicked()));
}

publish_screen::~publish_screen()
{
    std::cout << "Deleted publish_screen" << std::endl;
    delete ui;
}

std::string publish_screen::build_sql_statement(std::string ids)
{
    bool filled_box = false;
    while(!filled_box) {
        if(ids[0] == ' ')
            ids.erase(ids.begin());
        else
            filled_box = true;
    }
    if(!filled_box || ids == "")
        return "SELECT_*_from_TACIOT where type='123456'";

    char sql_statement[MAX_DB_COMMAND_SIZE];
    sprintf(sql_statement, "SELECT_*_from_TACIOT where type='123456' and (");

    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = ids.find(delimiter)) != std::string::npos) {
        token = ids.substr(0, pos);
        sprintf(sql_statement+strlen(sql_statement), "pk='%s' or ", token.c_str());
        ids.erase(0, pos + delimiter.length());
    }
    sprintf(sql_statement+strlen(sql_statement), "pk='%s')", ids.c_str());

    return std::string(sql_statement);
}

void publish_screen::send_clicked()
{
    const char* type = type_codes[ui->type_box->currentIndex()];
    if(!strcmp(type,""))
        hide();
    std::cout << "type_code: " << std::string(type) << std::endl;

    std::string ids;
    bool checked = (ui->default_check->checkState() != 0);
    std::cout << "box_checked: " << checked << std::endl;
    /*
    if(checked)
        read_perm_interface(std::string(type), &ids);
    else
        ids = ui->ids_lineedit->text().toStdString();
    */
    ids = ui->ids_lineedit->text().toStdString();

    std::string sql_statement = build_sql_statement(ids);
    std::cout << "IDs: " << ids << std::endl;
    std::cout << "sql_statement: " << sql_statement << std::endl;

    std::string perms = ui->idperms_lineedit->text().toStdString();
    std::cout << "perms: " << perms << std::endl;

    std::cout << "Calling function publish_interface()" << std::endl;
    int ret = publish_interface(std::string(type), sql_statement, perms, checked);
    result_screen::show_result_screen(ret);

    if(!ret)
        hide();
}

