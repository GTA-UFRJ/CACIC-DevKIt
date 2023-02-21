#include "result_screen.h"
#include "ui_result_screen.h"
#include "errors.h"
#include <iostream>

#include <QString>

void result_screen::show_result_screen(int error) {
    result_screen result_screen_object(error);
    result_screen_object.setModal(true);
    result_screen_object.exec();
}

result_screen::result_screen(int error, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::result_screen)
{
    ui->setupUi(this);

    config_error_screen(error);

    connect(ui->button, SIGNAL(pressed()), this, SLOT(button_clicked()));
}

result_screen::~result_screen()
{
    std::cout << "Deleted result_screen" << std::endl;
    delete ui;
}

void result_screen::config_error_screen(int error)
{ 
    setWindowTitle("CACIC - Result");
    QString text;
    if(!error)
        text = "Success";
    else {
        std::string error_message("Error\n");
        error_message += get_error_message((server_error_t)error);
        text = error_message.c_str();
    }
    ui->label->setText(text);
}

void result_screen::button_clicked()
{
    std::cout << "Return clicked" << std::endl;
    hide();
}

