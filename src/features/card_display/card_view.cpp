#include "card_view.h"

namespace {

QString utf16(const char16_t *text) {
    return QString::fromUtf16(reinterpret_cast<const ushort *>(text));
}

QString displayGender(const QString &gender) {
    if (gender == QStringLiteral("male")) {
        return QStringLiteral("Nam");
    }
    if (gender == QStringLiteral("female")) {
        return utf16(u"N\u1EEF");
    }
    if (gender.isEmpty()) {
        return QStringLiteral("-");
    }
    return gender;
}

}  // namespace

CardView::Presented CardView::present(const QString &type, const QJsonObject &payload) {
    if (!type.startsWith(QStringLiteral("card."))) {
        return {};
    }

    Presented presented;
    presented.displayTitle = payload.value(QStringLiteral("username")).toString();
    presented.cardUid = payload.value(QStringLiteral("card_uid")).toString();
    presented.gender = displayGender(payload.value(QStringLiteral("gender")).toString());
    presented.role = payload.value(QStringLiteral("role")).toString();
    presented.validUntil = payload.value(QStringLiteral("expiry_date")).toString();
    presented.searchText =
        (presented.displayTitle + QStringLiteral(" ") + presented.cardUid).trimmed();
    return presented;
}
