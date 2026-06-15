//Класс DataSaver отвечает за сохранение результата парсинга
#ifndef DATASAVER_H
#define DATASAVER_H

#include "models/ScrapedData.h"
#include <string>
#include <mutex>
#include <fstream>
#include <QDebug>
#include <iostream>

class DataSaver{

public:
    DataSaver(const std::string& fileName);
    virtual ~DataSaver();

    bool save(const PageData& pageData);

private:
    std::ofstream file; //выходной файловый поток
    std::mutex mtxDefenderRecord; //мютекс для защиты от записи
    std::string fileName; //имя файла
    bool isOpen; //флаг открытия файла
};

#endif // DATASAVER_H
