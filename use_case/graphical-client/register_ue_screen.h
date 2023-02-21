#ifndef REGISTER_UE_SCREEN_H
#define REGISTER_UE_SCREEN_H

#include <QDialog>

namespace Ui {
class register_ue_screen;
}

class register_ue_screen : public QDialog
{
    Q_OBJECT

public:
    explicit register_ue_screen(QWidget *parent = nullptr);
    ~register_ue_screen();

private slots:
    void send_clicked();

private:
    Ui::register_ue_screen *ui;
};

#endif // REGISTER_UE_SCREEN_H
