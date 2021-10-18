#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimedia/QMediaPlayer>
#include <ikcp.h>
#include <tetris.pb.h>
#include "game.h"


class GameWindow : public QWidget
{
Q_OBJECT
public:
    qint32 points;
    enum GameType
    {
        gameSingle, game1v1, game9
    };

    explicit GameWindow(GameType gameType, QWidget *parent = nullptr);

    ~GameWindow() override;

signals :

    void closeGame();

private:
    GameType gameType;
    QRect gameScreen;
    Game game;
    int block;
    int sBlock;
    bool isGameOver;

    QMediaPlaylist *mediaPlaylist;
    QMediaPlayer *mediaPlayer;
    QTimer *frameTimer;

    void printInterface(const Player &player, QPainter &painter, QBrush &brush);

    void showEvent(QShowEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

private slots:

    void updateFrame();

};

#endif // GAMEWINDOW_H
