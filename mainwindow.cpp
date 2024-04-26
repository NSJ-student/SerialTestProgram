#include "mainwindow.h"
#include "ui_mainwindow.h"

// https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setSerial();

    connect(&m_serial, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));;

    ui->cbSerialPort->setEnabled(true);
    ui->cbBaudrate->setEnabled(true);
    ui->lineSerialWrite->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadyRead()
{
    qDebug() << "rx: " << m_serial.bytesAvailable();
    QByteArray ba = m_serial.readAll();

    if(ui->cbShowTime->isChecked())
    {
        ui->txtSerialLog->insertPlainText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
    }
    if(ui->cbAddPrefix->isChecked())
    {
        ui->txtSerialLog->insertPlainText(QString("Read: "));
    }

    if(ui->cbEnableHex->isChecked())
    {
        ui->txtSerialLog->insertPlainText(ba.toHex().data());
    }
    else
    {
        QString input_text = QString(ba);
        input_text.replace("\r\r\n", "\r\n");
        ui->txtSerialLog->insertPlainText(input_text);
    }
    if(ui->cbEnableAutoScroll->isChecked())
    {
        ui->txtSerialLog->verticalScrollBar()->setValue(ui->txtSerialLog->verticalScrollBar()->maximum());
    }
}

void MainWindow::setSerial()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cbSerialPort->addItem(info.portName());
    }

    if(ui->cbSerialPort->count() > 0)
        m_serial.setPortName(ui->cbSerialPort->itemText(0));
    m_serial.setBaudRate(ui->cbBaudrate->currentText().toInt());
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::on_btnSerialOpenClose_clicked(bool checked)
{
    if(checked)
    {
        if(m_serial.isOpen())  m_serial.close();

        if(ui->cbSerialPort->count() > 0)
        {
            m_serial.setPortName(ui->cbSerialPort->currentText());
            m_serial.setBaudRate(ui->cbBaudrate->currentText().toInt());
            if(m_serial.open(QSerialPort::ReadWrite))
            {
                ui->txtSerialLog->append(QString("----------------------------- Open: %1")
                                  .arg(ui->cbSerialPort->currentText()));
                ui->cbSerialPort->setEnabled(false);
                ui->cbBaudrate->setEnabled(false);
                ui->lineSerialWrite->setEnabled(true);
                ui->btnSerialOpenClose->setText("Close");
            }
            else
            {
                ui->txtSerialLog->append("***************************** Open fail: " + m_serial.errorString());
            }
        }
    }
    else
    {
        if(m_serial.isOpen())   m_serial.close();

        ui->cbSerialPort->setEnabled(true);
        ui->cbBaudrate->setEnabled(true);
        ui->lineSerialWrite->setEnabled(false);
        ui->btnSerialOpenClose->setText("Connect");
        ui->txtSerialLog->append(QString("----------------------------- Close: %1")
                          .arg(ui->cbSerialPort->currentText()));
    }
}

void MainWindow::on_lineSerialWrite_returnPressed()
{
    if(m_serial.isOpen())
    {
        if(ui->cbEnableHex->isChecked())
        {
            QByteArray ba = QByteArray::fromHex(ui->lineSerialWrite->text().toUtf8());
            m_serial.write(ba.data(), ba.length());

            if(ui->cbEnableEcho->isChecked())
            {
                if(ui->cbShowTime->isChecked())
                {
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                    ui->txtSerialLog->textCursor().insertText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                }
                if(ui->cbAddPrefix->isChecked())
                {
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                    ui->txtSerialLog->textCursor().insertText(QString("Write: "));
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                }

                ui->txtSerialLog->moveCursor(QTextCursor::End);
                ui->txtSerialLog->textCursor().insertText(ui->lineSerialWrite->text().toUtf8());
                ui->txtSerialLog->moveCursor(QTextCursor::End);
            }
        }
        else
        {
            QByteArray ba = ui->lineSerialWrite->text().toUtf8();
            if(ui->cbAddCr->isChecked())
            {
                ba.append(0x0D);
            }
            if(ui->cbAddLf->isChecked())
            {
                ba.append(0x0A);
            }
            m_serial.write(ba.data(), ba.length());

            if(ui->cbEnableEcho->isChecked())
            {
                if(ui->cbShowTime->isChecked())
                {
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                    ui->txtSerialLog->textCursor().insertText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                }
                if(ui->cbAddPrefix->isChecked())
                {
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                    ui->txtSerialLog->textCursor().insertText(QString("Write: "));
                    ui->txtSerialLog->moveCursor(QTextCursor::End);
                }
                ui->txtSerialLog->moveCursor(QTextCursor::End);
                ui->txtSerialLog->textCursor().insertText(ui->lineSerialWrite->text());
                ui->txtSerialLog->moveCursor(QTextCursor::End);
            }
        }
        ui->lineSerialWrite->clear();
    }
    else
    {
        ui->txtSerialLog->append("** not opened");
    }
}

void MainWindow::on_btnPortRefresh_clicked()
{
    if(m_serial.isOpen())
    {
        return;
    }

    ui->cbSerialPort->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cbSerialPort->addItem(info.portName());
    }
}

void MainWindow::on_cbEnableHex_clicked(bool checked)
{
    ui->cbAddCr->setEnabled(!checked);
    ui->cbAddLf->setEnabled(!checked);
}

void MainWindow::on_btnLogClear_clicked()
{
    ui->txtSerialLog->clear();
}
