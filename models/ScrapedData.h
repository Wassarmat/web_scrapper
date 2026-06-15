//Модель данных которая хранит результат парсинга страницы
#ifndef SCRAPEDDATA_H
#define SCRAPEDDATA_H
#include <string>
#include <vector>
struct PageData
{
    std::string url;
    std::string title;
    std::string content;
    std::string errorMessage;
    int statusCode;
    std::vector<std::string> links;

    PageData(){
        this->url = "";
        this->title = "";
        this->content = "";
        this->errorMessage = "";
        this->statusCode= 0;
        this->links={};
    }

    PageData(std::string url, std::string title, std::string content, std::string errorMessage, int statusCode, std::vector<std::string> links){
        this->url=url;
        this->title=title;
        this->content= content;
        this->errorMessage= errorMessage;
        this->statusCode= statusCode;
        this->links=links;
    }

    bool isValid();
    std::string toString() const;
    std::string escapeJson(const std::string& str) const; //Экранирование для JSON формата
    std::string toJson() const;
};
using ScrapedDataList= std::vector<PageData>;
#endif // SCRAPEDDATA_H
