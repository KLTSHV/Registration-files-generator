// #ifndef VARSTORAGE_H
// #define VARSTORAGE_H

// #include <stdint.h>

// class VarStorage
// {
// public:
//     VarStorage();
//     //TUMsgHdr
//     int32_t   msgLength;    // Длина сообщения
//     int32_t   msgType;       // Тип сообщения = EUMsgType
//     int32_t   msgNum;      // Порядковый номер сообщения
//     TAUTime   msgTime;          // Время создания сообщения
//     char     senderType;    // Отправитель, КФП: EUFuncPrg
//     char     recipientType;  // Получатель,  КФП: EUFuncPrg
//     char     senderInstance;  // Отправитель, экземпляр приложения
//     char     recipientInstance;// Получатель,  экземпляр приложения

//     //TRegFileHDR
//     double  t;         // дата, время создания (сек)
//     short    sMarker;       // маркер -  значение 0х1B5B
//     char    ucSenderType;  // идентификатор источника
//     char     cARMNum,       // физический номер АРМ  1-14
//         cCPUNum,       // номер процессорного модуля из состава АРМ  1-2
//         szVersion[10]; // номер версии КФП

//     //Общие
//     int amountOfFiles;
//     int maxAmountOfMessages;
//     int minAmountOfMessages;
//     TAUTime msgTime1;
//     TAUTime msgTime2; // временной промежуток

// };

// #endif // VARSTORAGE_H
