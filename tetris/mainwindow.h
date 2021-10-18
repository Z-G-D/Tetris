#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <ikcp.h>
#include "ui_mainwindow.h"
#include "simplecrypt.h"

namespace Ui
{
    class mainwindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    Ui::mainwindow *ui;
    static QUdpSocket *udpSocket;
    QTimer *timer;
    QTimer *aliveTimer;
    ikcpcb *kcpClient;
    SimpleCrypt simpleCrypt;
    QString username;
    QString password;
    QString alias;
    QPixmap avatar;

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);

    enum
    {
        kLoginPage = 0, kRegisterPage, kMainPage, kEditPage
    };

    void closeEvent(QCloseEvent *event) override;

private slots:

    void onCloseGame();

    void on_Game9Button_clicked();

    void on_Game1v1Button_clicked();

    void on_GameSingleButton_clicked();

    void on_HelpButton_clicked();

    void onSocketReadyRead();

    void netSlap();

    void keepAlive();

    void on_loginButton_clicked();

    void on_logoutButton_clicked();

    void on_completeRegisterButton_clicked();

    void on_backToLoginPageButton_clicked();

    void on_registerButton_clicked();

    void on_backToMainPageButton_clicked();

    void on_editButton_clicked();

    void on_editedButton_clicked();

    void on_uploadButton_clicked();

    void on_uploadButton_2_clicked();

    void on_sendButton_clicked();

    void keyPressEvent(QKeyEvent *event) override;

};

#endif // MAINWIDGET_H
