#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QString>

namespace MqCodec {

struct Envelope {
    QString id;
    QDateTime timestamp;
    int version = 0;
    QString body;
};

struct DecodeResult {
    Envelope envelope;
    bool valid = false;
};

QByteArray encode(const QString &body);
DecodeResult decode(const QByteArray &data);

}  // namespace MqCodec
