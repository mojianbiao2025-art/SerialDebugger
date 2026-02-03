#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// Prevent Qt from accessing localStorage
EM_JS(void, js_block_localstorage, (), {
    // Override localStorage to prevent Qt from using it
    var originalLocalStorage = window.localStorage;
    Object.defineProperty(window, 'localStorage', {
        get: function() {
            console.warn('localStorage access blocked for Qt compatibility');
            return {
                getItem: function() { return null; },
                setItem: function() {},
                removeItem: function() {},
                clear: function() {},
                key: function() { return null; },
                length: 0
            };
        },
        configurable: false
    });
    console.log('Qt localStorage blocking enabled');
});
#endif

int main(int argc, char *argv[])
{
#ifdef __EMSCRIPTEN__
    // Block localStorage before Qt initialization
    js_block_localstorage();
    
    // Disable Qt features that might use embind
    qputenv("QT_NO_SETTINGS", "1");
    qputenv("QT_NO_SESSIONMANAGER", "1");
    qputenv("QT_LOGGING_RULES", "*.debug=false");
#endif
    
    QApplication a(argc, argv);
    
#ifndef __EMSCRIPTEN__
    // Set application icon (only for native platforms)
    a.setWindowIcon(QIcon(":/icons/app_icon.ico"));
#endif
    
    MainWindow w;
    w.show();
    return a.exec();
}
