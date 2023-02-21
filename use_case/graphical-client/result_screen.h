#ifndef RESULT_SCREEN_H
#define RESULT_SCREEN_H

#include <QDialog>

namespace Ui {
class result_screen;
}

class result_screen : public QDialog
{
    Q_OBJECT

public:
    static void show_result_screen(int error);

    explicit result_screen(int error, QWidget *parent = nullptr);
    ~result_screen();

    void config_error_screen(int error);

private slots:
    void button_clicked();

private:
    Ui::result_screen *ui;
};

#endif // RESULT_SCREEN_H
