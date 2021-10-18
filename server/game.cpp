//
// Created by 666 on 2021/4/5.
//

#include "game.h"
#include "global.h"
#include <QsLog.h>
#include <QUdpSocket>
#include <QDateTime>
#include <QNetworkDatagram>
#include <QRandomGenerator>

QUdpSocket *Game::gameSocket;

Game::Game(qint32 gameSize, QObject *parent) : QObject(parent), gameSize(gameSize), gameOverPlayers(0), gameOverFlag(false)
{
    gameSocket = new QUdpSocket(this);
    if (!gameSocket->bind())
        QLOG_ERROR() << "创建对局时端口绑定失败";
    gamePort = gameSocket->localPort();
    connect(gameSocket, &QUdpSocket::readyRead, this, &Game::readPendingDatagrams);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(netSlap()));
    timer->start(10);
}

Game::~Game()
{
    delete gameSocket;
    delete timer;
    for (auto kcpClient :kcpClientMap)
    {
        delete static_cast<QPair<QHostAddress, quint16> *>(kcpClient->user);
    }
}

void Game::netSlap()
{
    static QByteArray byteArray;
    quint32 current = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    for (auto kcpClient :kcpClientMap)
    {
        ikcp_update(kcpClient, current);
        while (true)
        {
            if (byteArray.size() < ikcp_peeksize(kcpClient))
                byteArray.resize(ikcp_peeksize(kcpClient));
            int dataSize = ikcp_recv(kcpClient, byteArray.data(), byteArray.size());
            if (dataSize < 0) break;

            proto::Instruction instruction;
            if (instruction.ParseFromArray(byteArray.data(), dataSize))
            {
                switch (instruction.type_case())
                {
                    case proto::Instruction::kClockwise:
                    case proto::Instruction::kCounterclockwise:
                    case proto::Instruction::kDown:
                    case proto::Instruction::kRight:
                    case proto::Instruction::kLeft:
                    case proto::Instruction::kDrop:
                    case proto::Instruction::kSwapBlock:
                    case proto::Instruction::kStackClear:
                        for (auto player : players)
                        {
                            ikcp_send(player, byteArray.data(), byteArray.size());
                        }
                        break;
                    case proto::Instruction::kRandomSeed:
                        break;
                    case proto::Instruction::kReadyToStart:
                        players.push_back(kcpClient);
                        /*if(gameSize==9)//9人测试版
                        {
                            quint32 randArray[10];
                            QRandomGenerator::global()->fillRange(randArray, gameSize);

                                for (int j = 0; j < 9; j++)
                                {
                                    instruction.set_random_seed(randArray[j]);
                                    ikcp_send(players.back(), instruction.SerializeAsString().data(),
                                              instruction.ByteSizeLong());
                                }
                                instruction.set_ready_to_start(0);
                                ikcp_send(players.back(), instruction.SerializeAsString().data(),
                                          instruction.ByteSizeLong());
                        }*/
                        if (players.size() == gameSize)
                        {
                            quint32 randArray[10];
                            QRandomGenerator::global()->fillRange(randArray, gameSize);
                            for (int i = 0; i < players.size(); i++)
                            {
                                for (int j = 0; j < players.size(); j++)
                                {
                                    instruction.set_random_seed(randArray[j]);
                                    ikcp_send(players[i], instruction.SerializeAsString().data(),
                                              instruction.ByteSizeLong());
                                }
                                instruction.set_ready_to_start(i);
                                ikcp_send(players[i], instruction.SerializeAsString().data(),
                                          instruction.ByteSizeLong());
                            }
                            break;
                        }
                    case proto::Instruction::kGameOver:
                    {
                        if (++gameOverPlayers == gameSize)
                            gameOverFlag = true;
                        break;
                    }
                    case proto::Instruction::TYPE_NOT_SET:
                        break;
                }
            }
            else
            {
                QLOG_ERROR() << "protobuf 解析错误";
            }
        }
    }
}


ikcpcb *Game::kcpClientCreate(QPair<QHostAddress, quint16> *ipAndPort)
{

    ikcpcb *kcpClient;
    kcpClient = ikcp_create(global::gameConv, ipAndPort);

    kcpClient->output = udp_output;
    ikcp_wndsize(kcpClient, 128, 128);
    // 启动快速模式
    // 第二个参数 nodelay-启用以后若干常规加速将启动
    // 第三个参数 interval为内部处理时钟，默认设置为 10ms
    // 第四个参数 resend为快速重传指标，设置为2
    // 第五个参数 为是否禁用常规流控，这里禁止
    ikcp_nodelay(kcpClient, 2, 10, 2, 1);
    ikcp_nodelay(kcpClient, 2, 10, 2, 1);
    kcpClient->rx_minrto = 10;
    kcpClient->fastresend = 1;
    return kcpClient;

}

void Game::readPendingDatagrams()
{
    while (gameSocket->hasPendingDatagrams())
    {
        auto datagram = gameSocket->receiveDatagram();
        auto recvIpAndPort = qMakePair(datagram.senderAddress(), datagram.senderPort());
        if (!kcpClientMap.contains(recvIpAndPort))
        {
            kcpClientMap.insert(recvIpAndPort, kcpClientCreate(new QPair<QHostAddress, quint16>(recvIpAndPort)));
            QLOG_TRACE() << "玩家加入对局成功 地址为:" << recvIpAndPort;
            ikcp_input(kcpClientMap[recvIpAndPort], datagram.data(), datagram.data().size());
        }
        else
        {
            ikcp_input(kcpClientMap[recvIpAndPort], datagram.data(), datagram.data().size());
        }
    }
}

quint16 Game::getGamePort() const
{
    return gamePort;
}


int Game::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    auto ipAndPort = static_cast<QPair<QHostAddress, quint16> *>(user);
    gameSocket->writeDatagram(buf, len, ipAndPort->first, ipAndPort->second);
    return 0;
}

bool Game::isGameOver() const
{
    return gameOverFlag;
}

