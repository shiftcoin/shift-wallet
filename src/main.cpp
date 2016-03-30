/*
    This file is part of SHIFT Wallet based on etherwall.
    SHIFT Wallet based on etherwall is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    SHIFT Wallet based on etherwall is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with SHIFT Wallet based on etherwall. If not, see <http://www.gnu.org/licenses/>.
*/
/** @file main.cpp
 * @author Ales Katona <almindor@gmail.com>
 * @date 2015
 *
 * Main entry point
 */

#include <QApplication>
#include <QTranslator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include "shiftlog.h"
#include "settings.h"
#include "clipboard.h"
#include "accountmodel.h"
#include "transactionmodel.h"
#include "currencymodel.h"

using namespace Etherwall;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Etherdiene");
    QCoreApplication::setOrganizationDomain("SHIFT Wallet based on etherwall.com");
    QCoreApplication::setApplicationName("Etherwall");
    QCoreApplication::setApplicationVersion("0.9.3");
    app.setWindowIcon(QIcon(QPixmap(":/images/icon")));

    QTranslator translator;
    translator.load("i18n/SHIFT Wallet based on etherwall_" + QLocale::system().name());
    app.installTranslator(&translator);

    Settings settings;
    ClipboardAdapter clipboard;
    ShiftLog log;
    ShiftIPC ipc;

    const QString ipcPath = settings.value("ipc/path", DefaultIPCPath).toString();

    if ( !settings.contains("ipc/path") ) {
        settings.setValue("ipc/path", ipcPath);
    }

    CurrencyModel currencyModel;
    AccountModel accountModel(ipc, currencyModel);
    TransactionModel transactionModel(ipc, accountModel);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("settings", &settings);
    engine.rootContext()->setContextProperty("ipc", &ipc);
    engine.rootContext()->setContextProperty("accountModel", &accountModel);
    engine.rootContext()->setContextProperty("transactionModel", &transactionModel);
    engine.rootContext()->setContextProperty("currencyModel", &currencyModel);
    engine.rootContext()->setContextProperty("clipboard", &clipboard);
    engine.rootContext()->setContextProperty("log", &log);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    log.log("Etherwall started");

    ipc.connectToServer(ipcPath);

    return app.exec();
}
