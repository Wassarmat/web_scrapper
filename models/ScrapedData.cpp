#include "ScrapedData.h"
#include <sstream>


bool PageData::isValid(){
    if(this->url !=""  && this->title!="" && this->content != "" && this->errorMessage != "" &&
        this->statusCode==0) return true;

    return false;
}

std::string PageData::toString() const{
    std::stringstream ss;
    ss<<"Логи страницы: \n"
       <<"URL: " << this->url <<"\n"
       <<"Title: " << this->title <<"\n"
       << "HTTP Status: " << this->statusCode << "\n"
       << "Content size: " << this->content.size() << "\n"
       << "Links: " << this->links.size() <<"\n";

    if(!errorMessage.empty()){
        ss << "Error: " << this->errorMessage << "\n";
    }
    return ss.str();
}


std::string PageData::escapeJson(const std::string& str) const{
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
        case '"': ss << "\\\""; break;
        case '\\': ss << "\\\\"; break;
        case '\n': ss << "\\n"; break;
        case '\r': ss << "\\r"; break;
        case '\t': ss << "\\t"; break;
        default: ss << c; break;
        }
    }
    return ss.str();
}

//Реализация экранирования
std::string PageData::toJson() const{
    std::stringstream ss;
    ss << "{\n"
       << "  \"url\": \"" << PageData::escapeJson(url) << "\",\n"
       << "  \"title\": \"" << PageData::escapeJson(title) << "\",\n"
       << "  \"content\": \"" << PageData::escapeJson(content) << "\",\n"
       << "  \"statusCode\": " << statusCode << ",\n"
       << "  \"errorMessage\": \"" << PageData::escapeJson(errorMessage) << "\",\n"
       << "  \"links\": [";

    for (size_t i = 0; i < links.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "\n    \"" << PageData::escapeJson(links[i]) << "\"";
    }

    ss << "\n  ]\n}";
    return ss.str();
}











