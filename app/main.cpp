#include "app_log.h"
#include "card_view.h"
#include "mq_connection.h"
#include "mq_service.h"
#include "mq_settings.h"
#include "mq_store.h"
#include "mq_tab.h"
#include "single_instance.h"
#include "thememanager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QObject>
#include <QSettings>

int main(int argc, char *argv[]) {
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("iSoft");
    QCoreApplication::setApplicationName("qt-mq-lab");

    AppLog::init();

    SingleInstance guard("qt-mq-lab-iSoft-singleinstance");
    if (!guard.isPrimary()) {
        const bool raised = guard.signalPrimaryToRaise();
        AppLog::info(raised
            ? "second instance refused; primary window raised"
            : "second instance refused; primary could not be reached");
        if (!raised) {
            QMessageBox::information(nullptr,
                                     "Already running",
                                     "Qt MQ Lab is already running, but its window could not be raised.");
        }
        return 0;
    }

    if (!ThemeManager::apply(ThemeManager::restore())) {
        AppLog::error("theme application failed during startup");
    }

    MqSettings settings;
    MqConnection connection;
    MqService service(&connection);
    MqStore store;
    const MessagePresenter presenter = [](const QString &type, const QJsonObject &payload) {
        const CardView::Presented presented = CardView::present(type, payload);
        QString detail;
        if (!presented.cardUid.isEmpty() || !presented.gender.isEmpty()) {
            detail = QString("%1  \xC2\xB7  %2").arg(presented.cardUid, presented.gender);
        }
        return PresentedMessage{presented.displayTitle, presented.searchText, detail};
    };
    MqTab window(&connection,
                 &service,
                 &settings,
                 &store,
                 presenter,
                 settings.host(),
                 settings.port(),
                 settings.vhost());

    QObject::connect(&guard, &SingleInstance::raiseRequested,
                     &window, &MqTab::raiseToFront);
    QObject::connect(&service, &MqService::errorOccurred,
                     [](const QString &message) { AppLog::error("AMQP: " + message); });

    window.show();
    connection.connectToHost(settings.host(),
                             settings.port(),
                             settings.vhost(),
                             settings.user(),
                             settings.password());

    const int exitCode = app.exec();
    AppLog::info(QString("application exited normally; exit code %1").arg(exitCode));
    return exitCode;
}
