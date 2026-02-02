#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Use WebSerialPort for WebAssembly, QSerialPort for native platforms
#ifdef __EMSCRIPTEN__
#include "webserialport.h"
#else
#include <QSerialPort>
#include <QSerialPortInfo>
#endif

#include <QTimer>
#include <QLabel>
#include <QTranslator>
#include <QActionGroup>
#include <QListWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QVector>
#include <QDateTime>
#include <QDockWidget>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QPainter>

// Simple delegate for single-line ComboBox items with custom height
class ComboBoxItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ComboBoxItemDelegate(int itemHeight, QObject *parent = nullptr) 
        : QStyledItemDelegate(parent), m_itemHeight(itemHeight) {}
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        return QSize(100, m_itemHeight);
    }
    
private:
    int m_itemHeight;
};

// Custom delegate for two-line ComboBox items
class TwoLineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TwoLineDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();
        
        // Draw background
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, QColor(52, 152, 219));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->fillRect(option.rect, QColor(236, 240, 241));
        } else {
            painter->fillRect(option.rect, Qt::white);
        }
        
        // Get text
        QString text = index.data(Qt::DisplayRole).toString();
        QStringList lines = text.split(" - ", Qt::SkipEmptyParts);
        
        // Set text color
        if (option.state & QStyle::State_Selected) {
            painter->setPen(Qt::white);
        } else {
            painter->setPen(QColor(44, 62, 80));
        }
        
        // Draw first line (port name) - bold
        if (!lines.isEmpty()) {
            QFont boldFont = painter->font();
            boldFont.setBold(true);
            boldFont.setPointSize(10);
            painter->setFont(boldFont);
            painter->drawText(option.rect.adjusted(12, 8, -12, -option.rect.height()/2 - 4), 
                            Qt::AlignLeft | Qt::AlignTop, lines[0]);
        }
        
        // Draw second line (description) - normal
        if (lines.size() > 1) {
            QFont normalFont = painter->font();
            normalFont.setBold(false);
            normalFont.setPointSize(9);
            painter->setFont(normalFont);
            
            if (option.state & QStyle::State_Selected) {
                painter->setPen(QColor(255, 255, 255, 200));
            } else {
                painter->setPen(QColor(127, 140, 141));
            }
            
            painter->drawText(option.rect.adjusted(12, option.rect.height()/2 + 4, -12, -8), 
                            Qt::AlignLeft | Qt::AlignBottom, lines[1]);
        }
        
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);
        return QSize(200, 68);  // Width, Height for two lines with generous spacing
    }
};

// Forward declaration
class PlotWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Data point structure for plotting
struct DataPoint {
    qint64 timestamp;
    double value;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_refreshButton_clicked();
    void on_openButton_clicked();
    void on_sendButton_clicked();
    void on_clearReceiveButton_clicked();
    void on_clearSendButton_clicked();
    void readData();
    void updateStatus();
    
    // Menu actions
    void on_actionSaveReceive_triggered();
    void on_actionSaveSend_triggered();
    void on_actionExit_triggered();
    void on_actionClearAll_triggered();
    void on_actionAbout_triggered();
    
    // Language actions
    void on_actionEnglish_triggered();
    void on_actionChinese_triggered();
    void on_actionJapanese_triggered();
    void on_actionGerman_triggered();
    void on_actionFrench_triggered();
    
    // Advanced features
    void on_autoSendTimer_timeout();
    void on_commandList_itemDoubleClicked(QListWidgetItem *item);
    void on_autoSendInterval_changed(int value);
    void addCommand();
    void deleteCommand();
    void parseReceivedData(const QByteArray &data);

private:
    Ui::MainWindow *ui;
    
#ifdef __EMSCRIPTEN__
    WebSerialPort *serialPort;
#else
    QSerialPort *serialPort;
#endif
    
    QTimer *statusTimer;
    qint64 rxBytes;  // Received bytes counter
    qint64 txBytes;  // Transmitted bytes counter
    
    // Status bar labels
    QLabel *statusLabel;
    QLabel *rxLabel;
    QLabel *txLabel;
    
    // Language support
    QTranslator *translator;
    QActionGroup *languageGroup;
    QString currentLanguage;
    
    // Advanced features
    QTimer *autoSendTimer;
    QListWidget *commandListWidget;
    QSpinBox *autoSendIntervalSpinBox;
    QCheckBox *autoSendCheckBox;
    QCheckBox *addLineBreakCheckBox;
    QCheckBox *addCarriageReturnCheckBox;
    QTabWidget *mainTabWidget;
    QTextEdit *plotterTextEdit;
    PlotWidget *plotWidget;  // Real-time plot widget
    QDockWidget *commandDock;  // Command list dock widget
    QPushButton *addCmdBtn;    // Add command button
    QPushButton *delCmdBtn;    // Delete command button
    
    // Data plotting
    QVector<QVector<DataPoint>> plotData;  // Multiple channels
    int maxDataPoints;
    
    void initUI();
    void refreshPortList();
    void appendReceiveText(const QString &text);
    void switchLanguage(const QString &language);
    void retranslateUI();
    void setupAdvancedUI();
    void updatePlotDisplay();
    void loadStyleSheet();
};

#endif // MAINWINDOW_H
