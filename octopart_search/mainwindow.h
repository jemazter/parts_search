#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mynetworkclass.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString filename;
    QString outputFilename;

    ~MainWindow();


private slots:
    void octopartData(QByteArray data);
    void on_selectFileButton_clicked();
    void on_fileName_editingFinished();
    void on_pushButton_clicked();
    void on_checkBox_clicked();
    void replyFinished();
    void on_checkBox_2_clicked();
    void on_checkBox_3_clicked();
    void on_checkBox_4_clicked();
    void on_checkBox_5_clicked();
    void on_checkBox_6_clicked();

private:
    Ui::MainWindow *ui;
    MyNetworkClass octopart;
    bool descricao = false;
    bool estilo = false;
    bool rohs = false;
    bool lifecycle = false;
    bool datasheet = false;
    bool encapsulamento = false;
    int i; //index for pn iterations
    bool newRequestAvalaible = true;
    QStringList wordList;
    int progressBar = 0;
    int completedRequests = 0;
};

#endif // MAINWINDOW_H
