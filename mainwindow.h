#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "core/WebScrapper.h"
#include "filesystem"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void onProgressChanged(int processed, int total);
    void onPageProcessed(const std::string& url, bool success);
    void onError(const std::string& message);
    void onFinished();
    void onStopButtonClicked();
    void saveAs();
    void clearLogs();

// signals:
//     void closeProgram();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<WebScrapper> scrapper;
};

#endif