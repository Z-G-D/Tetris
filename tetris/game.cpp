//
// Created by 666 on 2021/4/8.
//

#include <QNetworkDatagram>
#include <QDateTime>
#include <QsLog.h>
#include "game.h"
#include "global.h"

const QColor Player::color[7]
        {
                Qt::cyan, Qt::yellow, QColor(160, 32, 240)/*purple*/, Qt::green, Qt::red, QColor(255, 165, 0), Qt::blue
        };

const QVector<QPoint> Player::block[7/*方块类型 IOTSZJL */][4/*四种方向 0,R,2,L*/] =
        {
                {/*方块I*/
                        {
                                {0, 1}, {1, 1}, {2, 1}, {3, 1}
                        },
                        {
                                {2, 0}, {2, 1}, {2, 2}, {2, 3}
                        },
                        {
                                {0, 2}, {1, 2}, {2, 2}, {3, 2}
                        },
                        {
                                {1, 0}, {1, 1}, {1, 2}, {1, 3}
                        }
                },
                {/*方块O*/
                        {
                                {0, 1}, {0, 2}, {1, 1}, {1, 2}
                        },
                        {
                                {0, 1}, {0, 2}, {1, 1}, {1, 2}
                        },
                        {
                                {0, 1}, {0, 2}, {1, 1}, {1, 2}
                        },
                        {
                                {0, 1}, {0, 2}, {1, 1}, {1, 2}
                        }
                },
                {/*方块T*/
                        {
                                {0, 2}, {1, 1}, {1, 2}, {2, 2}
                        },
                        {
                                {1, 1}, {1, 2}, {1, 3}, {2, 2}
                        },
                        {
                                {0, 2}, {1, 2}, {1, 3}, {2, 2}
                        },
                        {
                                {0, 2}, {1, 1}, {1, 2}, {1, 3}
                        }
                },
                {/*方块s*/
                        {
                                {0, 1}, {1, 1}, {1, 2}, {2, 2}
                        },
                        {
                                {1, 2}, {1, 3}, {2, 1}, {2, 2}
                        },
                        {
                                {0, 2}, {1, 2}, {1, 3}, {2, 3}
                        },
                        {
                                {0, 2}, {0, 3}, {1, 1}, {1, 2}
                        }
                },
                {/*方块Z*/
                        {
                                {0, 2}, {1, 1}, {1, 2}, {2, 1}
                        },
                        {
                                {1, 1}, {1, 2}, {2, 2}, {2, 3}
                        },
                        {
                                {0, 3}, {1, 2}, {1, 3}, {2, 2}
                        },
                        {
                                {0, 1}, {0, 2}, {1, 2}, {1, 3}
                        }
                },
                {/*方块J*/
                        {
                                {0, 2}, {1, 2}, {2, 1}, {2, 2}
                        },
                        {
                                {1, 1}, {1, 2}, {1, 3}, {2, 3}
                        },
                        {
                                {0, 2}, {0, 3}, {1, 2}, {2, 2}
                        },
                        {
                                {0, 1}, {1, 1}, {1, 2}, {1, 3}
                        }
                },
                {/*方块L*/
                        {
                                {0, 1}, {0, 2}, {1, 2}, {2, 2}
                        },
                        {
                                {1, 1}, {1, 2}, {1, 3}, {2, 1}
                        },
                        {
                                {0, 2}, {1, 2}, {2, 2}, {2, 3}
                        },
                        {
                                {0, 3}, {1, 1}, {1, 2}, {1, 3}
                        }
                }
        };


const QPoint Player::wallKickDataOfOJLSTZ[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test坐标*/] =
        {
                /*
                0->R	( 0, 0)	(-1, 0)	(-1,+1)	( 0,-2)	(-1,-2)
                R->0	( 0, 0)	(+1, 0)	(+1,-1)	( 0,+2)	(+1,+2)
                R->2	( 0, 0)	(+1, 0)	(+1,-1)	( 0,+2)	(+1,+2)
                2->R	( 0, 0)	(-1, 0)	(-1,+1)	( 0,-2)	(-1,-2)
                2->L	( 0, 0)	(+1, 0)	(+1,+1)	( 0,-2)	(+1,-2)
                L->2	( 0, 0)	(-1, 0)	(-1,-1)	( 0,+2)	(-1,+2)
                L->0	( 0, 0)	(-1, 0)	(-1,-1)	( 0,+2)	(-1,+2)
                0->L	( 0, 0)	(+1, 0)	(+1,+1)	( 0,-2)	(+1,-2)
                */
                {   //0
                        {{0, 0}, {-1, 0}, {-1, 1},  {0, -2}, {-1, -2}},  //R
                        {{0, 0}, {1,  0}, {1,  1},  {0, -2}, {1,  -2}}   //L
                },
                {   //R
                        {{0, 0}, {1,  0}, {1,  -1}, {0, 2},  {1,  2}},  //2
                        {{0, 0}, {1,  0}, {1,  -1}, {0, 2},  {1,  2}}   //0
                },
                {   //2
                        {{0, 0}, {1,  0}, {1,  1},  {0, -2}, {1,  -2}},  //L
                        {{0, 0}, {-1, 0}, {-1, 1},  {0, -2}, {-1, -2}}   //R
                },
                {   //L
                        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},  //0
                        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2},  {-1, 2}}   //2
                }
        };
const QPoint Player::wallKickDataOfI[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test坐标*/] =
        {
                /*
                0->R	( 0, 0)	(-2, 0)	(+1, 0)	(-2,-1)	(+1,+2)
                R->0	( 0, 0)	(+2, 0)	(-1, 0)	(+2,+1)	(-1,-2)
                R->2	( 0, 0)	(-1, 0)	(+2, 0)	(-1,+2)	(+2,-1)
                2->R	( 0, 0)	(+1, 0)	(-2, 0)	(+1,-2)	(-2,+1)
                2->L	( 0, 0)	(+2, 0)	(-1, 0)	(+2,+1)	(-1,-2)
                L->2	( 0, 0)	(-2, 0)	(+1, 0)	(-2,-1)	(+1,+2)
                L->0	( 0, 0)	(+1, 0)	(-2, 0)	(+1,-2)	(-2,+1)
                0->L	( 0, 0)	(-1, 0)	(+2, 0)	(-1,+2)	(+2,-1)
                */
                {   //0
                        {{0, 0}, {-2, 0}, {1,  0}, {-2, -1}, {1,  2}},  //R
                        {{0, 0}, {-1, 0}, {2,  0}, {-1, 2},  {2,  -1}}   //L
                },
                {   //R
                        {{0, 0}, {-1, 0}, {2,  0}, {-1, 2},  {2,  -1}},  //2
                        {{0, 0}, {2,  0}, {-1, 0}, {2,  1},  {-1, -2}}   //0
                },
                {   //2
                        {{0, 0}, {2,  0}, {-1, 0}, {2,  1},  {-1, -2}},  //L
                        {{0, 0}, {1,  0}, {-2, 0}, {1,  -2}, {-2, 1}}   //R
                },
                {   //L
                        {{0, 0}, {1,  0}, {-2, 0}, {1,  -2}, {-2, 1}},  //0
                        {{0, 0}, {-2, 0}, {1,  0}, {-2, -1}, {1,  2}}   //2
                }
        };

Player::Player(qint32 seed) : isGameOver(false), holdBlockType(-1), stackCounter(0), damageCounter(0)
{
    randomGenerator.seed(seed);
    // 初始化游戏地图
    gameMap.resize(global::gameHeight + global::externLine);
    for (auto &line :gameMap)
    {
        line.resize(global::gameWidth);
    }

    getNextBlock();
    holdBlockType = bagOf7.front();
    bagOf7.pop_front();
}

void Player::clockwiseRotation()
{
    if (isGameOver)return;
    blockDirection = (blockDirection + 1) % 4;
    if (blockType == 0)//I
    {
        for (int i = 0; i < 5; i++)
        {
            QPoint dPoint = {wallKickDataOfI[(blockDirection + 3) % 4][0][i].x(),
                             -wallKickDataOfI[(blockDirection + 3) % 4][0][i].y()};
            QLOG_TRACE() << blockDirection << dPoint;
            if (collisionDetection(dPoint))
            {
                position += dPoint;
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            QPoint dPoint = {wallKickDataOfOJLSTZ[(blockDirection + 3) % 4][0][i].x(),
                             -wallKickDataOfOJLSTZ[(blockDirection + 3) % 4][0][i].y()};
            QLOG_TRACE() << blockDirection << dPoint;
            if (collisionDetection(dPoint))
            {
                position += dPoint;
                return;
            }
        }
    }
    blockDirection = (blockDirection + 3) % 4;

}

void Player::counterClockwiseRotation()
{
    if (isGameOver)return;
    blockDirection = (blockDirection + 3) % 4;
    if (blockType == 0)//I
    {
        for (int i = 0; i < 5; i++)
        {
            QPoint dPoint = {wallKickDataOfI[(blockDirection + 1) % 4][1][i].x(),
                             -wallKickDataOfI[(blockDirection + 1) % 4][1][i].y()};
            QLOG_TRACE() << blockDirection << dPoint;
            if (collisionDetection(dPoint))
            {
                position += dPoint;
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            QPoint dPoint = {wallKickDataOfOJLSTZ[(blockDirection + 1) % 4][1][i].x(),
                             -wallKickDataOfOJLSTZ[(blockDirection + 1) % 4][1][i].y()};
            QLOG_TRACE() << blockDirection << dPoint;
            if (collisionDetection(dPoint))
            {
                position += dPoint;
                return;
            }
        }
    }
    blockDirection = (blockDirection + 1) % 4;
}

const QColor &Player::getColor(qint32 i) const
{
    if (i)
    {
        return color[bagOf7[i - 1]];
    }
    return color[blockType];
}

bool Player::collisionDetection(QPoint dPoint) const
{

    for (QPoint point: getBlock(0))
    {
        auto testPoint = position + point + dPoint;
        if (testPoint.x() < 0 || testPoint.x() >= global::gameWidth ||
            testPoint.y() < 0 || testPoint.y() >= global::gameHeight + global::externLine ||
            gameMap[testPoint.y()][testPoint.x()].first)
        {
            return false;
        }
    }
    return true;
}

void Player::getNextBlock()
{
    //初始化7bag机制，获得7个方块的随机排序
    if (bagOf7.size() < 7)
    {
        qint32 bag[7]{0, 1, 2, 3, 4, 5, 6};
        //每次选出7中方块中的一种放在位置i
        for (int i = 0; i < 6; i++)
        {
            std::swap(bag[i], bag[randomGenerator.bounded(i, 6)]);
        }
        for (auto i : bag)
        {
            bagOf7.push_back(i);
        }
    }
    //将方块的位置回复到初始位置，并初始化block的各种属性
    position = global::homePosition;
    blockDirection = 0;
    blockType = bagOf7.front();
    bagOf7.pop_front();
    //当方块碰撞则上移，不造成玩家即死，可以上移至externLine中
    while (!collisionDetection({0, 0}))
    {
        position += {0, -1};
    }
}


bool Player::isDead()
{
    //判断最上面一行是否存在方块
    for (auto item :gameMap[global::externLine - 1])
    {
        if (item.first)
        {
            return isGameOver = true;
        }
    }
    return false;
}

const QVector<QPoint> &Player::getBlock(quint32 i) const
{
    if (i)
    {
        return block[bagOf7[i - 1]][0];//获取7bag中的第i-1个方块
    }
    return block[blockType][blockDirection];//获取当前方块
}

const QVector<QPoint> &Player::getHoldBlock() const
{
    return block[holdBlockType][0];
}

void Player::left()
{
    if (isGameOver)return;
    if (collisionDetection({-1, 0}))
    {
        position += {-1, 0};
    }
}

void Player::right()
{
    if (isGameOver)return;
    if (collisionDetection({1, 0}))
    {
        position += {1, 0};
    }
}

void Player::softDrop()
{
    if (isGameOver)return;
    if (collisionDetection({0, 1}))
    {
        position += {0, +1};
    }
}

void Player::swapBlock()
{
    if (isGameOver)return;
    std::swap(blockType, holdBlockType);
    position = global::homePosition;
    blockDirection = 0;
    //当位置初始化为homePosition仍然碰撞，就将方块上移至externalline中，避免玩家即死
    while (!collisionDetection({0, 0}))
    {
        position += {0, -1};
    }
}

void Player::hardDrop()
{
    if (isGameOver)return;
    auto heightFromBottom = getHeightFromBottom();
    //将方块锁定在当前位置的正下方
    for (QPoint point:getBlock(0))
    {
        gameMap[position.y() + point.y() + heightFromBottom][position.x() + point.x()] = {true, getColor(0)};
    }
    int clearLine = 0;//计数本次一共消除的行数
    for (int line = 0; line != gameMap.size(); line++)
    {
        bool isAllFull = true;//本行是或否全满
        for (auto &item: gameMap[line])
        {
            if (!item.first)
            {
                isAllFull = false;
                break;
            }
        }
        //全满则消除
        if (isAllFull)
        {
            clearLine++;
            //将本行及以上的行往下移动一格
            for (auto i = line; i != 0; i--)
            {
                gameMap[i] = gameMap[i - 1];
            }
        }
    }
    if (isDead())return;
    //消行伤害计算
    switch (clearLine)
    {
        case 1:
            stackCounter -= 1;
            break;
        case 2:
            stackCounter -= 3;
            break;
        case 3:
            stackCounter -= 5;
            break;
        case 4:
            stackCounter -= 7;
            break;
    }
    getNextBlock();
}


qint32 Player::getHeightFromBottom() const
{
    qint32 ret = 0;
    while (collisionDetection({0, ret + 1}))
        ret++;
    return ret;
}

const QVector<QVector<QPair<bool, QColor>>> &Player::getGameMap() const
{
    return gameMap;
}

const QPoint &Player::getPosition() const
{
    return position;
}

bool Player::isGamOver() const
{
    return isGameOver;
}

const QColor &Player::getHoldColor() const
{
    return color[holdBlockType];
}

void Player::stackClear()
{
    if (isGameOver)return;
    QLOG_INFO() << "stackClear() called stackCounter=" << stackCounter;
    //每次stackClear清除一半（向上取整）的stackCounter计数
    int n = (stackCounter + 1) / 2;
    stackCounter -= n;
    for (int i = 0; i < n; i++)
    {
        //判定stackClear是否造成玩家死亡，
        if (isDead())return;
        //往下塞一行黑色方块
        gameMap.push_back(QVector<QPair<bool, QColor>>(global::gameWidth, {true, global::stackColor}));
        gameMap.back()[randomGenerator.bounded(0, global::gameWidth - 1)] = {false, global::stackColor};
        gameMap.erase(gameMap.begin());
        //如果往下塞一行黑色方块造成方块碰撞，则将方块上移，不造成玩家即死
        if (!collisionDetection({0, 0}))
        {
            position += {0, -1};
        }
    }
}

qint32 Player::getStackCounter() const
{
    return stackCounter;
}

void Player::addStackCounter(qint32 i)
{
    stackCounter += i;
    if (stackCounter > global::stackSize)
    {
        stackClear();
    }
}

qint32 Player::getDamageCounter() const
{
    return damageCounter;
}

void Player::addDamageCounter(qint32 i)
{
    damageCounter += i;
}


QUdpSocket *Game::udpSocket = nullptr;

Game::Game()
{
    udpSocket = new QUdpSocket(this);//用于与连接的客户端通讯的QTcpSocket
    connect(udpSocket, &QUdpSocket::readyRead, this, &Game::onSocketReadyRead);

    if (udpSocket->bind())//绑定端口成功
    {
        QLOG_TRACE() << "bind succeed on port:" << udpSocket->localPort();
    }
    else
    {
        throw std::runtime_error("Bind port error");
    }


    kcpTimer = new QTimer(this);
    connect(kcpTimer, SIGNAL(timeout()), this, SLOT(netSlap()));
    kcpTimer->start(10);

    lockDelayTimer = new QTimer(this);
    connect(lockDelayTimer, SIGNAL(timeout()), this, SLOT(lockDelay()));
    lockDelayTimer->setSingleShot(true);
    lockDelayTimer->setInterval(500);

    stackClearTimer = new QTimer(this);
    connect(stackClearTimer, SIGNAL(timeout()), this, SLOT(stackClear()));
    stackClearTimer->setInterval(10 * 1000);

    autoFallTimer = new QTimer(this);
    connect(autoFallTimer, SIGNAL(timeout()), this, SLOT(autoFall()));
    autoFallTimer->start(800);

    kcpClient = ikcp_create(global::gameConv, nullptr);
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
    proto::Instruction instruction;
    instruction.set_ready_to_start(0);
    ikcp_send(kcpClient, instruction.SerializeAsString().data(), instruction.ByteSizeLong());
}

const QVector<Player> &Game::getPlayers() const
{
    return players;
}

qint32 Game::getPlayerNumber() const
{
    return playerNumber;
}


int Game::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    udpSocket->writeDatagram(buf, len, global::serverAddr, global::gamePort);
    return 0;
}

void Game::onSocketReadyRead()
{
    while (udpSocket->hasPendingDatagrams())
    {
        auto datagram = udpSocket->receiveDatagram();
        ikcp_input(kcpClient, datagram.data(), datagram.data().size());
    }
}

void Game::netSlap()
{
    ikcp_update(kcpClient, QDateTime::currentDateTime().currentMSecsSinceEpoch());
    static QByteArray byteArray;
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
                    QLOG_TRACE() << "Instruction = kClockwise = " << instruction.clockwise();
                    players[instruction.clockwise()].clockwiseRotation();

                    if (playerNumber == instruction.clockwise() && !players[instruction.clockwise()].getHeightFromBottom())
                        lockDelayTimer->start();
                    break;
                case proto::Instruction::kCounterclockwise:
                    QLOG_TRACE() << "Instruction = kCounterclockwise = " << instruction.counterclockwise();
                    players[instruction.counterclockwise()].counterClockwiseRotation();

                    if (playerNumber == instruction.counterclockwise() && !players[instruction.counterclockwise()].getHeightFromBottom())
                        lockDelayTimer->start();
                    break;
                case proto::Instruction::kDown:
                    //QLOG_TRACE() << "Instruction = kDown = " << instruction.down();
                    players[instruction.down()].softDrop();

                    if (playerNumber == instruction.down() && !players[instruction.down()].getHeightFromBottom())
                        lockDelayTimer->start();
                    break;
                case proto::Instruction::kRight:
                    QLOG_TRACE() << "Instruction = kRight = " << instruction.right();
                    players[instruction.right()].right();

                    if (playerNumber == instruction.right() && !players[instruction.right()].getHeightFromBottom())
                        lockDelayTimer->start();
                    break;
                case proto::Instruction::kLeft:
                    QLOG_TRACE() << "Instruction = kLeft = " << instruction.left();
                    players[instruction.left()].left();

                    if (playerNumber == instruction.left() && !players[instruction.left()].getHeightFromBottom())
                        lockDelayTimer->start();

                    break;
                case proto::Instruction::kDrop:
                {
                    QLOG_TRACE() << "Instruction = kDrop = " << instruction.drop();
                    lockDelayTimer->stop();
                    players[instruction.drop()].hardDrop();

                    if (players.size() == 1)//单人模式
                    {
                        qint32 damage = -players[instruction.drop()].getStackCounter();
                        players[playerNumber].addDamageCounter(damage);
                        players[playerNumber].addStackCounter(damage);
                    }
                    else//多人模式
                    {
                        qint32 damage = -players[instruction.drop()].getStackCounter();
                        quint32 aliveCount = getPlayerAliveCount();
                        if (damage > 0 && aliveCount > 1 && damage >= aliveCount - 1)
                        {
                            players[instruction.drop()].addDamageCounter(damage);
                            for (int i = 0; i < players.size(); i++)
                            {
                                if (players[i].isGamOver())
                                    continue;
                                if (i == instruction.drop())
                                    players[i].addStackCounter(damage);
                                else
                                    players[i].addStackCounter(damage / (aliveCount - 1));
                            }
                            if (players[playerNumber].getStackCounter() > 0)
                            {
                                stackClearTimer->start();
                            }
                        }
                    }

                    if (playerNumber == instruction.drop() && !players[instruction.drop()].getHeightFromBottom())
                        lockDelayTimer->start();
                    break;
                }
                case proto::Instruction::kRandomSeed:
                    QLOG_TRACE() << "Instruction = kRandomSeed kRandomSeed = " << instruction.random_seed();
                    players.push_back(Player(instruction.random_seed()));
                    break;
                case proto::Instruction::kReadyToStart:
                    QLOG_TRACE() << "Instruction = kReadyToStart playerNumber = " << instruction.ready_to_start();
                    playerNumber = instruction.ready_to_start();
                    global::isGameStart = true;
                    break;
                case proto::Instruction::kSwapBlock:
                    QLOG_TRACE() << "Instruction = kSwap playerNumber = " << instruction.swap_block();
                    players[instruction.swap_block()].swapBlock();
                    break;
                case proto::Instruction::kStackClear:
                    QLOG_TRACE() << "Instruction = kStackClear playerNumber = " << instruction.stack_clear();
                    players[instruction.stack_clear()].stackClear();
                    break;
                case proto::Instruction::kGameOver:
                case proto::Instruction::TYPE_NOT_SET:
                    break;
            }
        }
        else QLOG_ERROR() << "protobuf 解析错误";
    }
}

ikcpcb *Game::getKcpClient()
{
    return kcpClient;
}

void Game::lockDelay()
{
    if (players[playerNumber].isGamOver())return;
    proto::Instruction instruction;
    instruction.set_drop(playerNumber);
    ikcp_send(kcpClient, instruction.SerializeAsString().data(), instruction.ByteSizeLong());
}

void Game::stackClear()
{
    if (players[playerNumber].isGamOver())return;
    proto::Instruction instruction;
    instruction.set_stack_clear(playerNumber);
    ikcp_send(kcpClient, instruction.SerializeAsString().data(), instruction.ByteSizeLong());
}

void Game::autoFall()
{
    if (!global::isGameStart || players[playerNumber].isGamOver())
        return;
    proto::Instruction instruction;
    instruction.set_down(playerNumber);
    ikcp_send(kcpClient, instruction.SerializeAsString().data(), instruction.ByteSizeLong());
}

void Game::gameOver() const
{
    autoFallTimer->stop();
    lockDelayTimer->stop();
    stackClearTimer->stop();
}

quint32 Game::getPlayerAliveCount()
{
    quint32 aliveSize = 0;
    for (auto &player:players)
    {
        if (!player.isGamOver())
            aliveSize++;
    }
    return aliveSize;
}

Player &Game::getCurrentPlayer()
{
    return players[playerNumber];
}


