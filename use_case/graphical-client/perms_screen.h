#ifndef perms_SCREEN_H
#define perms_SCREEN_H

#include <QDialog>

namespace Ui {
class perms_screen;
}

class perms_screen : public QDialog
{
    Q_OBJECT

public:
    const char type_codes[3][7] = {"", "123456", "555555"};
    explicit perms_screen(QWidget *parent = nullptr);
    ~perms_screen();

    void collect_ue_perms(std::string* consumption_ids, std::string* aggregated_ids);
    void fill_ue_table(std::string consumption_ids, std::string aggregated_ids);
    void update_ue_table(std::string consumption_ids, std::string aggregated_ids);

    void collect_ap_perms(std::string* consumption_ids, std::string* aggregated_ids);
    void fill_ap_table(std::string consumption_ids, std::string aggregated_ids);
    void update_ap_table(std::string consumption_ids, std::string aggregated_ids);

private slots:
    void add_clicked();

private:
    Ui::perms_screen *ui;

    enum perms_column_t {TYPE, ID};
    typedef enum {ALL, UE, AP} device_t;
};

#endif // perms_SCREEN_H
