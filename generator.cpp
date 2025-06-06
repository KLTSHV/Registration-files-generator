#include "generator.h"

#include <QFile>
#include <QRandomGenerator>
#include <vector>
#include <cstring>
#include <QDateTime>

/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
/* Конструктор сохраняет настройки в s_                              */
/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
Generator::Generator(const VarStorage& settings)
    : s_(settings)
{}

/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
/* Запись заголовка регистрационного файла                          */
/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
void Generator::writeRegFileHdr(QDataStream& out) const
{
    out.writeRawData(reinterpret_cast<const char*>(&s_.regHdr),
                     sizeof(TRegFileHdr));
}

/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
/* Генерация .dat-файла с равномерным интервалом между сообщениями  */
/*–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
void Generator::generateDatFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    // 1) Пишем регистрационный заголовок
    writeRegFileHdr(out);

    // 2) Определяем количество сообщений
    int msgCount = s_.minMessages;
    if (s_.minMessages != s_.maxMessages) {
        msgCount = QRandomGenerator::global()
                       ->bounded(s_.minMessages, s_.maxMessages + 1);
    }

    // 3) Вычисляем базовое время и интервал (в секундах)
    const double baseTime = s_.minMsgTime.seconds;
    const double interval = s_.msgInterval.seconds; // из QTimeEdit TIMEINTERVAL

    // 4) Подготовка размеров
    constexpr std::size_t hdrSize  = sizeof(TUMsgHdr);
    const std::size_t bodySize = static_cast<std::size_t>(s_.msgLength);

    // 5) Главный цикл: генерируем и сразу записываем сообщения
    for (int i = 0; i < msgCount; ++i) {
        // Заголовок сообщения
        TUMsgHdr hdr{};
        hdr.msgType = s_.msgType;
        hdr.msgNum = i + 1;
        hdr.senderType = s_.senderType;
        hdr.recipientType = s_.recipientType;
        hdr.senderInstance = s_.senderInstance;
        hdr.xrecipientInstance = s_.recipientInstance;

        hdr.msgTime.seconds = baseTime + i * interval;

        // 6) Формируем тело сообщения
        std::vector<char> body;
        body.reserve(bodySize);
        switch (s_.msgType) {
        case 0: // Нули
            body.assign(bodySize, 0);
            break;

        case 1: // Случайные 0/1
            body.resize(bodySize);
            for (std::size_t b = 0; b < bodySize; ++b)
                body[b] = char(QRandomGenerator::global()->bounded(2));
            break;

        case 2: { // Повтор строки
            QByteArray pattern = s_.fillText.toUtf8();
            if (pattern.isEmpty())
                pattern.append('\0');
            const std::size_t patLen = static_cast<std::size_t>(pattern.size());
            body.resize(bodySize);
            std::size_t pos = 0;
            while (pos < bodySize) {
                std::size_t chunk = std::min(patLen, bodySize - pos);
                std::memcpy(body.data() + pos, pattern.constData(), chunk);
                pos += chunk;
            }
            break;
        }

        default:
            body.assign(bodySize, 0);
            break;
        }

        // 7) Выравнивание до 4 байт
        std::size_t totalBytes = hdrSize + bodySize;
        int padding = int((4 - (totalBytes % 4)) % 4);
        totalBytes += padding;
        hdr.msgLength = static_cast<std::int32_t>(totalBytes);

        // 8) Запись в файл
        out.writeRawData(reinterpret_cast<const char*>(&hdr),
                         static_cast<int>(hdrSize));
        if (!body.empty())
            out.writeRawData(body.data(), static_cast<int>(bodySize));
        if (padding > 0) {
            char pad[3] = {};
            out.writeRawData(pad, padding);
        }
    }
}
