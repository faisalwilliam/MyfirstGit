#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QByteArray>

inline QString encrypt(const QString &input) {
    const QString key = "MySuperSecretKey123";
    QByteArray inputBytes = input.toUtf8();
    QByteArray keyBytes = key.toUtf8();
    QByteArray output;
    for (int i = 0; i < inputBytes.size(); ++i) {
        output.append(inputBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }
    return QString(output.toBase64());
}

inline QString decrypt(const QString &input) {
    const QString key = "MySuperSecretKey123";
    QByteArray inputBytes = QByteArray::fromBase64(input.toUtf8());
    QByteArray keyBytes = key.toUtf8();
    QByteArray output;
    for (int i = 0; i < inputBytes.size(); ++i) {
        output.append(inputBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }
    return QString(output);
}

#endif // UTILS_H