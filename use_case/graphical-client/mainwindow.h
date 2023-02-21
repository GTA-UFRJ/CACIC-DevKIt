#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "register_ue_screen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void register_ue_clicked();

    void register_ap_clicked();

    void perms_clicked();

    void publish_clicked();

    void query_clicked();

private:
    Ui::MainWindow *ui;
    //register_ue_screen* register_ue_screen_objptr;
};
#endif // MAINWINDOW_H
