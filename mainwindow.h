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
    void onClear();
    void onConnect();
    void onPortRefresh();
    void onReadyRead();
    void onWriteEntered();
    void onHexToggled(bool toggled);

private:
    void setSerial();
    void InitializeUI();

    Ui::MainWindow *ui;
    QSerialPort m_serial;

    QWidget * m_mainWidget;
    QVBoxLayout * m_mainLayout;
        QGroupBox * m_serialGroup;
        QFormLayout * m_serialLayout;
            QWidget * m_connectWidget;
            QHBoxLayout * m_connectLayout;
            QCheckBox * m_timeEnable;
            QCheckBox * m_prefixEnable;
            QCheckBox * m_echoEnable;
            QFrame * m_vLine;
            QCheckBox * m_hexEnable;
            QCheckBox * m_crEnable;
            QCheckBox * m_lfEnable;
                QPushButton * m_serialConnect;
                QPushButton * m_textClear;
                QPushButton * m_portRefresh;
            QComboBox * m_serialPort;
            QLineEdit * m_serialBaudRate;
            QLineEdit * m_serialWrite;
        QTextEdit * m_textOut;
};

#endif // MAINWINDOW_H
