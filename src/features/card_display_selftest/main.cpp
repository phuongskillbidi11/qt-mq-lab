#include "card_view.h"

#include <QJsonObject>

#include <cstdio>

namespace {

QString utf16(const char16_t *text) {
    return QString::fromUtf16(reinterpret_cast<const ushort *>(text));
}

}  // namespace

int main() {
    bool allPassed = true;
    const auto check = [&allPassed](bool passed, const char *message) {
        std::printf("%s: %s\n", passed ? "PASS" : "FAIL", message);
        allPassed = allPassed && passed;
    };

    const QJsonObject payload{
        {"username", "Test User"},
        {"card_uid", "R2-THE-001"},
        {"gender", "female"},
        {"role", "employee"},
        {"expiry_date", "2027-08-15"},
    };
    const CardView::Presented card = CardView::present("card.issued", payload);
    check(card.displayTitle == "Test User"
              && card.searchText == "Test User R2-THE-001"
              && card.cardUid == "R2-THE-001"
              && card.gender == utf16(u"N\u1EEF")
              && card.role == "employee"
              && card.validUntil == "2027-08-15",
          "card.issued populates every presented field");

    const CardView::Presented unknown = CardView::present("lab.note", payload);
    check(unknown.displayTitle.isEmpty() && unknown.searchText.isEmpty()
              && unknown.cardUid.isEmpty() && unknown.gender.isEmpty()
              && unknown.role.isEmpty() && unknown.validUntil.isEmpty(),
          "unknown type returns an all-empty presentation");

    return allPassed ? 0 : 1;
}
