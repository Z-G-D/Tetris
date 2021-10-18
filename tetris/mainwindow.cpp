#include <QtCore/QBuffer>
#include <QtCore/QProcess>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QDateTime>
#include <QNetworkDatagram>
#include <QsLog.h>
#include <tetris.pb.h>
#include "mainwindow.h"
#include "gamewindow.h"
#include "helpwindow.h"
#include "global.h"

QUdpSocket *MainWindow::udpSocket = nullptr;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::mainwindow), simpleCrypt(global::privateKey)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);//用于与连接的客户端通讯的QTcpSocket
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));

    if (udpSocket->bind())//绑定端口成功
    {
        QLOG_TRACE() << "bind succeed on port:" << udpSocket->localPort();
    }
    else
    {
        throw std::runtime_error("Bind port error");
    }

    kcpClient = ikcp_create(global::sessionConv, nullptr);

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

    ui->passwordBox->setEchoMode(QLineEdit::Password);
    ui->passwordBox->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
    ui->pBox->setEchoMode(QLineEdit::Password);
    ui->pBox->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
    ui->pBox_2->setEchoMode(QLineEdit::Password);
    ui->pBox_2->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
    ui->WindowStack->setCurrentIndex(kLoginPage);
    ui->FloatWindow->show();

    aliveTimer = new QTimer(this);
    connect(aliveTimer, SIGNAL(timeout()), this, SLOT(keepAlive()));
    aliveTimer->setInterval(1000);

    proto::Information information;
    information.set_connect(0);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());


    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(netSlap()));
    timer->start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Game9Button_clicked()
{
    proto::Information information;
    information.set_game9(0);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    ui->FloatWindow->setText("正在匹配");
    ui->FloatWindow->showMaximized();
}

void MainWindow::on_Game1v1Button_clicked()
{
    proto::Information information;
    information.set_game1v1(0);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    ui->FloatWindow->setText("正在匹配");
    ui->FloatWindow->showMaximized();
}


void MainWindow::on_GameSingleButton_clicked()
{
    proto::Information information;
    information.set_game_single(0);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    ui->FloatWindow->setText("等待服务器响应");
    ui->FloatWindow->showMaximized();
}

void MainWindow::on_HelpButton_clicked()
{
    //弹出游戏帮助窗口
    auto *helpWindow = new HelpWindow(this);
    helpWindow->setWindowFlags(Qt::Dialog);
    helpWindow->setWindowModality(Qt::ApplicationModal);
    helpWindow->show();
}

int MainWindow::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    udpSocket->writeDatagram(buf, len, global::serverAddr, global::serverPort);
    return 0;
}

void MainWindow::onSocketReadyRead()
{
    while (udpSocket->hasPendingDatagrams())
    {
        auto datagram = udpSocket->receiveDatagram();
        ikcp_input(kcpClient, datagram.data(), datagram.data().size());
    }
}

void MainWindow::netSlap()
{
    ikcp_update(kcpClient, QDateTime::currentDateTime().currentMSecsSinceEpoch());
    static QByteArray byteArray;

    while (true)
    {
        if (byteArray.size() < ikcp_peeksize(kcpClient))
            byteArray.resize(ikcp_peeksize(kcpClient));
        int dataSize = ikcp_recv(kcpClient, byteArray.data(), byteArray.size());
        if (dataSize < 0) break;


        proto::Information information;

        if (information.ParseFromArray(byteArray.data(), dataSize))
        {
            switch (information.type_case())
            {
                case proto::Information::kConnect:
                    if (information.connect() == 0)break;
                    QLOG_TRACE() << "Receive connect ";
                    ui->FloatWindow->hide();
                    ui->WindowStack->setCurrentIndex(kLoginPage);
                    aliveTimer->start();
                    break;
                case proto::Information::kDisconnect:
                    QLOG_TRACE() << "Receive disconnect ";
                    close();
                    break;
                case proto::Information::kSignup:
                    QLOG_TRACE() << "Receive signup ";
                    if (information.signup().status() == proto::Account_Status_success)
                    {
                        ui->loginLabel->setText("注册成功！");
                        ui->WindowStack->setCurrentIndex(kLoginPage);
                    }
                    else
                    {
                        if (information.signup().status() == proto::Account_Status_username_in_Used)
                        {
                            ui->uBox->setText("");
                            ui->uBox->setPlaceholderText("用户名已经被注册！");
                        }
                        else if (information.signup().status() == proto::Account_Status_alias_in_used)
                        {
                            ui->eBox->setText("");
                            ui->eBox->setPlaceholderText("该昵称已经被使用");
                        }
                        ui->regLabel->setText("请按要求修改信息");
                    }
                    ui->FloatWindow->hide();
                    break;
                case proto::Information::kLogin:
                    QLOG_TRACE() << "Receive login ";
                    if (information.login().status() == proto::Account_Status_success)
                    {
                        avatar = QPixmap();
                        username = simpleCrypt.decryptToString(QString::fromStdString(information.login().username()));
                        password = simpleCrypt.decryptToString(QString::fromStdString(information.login().password()));
                        alias = simpleCrypt.decryptToString(QString::fromStdString(information.login().alias()));
                        avatar.loadFromData(
                                QByteArray::fromBase64(QByteArray::fromStdString(information.login().avatar())));
                        ui->loggedPic->setScaledContents(true);
                        ui->loggedPic->setPixmap(avatar);
                        ui->nameLabel->setText(alias);
                        ui->rankLabel->setText(QString::number(information.login().rankpoint()));
                        ui->WindowStack->setCurrentIndex(kMainPage);
                    }
                    else if (information.login().status() == proto::Account_Status_failed)
                    {
                        ui->loginLabel->setText("用户名或密码错误！");
                    }
                    ui->FloatWindow->hide();
                    break;
                case proto::Information::kAlter:
                    QLOG_TRACE() << "Receive Alter ";
                    if (information.alter().status() == proto::Account_Status_success)
                    {
                        ui->WindowStack->setCurrentIndex(kLoginPage);
                        ui->loginLabel->setText("个人信息修改成功！请重新登录");
                    }
                    else
                    {
                        if (information.alter().status() == proto::Account_Status_username_in_Used)
                        {
                            ui->uBox_2->setText("");
                            ui->uBox_2->setPlaceholderText("用户名已经被注册！");
                        }
                        else if (information.alter().status() == proto::Account_Status_alias_in_used)
                        {
                            ui->eBox_2->setText("");
                            ui->eBox_2->setPlaceholderText("该昵称已经被使用");
                        }
                        ui->regLabel_2->setText("请按要求修改信息");
                    }
                    ui->FloatWindow->hide();
                    break;
                case proto::Information::kPoints:
                    QLOG_TRACE() << "Receive kPoints ";
                    if (information.points().status() == proto::Account_Status_success)
                    {
                        ui->rankLabel->setText(QString::number(information.points().rankpoint()));
                        QLOG_TRACE() << "Receive ddd ";
                    }
                    break;
                case proto::Information::kMessage:
                    QLOG_TRACE() << "Receive message ";
                    ui->textEdit->appendPlainText(QString::fromStdString(information.message().alias()) + " : " +
                                                  QString::fromStdString(information.message().message()));
                    break;
                case proto::Information::kGame9:
                    ui->FloatWindow->hide();
                    global::gamePort = information.game9();
                    global::gameWindow = new GameWindow(GameWindow::GameType::game9);
                    connect(global::gameWindow, SIGNAL(closeGame()), this, SLOT(onCloseGame()));
                    global::mainWindow->hide();
                    global::gameWindow->show();
                    global::isGameStart = false;
                    break;
                case proto::Information::kGame1V1:
                    ui->FloatWindow->hide();
                    global::gamePort = information.game1v1();
                    global::gameWindow = new GameWindow(GameWindow::GameType::game1v1);
                    connect(global::gameWindow, SIGNAL(closeGame()), this, SLOT(onCloseGame()));
                    global::mainWindow->hide();
                    global::gameWindow->show();
                    global::isGameStart = false;
                    break;
                case proto::Information::kGameSingle:
                    ui->FloatWindow->hide();
                    global::gamePort = information.game_single();
                    global::gameWindow = new GameWindow(GameWindow::GameType::gameSingle);
                    connect(global::gameWindow, SIGNAL(closeGame()), this, SLOT(onCloseGame()));
                    global::mainWindow->hide();
                    global::gameWindow->show();
                    global::isGameStart = false;
                    break;
                case proto::Information::kError:
                    QLOG_TRACE() << "Receive error ";
                    ui->FloatWindow->setText("您已掉线，请重启程序");
                    ui->FloatWindow->showMaximized();
                    QMessageBox::question(this, tr("您已掉线，请重启程序"), tr("您已掉线，请重启程序"), QMessageBox::Yes);
                    QApplication::exit();
                    return;
                case proto::Information::TYPE_NOT_SET:
                    break;
            }
        }
        else QLOG_ERROR() << "protobuf 解析错误";
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int select = QMessageBox::question(this, tr("退出游戏"), tr("您确定要退出游戏吗？"), QMessageBox::Yes | QMessageBox::No);
    if (select == QMessageBox::Yes)
    {
        proto::Information information;
        information.set_disconnect(0);
        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
        ikcp_flush(kcpClient);
    }
    else
    {
        event->ignore();
    }
}


void MainWindow::on_loginButton_clicked()
{
    proto::Information information;
    auto account = information.mutable_login();
    account->set_username(simpleCrypt.encryptToString(ui->usernameBox->text()).toStdString());
    account->set_password(simpleCrypt.encryptToString(ui->passwordBox->text()).toStdString());

    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    ui->FloatWindow->setText("正在登录");
    ui->FloatWindow->showMaximized();
}


void MainWindow::on_registerButton_clicked()
{
    ui->uBox->setText(ui->usernameBox->text());
    ui->pBox->setText(ui->passwordBox->text());
    ui->WindowStack->setCurrentIndex(kRegisterPage);
}

void MainWindow::on_logoutButton_clicked()
{
    if (QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                        "退出弹窗", "确定要登出账号吗?",
                                        QMessageBox::Yes | QMessageBox::No).exec())
    {
        username.clear();
        password.clear();
        alias.clear();
        ui->passwordBox->setText("");
        ui->loginLabel->setText("已登出!");
        ui->WindowStack->setCurrentIndex(kLoginPage);
    }
}

void MainWindow::on_completeRegisterButton_clicked()
{
    bool halt = false;

    if (ui->uBox->text() == "")
    {
        ui->uBox->setPlaceholderText("用户名不能为空!");
        halt = true;
    }

    if (ui->pBox->text() == "")
    {
        ui->pBox->setPlaceholderText("密码不能为空!");
        halt = true;
    }

    if (ui->eBox->text() == "")
    {
        ui->eBox->setPlaceholderText("昵称不能为空!");
        halt = true;
    }


    if (halt)
    {
        ui->regLabel->setText("请按要求填写下列信息");
    }
    else
    {
        proto::Information information;

        auto account = information.mutable_signup();
        account->set_username(simpleCrypt.encryptToString(ui->uBox->text()).toStdString());
        account->set_password(simpleCrypt.encryptToString(ui->pBox->text()).toStdString());
        account->set_alias(simpleCrypt.encryptToString(ui->eBox->text()).toStdString());

        int i = 100;
        do
        {
            if (i == 0)
            {
                avatar = QPixmap();
                ui->rpLabel->setScaledContents(true);
                ui->rpLabel->setPixmap(avatar);
                ui->regLabel->setText("图片过大，请更换图片");
                return;
            }
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            avatar.save(&buffer, "jpg", i);
            account->set_avatar(byteArray.toBase64().toStdString());
            buffer.close();
            i -= 20;
        } while (information.ByteSizeLong() > 126 * 1376);


        QLOG_DEBUG() << "i=" << i;
        QLOG_DEBUG() << "error="
                     << ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
        ui->FloatWindow->setText("正在注册");
        ui->FloatWindow->showMaximized();
    }
}

void MainWindow::on_backToLoginPageButton_clicked()
{
    ui->loginLabel->setText("");
    ui->WindowStack->setCurrentIndex(kLoginPage);
}


void MainWindow::on_backToMainPageButton_clicked()
{
    ui->WindowStack->setCurrentIndex(kMainPage);
}

void MainWindow::on_editButton_clicked()
{
    ui->uBox_2->setText(username);
    ui->pBox_2->setText(password);
    ui->eBox_2->setText(alias);
    ui->rpLabel_2->setScaledContents(true);
    ui->rpLabel_2->setPixmap(avatar);
    ui->WindowStack->setCurrentIndex(kEditPage);
}


void MainWindow::on_editedButton_clicked()
{
    bool halt = false;

    if (ui->uBox_2->text() == "")
    {
        ui->uBox_2->setPlaceholderText("用户名不能为空!");
        halt = true;
    }

    if (ui->pBox_2->text() == "")
    {
        ui->pBox_2->setPlaceholderText("密码不能为空!");
        halt = true;
    }

    if (ui->eBox_2->text() == "")
    {
        ui->eBox_2->setPlaceholderText("昵称不能为空!");
        halt = true;
    }

    if (halt)
    {
        ui->regLabel_2->setText("请按要求填写下列信息");
    }
    else
    {
        proto::Information information;

        auto account = information.mutable_alter();
        account->set_username(simpleCrypt.encryptToString(ui->uBox_2->text()).toStdString());
        account->set_password(simpleCrypt.encryptToString(ui->pBox_2->text()).toStdString());
        account->set_alias(simpleCrypt.encryptToString(ui->eBox_2->text()).toStdString());

        int i = 100;
        do
        {
            if (i == 0)
            {
                avatar = QPixmap();
                ui->rpLabel_2->setScaledContents(true);
                ui->rpLabel_2->setPixmap(avatar);
                ui->regLabel_2->setText("图片过大，请更换图片");
                return;
            }
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            avatar.save(&buffer, "jpg", i);
            account->set_avatar(byteArray.toBase64().toStdString());
            buffer.close();
            i -= 20;
        } while (information.ByteSizeLong() > 126 * 1376);

        account->set_original_username(simpleCrypt.encryptToString(username).toStdString());
        account->set_original_password(simpleCrypt.encryptToString(password).toStdString());
        account->set_original_alias(simpleCrypt.encryptToString(alias).toStdString());
        ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
        ui->FloatWindow->setText("正在修改信息");
        ui->FloatWindow->showMaximized();
    }
}


void MainWindow::on_uploadButton_clicked()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    avatar.load(path);
    ui->rpLabel->setScaledContents(true);
    ui->rpLabel->setPixmap(avatar);
}

void MainWindow::on_uploadButton_2_clicked()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    avatar.load(path);
    ui->rpLabel_2->setScaledContents(true);
    ui->rpLabel_2->setPixmap(avatar);
}

void MainWindow::onCloseGame()
{
    proto::Information information;
    auto &points = *information.mutable_points();
    points.set_username(simpleCrypt.encryptToString(username).toStdString());
    points.set_password(simpleCrypt.encryptToString(password).toStdString());
    points.set_rankpoint(global::gameWindow->points);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    delete global::gameWindow;
}

void MainWindow::keepAlive()
{
    proto::Information information;
    information.set_keepalive(0);
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
}


void MainWindow::on_sendButton_clicked()
{
    if (ui->editMsg->text().isEmpty())return;
    proto::Information information;
    auto &message = *information.mutable_message();
    message.set_message(ui->editMsg->text().toStdString());
    message.set_alias(alias.toStdString());
    ikcp_send(kcpClient, information.SerializeAsString().data(), information.ByteSizeLong());
    ui->editMsg->clear();
    ui->editMsg->setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            QLOG_DEBUG() << "enter";
            switch (ui->WindowStack->currentIndex())
            {
                case kLoginPage:
                    on_loginButton_clicked();
                    break;
                case kMainPage:
                    on_sendButton_clicked();
                    break;
                case kRegisterPage:
                    on_completeRegisterButton_clicked();
                    break;
                case kEditPage:
                    on_editedButton_clicked();
                    break;
            }
            break;
    }
    QWidget::keyPressEvent(event);
}










