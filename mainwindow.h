#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTime>

#include "structures.h"
#include "generator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onMessageTypeChanged(int index);
    void onFillIntervalChanged(int state);
    void updateMaxMessages(const QTime &);
    void generate_files();

private:
    Ui::MainWindow *ui;
    QString filePath;
};

#endif // MAINWINDOW_H
