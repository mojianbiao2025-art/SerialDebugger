#ifndef WEBSERIALPORT_H
#define WEBSERIALPORT_H

#include <QObject>
#include <QByteArray>
#include <QString>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/val.h>
#endif

// Web Serial API wrapper for Qt WebAssembly
class WebSerialPort : public QObject
{
    Q_OBJECT

public:
    enum BaudRate {
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200
    };

    enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8
    };

    enum StopBits {
        OneStop = 1,
        TwoStop = 2
    };

    enum Parity {
        NoParity = 0,
        OddParity = 1,
        EvenParity = 2
    };

    explicit WebSerialPort(QObject *parent = nullptr);
    ~WebSerialPort();

    bool open();
    void close();
    bool isOpen() const;

    void setBaudRate(int baudRate);
    void setDataBits(DataBits dataBits);
    void setStopBits(StopBits stopBits);
    void setParity(Parity parity);

    qint64 write(const QByteArray &data);
    QByteArray readAll();

    QString errorString() const;

signals:
    void readyRead();
    void errorOccurred(const QString &error);

private:
    bool m_isOpen;
    int m_baudRate;
    DataBits m_dataBits;
    StopBits m_stopBits;
    Parity m_parity;
    QString m_errorString;
    QByteArray m_readBuffer;

#ifdef __EMSCRIPTEN__
    void setupWebSerial();
    static void onDataReceived(const char* data, int length);
#endif
};

#endif // WEBSERIALPORT_H
