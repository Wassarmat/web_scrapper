#include "HtmlParser.h"
#include <regex>

PageData HtmlParser::   parse(const std::string& html, const std::string& baseUrl){
    PageData pageData;
    pageData.url= baseUrl;
    std::regex ws(R"(\s+)");


    auto start = html.find("<title>");
    if (start != std::string::npos) {
        start += 7; // пропустить "<title>"
        auto end = html.find("</title>", start);
        if (end != std::string::npos) {
            pageData.title = html.substr(start, end - start);
            pageData.title = std::regex_replace(pageData.title, ws, " ");
        }
    }
    pageData.content= clearingTags(html);
    extractLinks(html, pageData, baseUrl);

    return pageData;
}


std::string HtmlParser::clearingTags(const std::string& html){
    bool inTag= false;
    bool inScriptTag=false;
    bool inStyleTag=false;

    std::string contentHtml;
    contentHtml.reserve(html.size());

    for(size_t i=0; i<html.size(); ++i){
        char c= html[i];
        //можно было бы использовать substr, но его многократное использование замедляет поток
        if ((inScriptTag || inStyleTag) && c == '<') {
            // Ищем закрывающий тег /script или /style
            if (i + 7 <= html.size() &&
                html[i+1] == '/' &&
                (html[i+2] == 's' || html[i+2] == 'S') &&
                (html[i+3] == 'c' || html[i+3] == 'C') &&
                (html[i+4] == 'r' || html[i+4] == 'R') &&
                (html[i+5] == 'i' || html[i+5] == 'I') &&
                (html[i+6] == 'p' || html[i+6] == 'P') &&
                (html[i+7] == 't' || html[i+7] == 'T'))
            {
                inScriptTag= false;
                i += 7;
                continue;
            }

            if (i + 6 <= html.size() &&
                html[i+1] == '/' &&
                (html[i+2] == 's' || html[i+2] == 'S') &&
                (html[i+3] == 't' || html[i+3] == 'T') &&
                (html[i+4] == 'y' || html[i+4] == 'Y') &&
                (html[i+5] == 'l' || html[i+5] == 'L') &&
                (html[i+6] == 'e' || html[i+6] == 'E'))
            {
                inStyleTag= false;
                i += 6;
                continue;
            }
        }

        if(c=='<') inTag=true;

        if(inTag){
            //ищем открывающий тег script или style
            if(i + 6 <= html.size() &&
                (html[i+1] == 's' || html[i+1] == 'S') &&
                (html[i+2] == 'c' || html[i+2] == 'C') &&
                (html[i+3] == 'r' || html[i+3] == 'R') &&
                (html[i+4] == 'i' || html[i+4] == 'I') &&
                (html[i+5] == 'p' || html[i+5] == 'P') &&
                (html[i+6] == 't' || html[i+6] == 'T'))
            {
                inScriptTag=true;
                i+=6;
                continue;
            }

            if (i + 5 <= html.size() &&
                (html[i+1] == 's' || html[i+1] == 'S') &&
                (html[i+2] == 't' || html[i+2] == 'T') &&
                (html[i+3] == 'y' || html[i+3] == 'Y') &&
                (html[i+4] == 'l' || html[i+4] == 'L') &&
                (html[i+5] == 'e' || html[i+5] == 'E'))
            {
                inStyleTag= true;
                i += 5;
                continue;
            }

        }

        if(inScriptTag || inStyleTag) continue;

        if(inTag  && c=='>'){
            inTag=false;
            continue;
        }
        if(inTag==true) continue;

        contentHtml+=c;
    }

    std::regex ws(R"(\s+)");
    return  std::regex_replace(contentHtml, ws, " ");
}





void HtmlParser::extractLinks(const std::string& html, PageData& pageData, const std::string& baseUrl){
    bool inLink= false;
    std::string link;

    for(size_t i=0; i< html.size(); ++i){

        //Поиск открывающего тега <a>
        if(!inLink && i+1 < html.size() && html[i] == '<' && (html[i+1] == 'a' || html[i+1] =='A')){
            inLink= true;
            i+=1;
            continue;
        }

        //Поиск закрывающего тега </a>
        if(inLink && i+3 < html.size() && html[i]== '<' && html[i+1] == '/' && (html[i+2] == 'a' || html[i+2] =='A') && html[i+3] == '>'){
            inLink= false;
            i+=3;
            continue;
        }

        //Поиск href внутри тега
        if(inLink && i+4< html.size() &&
            (html[i] == 'h' || html[i] == 'H') &&
            (html[i+1] == 'r' || html[i+1] == 'R') &&
            (html[i+2] == 'e' || html[i+2] == 'E') &&
            (html[i+3] == 'f' || html[i+3] == 'F')
            )
        {
            i+=4;

            //Пропускаем пробелы и табуляции до =
            while(html[i] == ' ' || html[i] == '\t')++i;

            //Проверка на наличие '='
            if(i < html.size() && html[i] == '='){

                ++i;
                //пропускаю пробелы и табуляции после '='
                while(i < html.size() && html[i] == ' ') ++i;

                //определяем вид ковычек
                char quote=0;
                if(i < html.size() && (html[i] == '"' || html[i]=='\'')){
                    quote= html[i];
                    ++i;
                }

                //проходимся до закрывающих кавычек или до пробела или конца тега
                link.clear();
                while (i < html.size())
                {
                    if(html[i] == quote) break;
                    else if(html[i] == ' ' || html[i] == '\t' || html[i] == quote || html[i] == '>') break;

                    link+=html[i];
                    ++i;
                }

                if (!link.empty()) {
                    std::string absLink = makeAbsoluteUrl(link, baseUrl);
                    pageData.links.push_back(absLink);
                }
            }
        }
    }
}


std::string HtmlParser::makeAbsoluteUrl(const std::string& relativeUrl, const std::string& baseUrl) {
    // Если ссылка уже абсолютная (начинается с http:// или https://)
    if (relativeUrl.find("http://") == 0 || relativeUrl.find("https://") == 0) {
        return relativeUrl;
    }

    // Если ссылка начинается с "//" — добавить текущий протокол (из baseUrl)
    if (relativeUrl.find("//") == 0) {
        size_t protoEnd = baseUrl.find("://");
        if (protoEnd != std::string::npos) {
            return baseUrl.substr(0, protoEnd + 1) + relativeUrl; // "https:" + "//example.com"
        }
        return "https:" + relativeUrl; // на всякий случай
    }

    // Разбираем baseUrl на части
    // Найдём протокол + домен (до последнего '/')
    size_t schemeEnd = baseUrl.find("://");
    if (schemeEnd == std::string::npos) {
        // Невалидный baseUrl, вернём как есть
        return relativeUrl;
    }

    std::string protocol = baseUrl.substr(0, schemeEnd + 3); // "https://"
    std::string rest = baseUrl.substr(schemeEnd + 3);

    size_t slashPos = rest.find('/');
    std::string domain = (slashPos == std::string::npos) ? rest : rest.substr(0, slashPos);

    // Для абсолютного пути (начинается с '/')
    if (relativeUrl.find("/") == 0) {
        return protocol + domain + relativeUrl;
    }

    // Относительный путь: нужно учитывать текущий каталог
    std::string path;
    if (slashPos == std::string::npos) {
        path = "/";
    } else {
        path = rest.substr(0, rest.rfind('/') + 1); // каталог без имени файла
    }

    // Обработка ".." и "." (упрощённо, для реального проекта лучше использовать std::filesystem или специальную библиотеку)
    std::string fullPath = path + relativeUrl;

    return protocol + domain + fullPath;
}



















