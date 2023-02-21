#include "register_ue_screen.h"
#include "ui_register_ue_screen.h"
#include "result_screen.h"
#include "client_generic_interface.h"
#include "config_macros.h"

#include <iostream>
#include <QLabel>
#include <QString>
#include <QLineEdit>

register_ue_screen::register_ue_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::register_ue_screen)
{
    ui->setupUi(this);

    connect(ui->send_button, SIGNAL(pressed()), this, SLOT(send_clicked()));
}

register_ue_screen::~register_ue_screen()
{
    std::cout << "Deleted register_ue_scream" << std::endl;
    delete ui;
}

void register_ue_screen::send_clicked()
{
    std::string id = ui->id_lineedit->text().toStdString();
    std::cout << "ID: " << id << std::endl;
    std::string ck = ui->ck_lineedit->text().toStdString();
    std::cout << "CK: " << ck << std::endl;

    std::cout << "Calling function register_interface()" << std::endl;
    int ret = register_interface(id, ck);
    result_screen::show_result_screen(ret);

    if(!ret)
        hide();
    //parent->show();
}

