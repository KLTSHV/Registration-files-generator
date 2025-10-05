#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTime>
#include <QDateTime>
#include <QIntValidator>
#include <QRandomGenerator>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Валидаторы для MINAMOUNTMSG, MAXAMOUNMSG и LENTHMSG
    ui->MINAMOUNTMSG->setValidator(new QIntValidator(1, 1'000'000, this));
    ui->MAXAMOUNMSG->setValidator(new QIntValidator(1, 1'000'000, this));
    ui->LENTHMSG->setValidator(new QIntValidator(0, 1'000'000, this));

    // Настройка TIMEGAPMIN
    ui->TIMEGAPMIN->setDisplayFormat("HH:mm:ss");
    ui->TIMEGAPMIN->setMinimumTime(QTime(0,0,0));
    ui->TIMEGAPMIN->setMaximumTime(QTime(23,59,59));
    ui->TIMEGAPMIN->setTime(QTime(0,0,0));

    // Настройка TIMEGAPMAX
    ui->TIMEGAPMAX->setDisplayFormat("HH:mm:ss");
    ui->TIMEGAPMAX->setMinimumTime(QTime(0,0,0));
    ui->TIMEGAPMAX->setMaximumTime(QTime(23,59,59));
    ui->TIMEGAPMAX->setTime(QTime(0,0,0));

    // Настройка TIMEINTERVAL
    ui->TIMEINTERVAL->setDisplayFormat("HH:mm:ss");
    ui->TIMEINTERVAL->setMinimumTime(QTime(0,0,0));
    ui->TIMEINTERVAL->setMaximumTime(QTime(23,59,59));
    ui->TIMEINTERVAL->setTime(QTime(0,0,1));  // по умолчанию 1 секунда

    // По умолчанию: MSGBODYTEXT скрыт
    ui->MSGBODYTEXT->setVisible(false);
    ui->MSGBODYTEXT->setEnabled(false);

    // DMAXAMOUNTMSG всегда видим, чтобы показать макс. число сообщений
    ui->DMAXAMOUNTMSG->setVisible(true);

    // Сигналы/слоты:
    connect(ui->FILLINTERVAL, &QCheckBox::stateChanged,
            this, &MainWindow::onFillIntervalChanged);

    connect(ui->TYPEOFMSG, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onMessageTypeChanged);

    // При любом изменении времени пересчитываем максимум
    connect(ui->TIMEGAPMIN, &QTimeEdit::timeChanged,
            this, &MainWindow::updateMaxMessages);
    connect(ui->TIMEGAPMAX, &QTimeEdit::timeChanged,
            this, &MainWindow::updateMaxMessages);
    connect(ui->TIMEINTERVAL, &QTimeEdit::timeChanged,
            this, &MainWindow::updateMaxMessages);

    connect(ui->CREATEFILE, &QPushButton::clicked,
            this, &MainWindow::generate_files);

    // Сразу посчитаем “максимум сообщений” для начальных значений
    updateMaxMessages(QTime());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMessageTypeChanged(int index)
{
    // Если выбран тип «Повтор строки» (index == 2), показываем поле MSGBODYTEXT
    const bool textMode = (index == 2);
    ui->MSGBODYTEXT->setVisible(textMode);
    ui->MSGBODYTEXT->setEnabled(textMode);
}

void MainWindow::onFillIntervalChanged(int state)
{
    const bool fill = (state == Qt::Checked);

    // Когда FILLINTERVAL включён, скрываем MINAMOUNTMSG и MAXAMOUNMSG
    // Когда выключен — показываем их.
    ui->MINAMOUNTMSG->setVisible(!fill);
    ui->MAXAMOUNMSG->setVisible(!fill);

    // Но DMAXAMOUNTMSG остаётся видимой всегда, никаких изменений здесь
    // Просто обновим значение, если нужно:
    updateMaxMessages(QTime());
}

void MainWindow::updateMaxMessages(const QTime &)
{
    // Всегда пересчитываем максимум (независимо от состояния FILLINTERVAL),
    // поскольку нам нужно показывать пользователю “сколько максимум сообщений”
    // он может вставить при текущих TIMEGAPMIN, TIMEGAPMAX, TIMEINTERVAL.

    // Лямбда для перевода QTime в секунды
    auto toSeconds = [](const QTime &t) {
        return t.hour() * 3600.0 + t.minute() * 60.0 + t.second();
    };

    const double timeMin = toSeconds(ui->TIMEGAPMIN->time());
    const double timeMax = toSeconds(ui->TIMEGAPMAX->time());
    const double interval = toSeconds(ui->TIMEINTERVAL->time());

    int count = 0;
    if (interval > 0.0 && timeMax >= timeMin) {
        double diff = timeMax - timeMin;
        count = static_cast<int>(std::floor(diff / interval)) + 1;
    }

    ui->DMAXAMOUNTMSG->setText(QString::number(count));
}

void MainWindow::generate_files()
{
    // 1. Открываем диалог «Сохранить как...»
    QString filter = tr("Data Files (*.dat)");
    QString outPath = QFileDialog::getSaveFileName(
        this, tr("Сохранить как"), QString(), filter);
    if (outPath.isEmpty())
        return;

    if (!outPath.endsWith(".dat", Qt::CaseInsensitive))
        outPath += ".dat";

    // 2. Считываем TIMEGAPMIN, TIMEGAPMAX, проверяем корректность
    auto toSeconds = [](const QTime &t) {
        return t.hour() * 3600.0 + t.minute() * 60.0 + t.second();
    };
    const double timeMin = toSeconds(ui->TIMEGAPMIN->time());
    const double timeMax = toSeconds(ui->TIMEGAPMAX->time());
    if (timeMax < timeMin) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Временной интервал некорректен"));
        return;
    }

    const double interval = toSeconds(ui->TIMEINTERVAL->time());
    if (interval <= 0.0) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Интервал между сообщениями должен быть > 0"));
        return;
    }

    // 3. Определяем minMsg и maxMsg:
    int minMsg = 0;
    int maxMsg = 0;
    if (ui->FILLINTERVAL->isChecked()) {
        // Автоматически вычисляем количество сообщений:
        double diff = timeMax - timeMin;
        int count = static_cast<int>(std::floor(diff / interval)) + 1;
        if (count <= 0) {
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Невозможно вычислить количество сообщений"));
            return;
        }
        minMsg = maxMsg = count;
    }
    else {
        // Режим ручного ввода min/max
        bool ok = false;
        minMsg = ui->MINAMOUNTMSG->text().toInt(&ok);
        if (!ok) return;

        maxMsg = ui->MAXAMOUNMSG->text().toInt(&ok);
        if (!ok || maxMsg < minMsg) {
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Максимум сообщений должен быть ≥ минимума"));
            return;
        }
        if(maxMsg > ui->DMAXAMOUNTMSG->text().toInt()){
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Максимум сообщений не должен превышать максимально возможное количество"));
            return;
        }
    }

    // 4. Считываем длину тела сообщения
    const int bodyLen = ui->LENTHMSG->text().toInt();
    if (bodyLen < 0) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Неверная длина тела сообщения"));
        return;
    }

    // 5. Считываем тип сообщения и, при необходимости, текст для «Повтор строки»
    const int typeIndex = ui->TYPEOFMSG->currentIndex();
    QString fillText;
    if (typeIndex == 2) {
        fillText = ui->MSGBODYTEXT->text().trimmed();
        if (fillText.isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Введите текст для повтора"));
            return;
        }
    }

    // 6. Собираем настройки в VarStorage
    VarStorage settings;
    settings.filePath.clear(); // не используется напрямую
    settings.minMessages = minMsg;
    settings.maxMessages = maxMsg;
    settings.amountOfFiles = 1;
    settings.msgLength = bodyLen;
    settings.minMsgTime.seconds = timeMin;
    settings.maxMsgTime.seconds = timeMax;
    settings.msgInterval.seconds = interval;
    settings.msgType = typeIndex;
    settings.fillText = fillText;
    settings.regHdr.t = QDateTime::currentSecsSinceEpoch();

    // 7. Генерируем .dat-файл
    Generator(settings).generateDatFile(outPath);

    QMessageBox::information(this, tr("Готово"),
                             tr("Файл «%1» успешно создан.").arg(outPath));
}
