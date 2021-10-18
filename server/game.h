//
// Created by 666 on 2021/4/5.
//

#ifndef TERISSERVER_GAME_H
#define TERISSERVER_GAME_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QVector>
#include <QUdpSocket>
#include <QTimer>
#include <ikcp.h>
#include <tetris.pb.h>

class Game : public QObject
{
Q_OBJECT
public:
    explicit Game(qint32 gameSize,QObject *parent = nullptr);
    ~Game() override;
    quint16 getGamePort()const;
    bool isGameOver()const;
private:
    qint32 gameSize;
    quint16 gamePort;
    QVector<ikcpcb*> players;
    qint32 gameOverPlayers;
    bool gameOverFlag;
    static ikcpcb *kcpClientCreate(QPair<QHostAddress, quint16> *ipAndPort);

    QHash<QPair<QHostAddress,quint16 >,ikcpcb *> kcpClientMap;

    QTimer *timer;
    static QUdpSocket *gameSocket;
private slots:

    void netSlap();

    void readPendingDatagrams();

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);

};
#endif //TERISSERVER_GAME_H
