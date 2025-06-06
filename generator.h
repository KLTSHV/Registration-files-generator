#ifndef GENERATOR_H
#define GENERATOR_H

#include "structures.h"
#include <QString>
#include <QDataStream>

/*------------------------------------------------------
 * Generator — создаёт .dat‑файлы по заданным параметрам
 *-----------------------------------------------------*/
class Generator
{
public:
    explicit Generator(const VarStorage& settings);

    // Основная функция: записывает файл filePath
    void generateDatFile(const QString& filePath) const;

private:
    // Внутренние вспомогательные методы
    void writeRegFileHdr(QDataStream& out) const;
    void writeUMsgHdrAndData(QDataStream& out, int msgIndex) const;

    const VarStorage& s_;
};

#endif // GENERATOR_H
