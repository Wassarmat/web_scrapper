#include "core/DataSaver.h"


DataSaver::DataSaver(const std::string& fileName) : fileName(fileName), isOpen(false){
    file.open(fileName,  std::ios::out |std::ios::trunc);
    isOpen= file.is_open();
}

DataSaver::~DataSaver() {
    if (file.is_open()) {
        file.close();
    }
}

bool DataSaver::save(const PageData &pageData){
    std::lock_guard<std::mutex> lock(mtxDefenderRecord);

    if(!isOpen) return false;

    std::string jsonString= pageData.toJson();
    file << jsonString << '\n';

    if(file.fail()){
        file.clear(); //очищаем флаг ошибки
        return false;
    }

    file.flush(); //Данные могут быть в буфере, и при внезапном закрытии приложения последние результаты не сохранятся. С помощью функции flush данные записываются мгновенно
    return true;
}