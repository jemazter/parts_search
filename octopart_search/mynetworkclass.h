#ifndef MYNETWORKCLASS_H
#define MYNETWORKCLASS_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class MyNetworkClass : public QObject
{
    Q_OBJECT
public:
    explicit MyNetworkClass(QObject *parent = 0);
    void makeRequest(QString endPointRequest,QString mpn, bool descricao, bool estilo, bool rohs, bool lifecycle,bool encapsulamento, bool datasheet);
signals:
    void dataReadyRead(QByteArray);
public slots:
    void readyRead(QNetworkReply *reply);
private:
    QNetworkAccessManager *qnam = new QNetworkAccessManager(this);
};

#endif // MYNETWORKCLASS_H
