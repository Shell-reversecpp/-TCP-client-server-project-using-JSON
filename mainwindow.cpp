#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Происходит сразу , без участия пользователя
    ui->setupUi(this);

    //создание сокета
    socket = new QTcpSocket(this);

    //соединение двух методов
    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //При нажатии -> соединение с сервером
    socket->connectToHost("127.0.0.1",5555);
}

void MainWindow::sockReady()
{
    //Проверка на устойчивое соединение в мили секундах
    if(socket->waitForConnected(1000))
    {
        socket->waitForReadyRead(1000);

        //Запись в переменную хранения всех данных из сокета
        Data = socket->readAll();

        //Получение в doc данных в json
        doc = QJsonDocument::fromJson(Data, &docError);

        //Проверка на ошибки
        if (docError.errorString().toInt()==QJsonParseError::NoError)
        {
            if((doc.object().value("type").toString() == "connect") && (doc.object().value("status").toString() == "yes"))
            {
                QMessageBox::information(this, "Информация","Соединение установлено");
            }
            else if (doc.object().value("type").toString() == "resultSelect")
            {
                //Создаем модель для запииси в itemview
                QStandardItemModel *model = new QStandardItemModel(nullptr);

                model->setHorizontalHeaderLabels(QStringList()<<"name");


                //Работа с массивами
                QJsonArray docAr = doc.object().value("result").toArray();

                for(int i = 0; i < docAr.count();i++)
                {
                    QStandardItem* col = new QStandardItem(docAr[i].toObject().value("name").toString());
                    model->appendRow(col);
                }
                ui->tableView->setModel(model);

            }
            else
            {
                QMessageBox::information(this, "Информация","Соединение не установлено");
            }
        }
        else
        {
            QMessageBox::information(this, "Информация","Ошибки с форматом передачи данных - "+docError.errorString());
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //Проверка на открытый сокет
    if(socket->isOpen())
    {
            //Отправка json команды на сервер
            socket->write("{\"type\":\"select\",\"params\":\"workers\"}");
            socket->waitForBytesWritten(1000);
    }
            else {
            QMessageBox::information(this,"Информация","Соединение не установлено");
    }
}

void MainWindow::sockDisc()
{
    //Отсоединеие
    socket->deleteLater();
}


