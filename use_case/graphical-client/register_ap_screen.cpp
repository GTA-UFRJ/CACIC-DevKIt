#include "register_ap_screen.h"
#include "ui_register_ap_screen.h"
#include "result_screen.h"
#include <iostream>
#include "client_generic_interface.h"
#include "config_macros.h"

#include <QLabel>
#include <QString>
#include <QLineEdit>

register_ap_screen::register_ap_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::register_ap_screen)
{
    ui->setupUi(this);

    connect(ui->send_button, SIGNAL(pressed()), this, SLOT(send_clicked()));
}

register_ap_screen::~register_ap_screen()
{
    std::cout << "Deleted register_ap_scream" << std::endl;
    delete ui;
}

void register_ap_screen::send_clicked()
{
    std::string id = ui->id_lineedit->text().toStdString();
    std::cout << "ID: " << id << std::endl;
    std::string ck = ui->ck_lineedit->text().toStdString();
    std::cout << "CK: " << ck << std::endl;

    std::cout << "Calling function register_ap_interface()" << std::endl;
    int ret = register_ap_interface(id, ck);
    result_screen::show_result_screen(ret);

    if(!ret)
        hide();

}

