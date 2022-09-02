#include "doctorform.h"
#include "ui_doctorform.h"

doctorForm::doctorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::doctorForm)
{
    ui->setupUi(this);
    dbpage = new DbPage;
    qmodel = new QSqlQueryModel(this);


}

doctorForm::~doctorForm()
{
    delete ui;
    delete dbpage;
    delete qmodel;
}

void doctorForm::changeAttend()
{
    ui->label->setText("添加主治医师");

    qmodel->setQuery("SELECT attendname FROM attend");
    qmodel->setHeaderData(0,Qt::Horizontal,"主治医师");
    ui->tableView->setModel(qmodel);

}

void doctorForm::changeAssistant()
{
    ui->label->setText("添加助理医师");
    qmodel->setQuery("SELECT assistantname FROM assistant");
    qmodel->setHeaderData(0,Qt::Horizontal,"助理医师");
    ui->tableView->setModel(qmodel);
}


void doctorForm::on_btnSave_clicked()
{
    if(ui->label->text() == "添加主治医师"){
        dbpage->selectAttend(ui->edtName->text());
        qmodel->setQuery("SELECT attendname FROM attend");
        qmodel->setHeaderData(0,Qt::Horizontal,"主治医师");
        //ui->tableView->setModel(qmodel);
    }
    else{
        dbpage->selectAssistant(ui->edtName->text());
        qmodel->setHeaderData(0,Qt::Horizontal,"助理医师");
        qmodel->setQuery("SELECT assistantname FROM assistant");
        //ui->tableView->setModel(qmodel);
    }
}

void doctorForm::on_btnDelet_clicked()
{
    if(ui->label->text() == "添加主治医师"){
        QModelIndex curIndex = ui->tableView->currentIndex();
        QSqlRecord curRec = qmodel->record(curIndex.row());
        if(curRec.isEmpty())
            return;
       QModelIndex index = qmodel->index(curIndex.row(),0);
       //QString = index.data.toString();
       QSqlQuery sqlQuery;
       sqlQuery.prepare("DELETE FROM attend WHERE attendname = '"+index.data().toString()+"' ");
       qDebug()<<index.data().toString();
       sqlQuery.exec();
       qmodel->setQuery("SELECT attendname FROM attend");
    }
    else
    {
        QModelIndex curIndex = ui->tableView->currentIndex();
        QSqlRecord curRec = qmodel->record(curIndex.row());
        if(curRec.isEmpty())
            return;
       QModelIndex index = qmodel->index(curIndex.row(),0);
       //QString = index.data.toString();
       QSqlQuery sqlQuery;
       sqlQuery.prepare("DELETE FROM assistant WHERE assistantname = '"+index.data().toString()+"' ");
       sqlQuery.exec();
       qmodel->setQuery("SELECT assistantname FROM assistant");
    }

}
