#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTime>
#include <QDebug>
#include <QScrollBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onReadyRead();

private slots:
    void on_btnSerialOpenClose_clicked(bool checked);
    void on_lineSerialWrite_returnPressed();
    void on_btnPortRefresh_clicked();
    void on_cbEnableHex_clicked(bool checked);
    void on_btnLogClear_clicked();

private:
    void setSerial();

    Ui::MainWindow *ui;
    QSerialPort m_serial;

};

#endif // MAINWINDOW_H
