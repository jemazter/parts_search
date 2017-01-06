#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfiledialog.h>
#include <QFile>
#include <QThread>
#include <QStringList>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&octopart,SIGNAL(dataReadyRead(QByteArray)),this,SLOT(octopartData(QByteArray)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFileButton_clicked()
{
    filename=QFileDialog::getOpenFileName(this,tr("Abrir Arquivo"),"C://","Todos os Arquivos (*.*);;Texto Separado por Virgulas (*.csv)");
    ui->fileName->setText(filename);
}



void MainWindow::on_pushButton_clicked()
{

    QFile file;
    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
       qDebug() << file.errorString();
    }


    while (!file.atEnd())
    {
       QByteArray line = file.readLine();
       wordList.append(line.split(',').first());
    }
    // qDebug()<<wordList.size();
    for(i=0;i<(wordList.size());i++)
    {
        // qDebug()<<wordList[i];
         octopart.makeRequest("https://octopart.com/api/v3/parts/match",wordList[i],descricao,estilo,rohs,lifecycle,encapsulamento,datasheet);
         QThread::msleep(400);
    }
}


void MainWindow::octopartData(QByteArray data)
{
    const QJsonDocument temp = QJsonDocument::fromJson(data);
    const QJsonObject jObject = temp.object();
    const QJsonArray results = jObject["results"].toArray();

    for(const QJsonValue& res : results){
        const QJsonObject result= res.toObject();
        const QJsonArray items = result["items"].toArray();
//        for(const QJsonValue& ite : items){
            const QJsonObject item = items.at(0).toObject();
            const QJsonObject specs = item["specs"].toObject();
            const QJsonObject case_package = specs["case_package"].toObject();
            const QJsonArray value = case_package["value"].toArray();
            const QJsonObject lifecycle_1 = specs["lifecycle_status"].toObject();
            const QJsonArray lifecycle_2 = lifecycle_1["value"].toArray();
            const QJsonArray mpn = item["mpn"].toArray();
            //for(auto& val : value){
            if(value.isEmpty())
                qDebug() << "N.D" <<lifecycle_2;
            else
                qDebug() << value << lifecycle_2 << mpn;
            //}
//        }
    }
}

void MainWindow::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked())
    {
        descricao = true;
        qDebug()<<"true";
    }
    else
        descricao = false;
}

void MainWindow::replyFinished()
{
    qDebug("OK");
}

void MainWindow::on_fileName_editingFinished()
{

}

void MainWindow::on_checkBox_2_clicked()
{
    if(ui->checkBox_2->isChecked())
    {
        estilo = true;
    }
    else
        estilo = false;
}

void MainWindow::on_checkBox_3_clicked()
{
    if(ui->checkBox_3->isChecked())
    {
        rohs = true;
        qDebug()<<("ROHS TRUE");
    }
    else
        rohs = false;
}

void MainWindow::on_checkBox_4_clicked()
{
    if(ui->checkBox_4->isChecked())
    {
        lifecycle = true;
    }
    else
        lifecycle = false;
}

void MainWindow::on_checkBox_5_clicked()
{
    if(ui->checkBox_5->isChecked())
    {
        encapsulamento = true;
    }
    else
        encapsulamento = false;
}

void MainWindow::on_checkBox_6_clicked()
{
    if(ui->checkBox_6->isChecked())
    {
        datasheet = true;
    }
    else
        datasheet = false;
}
