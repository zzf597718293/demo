#ifndef DOCTORFORM_H
#define DOCTORFORM_H
#include"dbpage.h"
#include <QSqlTableModel>
#include <QWidget>

namespace Ui {
class doctorForm;
}

class doctorForm : public QWidget
{
    Q_OBJECT

public:
    explicit doctorForm(QWidget *parent = nullptr);
    ~doctorForm();
    void changeAttend();
    void changeAssistant();
    DbPage *dbpage;
    QSqlQueryModel *qmodel;
    QSqlTableModel *tmodel;
private slots:
    void on_btnSave_clicked();

    void on_btnDelet_clicked();

private:
    Ui::doctorForm *ui;
};

#endif // DOCTORFORM_H
