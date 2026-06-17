#include "WebScrapper.h"
#include "HtmlParser.h"
#include "DataSaver.h"
#include "models/ScrapedData.h"
#include <curl/curl.h>
#include <iostream>
#include <regex>

WebScrapper::WebScrapper(int maxThreads, QObject *parent): QObject(parent)
    ,maxThreads(maxThreads)
    ,isRunning(false)
    ,parsedUrlsCount(0)
    ,countUrlInQueue(0)
    ,ptr_htmlParser(std::make_unique<HtmlParser>())
    ,ptr_DataSaver(std::make_unique<DataSaver>("data.txt"))
{
    /*какие поля я не создаю и почему:
    threads — вектор пуст по умолчанию, добавим при запуске

    waitingUrls — очередь пуста по умолчанию

    mtxQueue — мьютекс создается в инициализированном состоянии автоматически

    finishedUrls — множество, пусто по умолчанию

    mtxProcessed — мьютекс создается автоматически */
}


//Функция ниже нужна для получения тела HTML от сервера. Возвращая totalSize мы проверяем, произошла ли ошибка записи
static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


WebScrapper::~WebScrapper(){
    WebScrapper::stop();
}


void WebScrapper::start(const std::vector<std::string>& startUrls){
    if(isRunning || !threads.empty()){ //Если процесс уже запущен, очищаю очереди и сбрасываю счетчики
        stop();
    }

    std::unique_lock<std::mutex> lockQueue(mtxQueue);
    waitingUrls = {};
    waitingUrlsSet.clear();
    for (const auto& url : startUrls) {
        waitingUrls.push(url);
        waitingUrlsSet.insert(url);
    }
    countUrlInQueue = waitingUrls.size();
    lockQueue.unlock();

    std::unique_lock<std::mutex> lockFinished(mtxFinishedUrls);
    finishedUrls.clear();
    lockFinished.unlock();


    parsedUrlsCount=0;
    isRunning=true;
    threads.clear();

    for(size_t i=0; i< maxThreads; ++i){
        threads.emplace_back(&WebScrapper::worker, this);
    }
}


void WebScrapper::worker(){
    while(isRunning){
        if(parsedUrlsCount >= maxCountCheckUrl){
            isRunning=false;
            emit finished(); //если количество спарсенных страниц превысило лимит, брякаем
            break;
        }

        std::unique_lock<std::mutex> lockQueue(mtxQueue);
        if(waitingUrls.empty()) break; //Если очередь пуста завершаю работу функции

        std::string url = waitingUrls.front();  // берем URL
        url = waitingUrls.front();
        waitingUrls.pop();
        waitingUrlsSet.erase(url);
        lockQueue.unlock();

        std::unique_lock<std::mutex> lockFinished(mtxFinishedUrls);
        if(finishedUrls.contains(url)) continue; //Если очередь имеет URL пропускаю итерацию
        lockFinished.unlock();


        processPage(url);
    }
}



void WebScrapper::processPage(const std::string& url){
    //HTTP-запрос через libcurl
    CURL* curl = curl_easy_init();
    if (!curl) {
        emit error("Ошибка инициализации для " + url);
        emit pageProcessed(url, false);
        return;
    }

    std::string responseHtml;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);   //функция из которой получаем блоки данных
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseHtml);   //указатель на строку, в которой будем хранить данные
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); //следовать за редиректами (с HTTP на HTTPS)
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);   //макс ожидание ответа в секундах
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "WebScrapper/1.0");   //эта строчка помогает от блокировок на сервере

    //здесь выполняем запрос и получаем ответ от сервера. Затем очищаем нашу сессию curl
    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || httpCode != 200) {
        std::string err = "Неудачное соединение " + url + " (curl: " + std::to_string(res) + ", HTTP: " + std::to_string(httpCode) + ")";
        emit error(err);
        emit pageProcessed(url, false);
        return;
    }

    //Парсинг
    PageData pageData = ptr_htmlParser->parse(responseHtml, url);

    //Сохранение
    ptr_DataSaver->save(pageData);

    //Добавление новых ссылок (с защитой от дубликатов)
    {
        std::lock_guard<std::mutex> lock(mtxQueue);
        for (const auto& link : pageData.links) {
            // Проверяем: не в очереди и не обработан
            if (waitingUrlsSet.count(link) == 0 && finishedUrls.count(link) == 0) {
                waitingUrls.push(link);
                waitingUrlsSet.insert(link);
                countUrlInQueue++;
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtxFinishedUrls);
        finishedUrls.insert(url);
    }

    //Отправляем сигнал об успешности обработки конкретной страницы и о кол-ве успешно обработанных URL
    parsedUrlsCount++;
    emit progressChanged(parsedUrlsCount, countUrlInQueue.load());
    emit pageProcessed(url, true);

}


void WebScrapper::stop(){
    if (!isRunning) return;


    isRunning=false;
    for(std::thread& td : threads)
        if (td.joinable()) td.join();

    threads.clear();
    emit finished();
}











