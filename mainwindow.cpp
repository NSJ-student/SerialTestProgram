#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitializeUI();
    setSerial();

    connect(m_hexEnable, SIGNAL(toggled(bool)),
            this, SLOT(onHexToggled(bool)));
    connect(m_textClear, SIGNAL(clicked()),
            this, SLOT(onClear()));
    connect(m_serialConnect, SIGNAL(clicked()),
            this, SLOT(onConnect()));
    connect(&m_serial, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
    connect(m_serialWrite, SIGNAL(returnPressed()),
            this, SLOT(onWriteEntered()));

    m_serialWrite->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClear()
{
    m_textOut->clear();
}

void MainWindow::onConnect()
{
    if(m_serialConnect->text() == "Connect")
    {
        if(m_serial.isOpen())  m_serial.close();

        if(m_serialPort->count() > 0)
        {
            m_serial.setPortName(m_serialPort->currentText());
            m_serial.setBaudRate(m_serialBaudRate->text().toInt());
            if(m_serial.open(QSerialPort::ReadWrite))
            {
                m_textOut->append(QString("----------------------------- Open: %1")
                                  .arg(m_serialPort->currentText()));
                m_serialWrite->setEnabled(true);
                m_serialConnect->setText("Close");
            }
            else
            {
                m_textOut->append("***************************** Open fail: " + m_serial.errorString());
            }
        }
    }
    else
    {
        if(m_serial.isOpen())   m_serial.close();

        m_serialWrite->setEnabled(false);
        m_serialConnect->setText("Connect");
        m_textOut->append(QString("----------------------------- Close: %1")
                          .arg(m_serialPort->currentText()));
    }
}

void MainWindow::onReadyRead()
{
    qDebug() << "rx: " << m_serial.bytesAvailable();
    QByteArray ba = m_serial.readAll();

    if(m_timeEnable->isChecked())
    {
        m_textOut->moveCursor(QTextCursor::End);
        m_textOut->textCursor().insertText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
        m_textOut->moveCursor(QTextCursor::End);
    }
    if(m_prefixEnable->isChecked())
    {
        m_textOut->moveCursor(QTextCursor::End);
        m_textOut->textCursor().insertText(QString("Read: "));
        m_textOut->moveCursor(QTextCursor::End);
    }

    if(m_hexEnable->isChecked())
    {
        m_textOut->moveCursor(QTextCursor::End);
        m_textOut->textCursor().insertText(ba.toHex().data());
        m_textOut->moveCursor(QTextCursor::End);
    }
    else
    {
        m_textOut->moveCursor(QTextCursor::End);
        m_textOut->textCursor().insertText(QString(ba));
        m_textOut->moveCursor(QTextCursor::End);
    }
}

void MainWindow::onWriteEntered()
{
    if(m_serial.isOpen())
    {
        if(m_hexEnable->isChecked())
        {
            QByteArray ba = QByteArray::fromHex(m_serialWrite->text().toUtf8());
            m_serial.write(ba.data(), ba.length());

            if(m_echoEnable->isChecked())
            {
                if(m_timeEnable->isChecked())
                {
                    m_textOut->moveCursor(QTextCursor::End);
                    m_textOut->textCursor().insertText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
                    m_textOut->moveCursor(QTextCursor::End);
                }
                if(m_prefixEnable->isChecked())
                {
                    m_textOut->moveCursor(QTextCursor::End);
                    m_textOut->textCursor().insertText(QString("Write: "));
                    m_textOut->moveCursor(QTextCursor::End);
                }

                m_textOut->moveCursor(QTextCursor::End);
                m_textOut->textCursor().insertText(m_serialWrite->text().toUtf8());
                m_textOut->moveCursor(QTextCursor::End);
            }
        }
        else
        {
            QByteArray ba = m_serialWrite->text().toUtf8();
            if(m_crEnable->isChecked())
            {
                ba.append(0x0D);
            }
            if(m_lfEnable->isChecked())
            {
                ba.append(0x0A);
            }
            m_serial.write(ba.data(), ba.length());

            if(m_echoEnable->isChecked())
            {
                if(m_timeEnable->isChecked())
                {
                    m_textOut->moveCursor(QTextCursor::End);
                    m_textOut->textCursor().insertText(QTime::currentTime().toString("[hh:mm:ss.zzz] "));
                    m_textOut->moveCursor(QTextCursor::End);
                }
                if(m_prefixEnable->isChecked())
                {
                    m_textOut->moveCursor(QTextCursor::End);
                    m_textOut->textCursor().insertText(QString("Write: "));
                    m_textOut->moveCursor(QTextCursor::End);
                }
                m_textOut->moveCursor(QTextCursor::End);
                m_textOut->textCursor().insertText(m_serialWrite->text());
                m_textOut->moveCursor(QTextCursor::End);
            }
        }
        m_serialWrite->clear();
    }
    else
    {
        m_textOut->append("** not opened");
    }
}

void MainWindow::onHexToggled(bool toggled)
{
    m_crEnable->setEnabled(!toggled);
    m_lfEnable->setEnabled(!toggled);
}

void MainWindow::setSerial()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        m_serialPort->addItem(info.portName());
    }

    if(m_serialPort->count() > 0)
        m_serial.setPortName(m_serialPort->itemText(0));
    m_serial.setBaudRate(m_serialBaudRate->text().toInt());
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::InitializeUI()
{
    m_mainWidget = new QWidget;
    m_mainLayout = new QVBoxLayout;
    m_mainWidget->setLayout(m_mainLayout);
        m_serialGroup = new QGroupBox("Serial");
        m_serialGroup->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);
        m_serialLayout = new QFormLayout;
        m_serialGroup->setLayout(m_serialLayout);
            m_connectWidget = new QWidget;
            m_connectLayout = new QHBoxLayout;
            m_connectLayout->setMargin(0);
            m_connectWidget->setLayout(m_connectLayout);
                m_timeEnable = new QCheckBox("Time");
                m_echoEnable = new QCheckBox("Echo");
                m_prefixEnable = new QCheckBox("Prefix");
                m_vLine = new QFrame();
                m_vLine->setFrameShape(QFrame::VLine);
                m_vLine->setFrameShadow(QFrame::Sunken);
                m_hexEnable = new QCheckBox("Hex");
                m_crEnable = new QCheckBox("CR");
                m_crEnable->setChecked(true);
                m_lfEnable = new QCheckBox("LF");
                m_serialConnect = new QPushButton("Connect");
                m_textClear = new QPushButton("Clear");
                m_connectLayout->addWidget(m_timeEnable);
                m_connectLayout->addWidget(m_echoEnable);
                m_connectLayout->addWidget(m_prefixEnable);
                m_connectLayout->addWidget(m_vLine);
                m_connectLayout->addWidget(m_hexEnable);
                m_connectLayout->addWidget(m_crEnable);
                m_connectLayout->addWidget(m_lfEnable);
                m_connectLayout->addStretch();
                m_connectLayout->addWidget(m_serialConnect);
                m_connectLayout->addWidget(m_textClear);
            m_serialPort = new QComboBox();
            m_serialBaudRate = new QLineEdit("115200");
            m_serialWrite = new QLineEdit;

            m_serialLayout->addRow("Port: ", m_serialPort);
            m_serialLayout->addRow("BaudRate: ", m_serialBaudRate);
            m_serialLayout->addRow("Write: ", m_serialWrite);
            m_serialLayout->addRow(m_connectWidget);

        m_textOut = new QTextEdit();
        m_textOut->setFont(QFont(QString("Courier 10 Pitch")));

        m_mainLayout->addWidget(m_serialGroup);
        m_mainLayout->addWidget(m_textOut);

    this->setCentralWidget(m_mainWidget);
}
