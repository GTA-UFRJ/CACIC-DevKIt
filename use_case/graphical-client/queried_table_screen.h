#ifndef QUERIED_TABLE_SCREEN_H
#define QUERIED_TABLE_SCREEN_H

#include <QDialog>

typedef struct queried_data {
    QString time;
    QString type;
    QString id;
    QString payload;
} queried_data_t;

namespace Ui {
class queried_table_screen;
}

class queried_table_screen : public QDialog
{
    Q_OBJECT

public:
    static void show_queried_table_screen(std::vector<std::string>);
    static void parse_data_fields(std::string, queried_data_t*);

    explicit queried_table_screen(std::vector<std::string> datas, QWidget *parent = nullptr);
    ~queried_table_screen();

    void config_queried_screen();

private slots:
    void return_clicked();
    void gencsv_clicked();


private:
    Ui::queried_table_screen *ui;

    enum queried_table_column_t {TIME, TYPE, ID, PAYLOAD};
    std::vector<std::string> datas;
    std::vector<queried_data_t> structured_datas;
};

#endif // QUERIED_TABLE_SCREEN_H
