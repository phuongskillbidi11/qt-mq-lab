#pragma once

#include "app_settings.h"

#include <QString>
#include <QtGlobal>

class MqSettings : public AppSettings {
public:
    QString host() const;
    quint16 port() const;
    QString vhost() const;
    QString user() const;
    QString password() const;

    bool isLocalBroker() const;
    QString connectionDescription() const;
};
