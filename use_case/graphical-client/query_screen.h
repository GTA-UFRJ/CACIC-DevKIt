#ifndef QUERY_SCREEN_H
#define QUERY_SCREEN_H

#include <QDialog>

namespace Ui {
class query_screen;
}

class query_screen : public QDialog
{
    Q_OBJECT

public:
    const char type_codes[3][31] = {"type='123456' or type='555555'", "type='123456'", "type='555555'"};
    explicit query_screen(QWidget *parent = nullptr);
    ~query_screen();

private slots:
    void query_clicked();

private:
    Ui::query_screen *ui;
    std::string build_sql_statement(std::string types, std::string ids);
};

#endif // QUERY_SCREEN_H
