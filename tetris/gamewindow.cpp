#include <QPainter>
#include <QTime>
#include <QScreen>
#include <QSoundEffect>
#include <QsLog.h>
#include "gamewindow.h"
#include "global.h"

GameWindow::GameWindow(GameType gameType, QWidget *parent) :
        gameType(gameType),
        QWidget(parent),
        isGameOver(false)
{
    gameScreen = QGuiApplication::screens().front()->geometry();
    block = gameScreen.height() * 3 / 4 / global::gameHeight;
    sBlock = block * 3 / 4;

    frameTimer = new QTimer(this);
    connect(frameTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    frameTimer->start(33);

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(this->backgroundRole(),
                     QPixmap(":/image/background.png").scaled(gameScreen.width(), gameScreen.height(),
                                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setPalette(palette);
    mediaPlaylist = new QMediaPlaylist();
    mediaPlaylist->addMedia(QUrl("qrc:/music/DefenseBattle1.mp3"));
    mediaPlaylist->addMedia(QUrl("qrc:/music/DefenseBattle2.mp3"));
    mediaPlaylist->addMedia(QUrl("qrc:/music/DefenseBattle3.mp3"));
    mediaPlaylist->setCurrentIndex(1);
    mediaPlaylist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setPlaylist(mediaPlaylist);
}


GameWindow::~GameWindow()
{
    QLOG_DEBUG() << "~GameWindow Called";

    mediaPlayer->stop();
    delete frameTimer;
    delete mediaPlayer;
    delete mediaPlaylist;
}

void GameWindow::showEvent(QShowEvent *event)
{
    this->showFullScreen();
    setFocus();
    mediaPlayer->play();
}

void GameWindow::printInterface(const Player &player, QPainter &painter, QBrush &brush)
{
    brush.setColor(QColor(255, 255, 255, 200));
    painter.setBrush(brush);
    painter.drawRect(0, 0, block * global::gameWidth, block * global::gameHeight);

    painter.drawRect(-sBlock * 4, 0, sBlock * 4, sBlock * (1 + 4));
    painter.drawText(-sBlock * 4, sBlock, tr("hold"));

    painter.drawRect(-block, block * 4, block, block * 16);

    painter.drawRect(global::gameWidth * block, 0, sBlock * 4, sBlock * (1 + 5 * 4));
    painter.drawText(global::gameWidth * block, sBlock, tr("next"));

    painter.drawRect(global::gameWidth * block, block * 16, sBlock * 4, block * 4);
    painter.drawText(global::gameWidth * block, block * 16 + sBlock, tr("score"));
    painter.drawText(global::gameWidth * block, block * 16 + 4 * sBlock, QString::number(player.getDamageCounter()));
    //printHodeblock
    brush.setColor(player.getHoldColor());
    painter.setBrush(brush);
    for (auto &e:player.getHoldBlock())
    {
        painter.drawRect(-sBlock * 4 + e.x() * sBlock, sBlock * (e.y() + 1), sBlock, sBlock);
    }

    //printatkstack
    if (player.getStackCounter() > 0)
    {
        brush.setColor(global::stackColor);
        painter.setBrush(brush);
        for (int i = 0; i < player.getStackCounter(); i++)
        {
            painter.drawRect(-block, block * (15 - i + 4), block, block);
        }
    }
    else
    {
        brush.setColor(Qt::white);
        painter.setBrush(brush);
        for (int i = 0; i < -player.getStackCounter(); i++)
        {
            painter.drawRect(-block, block * (15 - i + 4), block, block);
        }
    }

    //printNextBlock
    for (int i = 0; i < 5; i++)
    {
        brush.setColor(player.getColor(i + 1));
        painter.setBrush(brush);
        for (auto &e:player.getBlock(i + 1))
        {
            painter.drawRect(global::gameWidth * block + e.x() * sBlock, (i * 4 + e.y() + 1) * sBlock, sBlock, sBlock);
        }
    }

    //printscorecounter
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    if (!global::isGameStart)return;
    QPainter painter(this);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    QFont font("微软雅黑", 15, QFont::Bold, true);
    font.setCapitalization(QFont::Capitalization::SmallCaps);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 10);
    painter.setFont(font);
    auto &players = game.getPlayers();


    qint32 game9other = 0;
    for (int n = 0; n < players.size(); n++)
    {
        painter.resetTransform();
        if (n == game.getPlayerNumber())//玩家自己
        {
            if (gameType == gameSingle)
            {
                painter.translate(gameScreen.width() / 2 - global::gameWidth * block / 2, block * 4);
                printInterface(players[n], painter, brush);
            }
            else if (gameType == game1v1)
            {
                painter.translate(gameScreen.width() / 2 - global::gameWidth * block - block * 4, block * 4);
                printInterface(players[n], painter, brush);
            }
            else if (gameType == game9)
            {
                painter.translate(gameScreen.width() / 2 - global::gameWidth * block / 2, block * 4);
                printInterface(players[n], painter, brush);
            }
            painter.translate(0, -block * 4);
            for (int i = 0; i < global::gameHeight + global::externLine; i++)
            {
                for (int j = 0; j < global::gameWidth; j++)
                {
                    if (players[n].getGameMap()[i][j].first)
                    {
                        brush.setColor(players[n].getGameMap()[i][j].second);
                        painter.setBrush(brush);
                        painter.drawRect(j * block, i * block, block, block);
                    }
                }
            }

            if (players[n].isGamOver())
            {
                QColor color(0, 0, 0, 100);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(0, 4 * block, block * global::gameWidth, block * global::gameHeight);
                font.setPointSize(40);
                painter.setFont(font);
                painter.drawText(0, 4 * block + block * global::gameHeight / 2 + 1, tr("game over"));
                font.setPointSize(15);
                painter.setFont(font);
                continue;
            }
            auto heightFromBottom = players[n].getHeightFromBottom();
            for (auto &e:players[n].getBlock(0))
            {
                auto position = players[n].getPosition();

                brush.setColor(players[n].getColor(0));
                painter.setBrush(brush);
                painter.drawRect((e.x() + position.x()) * block, (e.y() + position.y()) * block, block, block);

                if (heightFromBottom)
                {
                    QColor color(players[n].getColor(0));
                    color.setAlpha(100);
                    brush.setColor(color);
                    painter.setBrush(brush);
                    painter.drawRect((e.x() + position.x()) * block, (e.y() + position.y() + heightFromBottom) * block,
                                     block, block);
                }
            }
        }
        else//其他玩家
        {
            if (gameType == gameSingle)
            {
                QLOG_FATAL() << "error";
            }
            else if (gameType == game1v1)
            {
                painter.translate(gameScreen.width() / 2 + block * 4, block * 4);
                printInterface(players[n], painter, brush);
            }
            else if (gameType == game9)
            {
                painter.scale(0.5, 0.5);
                //sBlock=block/2;
                switch (game9other)
                {
                    case 0:
                        painter.translate(sBlock * 4, block * global::externLine);
                        break;
                    case 1:
                        painter.translate(sBlock * 12 + block * global::gameWidth, block * global::externLine);
                        break;
                    case 2:
                        painter.translate(sBlock * 4, block * (global::gameHeight + global::externLine * 2));
                        break;
                    case 3:
                        painter.translate(sBlock * 12 + block * global::gameWidth,
                                          block * (global::gameHeight + global::externLine * 2));
                        break;
                    case 4:
                        painter.translate(gameScreen.width() * 2 - global::gameWidth * block * 2 - 12 * sBlock,
                                          block * global::externLine);
                        break;
                    case 5:
                        painter.translate(gameScreen.width() * 2 - global::gameWidth * block - 4 * sBlock,
                                          block * global::externLine);
                        break;
                    case 6:
                        painter.translate(gameScreen.width() * 2 - global::gameWidth * block * 2 - 12 * sBlock,
                                          block * (global::gameHeight + global::externLine * 2));
                        break;
                    case 7:
                        painter.translate(gameScreen.width() * 2 - global::gameWidth * block - 4 * sBlock,
                                          block * (global::gameHeight + global::externLine * 2));
                        break;
                }
                game9other++;
                printInterface(players[n], painter, brush);
            }
            painter.translate(0, -block * 4);
            for (int i = 0; i < global::gameHeight + global::externLine; i++)
            {
                for (int j = 0; j < global::gameWidth; j++)
                {
                    if (players[n].getGameMap()[i][j].first)
                    {
                        brush.setColor(players[n].getGameMap()[i][j].second);
                        painter.setBrush(brush);
                        painter.drawRect(j * block, i * block, block, block);
                    }
                }
            }
            if (players[n].isGamOver())
            {
                QColor color(0, 0, 0, 100);
                brush.setColor(color);
                painter.setBrush(brush);
                painter.drawRect(0, 4 * block, block * global::gameWidth, block * global::gameHeight);
                font.setPointSize(40);
                painter.setFont(font);
                painter.drawText(0, 4 * block + block * global::gameHeight / 2 + 1, tr("game over"));
                font.setPointSize(15);
                painter.setFont(font);
                continue;
            }
            auto heightFromBottom = players[n].getHeightFromBottom();
            for (auto &e:players[n].getBlock(0))
            {
                auto position = players[n].getPosition();

                brush.setColor(players[n].getColor(0));
                painter.setBrush(brush);
                painter.drawRect((e.x() + position.x()) * block, (e.y() + position.y()) * block, block, block);
                if (heightFromBottom)
                {
                    QColor color(players[n].getColor(0));
                    color.setAlpha(100);
                    brush.setColor(color);
                    painter.setBrush(brush);
                    painter.drawRect((e.x() + position.x()) * block, (e.y() + position.y() + heightFromBottom) * block,
                                     block, block);
                }
            }
        }
    }

    auto alive = game.getPlayerAliveCount();
    if (gameType == GameType::gameSingle)
    {
        if (alive == 0)
        {
            isGameOver = true;
            painter.resetTransform();
            QColor color(0, 0, 0, 100);
            brush.setColor(color);
            painter.drawRect(0, 0, this->width(), this->height());
            font.setPointSize(100);
            painter.setFont(font);
            painter.drawText(gameScreen.width() / 4, gameScreen.height() / 2, tr("game over"));
        }
    }
    else
    {
        if (alive <= 1)
        {
            isGameOver = true;
            game.gameOver();
            painter.resetTransform();
            QColor color(0, 0, 0, 100);
            brush.setColor(color);
            painter.drawRect(0, 0, this->width(), this->height());
            font.setPointSize(100);
            painter.setFont(font);
            if (game.getCurrentPlayer().isGamOver())
            {
                painter.drawText(gameScreen.width() / 4, gameScreen.height() / 2, tr("you lost"));
            }
            else
            {
                painter.drawText(gameScreen.width() / 4, gameScreen.height() / 2, tr("you win"));
            }
        }
    }
}


void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (isGameOver)
    {
        close();
        return;
    }
    if (!global::isGameStart || game.getCurrentPlayer().isGamOver())
        return;
    proto::Instruction instruction;
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_D:
            instruction.set_clockwise(game.getPlayerNumber());
            break;
        case Qt::Key_A:
            instruction.set_counterclockwise(game.getPlayerNumber());
            break;
        case Qt::Key_Down:
        case Qt::Key_K:
            instruction.set_down(game.getPlayerNumber());
            break;
        case Qt::Key_Left:
        case Qt::Key_J:
            instruction.set_left(game.getPlayerNumber());
            break;
        case Qt::Key_Right:
        case Qt::Key_L:
            instruction.set_right(game.getPlayerNumber());
            break;
        case Qt::Key_S:
            instruction.set_swap_block(game.getPlayerNumber());
            break;
        case Qt::Key_Space:
            instruction.set_drop(game.getPlayerNumber());
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    ikcp_send(game.getKcpClient(), instruction.SerializeAsString().data(), instruction.ByteSizeLong());
}

void GameWindow::closeEvent(QCloseEvent *event)
{
    QLOG_DEBUG() << "send game over";
    proto::Instruction instruction;
    instruction.set_game_over(0);
    ikcp_send(game.getKcpClient(), instruction.SerializeAsString().data(), instruction.ByteSizeLong());
    points = game.getCurrentPlayer().getDamageCounter();
    ikcp_flush(game.getKcpClient());
    emit closeGame();
    global::mainWindow->show();
}

void GameWindow::updateFrame()
{
    this->update();
}











