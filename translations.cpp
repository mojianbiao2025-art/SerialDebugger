#include "translations.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>

QMap<QString, QString> Translations::loadFromFile(const QString &filename)
{
    QMap<QString, QString> trans;
    
    // Try to find the file in multiple locations
    QStringList searchPaths;
    searchPaths << filename  // Current directory
                << QCoreApplication::applicationDirPath() + "/" + filename  // Application directory
                << ":/lang/" + filename;  // Resource file
    
    QString filePath;
    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            filePath = path;
            break;
        }
    }
    
    if (filePath.isEmpty()) {
        return trans;  // Return empty map if file not found
    }
    
    QSettings settings(filePath, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif
    
    // Load all groups and keys
    QStringList groups = settings.childGroups();
    for (const QString &group : groups) {
        settings.beginGroup(group);
        QStringList keys = settings.childKeys();
        for (const QString &key : keys) {
            trans[key] = settings.value(key).toString();
        }
        settings.endGroup();
    }
    
    return trans;
}

QMap<QString, QString> Translations::getTranslations(const QString &language)
{
    QString filename = QString("lang_%1.ini").arg(language);
    QMap<QString, QString> trans = loadFromFile(filename);
    
    // If translation file not found, return English as fallback
    if (trans.isEmpty() && language != "en") {
        trans = loadFromFile("lang_en.ini");
    }
    
    // If still empty, provide minimal hardcoded fallback
    if (trans.isEmpty()) {
        trans["window_title"] = "Serial Port Debugger";
        trans["port_settings"] = "Port Settings";
        trans["port"] = "Port:";
        trans["refresh"] = "Refresh";
        trans["baud_rate"] = "Baud Rate:";
        trans["data_bits"] = "Data Bits:";
        trans["stop_bits"] = "Stop Bits:";
        trans["parity"] = "Parity:";
        trans["open_port"] = "Open Port";
        trans["close_port"] = "Close Port";
        trans["receive"] = "Receive";
        trans["hex_display"] = "HEX Display";
        trans["show_timestamp"] = "Show Timestamp";
        trans["clear"] = "Clear";
        trans["send"] = "Send";
        trans["hex_send"] = "HEX Send";
        trans["add_newline"] = "Add New Line";
        trans["send_btn"] = "Send";
        trans["status_disconnected"] = "Status: Disconnected";
        trans["status_connected"] = "Status: Connected - ";
        trans["parity_none"] = "None";
        trans["parity_odd"] = "Odd";
        trans["parity_even"] = "Even";
        trans["warning"] = "Warning";
        trans["error"] = "Error";
        trans["open_port_first"] = "Please open the port first";
        trans["failed_to_open"] = "Failed to open port: ";
        trans["menu_file"] = "File";
        trans["menu_view"] = "View";
        trans["menu_language"] = "Language";
        trans["menu_help"] = "Help";
        trans["save_receive"] = "Save Received Data...";
        trans["save_send"] = "Save Send Data...";
        trans["exit"] = "Exit";
        trans["clear_all"] = "Clear All";
        trans["about"] = "About";
        trans["about_text"] = "Serial Port Debugger v1.0\n\nA simple and easy-to-use serial communication tool\n\nSupports multilingual interface\n\nAuthor: Mo Jianbiao\nCompany: Shanghai Han's CNC Technology Co., Ltd.";
        trans["save_file"] = "Save File";
        trans["text_files"] = "Text Files (*.txt);;All Files (*.*)";
        trans["save_success"] = "File saved successfully";
        trans["save_failed"] = "Failed to save file";
        trans["tab_main"] = "Main";
        trans["tab_plotter"] = "Plotter";
        trans["plot_title"] = "Real-time Data Plot";
        trans["plot_value"] = "Value";
        trans["plot_points"] = "Number of points";
        trans["plot_waiting"] = "Waiting for data...\nSend numeric values to plot";
        trans["auto_send"] = "Auto Send";
        trans["add_lf"] = "Add LF";
        trans["add_cr"] = "Add CR";
    }
    
    return trans;
}

QStringList Translations::getAvailableLanguages()
{
    QStringList languages;
    
    // Check for language files in application directory
    QDir dir(QCoreApplication::applicationDirPath());
    QStringList filters;
    filters << "lang_*.ini";
    
    QStringList files = dir.entryList(filters, QDir::Files);
    for (const QString &file : files) {
        // Extract language code from filename (lang_XX.ini -> XX)
        QString langCode = file.mid(5, file.length() - 9);
        languages << langCode;
    }
    
    // If no files found, check current directory
    if (languages.isEmpty()) {
        QDir currentDir(".");
        files = currentDir.entryList(filters, QDir::Files);
        for (const QString &file : files) {
            QString langCode = file.mid(5, file.length() - 9);
            languages << langCode;
        }
    }
    
    // Fallback to default languages if no files found
    if (languages.isEmpty()) {
        languages << "en" << "zh" << "ja" << "de" << "fr";
    }
    
    return languages;
}
