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
#include <QDateTime>


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

    ui->progressBar->setValue(0);
    QFile file;    
    //------------------------------------------------------------------------
    // Geração do nome do novo arquivo
    //------------------------------------------------------------------------
    outputFilename = filename;
    outputFilename = outputFilename.remove(filename.size()-4,filename.size());
    outputFilename.append("_editado.csv");

    qDebug()<<outputFilename;
    file.setFileName(filename);
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Abertura do arquivo para leitura dos PNs
    //------------------------------------------------------------------------
    if (!file.open(QIODevice::ReadOnly))
    {
       qDebug() << file.errorString();
       ui->textBrowser->append(file.errorString());
    }

    ui->textBrowser->append("Lendo arquivo de entrada.");

    while (!file.atEnd())
    {
       QByteArray line = file.readLine();
       wordList.append(line.split(',').first());
       QCoreApplication::processEvents();
    }
    //------------------------------------------------------------------------

    ui->textBrowser->append("Numero de Itens:");
    ui->textBrowser->append(QString::number(wordList.size()));

    //------------------------------------------------------------------------
    //Leitura do Arquivo
    //------------------------------------------------------------------------
    for(i=0;i<(wordList.size());i++)
    {
         //qDebug()<<wordList[i];
         octopart.makeRequest("https://octopart.com/api/v3/parts/match",wordList[i],descricao,estilo,rohs,lifecycle,encapsulamento,datasheet);
         QThread::msleep(400);
    }
    //------------------------------------------------------------------------
    i = wordList.size();
}


void MainWindow::octopartData(QByteArray data)
{
    //---------------------------------------------------------------------------
    //Atualizaça~oda barra de progresso
    //---------------------------------------------------------------------------
    completedRequests++;
    progressBar = (100*completedRequests)/i;
    ui->progressBar->setValue(progressBar);
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    //JSon Parsing
    //---------------------------------------------------------------------------

    //ui->progressBar->setValue();

    const QJsonDocument temp = QJsonDocument::fromJson(data);
    const QJsonObject jObject = temp.object();
    const QJsonArray results = jObject["results"].toArray();
    //qDebug()<< results;
    for(const QJsonValue& res : results){
        const QJsonObject result= res.toObject();
        const QJsonArray items = result["items"].toArray();
//        for(const QJsonValue& ite : items){                                           //iterates troughr all items (0,1,2,3,4...)

            const QJsonObject item = items.at(0).toObject();
            const QJsonObject specs = item["specs"].toObject();

            const QJsonObject case_package = specs["case_package"].toObject();
            const QJsonArray case_pack_value = case_package["value"].toArray();

            const QJsonObject lifecycle_status = specs["lifecycle_status"].toObject();
            const QJsonArray lifecycle__status_value = lifecycle_status["value"].toArray();

            const QString mpn = item["mpn"].toString();

            const QJsonObject rohs_status = specs["rohs_status"].toObject();
            const QJsonArray rohs_status_value = rohs_status["value"].toArray();
            //for(auto& val : value){                                                   //iterates trough all values from all items (0,1,2,3,4...)

            const QJsonObject mounting_style = specs["mounting_style"].toObject();
            const QJsonArray mounting_style_value = mounting_style["value"].toArray();

            const QJsonArray datasheet_array = item["datasheets"].toArray();
            const QJsonObject datasheet_item = datasheet_array.at(0).toObject();
            const QString datasheet_url = datasheet_item["url"].toString();

            const QJsonArray descriptions_array = item["descriptions"].toArray();
            const QJsonObject description_item = descriptions_array.at(0).toObject();
            const QString description_value = description_item["value"].toString();

            //---------------------------------------------------------------------------------

            qDebug() << description_value;


            //---------------------------------------------------------------------------------
            //Escrita no arquivo de saida
            //---------------------------------------------------------------------------------
            QFile outputFile(outputFilename);
            if ( outputFile.open(QIODevice::Append) )
            {
                QTextStream stream( &outputFile );
                if(completedRequests==1)
                {
                    ui->textBrowser->append("Iniciando consulta ao servidor...");
                    stream << "Part Number" << "," << "Descrição" << "," << "Estilo de Montagem" << "," << "ROHS" << "," << "Ciclo de Vida" << "," << "Encapsulamento" << "," << "Datasheet" << "\r\n";
                }
                stream << mpn << ",";
                if(descricao==true)
                {
                   if(description_value.isEmpty())
                       stream << "N.D";
                   else
                    stream << description_value << ",";
                }
                if(estilo==true)
                {
                    if(mounting_style_value[0].toString().isEmpty())
                        stream << "N.D";
                    else
                        stream << mounting_style_value[0].toString() << ",";
                }
                if(rohs==true)
                {
                    if(rohs_status_value[0].toString().isEmpty())
                        stream << "N.D";
                    else
                        stream << rohs_status_value[0].toString() << ",";
                }
                if(lifecycle==true)
                {
                    if(lifecycle__status_value[0].toString().isEmpty())
                        stream << "N.D";
                    else
                        stream << lifecycle__status_value[0].toString() << ",";
                }
                if(encapsulamento==true)
                {
                    if(case_pack_value[0].toString().isEmpty())
                        stream << "N.D";
                    else
                        stream << case_pack_value[0].toString() << ",";
                }
                if(datasheet==true)
                {
                    if(datasheet_url.isEmpty())
                        stream << "N.D";
                    else
                        stream << datasheet_url << ",";
                }

                stream << "\r\n";
            }
            outputFile.close();

            //---------------------------------------------------------------------------------

            //---------------------------------------------------------------------------------
            //"Limpeza de Variaveis"
            //---------------------------------------------------------------------------------
            if(i==completedRequests)
            {
                i=0;
                completedRequests = 0;
                progressBar = 0;
                wordList.clear();
                ui->textBrowser->append("Solicitação Concluida!");
            }

           //}
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
