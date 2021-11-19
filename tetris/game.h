//
// Created by 666 on 2021/4/8.
//

#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <QPainter>
#include <QUdpSocket>
#include <QTimer>
#include <QtCore/QQueue>
#include <ikcp.h>
#include <tetris.pb.h>
#include <QRandomGenerator>


class Player
{
public:
    explicit Player(qint32 seed);

    void clockwiseRotation();

    void counterClockwiseRotation();

    void left();

    void right();

    void softDrop();//软降，下降一格

    void hardDrop();//硬降,将当前方块下落至底部

    void swapBlock();//切换当前方块和hold槽中的方块，并将方块各项属性初始化

    void stackClear();

    qint32 getHeightFromBottom() const;

    //输入0获取当前方块，否则获得7bag中第i-1个方块
    const QVector<QPoint> &getBlock(quint32 i) const;

    const QVector<QPoint> &getHoldBlock() const;

    const QPoint &getPosition() const;

    //输入0获取当前方块颜色，否则获得7bag中第i-1个方块颜色
    const QColor &getColor(qint32 i) const;

    const QColor &getHoldColor() const;

    const QVector<QVector<QPair<bool, QColor>>> &getGameMap() const;

    qint32 getStackCounter() const;

    //将伤害缓冲槽计数加上i，i>stackSize则出发stackClear
    void addStackCounter(qint32 i);

    qint32 getDamageCounter() const;

    //添加玩家的伤害得分
    void addDamageCounter(qint32 i);

    bool isGamOver() const;

private:
    bool isGameOver;
    QQueue<qint32> bagOf7;//玩家的获得的方块序列，7个一组更新
    QVector<QVector<QPair<bool, QColor>>> gameMap;//游戏地图，key是否存在在方块和方块的颜色
    QPoint position;//当前方块位置
    qint32 blockDirection;//当前方块方向
    qint32 blockType;//当前方块类型
    qint32 holdBlockType;//hold槽中方块类型
    qint32 stackCounter;//伤害缓冲槽计数，负数代表存储的输出伤害，表现为白色方块，正数代表缓和的遭受伤害，表现为黑色方块
    qint32 damageCounter;//造成伤害的计数，也是本局游戏的得分
    QRandomGenerator randomGenerator;

    bool collisionDetection(QPoint dPoint) const;//方块是否碰撞到其他方块或者墙体

    void getNextBlock();

    bool isDead();//判断玩家是否死亡

    static const QColor color[7/*方块类型 IOTSZJL */];
    static const QVector<QPoint> block[7/*方块类型 IOTSZJL */][4/*四种方向 0,R,2,L*/];
    static const QPoint wallKickDataOfOJLSTZ[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test*/];
    static const QPoint wallKickDataOfI[4/*四个初始方向*/][2/*两个旋转方向*/][5/*四个Test*/];
};

class Game : public QObject
{
Q_OBJECT
public:
    Game();

    const QVector<Player> &getPlayers() const;

    qint32 getPlayerNumber() const;

    ikcpcb *getKcpClient();

    void gameOver() const;

    quint32 getPlayerAliveCount();

    Player& getCurrentPlayer();
private:
    QVector<Player> players;//游戏中所有玩家对象
    qint32 playerNumber;//当前玩家编号
    static QUdpSocket *udpSocket;

    QTimer *stackClearTimer;
    QTimer *lockDelayTimer;
    QTimer *autoFallTimer;
    QTimer *kcpTimer;
    ikcpcb *kcpClient;

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);



private slots:

    void onSocketReadyRead();

    void netSlap();

    void lockDelay();

    void stackClear();

    void autoFall();


};

#endif //TETRIS_GAME_H
