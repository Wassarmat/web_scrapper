//Заголовочный файл многопоточного сканирования
/*Класс WebScrapper берет список URL, распределяет между несколькими потоками
каждый поток скачивает и обрабатывает свою страницу. Класс следит за тем, чтобы
-одна страница не обрабатывалась дважды
-очередь не переполнялась*/
#ifndef WEBSCRAPPER_H
#define WEBSCRAPPER_H

#include <QObject>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_set>
#include <memory>
#include <atomic>


class HtmlParser;
class DataSaver;


class WebScrapper : public QObject
{
    Q_OBJECT
public:
    explicit WebScrapper(int maxThreads= 6, QObject *parent= nullptr);
    virtual ~WebScrapper();

    void start(const std::vector<std::string>& startUrls);
    void stop();

    int getMaxCountUrl(){return maxCountCheckUrl;}
    void setMaxCountCheckUrl(int num){maxCountCheckUrl.store(num);}
signals:
    void progressChanged(int processed, int total); //кол-во обработанных страниц из общего списка
    void pageProcessed(const std::string& url, bool success); //информация о конкретной странице
    void finished();
    void error(const std::string& message); //Ошибка
private:
    std::vector<std::thread> threads;
    std::atomic<bool> isRunning; //Флаг процесса
    int maxThreads=6; //Макс. кол-во потоков
    std::atomic<int> maxCountCheckUrl=20; //Макс. кол-во URL которое будет считывать (по умолчанию20)

    std::queue<std::string> waitingUrls; //очередь ожидающих URL
    std::unordered_set<std::string> waitingUrlsSet; // множество ожидающих URL. Оно необходимо для предотвращения дубликатов, скорость проверки О(1). В queueu О(n)
    std::mutex mtxQueue; //мютекс для очереди URL
    std::unordered_set<std::string> finishedUrls; //обработанные URL
    std::mutex mtxFinishedUrls; //мьютекс для множества обработанных URL

    std::atomic<int> parsedUrlsCount; //кол-во обработанных страниц
    std::atomic<int> countUrlInQueue; //кол-во URL в очереди

    std::unique_ptr<HtmlParser> ptr_htmlParser;
    std::unique_ptr<DataSaver> ptr_DataSaver;

    void worker(); //функция, которую выполняет каждый поток
    void processPage(const std::string& url); //обработка одной страницы

};
#endif // WEBSCRAPPER_H
