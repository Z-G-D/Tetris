#include <QApplication>
#include <QDir>
#include <QtXml/QtXml>
#include <QsLog.h>
#include <QsLogDest.h>
#include <QsLogLevel.h>
#include "mainwindow.h"
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QsLogging::Logger &logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(QApplication::applicationDirPath()).filePath("log.txt"));

    QsLogging::DestinationPtr fileDestination(
            QsLogging::DestinationFactory::MakeFileDestination(
                    sLogPath,
                    QsLogging::EnableLogRotation,
                    QsLogging::MaxSizeBytes(512 * 1024),
                    QsLogging::MaxOldLogCount(1)
            )
    );
    QsLogging::DestinationPtr debugDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());

    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);

    QDomDocument xml;
    QFile file("config.xml");
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QString error = "";
            int row = 0, column = 0;
            if (!xml.setContent(&file, false, &error, &row, &column))
            {
                QLOG_INFO() << "parse config.xml failed:" << row << "---" << column << ":" << error;
                QLOG_INFO() << "delete the file to reset config.xml";
                return -1;
            }
            QDomElement root = xml.documentElement();
            global::serverAddr.setAddress(root.elementsByTagName("ServerIP").at(0).toElement().text());
            global::serverPort = root.elementsByTagName("ServerPort").at(0).toElement().text().toUShort();
            global::privateKey = root.elementsByTagName("PrivateKey").at(0).toElement().text().toULongLong();
            file.close();
        }
        else
        {
            QLOG_INFO() << "read config.xml failed:";
            return -1;
        }
    }
    else
    {
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QDomProcessingInstruction instruction = xml.createProcessingInstruction("xml", R"(version="1.0" encoding="UTF-8")");
            xml.appendChild(instruction);

            auto root = xml.createElement("Config");
            xml.appendChild(root);

            QDomText text;
            QDomElement element;

            element = xml.createElement("ServerIP");
            element.appendChild(xml.createTextNode(global::serverAddr.toString()));
            root.appendChild(element);

            element = xml.createElement("ServerPort");
            element.appendChild(xml.createTextNode(QString::number(global::serverPort)));
            root.appendChild(element);

            element = xml.createElement("PrivateKey");
            element.appendChild(xml.createTextNode(QString::number(global::privateKey)));
            root.appendChild(element);
            root.setAttribute("ServerIP", "服务器IP地址");
            root.setAttribute("ServerPort", "服务器端口");
            root.setAttribute("PrivateKey", "客户端密钥");
            QTextStream out(&file);
            out.setCodec("UTF-8");
            xml.save(out, 4, QDomNode::EncodingFromTextStream);
            file.close();
        }
        else
        {
            QLOG_INFO() << "generate config.xml failed:";
            return -1;
        }
    }
    QLOG_INFO() << "服务器IP地址 = " << global::serverAddr;
    QLOG_INFO() << "服务器端口 = " << global::serverPort;
    QLOG_INFO() << "客户端密钥 = " << global::privateKey;

    //加载所有窗口
    global::mainWindow = new MainWindow();
    //显示主界面
    global::mainWindow->show();

    return QApplication::exec();
}
