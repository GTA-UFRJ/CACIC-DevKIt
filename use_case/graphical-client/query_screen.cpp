#include "query_screen.h"
#include "ui_query_screen.h"
#include "result_screen.h"
#include "client_generic_interface.h"
#include "config_macros.h"
#include "errors.h"
#include "queried_table_screen.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

query_screen::query_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::query_screen)
{
    ui->setupUi(this);

    connect(ui->query_button, SIGNAL(pressed()), this, SLOT(query_clicked()));
}

query_screen::~query_screen()
{
    std::cout << "Deleted publish_screen" << std::endl;
    delete ui;
}


std::string query_screen::build_sql_statement(std::string types, std::string ids)
{
    bool filled_box = false;
    while(!filled_box) {
        if(ids[0] == ' ')
            ids.erase(ids.begin());
        else
            filled_box = true;
    }
    if(!filled_box || ids == "")
        return "SELECT_*_from_TACIOT where " + types;

    char sql_statement[MAX_DB_COMMAND_SIZE];
    sprintf(sql_statement, "SELECT * from TACIOT where (%s) and (", types.c_str());

    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = ids.find(delimiter)) != std::string::npos) {
        token = ids.substr(0, pos);
        sprintf(sql_statement+strlen(sql_statement), "pk='%s' or ", token.c_str());
        ids.erase(0, pos + delimiter.length());
    }
    sprintf(sql_statement+strlen(sql_statement), "pk='%s')", ids.c_str());
    printf("%s\n", sql_statement);

    return std::string(sql_statement);
}

void query_screen::query_clicked()
{
    const char* types_char = type_codes[ui->types_box->currentIndex()];
    std::string types(types_char);
    std::cout << "types: " << types << std::endl;
    std::string ids = ui->ids_lineedit->text().toStdString();
    std::cout << "IDs: " << ids << std::endl;

    std::string sql_statement = build_sql_statement(types, ids);
    std::cout << "sql_statement: " << sql_statement << std::endl;

    int ret = 0;
    uint32_t index = 0;
    std::string returned_query;
    std::vector<std::string> queried_datas;
    while((server_error_t)ret != OUT_OF_BOUND_INDEX) {

        std::cout << "Calling function query_interface()" << std::endl;
        ret = query_interface(index, sql_statement, &returned_query);
        std::cout << "Returned query: " << returned_query << std::endl;

        if(!ret) queried_datas.push_back(returned_query);

        if(ret && ret != ACCESS_DENIED && ret != OUT_OF_BOUND_INDEX && ret != 26) {
            std::cout << "Error in query_interface()" << std::endl;
            result_screen::show_result_screen(ret);
            return;
        }
        index++;
    }

    queried_table_screen::show_queried_table_screen(queried_datas);
    //hide();
}

