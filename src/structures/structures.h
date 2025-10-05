#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <cstring>
#include <QString>

using uchar = unsigned char;

// Маркер регистрационного файла
constexpr std::uint16_t REG_AMSL_MARKER = 0x1B5B;

// Время (TAUTime)
struct TAUTime {
    double seconds{0.0};
};

// Заголовок регистрационного файла
struct TRegFileHdr {
    double t;
    std::uint16_t  marker;
    uchar senderType;
    char armNum;
    char cpuNum;
    char version[10];

    TRegFileHdr()
        : t(0.0), marker(REG_AMSL_MARKER),
        senderType(0), armNum(0), cpuNum(0)
    {
        std::memset(version, 0, sizeof(version));
    }
};

// Заголовок сообщения
struct __attribute__((packed)) TUMsgHdr {
    std::int32_t msgLength;
    std::int32_t msgType;
    std::int32_t msgNum;
    TAUTime msgTime;
    uchar senderType;
    uchar recipientType;
    uchar senderInstance;
    uchar xrecipientInstance;
};
static_assert(sizeof(TUMsgHdr) == 24,
              "TUMsgHdr должно быть ровно 24 байта!");

// Хранилище параметров
struct VarStorage {
    int       minMessages{1};
    int       maxMessages{1};
    int       amountOfFiles{1};
    int       maxMessagesDisplayed{0};

    QString   filePath;
    QString   fillText;
    int       msgLength;
    int       msgType{0};

    TAUTime   minMsgTime;
    TAUTime   maxMsgTime;
    TAUTime   msgInterval;

    uchar     senderType{0};
    uchar     recipientType{0};
    uchar     senderInstance{0};
    uchar     recipientInstance{0};

    TRegFileHdr regHdr;
};

#endif // STRUCTURES_H
