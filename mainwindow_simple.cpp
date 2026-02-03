#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QGroupBox>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , rxBytes(0)
    , txBytes(0)
{
    qDebug() << "=== MainWindow 构造开始 ===";
    
    // 设置窗口属性
    setWindowTitle("串口调试助手 - WebAssembly 版");
    setMinimumSize(800, 600);
    
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("QWidget { background-color: #ffffff; }");
    setCentralWidget(centralWidget);
    qDebug() << "中心部件已创建";
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 标题
    QLabel *titleLabel = new QLabel("🔌 串口调试助手", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #667eea; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    qDebug() << "标题已添加";
    
    // 状态标签
    QLabel *statusLabel = new QLabel("✅ Qt WebAssembly 渲染正常工作！", this);
    statusLabel->setStyleSheet("font-size: 16px; color: #28a745; padding: 10px; background-color: #d4edda; border-radius: 4px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);
    qDebug() << "状态标签已添加";
    
    // 串口配置组
    QGroupBox *configGroup = new QGroupBox("串口配置", this);
    configGroup->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; padding: 10px; }");
    QHBoxLayout *configLayout = new QHBoxLayout(configGroup);
    
    QLabel *portLabel = new QLabel("端口:", this);
    portLabel->setStyleSheet("font-size: 14px;");
    configLayout->addWidget(portLabel);
    
    QLineEdit *portEdit = new QLineEdit("COM1", this);
    portEdit->setStyleSheet("font-size: 14px; padding: 5px;");
    configLayout->addWidget(portEdit);
    
    QPushButton *connectBtn = new QPushButton("连接", this);
    connectBtn->setStyleSheet("font-size: 14px; padding: 8px 20px; background-color: #667eea; color: white; border: none; border-radius: 4px;");
    configLayout->addWidget(connectBtn);
    
    mainLayout->addWidget(configGroup);
    qDebug() << "配置组已添加";
    
    // 接收区域
    QGroupBox *receiveGroup = new QGroupBox("接收区域", this);
    receiveGroup->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    QVBoxLayout *receiveLayout = new QVBoxLayout(receiveGroup);
    
    QTextEdit *receiveText = new QTextEdit(this);
    receiveText->setPlaceholderText("接收到的数据将显示在这里...");
    receiveText->setStyleSheet("font-size: 14px; padding: 10px; border: 1px solid #ddd; border-radius: 4px;");
    receiveText->setMinimumHeight(200);
    receiveLayout->addWidget(receiveText);
    
    mainLayout->addWidget(receiveGroup);
    qDebug() << "接收区域已添加";
    
    // 发送区域
    QGroupBox *sendGroup = new QGroupBox("发送区域", this);
    sendGroup->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    QVBoxLayout *sendLayout = new QVBoxLayout(sendGroup);
    
    QLineEdit *sendEdit = new QLineEdit(this);
    sendEdit->setPlaceholderText("输入要发送的数据...");
    sendEdit->setStyleSheet("font-size: 14px; padding: 8px; border: 1px solid #ddd; border-radius: 4px;");
    sendLayout->addWidget(sendEdit);
    
    QPushButton *sendBtn = new QPushButton("发送", this);
    sendBtn->setStyleSheet("font-size: 16px; padding: 10px; background-color: #28a745; color: white; border: none; border-radius: 4px;");
    sendLayout->addWidget(sendBtn);
    
    mainLayout->addWidget(sendGroup);
    qDebug() << "发送区域已添加";
    
    // 底部信息
    QLabel *infoLabel = new QLabel("WebAssembly 版本 | 作者：莫建标 | 上海大族富创得股份有限公司", this);
    infoLabel->setStyleSheet("font-size: 12px; color: #666; padding: 10px;");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);
    
    // 连接信号
    connect(connectBtn, &QPushButton::clicked, [=]() {
        qDebug() << "连接按钮被点击";
        statusLabel->setText("🔄 正在连接...");
        statusLabel->setStyleSheet("font-size: 16px; color: #856404; padding: 10px; background-color: #fff3cd; border-radius: 4px;");
    });
    
    connect(sendBtn, &QPushButton::clicked, [=]() {
        QString text = sendEdit->text();
        qDebug() << "发送按钮被点击，内容：" << text;
        if (!text.isEmpty()) {
            receiveText->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] 发送: " + text);
            sendEdit->clear();
        }
    });
    
    qDebug() << "=== MainWindow 构造完成 ===";
    qDebug() << "窗口尺寸:" << size();
    qDebug() << "中心部件尺寸:" << centralWidget->size();
}

MainWindow::~MainWindow()
{
    qDebug() << "=== MainWindow 析构 ===";
}
