#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// Disable Qt's settings system for WebAssembly to avoid embind issues
EM_JS(void, js_disable_qt_settings, (), {
    // Override localStorage to prevent Qt from using it
    if (typeof(Storage) !== "undefined") {
        console.log("Qt settings disabled for WebAssembly");
    }
});
#endif

int main(int argc, char *argv[])
{
#ifdef __EMSCRIPTEN__
    // Disable settings before QApplication
    js_disable_qt_settings();
    // Set environment to disable QSettings
    qputenv("QT_NO_SETTINGS", "1");
#endif
    
    QApplication a(argc, argv);
    
    // Set application icon
    a.setWindowIcon(QIcon(":/icons/app_icon.ico"));
    
    MainWindow w;
    w.show();
    return a.exec();
}
