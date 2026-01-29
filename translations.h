#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <QMap>
#include <QString>
#include <QSettings>

class Translations
{
public:
    static QMap<QString, QString> getTranslations(const QString &language);
    static QStringList getAvailableLanguages();
    
private:
    static QMap<QString, QString> loadFromFile(const QString &filename);
};

#endif // TRANSLATIONS_H
