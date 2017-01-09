#include "mynetworkclass.h"
#include <QUrlQuery>
#include <qjsonobject.h>
MyNetworkClass::MyNetworkClass(QObject *parent) : QObject(parent)
{
    connect(qnam,SIGNAL(finished(QNetworkReply*)),this,SLOT(readyRead(QNetworkReply*)));
}

void MyNetworkClass::makeRequest(QString endPointRequest,QString mpn,bool descricao, bool estilo, bool rohs, bool lifecycle,bool encapsulamento, bool datasheet)
{
    QUrl url = endPointRequest;
    QUrlQuery query;
    QString mpn_completo = "[{\"mpn\":\"" + mpn + "\"}]";
    if(descricao==true)
    {
        query.addQueryItem("include[]","descriptions");
        qDebug()<<"descriptions";
    }
    if((estilo==true)||(rohs==true)||(lifecycle==true)||(encapsulamento==true))
    {
        query.addQueryItem("include[]","specs");
        //qDebug()<<"SPECS TRUE";
    }
    if(datasheet==true)
    {
        query.addQueryItem("include[]","datasheets");
    }
    //-------------------------------------------------
    // Adiciona os itens solicitados a pesquisa (query)
    //-------------------------------------------------
    query.addQueryItem("apikey","12e227ea");
    query.addQueryItem("queries",mpn_completo);
    query.addQueryItem("pretty_print","true");
    url.setQuery(query.query());
    //-------------------------------------------------

    qnam->get(QNetworkRequest(url));
}

void MyNetworkClass::readyRead(QNetworkReply *reply)
{
    QByteArray myData;
    myData = reply->readAll();
    emit(dataReadyRead(myData));

}


