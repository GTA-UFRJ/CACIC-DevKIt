#ifndef REGISTER_AP_SCREEN_H
#define REGISTER_AP_SCREEN_H

#include <QDialog>

namespace Ui {
class register_ap_screen;
}

class register_ap_screen : public QDialog
{
    Q_OBJECT

public:
    explicit register_ap_screen(QWidget *parent = nullptr);
    ~register_ap_screen();

private slots:
    void send_clicked();

private:
    Ui::register_ap_screen *ui;
};

#endif // REGISTER_AP_SCREEN_H
