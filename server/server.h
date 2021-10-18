//
// Created by 666 on 2021/4/1.
//

#ifndef TETRISSERVER_SEVER_H
#define TETRISSERVER_SEVER_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QTimer>
#include <QDateTime>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <ikcp.h>
#include <tetris.pb.h>
#include "game.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "simplecrypt.h"

class Server : public QObject
{
Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    ~Server() override;

private:

    static ikcpcb *kcpClientCreate(QPair<QHostAddress, quint16> *ipAndPort);

    void deleteClient(ikcpcb *kcpClient);

    //key=ip,port value=kcp句柄,心跳计数（aliveCounter）
    using KcpClientMap = QHash<QPair<QHostAddress, quint16>, QPair<ikcpcb *, qint32>>;

    KcpClientMap kcpClientMap;
    static QUdpSocket *serverSocket;
    QTimer *kcpTimer;
    QTimer *keepAliveTimer;

    QList<ikcpcb *> game1v1q, game9q;
    QList<Game *> games;

    SimpleCrypt simpleCrypt;
    QSqlDatabase db;
private slots:

    void netSlap();

    void HeartBeat();

    void readPendingDatagrams();

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);

};


#endif //TETRISSERVER_SEVER_H
