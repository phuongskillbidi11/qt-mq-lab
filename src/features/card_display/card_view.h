#pragma once

#include <QJsonObject>
#include <QString>

namespace CardView {

struct Presented {
    QString displayTitle;
    QString searchText;
    QString cardUid;
    QString gender;
    QString role;
    QString validUntil;
};

Presented present(const QString &type, const QJsonObject &payload);

}  // namespace CardView
