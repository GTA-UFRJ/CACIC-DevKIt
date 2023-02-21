#ifndef PUBLISH_SCREEN_H
#define PUBLISH_SCREEN_H

#include <QDialog>


namespace Ui {
class publish_screen;
}

class publish_screen : public QDialog
{
    Q_OBJECT

public:
    const char type_codes[2][7] = {"", "555555"};
    explicit publish_screen(QWidget *parent = nullptr);
    ~publish_screen();

private slots:
    void send_clicked();


private:
    Ui::publish_screen *ui;
    std::string build_sql_statement(std::string ids);
};

#endif // PUBLISH_SCREEN_H
