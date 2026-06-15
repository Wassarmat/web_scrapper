#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QPlainTextEdit>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    scrapper= std::make_unique<WebScrapper>(6,this);

    //подключаем сигналы к слотам. В QT следующий порядок аргументов: объект отправляющий сигнал; сигнал к которому осуществляем соед.; получатель сигнала; слот вызываемый сигналом
    connect(scrapper.get(), &WebScrapper::progressChanged, this, &MainWindow::onProgressChanged);
    connect(scrapper.get(), &WebScrapper::pageProcessed, this, &MainWindow::onPageProcessed);
    connect(scrapper.get(), &WebScrapper::error, this, &MainWindow::onError);
    connect(scrapper.get(), &WebScrapper::finished, this, &MainWindow::onFinished);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);

    ui->plainTextEdit->setEnabled(true);
    ui->plainTextEdit->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    if(scrapper) scrapper->stop();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // Берём URL из lineEdit
    QString urlText = ui->lineEdit->text();
    if (urlText.isEmpty()) {
        ui->plainTextEdit->appendPlainText("Ошибка: введите URL");
        return;
    }

    // Преобразуем QString в std::string
    std::string startUrl = urlText.toStdString();
    std::vector<std::string> urls = { startUrl };

    // Отключаем кнопку, чтобы не запустить повторно
    ui->pushButton->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
    ui->plainTextEdit->appendPlainText("Запуск сканирования с " + urlText);

    // Запускаем скрейпер
    scrapper->start(urls);
}

void MainWindow::onProgressChanged(int processed, int total)
{
    // Выводим прогресс в текстовое поле (или можно обновить progress bar)
    ui->plainTextEdit->appendPlainText(
        QString("Прогресс: %1 / %2. Лимит просматриваемых страниц: %3. ").arg(processed).arg(total).arg(scrapper->getMaxCountUrl()));
}

void MainWindow::onPageProcessed(const std::string& url, bool success)
{
    if (success) {
        ui->plainTextEdit->appendPlainText(
            QString("Обработано: %1").arg(QString::fromStdString(url)));
    } else {
        ui->plainTextEdit->appendPlainText(
            QString("Ошибка при обработке: %1").arg(QString::fromStdString(url)));
    }
}

void MainWindow::onError(const std::string& message)
{
    ui->plainTextEdit->appendPlainText(
        QString("ОШИБКА: %1").arg(QString::fromStdString(message)));
}

void MainWindow::onFinished()
{
    ui->plainTextEdit->appendPlainText("Сканирование завершено.");
    ui->pushButton->setEnabled(true);   // снова включаем кнопку
}

void MainWindow::onStopButtonClicked(){
    if(scrapper){
        scrapper->stop();
        ui->pushButton->setEnabled(true);
        ui->pushButton_3->setEnabled(false);
    }
}


