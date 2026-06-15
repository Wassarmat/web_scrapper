//Класс HtmlParser необходим для извлечения информации из HTML
#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "models/ScrapedData.h"
#include <vector>
#include <string>
#include <algorithm>

class HtmlParser{

public:
    PageData parse(const std::string& html, const std::string& baseUrl);
private:
    std::string clearingTags(const std::string& html); //функция очищающая документ HTML от тегов
    void extractLinks(const std::string& html, PageData& pageData, const std::string& baseUrl); //функция извлекающая ссылку из тега <a>
    std::string makeAbsoluteUrl(const std::string& relativeUrl, const std::string& baseUrl); //функция для создания абсолютной ссылки
};




#endif // HTMLPARSER_H
