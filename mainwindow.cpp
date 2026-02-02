#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "translations.h"
#include "plotwidget.h"
#include <QMessageBox>
#include <QDateTime>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QCoreApplication>
#include <QFile>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRegularExpression>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
#ifdef __EMSCRIPTEN__
    , serialPort(new WebSerialPort(this))
#else
    , serialPort(new QSerialPort(this))
#endif
    , statusTimer(new QTimer(this))
    , rxBytes(0)
    , txBytes(0)
    , translator(new QTranslator(this))
    , currentLanguage("zh")
    , autoSendTimer(new QTimer(this))
    , maxDataPoints(1000)
{
    ui->setupUi(this);
    
    // Set window icon
    setWindowIcon(QIcon(":/icons/app_icon.ico"));
    
    // Load stylesheet
    loadStyleSheet();
    
    // Set window properties
    setWindowTitle("Serial Port Debugger");
    resize(1000, 700);
    
    // Create status bar labels
    statusLabel = new QLabel("Status: Disconnected", this);
    rxLabel = new QLabel("RX: 0 bytes", this);
    txLabel = new QLabel("TX: 0 bytes", this);
    
    ui->statusbar->addWidget(statusLabel);
    ui->statusbar->addPermanentWidget(rxLabel);
    ui->statusbar->addPermanentWidget(txLabel);
    
    // Setup language action group
    languageGroup = new QActionGroup(this);
    languageGroup->addAction(ui->actionEnglish);
    languageGroup->addAction(ui->actionChinese);
    languageGroup->addAction(ui->actionJapanese);
    languageGroup->addAction(ui->actionGerman);
    languageGroup->addAction(ui->actionFrench);
    ui->actionChinese->setChecked(true);
    
    // Initialize plot data with 6 channels
    plotData.resize(6);
    
    initUI();
    setupAdvancedUI();
    
#ifdef __EMSCRIPTEN__
    connect(serialPort, &WebSerialPort::readyRead, this, &MainWindow::readData);
#else
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);
#endif
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    connect(autoSendTimer, &QTimer::timeout, this, &MainWindow::on_autoSendTimer_timeout);
    
    statusTimer->start(100);
    
    // Apply default language (Chinese)
    retranslateUI();
}

MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete ui;
}

void MainWindow::initUI()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    setWindowTitle(trans["window_title"]);
    
    // Set custom delegate for port combo box to show two-line items
    ui->portCombo->setItemDelegate(new TwoLineDelegate(this));
    
    // Set custom delegates for other combo boxes with 40px item height
    ui->baudRateCombo->setItemDelegate(new ComboBoxItemDelegate(40, this));
    ui->dataBitsCombo->setItemDelegate(new ComboBoxItemDelegate(40, this));
    ui->stopBitsCombo->setItemDelegate(new ComboBoxItemDelegate(40, this));
    ui->parityCombo->setItemDelegate(new ComboBoxItemDelegate(40, this));
    
    // Baud rate options
    ui->baudRateCombo->addItems({"9600", "19200", "38400", "57600", "115200"});
    ui->baudRateCombo->setCurrentText("115200");
    
    // Data bits
    ui->dataBitsCombo->addItems({"5", "6", "7", "8"});
    ui->dataBitsCombo->setCurrentText("8");
    
    // Stop bits
    ui->stopBitsCombo->addItems({"1", "1.5", "2"});
    ui->stopBitsCombo->setCurrentText("1");
    
    // Parity
    ui->parityCombo->addItems({trans["parity_none"], trans["parity_odd"], trans["parity_even"]});
    ui->parityCombo->setCurrentIndex(0);
    
    // Refresh port list
    refreshPortList();
    
    ui->openButton->setText(trans["open_port"]);
}

void MainWindow::refreshPortList()
{
    ui->portCombo->clear();
    
#ifndef __EMSCRIPTEN__
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QString itemText = info.portName() + " - " + info.description();
        ui->portCombo->addItem(itemText);
        
        // Set tooltip for the item to show full text
        int index = ui->portCombo->count() - 1;
        ui->portCombo->setItemData(index, itemText, Qt::ToolTipRole);
    }
#else
    // WebAssembly: User will select port through browser dialog
    ui->portCombo->addItem("Click 'Open Port' to select...");
#endif
}

void MainWindow::on_refreshButton_clicked()
{
    refreshPortList();
}

void MainWindow::on_openButton_clicked()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    
    if (serialPort->isOpen()) {
        serialPort->close();
        ui->openButton->setText(trans["open_port"]);
        statusLabel->setText(trans["status_disconnected"]);
        ui->portCombo->setEnabled(true);
        ui->baudRateCombo->setEnabled(true);
        ui->dataBitsCombo->setEnabled(true);
        ui->stopBitsCombo->setEnabled(true);
        ui->parityCombo->setEnabled(true);
    } else {
        QString portName = ui->portCombo->currentText().split(" - ").first();
        serialPort->setPortName(portName);
        serialPort->setBaudRate(ui->baudRateCombo->currentText().toInt());
        
        // Set data bits
        switch (ui->dataBitsCombo->currentText().toInt()) {
#ifdef __EMSCRIPTEN__
            case 5: serialPort->setDataBits(WebSerialPort::Data5); break;
            case 6: serialPort->setDataBits(WebSerialPort::Data6); break;
            case 7: serialPort->setDataBits(WebSerialPort::Data7); break;
            case 8: serialPort->setDataBits(WebSerialPort::Data8); break;
#else
            case 5: serialPort->setDataBits(QSerialPort::Data5); break;
            case 6: serialPort->setDataBits(QSerialPort::Data6); break;
            case 7: serialPort->setDataBits(QSerialPort::Data7); break;
            case 8: serialPort->setDataBits(QSerialPort::Data8); break;
#endif
        }
        
        // Set stop bits
        if (ui->stopBitsCombo->currentText() == "1") {
#ifdef __EMSCRIPTEN__
            serialPort->setStopBits(WebSerialPort::OneStop);
        } else if (ui->stopBitsCombo->currentText() == "1.5") {
            // WebAssembly doesn't support 1.5 stop bits, use 1
            serialPort->setStopBits(WebSerialPort::OneStop);
#else
            serialPort->setStopBits(QSerialPort::OneStop);
        } else if (ui->stopBitsCombo->currentText() == "1.5") {
            serialPort->setStopBits(QSerialPort::OneAndHalfStop);
#endif
        } else {
#ifdef __EMSCRIPTEN__
            serialPort->setStopBits(WebSerialPort::TwoStop);
#else
            serialPort->setStopBits(QSerialPort::TwoStop);
#endif
        }
        
        // Set parity based on index
        int parityIndex = ui->parityCombo->currentIndex();
        if (parityIndex == 0) {
#ifdef __EMSCRIPTEN__
            serialPort->setParity(WebSerialPort::NoParity);
        } else if (parityIndex == 1) {
            serialPort->setParity(WebSerialPort::OddParity);
        } else {
            serialPort->setParity(WebSerialPort::EvenParity);
#else
            serialPort->setParity(QSerialPort::NoParity);
        } else if (parityIndex == 1) {
            serialPort->setParity(QSerialPort::OddParity);
        } else {
            serialPort->setParity(QSerialPort::EvenParity);
#endif
        }
        
#ifdef __EMSCRIPTEN__
        serialPort->setFlowControl(WebSerialPort::NoFlowControl);
#else
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
#endif
        
        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->openButton->setText(trans["close_port"]);
            statusLabel->setText(trans["status_connected"] + portName);
            ui->portCombo->setEnabled(false);
            ui->baudRateCombo->setEnabled(false);
            ui->dataBitsCombo->setEnabled(false);
            ui->stopBitsCombo->setEnabled(false);
            ui->parityCombo->setEnabled(false);
            rxBytes = 0;
            txBytes = 0;
        } else {
            QMessageBox::critical(this, trans["error"], trans["failed_to_open"] + serialPort->errorString());
        }
    }
}

void MainWindow::on_sendButton_clicked()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    
    if (!serialPort->isOpen()) {
        QMessageBox::warning(this, trans["warning"], trans["open_port_first"]);
        return;
    }
    
    QString text = ui->sendText->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    
    QByteArray data;
    if (ui->hexSendCheck->isChecked()) {
        // HEX send mode
        text = text.replace(" ", "").replace("\n", "").replace("\r", "");
        data = QByteArray::fromHex(text.toLatin1());
    } else {
        data = text.toUtf8();
        
        // Add line endings based on checkboxes
        if (addCarriageReturnCheckBox && addCarriageReturnCheckBox->isChecked()) {
            data.append("\r");
        }
        if (addLineBreakCheckBox && addLineBreakCheckBox->isChecked()) {
            data.append("\n");
        }
        // Fallback to old checkbox
        if (ui->sendNewLineCheck->isChecked() && 
            (!addCarriageReturnCheckBox || !addLineBreakCheckBox)) {
            data.append("\r\n");
        }
    }
    
    qint64 written = serialPort->write(data);
    if (written != -1) {
        txBytes += written;
    }
}

void MainWindow::readData()
{
    QByteArray data = serialPort->readAll();
    rxBytes += data.size();
    
    // Parse data for plotting
    parseReceivedData(data);
    
    QString text;
    if (ui->hexReceiveCheck->isChecked()) {
        // HEX display mode - Qt 5.6 compatible
        text = data.toHex().toUpper();
        // Add spaces between bytes
        QString formatted;
        for (int i = 0; i < text.length(); i += 2) {
            if (i > 0) formatted += " ";
            formatted += text.mid(i, 2);
        }
        text = formatted;
    } else {
        text = QString::fromUtf8(data);
    }
    
    appendReceiveText(text);
}

void MainWindow::appendReceiveText(const QString &text)
{
    if (ui->timestampCheck->isChecked()) {
        QString timestamp = "[" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "] ";
        ui->receiveText->append(timestamp + text);
    } else {
        ui->receiveText->insertPlainText(text);
    }
    
    // Auto scroll to bottom
    QTextCursor cursor = ui->receiveText->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->receiveText->setTextCursor(cursor);
}

void MainWindow::on_clearReceiveButton_clicked()
{
    ui->receiveText->clear();
    rxBytes = 0;
}

void MainWindow::on_clearSendButton_clicked()
{
    ui->sendText->clear();
}

void MainWindow::updateStatus()
{
    if (serialPort->isOpen()) {
        rxLabel->setText(QString("RX: %1 bytes").arg(rxBytes));
        txLabel->setText(QString("TX: %1 bytes").arg(txBytes));
    }
}

// Menu actions implementation
void MainWindow::on_actionSaveReceive_triggered()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    QString fileName = QFileDialog::getSaveFileName(this, 
        trans["save_file"], 
        "", 
        trans["text_files"]);
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->receiveText->toPlainText();
            file.close();
            QMessageBox::information(this, trans["menu_file"], trans["save_success"]);
        } else {
            QMessageBox::critical(this, trans["error"], trans["save_failed"]);
        }
    }
}

void MainWindow::on_actionSaveSend_triggered()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    QString fileName = QFileDialog::getSaveFileName(this, 
        trans["save_file"], 
        "", 
        trans["text_files"]);
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->sendText->toPlainText();
            file.close();
            QMessageBox::information(this, trans["menu_file"], trans["save_success"]);
        } else {
            QMessageBox::critical(this, trans["error"], trans["save_failed"]);
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionClearAll_triggered()
{
    ui->receiveText->clear();
    ui->sendText->clear();
    rxBytes = 0;
    txBytes = 0;
    
    // Clear plot data
    if (plotWidget) {
        plotWidget->clearData();
    }
    for (int i = 0; i < plotData.size(); ++i) {
        plotData[i].clear();
    }
    updatePlotDisplay();
}

void MainWindow::on_actionAbout_triggered()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    QMessageBox::about(this, trans["about"], trans["about_text"]);
}

// Language switching
void MainWindow::on_actionEnglish_triggered()
{
    switchLanguage("en");
}

void MainWindow::on_actionChinese_triggered()
{
    switchLanguage("zh");
}

void MainWindow::on_actionJapanese_triggered()
{
    switchLanguage("ja");
}

void MainWindow::on_actionGerman_triggered()
{
    switchLanguage("de");
}

void MainWindow::on_actionFrench_triggered()
{
    switchLanguage("fr");
}

void MainWindow::switchLanguage(const QString &language)
{
    currentLanguage = language;
    retranslateUI();
}

void MainWindow::retranslateUI()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    
    // Window title
    setWindowTitle(trans["window_title"]);
    
    // Tab titles
    if (mainTabWidget) {
        mainTabWidget->setTabText(0, trans["tab_main"]);
        mainTabWidget->setTabText(1, trans["tab_plotter"]);
    }
    
    // Update plot widget text
    if (plotWidget) {
        plotWidget->setPlotTexts(trans["plot_title"], trans["plot_value"], 
                                trans["plot_points"], trans["plot_waiting"]);
    }
    
    // Group boxes
    ui->groupBox->setTitle(trans["port_settings"]);
    ui->groupBox_2->setTitle(trans["receive"]);
    ui->groupBox_3->setTitle(trans["send"]);
    
    // Labels
    ui->label->setText(trans["port"]);
    ui->label_2->setText(trans["baud_rate"]);
    ui->label_3->setText(trans["data_bits"]);
    ui->label_4->setText(trans["stop_bits"]);
    ui->label_5->setText(trans["parity"]);
    
    // Buttons
    ui->refreshButton->setText(trans["refresh"]);
    ui->clearReceiveButton->setText(trans["clear"]);
    ui->clearSendButton->setText(trans["clear"]);
    ui->sendButton->setText(trans["send_btn"]);
    
    // Checkboxes
    ui->hexReceiveCheck->setText(trans["hex_display"]);
    ui->timestampCheck->setText(trans["show_timestamp"]);
    ui->hexSendCheck->setText(trans["hex_send"]);
    ui->sendNewLineCheck->setText(trans["add_newline"]);
    
    // Open/Close button
    if (serialPort->isOpen()) {
        ui->openButton->setText(trans["close_port"]);
        statusLabel->setText(trans["status_connected"] + serialPort->portName());
    } else {
        ui->openButton->setText(trans["open_port"]);
        statusLabel->setText(trans["status_disconnected"]);
    }
    
    // Update parity combo
    int currentParityIndex = ui->parityCombo->currentIndex();
    ui->parityCombo->clear();
    ui->parityCombo->addItems({trans["parity_none"], trans["parity_odd"], trans["parity_even"]});
    ui->parityCombo->setCurrentIndex(currentParityIndex);
    
    // Menu
    ui->menuFile->setTitle(trans["menu_file"]);
    ui->menuView->setTitle(trans["menu_view"]);
    ui->menuLanguage->setTitle(trans["menu_language"]);
    ui->menuHelp->setTitle(trans["menu_help"]);
    
    ui->actionSaveReceive->setText(trans["save_receive"]);
    ui->actionSaveSend->setText(trans["save_send"]);
    ui->actionExit->setText(trans["exit"]);
    ui->actionClearAll->setText(trans["clear_all"]);
    ui->actionAbout->setText(trans["about"]);
    
    // Update status labels
    rxLabel->setText(QString("RX: %1 bytes").arg(rxBytes));
    txLabel->setText(QString("TX: %1 bytes").arg(txBytes));
    
    // Update auto-send controls if they exist
    if (autoSendCheckBox) {
        autoSendCheckBox->setText(trans["auto_send"]);
    }
    if (addLineBreakCheckBox) {
        addLineBreakCheckBox->setText(trans["add_lf"]);
    }
    if (addCarriageReturnCheckBox) {
        addCarriageReturnCheckBox->setText(trans["add_cr"]);
    }
    
    // Update command list dock widget
    if (commandDock) {
        commandDock->setWindowTitle(trans["command_list"]);
    }
    if (addCmdBtn) {
        addCmdBtn->setText(trans["add_command"]);
    }
    if (delCmdBtn) {
        delCmdBtn->setText(trans["delete_command"]);
    }
    
    // Update example commands in the list
    if (commandListWidget && commandListWidget->count() == 3) {
        // Only update if these are still the default example commands
        commandListWidget->item(0)->setText("CMD_1 | " + trans["example_command"] + " 1");
        commandListWidget->item(1)->setText("CMD_2 | " + trans["example_command"] + " 2");
        commandListWidget->item(2)->setText("CMD_3 | " + trans["example_command"] + " 3");
    }
}

// Advanced features implementation
void MainWindow::setupAdvancedUI()
{
    // Create tab widget for main/plotter views
    mainTabWidget = new QTabWidget(this);
    
    // Get the current central widget
    QWidget *currentCentral = centralWidget();
    
    // Create plotter tab with splitter for resizable areas
    QWidget *plotterTab = new QWidget(this);
    QVBoxLayout *plotterLayout = new QVBoxLayout(plotterTab);
    plotterLayout->setContentsMargins(0, 0, 0, 0);
    plotterLayout->setSpacing(0);
    
    // Create splitter for resizable plot and text areas
    QSplitter *plotterSplitter = new QSplitter(Qt::Vertical, plotterTab);
    plotterSplitter->setHandleWidth(6);
    plotterSplitter->setChildrenCollapsible(false);
    
    // Create plot widget
    plotWidget = new PlotWidget(plotterSplitter);
    plotWidget->setMinimumHeight(200);
    
    // Create text info area
    plotterTextEdit = new QTextEdit(plotterSplitter);
    plotterTextEdit->setReadOnly(true);
    plotterTextEdit->setFont(QFont("Courier", 9));
    plotterTextEdit->setMinimumHeight(80);
    plotterTextEdit->setMaximumHeight(200);
    
    // Add widgets to splitter
    plotterSplitter->addWidget(plotWidget);
    plotterSplitter->addWidget(plotterTextEdit);
    
    // Set initial sizes (70% plot, 30% text)
    QList<int> sizes;
    sizes << 400 << 150;
    plotterSplitter->setSizes(sizes);
    
    // Set splitter style
    plotterSplitter->setStyleSheet(
        "QSplitter::handle {"
        "    background-color: #3498db;"
        "    border: 1px solid #2980b9;"
        "}"
        "QSplitter::handle:hover {"
        "    background-color: #2980b9;"
        "}"
        "QSplitter::handle:pressed {"
        "    background-color: #21618c;"
        "}"
    );
    
    plotterLayout->addWidget(plotterSplitter);
    
    // Add tabs with proper text
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    mainTabWidget->addTab(currentCentral, trans["tab_main"]);
    mainTabWidget->addTab(plotterTab, trans["tab_plotter"]);
    
    // Set tab bar style and properties
    mainTabWidget->setTabPosition(QTabWidget::North);
    mainTabWidget->setDocumentMode(false);
    
    // Ensure tab text is properly displayed with extra styling
    mainTabWidget->tabBar()->setExpanding(false);
    mainTabWidget->tabBar()->setUsesScrollButtons(false);
    mainTabWidget->tabBar()->setDrawBase(true);
    
    // Set minimum size for tab bar to ensure text is visible
    mainTabWidget->tabBar()->setMinimumHeight(45);
    
    setCentralWidget(mainTabWidget);
    
    // Create command list widget (right side panel)
    commandDock = new QDockWidget(trans["command_list"], this);
    commandDock->setAllowedAreas(Qt::RightDockWidgetArea);
    
    QWidget *commandWidget = new QWidget(commandDock);
    QVBoxLayout *commandLayout = new QVBoxLayout(commandWidget);
    
    commandListWidget = new QListWidget(commandWidget);
    commandListWidget->addItem("CMD_1 | " + trans["example_command"] + " 1");
    commandListWidget->addItem("CMD_2 | " + trans["example_command"] + " 2");
    commandListWidget->addItem("CMD_3 | " + trans["example_command"] + " 3");
    
    addCmdBtn = new QPushButton(trans["add_command"], commandWidget);
    delCmdBtn = new QPushButton(trans["delete_command"], commandWidget);
    
    commandLayout->addWidget(commandListWidget);
    commandLayout->addWidget(addCmdBtn);
    commandLayout->addWidget(delCmdBtn);
    
    commandDock->setWidget(commandWidget);
    addDockWidget(Qt::RightDockWidgetArea, commandDock);
    
    // Set initial width for command dock (narrower)
    commandDock->setMaximumWidth(250);
    commandDock->setMinimumWidth(200);
    
    connect(commandListWidget, &QListWidget::itemDoubleClicked, 
            this, &MainWindow::on_commandList_itemDoubleClicked);
    connect(addCmdBtn, &QPushButton::clicked, this, &MainWindow::addCommand);
    connect(delCmdBtn, &QPushButton::clicked, this, &MainWindow::deleteCommand);
    
    // Add auto-send controls to send group
    QWidget *sendWidget = ui->groupBox_3;
    QVBoxLayout *sendLayout = qobject_cast<QVBoxLayout*>(sendWidget->layout());
    
    if (sendLayout) {
        QHBoxLayout *autoSendLayout = new QHBoxLayout();
        
        QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
        
        autoSendCheckBox = new QCheckBox(trans["auto_send"], sendWidget);
        autoSendIntervalSpinBox = new QSpinBox(sendWidget);
        autoSendIntervalSpinBox->setRange(10, 10000);
        autoSendIntervalSpinBox->setValue(1000);
        autoSendIntervalSpinBox->setSuffix(" ms");
        
        addLineBreakCheckBox = new QCheckBox(trans["add_lf"], sendWidget);
        addCarriageReturnCheckBox = new QCheckBox(trans["add_cr"], sendWidget);
        
        autoSendLayout->addWidget(autoSendCheckBox);
        autoSendLayout->addWidget(autoSendIntervalSpinBox);
        autoSendLayout->addWidget(addLineBreakCheckBox);
        autoSendLayout->addWidget(addCarriageReturnCheckBox);
        autoSendLayout->addStretch();
        
        sendLayout->insertLayout(sendLayout->count() - 1, autoSendLayout);
        
        connect(autoSendCheckBox, &QCheckBox::toggled, [this](bool checked) {
            if (checked) {
                autoSendTimer->start(autoSendIntervalSpinBox->value());
            } else {
                autoSendTimer->stop();
            }
        });
        
        connect(autoSendIntervalSpinBox, SIGNAL(valueChanged(int)), 
                this, SLOT(on_autoSendInterval_changed(int)));
    }
}

void MainWindow::on_autoSendTimer_timeout()
{
    if (serialPort->isOpen() && !ui->sendText->toPlainText().isEmpty()) {
        on_sendButton_clicked();
    }
}

void MainWindow::on_commandList_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;
    
    QString text = item->text();
    int separatorPos = text.indexOf('|');
    if (separatorPos > 0) {
        QString command = text.mid(separatorPos + 1).trimmed();
        ui->sendText->setPlainText(command);
    }
}

void MainWindow::addCommand()
{
    QString currentText = ui->sendText->toPlainText();
    if (currentText.isEmpty()) return;
    
    int cmdNum = commandListWidget->count() + 1;
    QString cmdName = QString("CMD_%1").arg(cmdNum);
    QString cmdItem = QString("%1 | %2").arg(cmdName).arg(currentText);
    
    commandListWidget->addItem(cmdItem);
}

void MainWindow::deleteCommand()
{
    QListWidgetItem *item = commandListWidget->currentItem();
    if (item) {
        delete item;
    }
}

void MainWindow::parseReceivedData(const QByteArray &data)
{
    // Try to parse data as numeric values for plotting
    QString dataStr = QString::fromUtf8(data).trimmed();
    
    // Check if data contains "plotter" keyword or numeric values
    if (dataStr.contains("plotter", Qt::CaseInsensitive) || 
        dataStr.contains(QRegularExpression("[0-9\\-\\.]+"))) {
        
        // Remove "plotter" keyword if present
        dataStr.remove("plotter", Qt::CaseInsensitive);
        
        // Extract numeric values
        QStringList parts = dataStr.split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
        
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        int channelIndex = 0;
        
        QString plotInfo;
        
        for (const QString &part : parts) {
            bool ok;
            double value = part.toDouble(&ok);
            
            if (ok && channelIndex < 6) {
                // Add to plot widget
                plotWidget->addDataPoint(channelIndex, value);
                
                // Store in data structure
                if (channelIndex < plotData.size()) {
                    DataPoint point;
                    point.timestamp = timestamp;
                    point.value = value;
                    
                    plotData[channelIndex].append(point);
                    
                    // Limit data points
                    if (plotData[channelIndex].size() > maxDataPoints) {
                        plotData[channelIndex].removeFirst();
                    }
                }
                
                plotInfo += QString("Ch%1: %2  ").arg(channelIndex + 1).arg(value, 0, 'f', 4);
                channelIndex++;
            }
        }
        
        if (!plotInfo.isEmpty()) {
            updatePlotDisplay();
        }
    }
}

void MainWindow::updatePlotDisplay()
{
    QString plotText;
    
    for (int i = 0; i < plotData.size(); ++i) {
        if (plotData[i].isEmpty()) continue;
        
        plotText += QString("Graph %1: ").arg(i + 1);
        
        // Show statistics
        double sum = 0;
        double minVal = plotData[i].first().value;
        double maxVal = plotData[i].first().value;
        
        for (const DataPoint &point : plotData[i]) {
            sum += point.value;
            if (point.value < minVal) minVal = point.value;
            if (point.value > maxVal) maxVal = point.value;
        }
        
        double avg = sum / plotData[i].size();
        double lastVal = plotData[i].last().value;
        
        plotText += QString("Last=%1  Avg=%2  Min=%3  Max=%4  Points=%5\n")
                    .arg(lastVal, 0, 'f', 4)
                    .arg(avg, 0, 'f', 4)
                    .arg(minVal, 0, 'f', 4)
                    .arg(maxVal, 0, 'f', 4)
                    .arg(plotData[i].size());
    }
    
    plotterTextEdit->setPlainText(plotText);
}

void MainWindow::on_autoSendInterval_changed(int value)
{
    if (autoSendCheckBox && autoSendCheckBox->isChecked()) {
        autoSendTimer->setInterval(value);
    }
}

void MainWindow::loadStyleSheet()
{
    QFile styleFile(":/styles/styles.qss");
    
    // Try to load from resource first
    if (!styleFile.open(QFile::ReadOnly)) {
        // Try to load from file system
        styleFile.setFileName("styles.qss");
        if (!styleFile.open(QFile::ReadOnly)) {
            // Use embedded style if file not found
            QString style = 
                "QMainWindow { background-color: #f5f5f5; }"
                "QMenuBar { background-color: #2c3e50; color: white; padding: 4px; }"
                "QMenuBar::item { padding: 6px 12px; }"
                "QMenuBar::item:selected { background-color: #34495e; }"
                "QStatusBar { background-color: #34495e; color: white; }"
                "QStatusBar QLabel { color: white; padding: 2px 8px; }"
                "QGroupBox { border: 2px solid #3498db; border-radius: 6px; margin-top: 12px; "
                "           padding-top: 10px; font-weight: bold; background-color: white; }"
                "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
                "                   padding: 2px 10px; background-color: #3498db; color: white; "
                "                   border-radius: 4px; left: 10px; }"
                "QPushButton { background-color: #3498db; color: white; border: none; "
                "             border-radius: 4px; padding: 6px 16px; font-weight: bold; min-width: 80px; }"
                "QPushButton:hover { background-color: #2980b9; }"
                "QPushButton:pressed { background-color: #21618c; }"
                "QPushButton#openButton { background-color: #27ae60; min-width: 100px; }"
                "QPushButton#openButton:hover { background-color: #229954; }"
                "QPushButton#sendButton { background-color: #e74c3c; min-width: 100px; }"
                "QPushButton#sendButton:hover { background-color: #c0392b; }"
                "QPushButton#refreshButton { background-color: #f39c12; }"
                "QPushButton#refreshButton:hover { background-color: #d68910; }"
                "QComboBox { border: 2px solid #bdc3c7; border-radius: 4px; padding: 4px 8px; "
                "           background-color: white; min-width: 100px; }"
                "QComboBox:hover { border-color: #3498db; }"
                "QTextEdit { border: 2px solid #bdc3c7; border-radius: 4px; background-color: white; "
                "           padding: 4px; font-family: 'Consolas', 'Courier New', monospace; }"
                "QTextEdit:focus { border-color: #3498db; }"
                "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid #bdc3c7; "
                "                       border-radius: 3px; background-color: white; }"
                "QCheckBox::indicator:checked { background-color: #3498db; border-color: #3498db; }"
                "QTabBar::tab { background-color: #ecf0f1; color: #2c3e50; border: 2px solid #bdc3c7; "
                "              padding: 8px 20px; font-weight: bold; border-top-left-radius: 4px; "
                "              border-top-right-radius: 4px; }"
                "QTabBar::tab:selected { background-color: white; color: #3498db; border-color: #3498db; }"
                "QListWidget { border: 2px solid #bdc3c7; border-radius: 4px; background-color: white; }"
                "QListWidget::item:selected { background-color: #3498db; color: white; }"
                "QDockWidget::title { background-color: #34495e; color: white; padding: 6px; font-weight: bold; }";
            
            qApp->setStyleSheet(style);
            return;
        }
    }
    
    QString styleSheet = QString::fromUtf8(styleFile.readAll());
    qApp->setStyleSheet(styleSheet);
    styleFile.close();
}
