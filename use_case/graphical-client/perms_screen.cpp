#include "perms_screen.h"
#include "ui_perms_screen.h"
#include "result_screen.h"
#include "client_generic_interface.h"
#include "config_macros.h"

#include <iostream>

perms_screen::perms_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::perms_screen)
{
    ui->setupUi(this);

    std::string consumption_ids("");
    std::string aggregated_ids("");
    collect_ue_perms(&consumption_ids, &aggregated_ids);
    fill_ue_table(consumption_ids, aggregated_ids);

    consumption_ids = "";
    aggregated_ids = "";
    collect_ap_perms(&consumption_ids, &aggregated_ids);
    fill_ap_table(consumption_ids, aggregated_ids);

    connect(ui->add_button, SIGNAL(pressed()), this, SLOT(add_clicked()));
}

perms_screen::~perms_screen()
{
    std::cout << "Deleted perms_screen" << std::endl;
    delete ui;
}


void perms_screen::collect_ue_perms(std::string *consumption_ids, std::string *aggregated_ids)
{
    std::string consumption_type("123456");
    read_perm_interface(consumption_type, consumption_ids);

    std::string aggregated_type("555555");
    read_perm_interface(aggregated_type, aggregated_ids);
}
void perms_screen::collect_ap_perms(std::string *consumption_ids, std::string *aggregated_ids)
{
    std::string consumption_type("123456");
    read_ap_perm_interface(consumption_type, consumption_ids);

    std::string aggregated_type("555555");
    read_ap_perm_interface(aggregated_type, aggregated_ids);
}


void perms_screen::fill_ue_table(std::string consumption_ids, std::string aggregated_ids)
{
    ui->ue_table->setColumnCount(2);
    QStringList column_names;
    column_names << "Type" << "Permitted IDs";
    ui->ue_table->setHorizontalHeaderLabels(column_names);

    //int row = ui->table->rowCount();
    ui->ue_table->insertRow(0);
    ui->ue_table->setItem(0, TYPE, new QTableWidgetItem(QString("Smart Meter")));
    //row = ui->table->rowCount();
    ui->ue_table->insertRow(1);
    ui->ue_table->setItem(1, TYPE, new QTableWidgetItem(QString("Aggregated")));

    update_ue_table(consumption_ids, aggregated_ids);
}
void perms_screen::update_ue_table(std::string consumption_ids, std::string aggregated_ids)
{
    if(consumption_ids.size())
        ui->ue_table->setItem(0, ID, new QTableWidgetItem(QString(consumption_ids.c_str())));
    if(aggregated_ids.size())
        ui->ue_table->setItem(1, ID, new QTableWidgetItem(QString(aggregated_ids.c_str())));
}


void perms_screen::fill_ap_table(std::string consumption_ids, std::string aggregated_ids)
{
    ui->ap_table->setColumnCount(2);
    QStringList column_names;
    column_names << "Type" << "Permitted IDs";
    ui->ap_table->setHorizontalHeaderLabels(column_names);

    //int row = ui->table->rowCount();
    ui->ap_table->insertRow(0);
    ui->ap_table->setItem(0, TYPE, new QTableWidgetItem(QString("Smart Meter")));
    //row = ui->table->rowCount();
    ui->ap_table->insertRow(1);
    ui->ap_table->setItem(1, TYPE, new QTableWidgetItem(QString("Aggregated")));

    update_ap_table(consumption_ids, aggregated_ids);
}
void perms_screen::update_ap_table(std::string consumption_ids, std::string aggregated_ids)
{
    if(consumption_ids.size())
        ui->ap_table->setItem(0, ID, new QTableWidgetItem(QString(consumption_ids.c_str())));
    if(aggregated_ids.size())
        ui->ap_table->setItem(1, ID, new QTableWidgetItem(QString(aggregated_ids.c_str())));
}


void perms_screen::add_clicked()
{
    const char* types_char = type_codes[ui->types_box->currentIndex()];
    if(!strcmp(types_char,""))
        hide();
    std::string type(types_char);
    std::cout << "types: " << type << std::endl;

    std::string perms = ui->perms_lineedit->text().toStdString();
    std::cout << "perms: " << perms << std::endl;

    device_t device = (device_t) (ui->location_box->currentIndex());
    std::cout << "device: " << (int)device << std::endl;

    int ret = 0;
    if(device == ALL) {
        std::cout << "Calling function write_perm_interface()" << std::endl;
        ret = write_perm_interface(type, perms);
        if(ret) result_screen::show_result_screen(ret);
        std::cout << "Calling function write_ap_perm_interface()" << std::endl;
        ret = write_ap_perm_interface(type, perms);
        if(ret) result_screen::show_result_screen(ret);

        std::string consumption_ids("");
        std::string aggregated_ids("");
        collect_ue_perms(&consumption_ids, &aggregated_ids);
        update_ue_table(consumption_ids, aggregated_ids);

        consumption_ids = "";
        aggregated_ids = "";
        collect_ap_perms(&consumption_ids, &aggregated_ids);
        update_ap_table(consumption_ids, aggregated_ids);
    }
    else if(device == UE) {
        std::cout << "Calling function write_perm_interface()" << std::endl;
        ret = write_perm_interface(type, perms);
        if(ret) result_screen::show_result_screen(ret);

        std::string consumption_ids("");
        std::string aggregated_ids("");
        collect_ue_perms(&consumption_ids, &aggregated_ids);
        update_ue_table(consumption_ids, aggregated_ids);
    }
    else {
        std::cout << "Calling function write_ap_perm_interface()" << std::endl;
        ret = write_ap_perm_interface(type, perms);
        if(ret) result_screen::show_result_screen(ret);

        std::string consumption_ids("");
        std::string aggregated_ids("");
        collect_ap_perms(&consumption_ids, &aggregated_ids);
        update_ap_table(consumption_ids, aggregated_ids);
    }

    //hide();
}

