//
// Created by 666 on 2021/4/1.
//

#include "server.h"
#include <QsLog.h>
#include <QtCore/QCoreApplication>
#include "global.h"

QUdpSocket *Server::serverSocket;

Server::Server(QObject *parent) : QObject(parent), simpleCrypt(global::privateKey)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/tetris.db");
    if (!db.open())
    {
        QLOG_FATAL() << "数据库连接失败" << QCoreApplication::applicationDirPath();
    }
    QSqlQuery query(db);
    query.exec(
            "CREATE TABLE IF NOT EXISTS account(id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL UNIQUE, password TEXT NOT NULL, alias TEXT NOT NULL UNIQUE,rankpoint INTEGER DEFAULT (0), avatar TEXT)");

    serverSocket = new QUdpSocket(this);
    if (serverSocket->bind(global::serverPort))
    {
        QLOG_INFO() << "成功绑定在端口 :" << serverSocket->localPort();
    }
    else
    {
        //perror("Bind error");
        QLOG_FATAL() << "端口绑定失败 port = " << global::serverPort;
        throw std::runtime_error("Bind port error");
    }
    connect(serverSocket, &QUdpSocket::readyRead, this, &Server::readPendingDatagrams);

    kcpTimer = new QTimer(this);
    connect(kcpTimer, SIGNAL(timeout()), this, SLOT(netSlap()));
    kcpTimer->start(100);

    keepAliveTimer = new QTimer(this);
    connect(keepAliveTimer, SIGNAL(timeout()), this, SLOT(HeartBeat()));
    keepAliveTimer->start(3000);

}

Server::~Server()
{
    if (db.isOpen())db.close();
    delete kcpTimer;
    delete keepAliveTimer;
}

ikcpcb *Server::kcpClientCreate(QPair<QHostAddress, quint16> *ipAndPort)
{
    ikcpcb *kcpClient;
    kcpClient = ikcp_create(global::sessionConv, ipAndPort);
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

void Server::deleteClient(ikcpcb *kcpClient)
{
    QLOG_DEBUG() << "delete client and remove from queue";
    for (auto i = game1v1q.begin(); i != game1v1q.end(); i++)
    {
        if (*i == kcpClient)
        {
            game1v1q.erase(i);
            break;
        }
    }
    for (auto i = game9q.begin(); i != game9q.end(); i++)
    {
        if (*i == kcpClient)
        {
            game9q.erase(i);
            break;
        }
    }
    auto *ipAndPort = static_cast<QPair<QHostAddress, quint16> *>(kcpClient->user);
    kcpClientMap.remove(*ipAndPort);
    delete ipAndPort;
    delete kcpClient;
}

void Server::netSlap()
{
    static QByteArray byteArray;
    quint32 current = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    for (auto &iter : kcpClientMap)
    {
        auto kcpClient = iter.first;
        auto &aliveCounter = iter.second;
        ikcp_update(kcpClient, current);
        while (true)
        {
            if (byteArray.size() < ikcp_peeksize(kcpClient))
                byteArray.resize(ikcp_peeksize(kcpClient));
            int dataSize = ikcp_recv(kcpClient, byteArray.data(), byteArray.size());
            if (dataSize < 0) break;

            proto::Information information;

            if (information.ParseFromArray(byteArray.data(), dataSize))
            {
                if (information.type_case() != proto::Information::kConnect && aliveCounter <= 0)
                {
                    QLOG_TRACE() << "send error ";
                    information.set_error(0);
                    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                    ikcp_flush(kcpClient);
                    deleteClient(kcpClient);
                    return;//调用deleteClient删除迭代器后不能再循环
                }
                switch (information.type_case())
                {
                    case proto::Information::kConnect:
                    {
                        QLOG_TRACE() << "Receive connect ";
                        QLOG_TRACE() << "客户端连接成功";
                        information.set_connect(1);
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        aliveCounter = 3;
                        break;
                    }
                    case proto::Information::kDisconnect:
                    {
                        QLOG_TRACE() << "Receive disconnect ";
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        ikcp_flush(kcpClient);
                        deleteClient(kcpClient);
                        return;//调用deleteClient删除迭代器后不能再循环
                    }
                    case proto::Information::kKeepAlive:
                    {
                        QLOG_TRACE() << "Receive keepAlive ";
                        if (aliveCounter > 0)aliveCounter = 3;
                        break;
                    }
                    case proto::Information::kSignup:
                    {
                        QLOG_TRACE() << "Receive signup ";
                        auto &signup = *information.mutable_signup();
                        QSqlQuery query(db);
                        query.prepare("select * from account where username = ?");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(signup.username())));
                        query.exec();
                        if (query.next())
                        {
                            signup.set_status(proto::Account_Status_username_in_Used);
                            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                            return;
                        }
                        query.prepare("select * from account where alias = ?");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(signup.alias())));
                        query.exec();
                        if (query.next())
                        {
                            signup.set_status(proto::Account_Status_alias_in_used);
                            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                            return;
                        }

                        query.prepare("insert into account (username,password,alias,avatar) values (?,?,?,?)");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(signup.username())));
                        query.bindValue(1, simpleCrypt.decryptToString(QString::fromStdString(signup.password())));
                        query.bindValue(2, simpleCrypt.decryptToString(QString::fromStdString(signup.alias())));
                        query.bindValue(3, QString::fromStdString(signup.avatar()));
                        if (!query.exec())
                        {
                            QLOG_FATAL() << "insert error";
                        }
                        signup.set_status(proto::Account_Status_success);
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        break;
                    }
                    case proto::Information::kLogin:
                    {
                        QLOG_TRACE() << "Receive login ";
                        QLOG_TRACE() << "before decode username" << information.login().username().data();
                        QLOG_TRACE() << "before decode password" << information.login().password().data();
                        QLOG_TRACE() << "after decode username" << simpleCrypt.decryptToString(
                                    QString::fromStdString(information.login().username()));
                        QLOG_TRACE() << "after decode password" << simpleCrypt.decryptToString(
                                    QString::fromStdString(information.login().password()));
                        auto &login = *information.mutable_login();
                        QSqlQuery query(db);
                        query.prepare("select * from account where username = ? and password = ?");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(login.username())));
                        query.bindValue(1, simpleCrypt.decryptToString(QString::fromStdString(login.password())));
                        query.exec();
                        if (query.next())
                        {
                            login.mutable_alias()->assign(
                                    simpleCrypt.encryptToString(query.value("alias").toString()).toStdString());
                            login.mutable_avatar()->assign(query.value("avatar").toString().toStdString());
                            login.set_rankpoint(query.value("rankpoint").toInt());
                            login.set_status(proto::Account_Status_success);
                            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        }
                        else
                        {
                            login.set_status(proto::Account_Status_failed);
                            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        }
                        break;
                    }

                    case proto::Information::kAlter:
                    {
                        QLOG_TRACE() << "Receive alter ";
                        auto &alter = *information.mutable_alter();
                        QSqlQuery query(db);
                        if (!(simpleCrypt.decryptToString(QString::fromStdString(alter.username())) ==
                              simpleCrypt.decryptToString(QString::fromStdString(alter.original_username()))))
                        {
                            query.prepare("select * from account where username = ?");
                            query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(alter.username())));
                            query.exec();
                            if (query.next())
                            {
                                alter.set_status(proto::Account_Status_username_in_Used);
                                ikcp_send(kcpClient, information.SerializeAsString().data(),
                                          information.ByteSizeLong());
                                return;
                            }
                        }
                        if (!(simpleCrypt.decryptToString(QString::fromStdString(alter.alias())) ==
                              simpleCrypt.decryptToString(QString::fromStdString(alter.original_alias()))))
                        {
                            query.prepare("select * from account where alias = ?");
                            query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(alter.alias())));
                            query.exec();
                            if (query.next())
                            {
                                alter.set_status(proto::Account_Status_alias_in_used);
                                ikcp_send(kcpClient, information.SerializeAsString().data(),
                                          information.ByteSizeLong());
                                return;
                            }
                        }


                        query.prepare(
                                "update account set  username = ?,password=?,alias=?,avatar=? where username =? and password=?");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(alter.username())));
                        query.bindValue(1, simpleCrypt.decryptToString(QString::fromStdString(alter.password())));
                        query.bindValue(2, simpleCrypt.decryptToString(QString::fromStdString(alter.alias())));
                        query.bindValue(3, QString::fromStdString(alter.avatar()));
                        query.bindValue(4,
                                        simpleCrypt.decryptToString(QString::fromStdString(alter.original_username())));
                        query.bindValue(5,
                                        simpleCrypt.decryptToString(QString::fromStdString(alter.original_password())));
                        if (!query.exec())
                        {
                            QLOG_FATAL() << "update error";
                        }
                        alter.set_status(proto::Account_Status_success);
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        break;
                    }
                    case proto::Information::kPoints:
                    {
                        QLOG_TRACE() << "Receive kPoints ";
                        auto &points = *information.mutable_points();
                        QSqlQuery query(db);

                        query.prepare("update account set rankpoint = rankpoint +? where username =? and password=?");
                        query.bindValue(0, points.rankpoint());
                        query.bindValue(1, simpleCrypt.decryptToString(QString::fromStdString(points.username())));
                        query.bindValue(2, simpleCrypt.decryptToString(QString::fromStdString(points.password())));

                        if (!query.exec())
                        {
                            QLOG_FATAL() << "kPoints error";
                        }
                        query.prepare("select rankpoint from account where username =? and password=?");
                        query.bindValue(0, simpleCrypt.decryptToString(QString::fromStdString(points.username())));
                        query.bindValue(1, simpleCrypt.decryptToString(QString::fromStdString(points.password())));
                        query.exec();
                        if (query.next())
                        {
                            points.set_rankpoint(query.value("rankpoint").toInt());
                            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        }
                        points.set_status(proto::Account_Status_success);
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        break;
                    }
                    case proto::Information::kMessage:
                    {
                        QLOG_TRACE() << "Receive message ";
                        for (auto &e:kcpClientMap)
                        {
                            ikcp_send(e.first, information.SerializeAsString().data(), information.ByteSizeLong());
                        }
                        break;
                    }
                    case proto::Information::kGame9:
                    {
                        QLOG_TRACE() << "Receive kGame9 ";
                        /*{//单人测试
                            game9q.push_back(kcpClient);
                            Game *game = new Game(9);
                            information.set_game9(game->getGamePort());

                            ikcp_send(game9q.front(), information.SerializeAsString().data(),
                                      information.ByteSizeLong());
                            game9q.pop_front();

                            games.push_back(game);
                        }*/
                        game9q.push_back(kcpClient);
                        if (game9q.size() >= 9)
                        {
                            Game *game = new Game(9);
                            information.set_game9(game->getGamePort());
                            for (int i = 0; i < 9; i++)
                            {
                                ikcp_send(game9q.front(), information.SerializeAsString().data(),
                                          information.ByteSizeLong());
                                game9q.pop_front();
                            }
                            games.push_back(game);
                        }
                        break;
                    }
                    case proto::Information::kGame1V1:
                    {
                        QLOG_TRACE() << "Receive kGame1V1 ";
                        game1v1q.push_back(kcpClient);
                        if (game1v1q.size() >= 2)
                        {
                            Game *game = new Game(2);
                            information.set_game1v1(game->getGamePort());
                            for (int i = 0; i < 2; i++)
                            {
                                ikcp_send(game1v1q.front(), information.SerializeAsString().data(),
                                          information.ByteSizeLong());
                                game1v1q.pop_front();
                            }
                            games.push_back(game);
                        }
                        break;
                    }
                    case proto::Information::kGameSingle:
                    {
                        QLOG_TRACE() << "Receive kGameSingle ";

                        Game *game = new Game(1);
                        information.set_game_single(game->getGamePort());
                        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
                        games.push_back(game);
                        break;
                    }
                    case proto::Information::TYPE_NOT_SET:
                        break;
                }
            }
            else QLOG_ERROR() << "protobuf 解析错误";
        }
    }
}

void Server::readPendingDatagrams()
{
    while (serverSocket->hasPendingDatagrams())
    {
        auto datagram = serverSocket->receiveDatagram();
        auto recvIpAndPort = qMakePair(datagram.senderAddress(), datagram.senderPort());
        auto kcpClientAndAliveCounter = qMakePair(kcpClientCreate(new QPair<QHostAddress, quint16>(recvIpAndPort)), 0);
        if (!kcpClientMap.contains(recvIpAndPort))
        {
            kcpClientMap.insert(recvIpAndPort, kcpClientAndAliveCounter);
        }
        ikcp_input(kcpClientMap[recvIpAndPort].first, datagram.data(), datagram.data().size());
    }
}

int Server::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    auto ipAndPort = static_cast<QPair<QHostAddress, quint16> *>(user);
    serverSocket->writeDatagram(buf, len, ipAndPort->first, ipAndPort->second);
    return 0;
}

void Server::HeartBeat()
{
    //将已经结束的游戏释放
    for (auto iter = games.begin(); iter != games.end();)
    {
        if ((*iter)->isGameOver())
        {
            QLOG_TRACE() << "delete game";
            delete *iter;
            iter = games.erase(iter);
            continue;
        }
        iter++;
    }
    //递减所有用户的心跳计数 并将超时的客户端连接释放
    QVector<ikcpcb *> deleteVec;
    for (auto iter = kcpClientMap.begin(); iter != kcpClientMap.end(); iter++)
    {
        const auto &ipAndPort = iter.key();
        auto kcpClient = iter.value().first;
        auto &aliveCounter = iter.value().second;
        if (aliveCounter < 0)
        {
            QLOG_TRACE() << "send error ";
            proto::Information information;
            information.set_error(0);
            ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
            ikcp_flush(kcpClient);
            deleteVec.push_back(kcpClient);
        }
        aliveCounter--;
    }
    for (auto e : deleteVec)
    {
        deleteClient(e);
    }

    QLOG_INFO() << "--------------------------------------------------------------------";
    QLOG_INFO() << "目前正在进行的游戏数目=" << games.size() << "||" << games;
    QLOG_INFO() << "目前已连接的客户端数目=" << kcpClientMap.size() << "||" << kcpClientMap;
    QLOG_INFO() << "目前正在1v1对决中匹配的玩家数目=" << game1v1q.size() << "||" << game1v1q;
    QLOG_INFO() << "目前正在9人乱斗中匹配的玩家数目=" << game9q.size() << "||" << game9q;
}




