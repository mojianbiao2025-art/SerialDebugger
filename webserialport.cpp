#include "webserialport.h"
#include <QDebug>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>

// Global pointer for callback
static WebSerialPort* g_webSerialPort = nullptr;

// JavaScript interface
EM_JS(void, js_openSerialPort, (int baudRate, int dataBits, int stopBits, int parity), {
    if (!navigator.serial) {
        console.error('Web Serial API not supported');
        return;
    }

    navigator.serial.requestPort()
        .then(port => {
            window.serialPort = port;
            
            const options = {
                baudRate: baudRate,
                dataBits: dataBits,
                stopBits: stopBits,
                parity: parity === 0 ? 'none' : (parity === 1 ? 'odd' : 'even')
            };
            
            return port.open(options);
        })
        .then(() => {
            console.log('Serial port opened');
            Module.ccall('webserial_onOpened', 'void', [], []);
            startReading();
        })
        .catch(error => {
            console.error('Error opening serial port:', error);
            const errorMsg = error.toString();
            const errorPtr = allocateUTF8(errorMsg);
            Module.ccall('webserial_onError', 'void', ['number'], [errorPtr]);
            _free(errorPtr);
        });

    async function startReading() {
        const port = window.serialPort;
        while (port.readable) {
            const reader = port.readable.getReader();
            try {
                while (true) {
                    const { value, done } = await reader.read();
                    if (done) break;
                    
                    // Send data to C++
                    const dataPtr = Module._malloc(value.length);
                    Module.HEAPU8.set(value, dataPtr);
                    Module.ccall('webserial_onDataReceived', 'void', 
                                ['number', 'number'], [dataPtr, value.length]);
                    Module._free(dataPtr);
                }
            } catch (error) {
                console.error('Read error:', error);
            } finally {
                reader.releaseLock();
            }
        }
    }
});

EM_JS(void, js_closeSerialPort, (), {
    if (window.serialPort) {
        window.serialPort.close()
            .then(() => {
                console.log('Serial port closed');
                window.serialPort = null;
            })
            .catch(error => {
                console.error('Error closing serial port:', error);
            });
    }
});

EM_JS(void, js_writeSerialPort, (const char* data, int length), {
    if (window.serialPort && window.serialPort.writable) {
        const writer = window.serialPort.writable.getWriter();
        const dataArray = new Uint8Array(Module.HEAPU8.buffer, data, length);
        const dataCopy = new Uint8Array(dataArray);
        
        writer.write(dataCopy)
            .then(() => {
                console.log('Data written:', dataCopy.length, 'bytes');
            })
            .catch(error => {
                console.error('Write error:', error);
            })
            .finally(() => {
                writer.releaseLock();
            });
    }
});

// C++ callbacks called from JavaScript
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void webserial_onOpened() {
        if (g_webSerialPort) {
            g_webSerialPort->m_isOpen = true;
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void webserial_onError(const char* error) {
        if (g_webSerialPort) {
            g_webSerialPort->m_errorString = QString::fromUtf8(error);
            emit g_webSerialPort->errorOccurred(g_webSerialPort->m_errorString);
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void webserial_onDataReceived(const char* data, int length) {
        if (g_webSerialPort) {
            g_webSerialPort->m_readBuffer.append(data, length);
            emit g_webSerialPort->readyRead();
        }
    }
}

#endif // __EMSCRIPTEN__

WebSerialPort::WebSerialPort(QObject *parent)
    : QObject(parent)
    , m_isOpen(false)
    , m_baudRate(115200)
    , m_dataBits(Data8)
    , m_stopBits(OneStop)
    , m_parity(NoParity)
{
#ifdef __EMSCRIPTEN__
    g_webSerialPort = this;
#endif
}

WebSerialPort::~WebSerialPort()
{
    if (m_isOpen) {
        close();
    }
#ifdef __EMSCRIPTEN__
    if (g_webSerialPort == this) {
        g_webSerialPort = nullptr;
    }
#endif
}

bool WebSerialPort::open()
{
#ifdef __EMSCRIPTEN__
    js_openSerialPort(m_baudRate, m_dataBits, m_stopBits, m_parity);
    return true; // Actual result comes via callback
#else
    m_errorString = "Web Serial API only available in WebAssembly build";
    return false;
#endif
}

void WebSerialPort::close()
{
#ifdef __EMSCRIPTEN__
    js_closeSerialPort();
#endif
    m_isOpen = false;
}

bool WebSerialPort::isOpen() const
{
    return m_isOpen;
}

void WebSerialPort::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

void WebSerialPort::setDataBits(DataBits dataBits)
{
    m_dataBits = dataBits;
}

void WebSerialPort::setStopBits(StopBits stopBits)
{
    m_stopBits = stopBits;
}

void WebSerialPort::setParity(Parity parity)
{
    m_parity = parity;
}

qint64 WebSerialPort::write(const QByteArray &data)
{
#ifdef __EMSCRIPTEN__
    if (m_isOpen) {
        js_writeSerialPort(data.constData(), data.length());
        return data.length();
    }
#endif
    return -1;
}

QByteArray WebSerialPort::readAll()
{
    QByteArray data = m_readBuffer;
    m_readBuffer.clear();
    return data;
}

QString WebSerialPort::errorString() const
{
    return m_errorString;
}
