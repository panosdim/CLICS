#include <QtWidgets>
#include <QtNetwork>

#include "webdav.h"
#include "ui_authenticationdialog.h"

const QString webDavUrl = "http://mto.cc.nf/";
const QString localDBName = "/clics.db3";
const QString backupDBName = "/clics.db3.bak";

WebDAV::WebDAV(QWidget *parent) :
    QWidget(parent),
    m_Reply(nullptr),
    m_File(nullptr)
{
    connect(&m_qnam, &QNetworkAccessManager::authenticationRequired,
            this, &WebDAV::slotAuthenticationRequired);
}

void WebDAV::getRemoteDB(){
    // Restore saved username and password
    readCredentials();

    setupUrl();

    m_Reply = m_qnam.get(QNetworkRequest(m_Url));
    connect(m_Reply, &QNetworkReply::finished, this, &WebDAV::getRemoteDBFinished);
    connect(m_Reply, &QIODevice::readyRead, this, &WebDAV::getRemoteDBReadyRead);
}

void WebDAV::storeRemoteDB()
{
    uploadDB();
}

void WebDAV::writeCredentials(QString user, QString password)
{
    QSettings settings("Intracom Telecom", "CLICS");

    settings.beginGroup("WebDAV");
    settings.setValue("user", user);
    settings.setValue("password", password);
    settings.endGroup();
}

void WebDAV::readCredentials()
{
    QSettings settings("Intracom Telecom", "CLICS");

    settings.beginGroup("WebDAV");
    m_User = settings.value("user", "").toString();
    m_Password = settings.value("password", "").toString();
    settings.endGroup();
}

void WebDAV::writeDB(QString dbName)
{
    QSettings settings("Intracom Telecom", "CLICS");

    settings.beginGroup("WebDAV");
    settings.setValue("dbName", dbName);
    settings.endGroup();
}

void WebDAV::readDB()
{
    QSettings settings("Intracom Telecom", "CLICS");

    settings.beginGroup("WebDAV");
    m_LocalDB = settings.value("dbName", "").toString();
    settings.endGroup();
}

void WebDAV::setupUrl(QString file)
{
    m_Url.setUrl(webDavUrl + file);
    if (!m_User.isEmpty() && !m_Password.isEmpty()) {
        m_Url.setUserName(m_User);
        m_Url.setPassword(m_Password);
    }

    m_Url.setPort(2077);
    if (!m_Url.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(webDavUrl + file, m_Url.errorString()));
        return;
    }
}

void WebDAV::deletePreviousRemoteDB(QString remoteDB)
{
    // Restore saved username and password
    readCredentials();

    setupUrl(remoteDB);

    m_Reply = m_qnam.deleteResource(QNetworkRequest(m_Url));
    connect(m_Reply, &QNetworkReply::finished, this, &WebDAV::deleteFinished);
}

void WebDAV::getRemoteDBReadyRead() {
    QString result = m_Reply->readAll();
    QRegularExpression re("\\d{13}\\.db3");
    QRegularExpressionMatch match = re.match(result);
    if (match.hasMatch()) {
        m_RemoteDB = match.captured(0);
    }

}

void WebDAV::getRemoteDBFinished()
{
    // Read Local DB name
    readDB();

    // Check if Remote DB not exist
    if (m_RemoteDB.isEmpty()) {
        // Notify MainWindow that sync of DB is finished
        emit syncFinished();
        return;
    }

    if (m_LocalDB.isEmpty()) {
        // Download Remote DB
        downloadDB(m_RemoteDB);
        writeDB(m_RemoteDB);
    } else {
        // Compare timestamp to see if we need to download Remote DB
        QString remoteTS = m_RemoteDB.left(m_RemoteDB.lastIndexOf(".db3"));
        QString localTS = m_LocalDB.left(m_LocalDB.lastIndexOf(".db3"));
        if (localTS.toLongLong() < remoteTS.toLongLong()) {
            // Download Remote DB
            downloadDB(m_RemoteDB);
            writeDB(m_RemoteDB);
        } else {
            // Notify MainWindow that sync of DB is finished
            emit syncFinished();
        }
    }
}

void WebDAV::downloadDB(QString dbName)
{
    // Restore saved username and password
    readCredentials();

    setupUrl(dbName);

    QString fileName = QCoreApplication::applicationDirPath() + localDBName;
    QString backupFileName = QCoreApplication::applicationDirPath() + backupDBName;

    if (QFile::exists(fileName)) {
        if (QFile::exists(backupFileName))
            QFile::remove(backupFileName);
        QFile::rename(fileName, backupFileName);
    }

    m_File = new QFile(fileName);
    if (!m_File->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1")
                                 .arg(QDir::toNativeSeparators(m_File->fileName())));
        return;
    }

    m_Reply = m_qnam.get(QNetworkRequest(m_Url));

    connect(m_Reply, &QNetworkReply::finished, this, &WebDAV::downloadFinished);
    connect(m_Reply, &QIODevice::readyRead, this, &WebDAV::downloadReadyRead);
}

void WebDAV::uploadDB()
{
    // Restore saved username and password
    readCredentials();

    QString dbName =  QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + ".db3";
    m_LocalDB = dbName;
    setupUrl(dbName);

    QString fileName = QCoreApplication::applicationDirPath() + localDBName;
    m_File = new QFile(fileName);
    if (!m_File->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to read the file %1")
                                 .arg(QDir::toNativeSeparators(m_File->fileName())));
        return;
    }

    m_Reply = m_qnam.put(QNetworkRequest(m_Url), m_File);
    connect(m_Reply, &QNetworkReply::finished, this, &WebDAV::uploadFinished);
}

void WebDAV::uploadFinished()
{
    if (m_File) {
        m_File->close();
        delete m_File;
        m_File = nullptr;
    }

    if (m_Reply->error()) {
        qDebug() << m_Reply->errorString();
        m_Reply->deleteLater();
        m_Reply = nullptr;
        return;
    }

    writeDB(m_LocalDB);
    if (!m_RemoteDB.isEmpty())
        deletePreviousRemoteDB(m_RemoteDB);
    else
        emit storeFinished();
}

void WebDAV::downloadFinished()
{
    QFileInfo fi;
    if (m_File) {
        fi.setFile(m_File->fileName());
        m_File->close();
        delete m_File;
        m_File = nullptr;
    }

    if (m_Reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        qDebug() << m_Reply->errorString();
        m_Reply->deleteLater();
        m_Reply = nullptr;
        return;
    }

    // Notify MainWindow that sync of DB is finished
    emit syncFinished();
}

void WebDAV::deleteFinished()
{
    if (m_Reply->error()) {
        qDebug() << m_Reply->errorString();
        m_Reply->deleteLater();
        m_Reply = nullptr;
        return;
    }

    emit storeFinished();
}

void WebDAV::downloadReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (m_File)
        m_File->write(m_Reply->readAll());
}

void WebDAV::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    QDialog authenticationDialog;
    Ui_Dialog ui;
    ui.setupUi(&authenticationDialog);

    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), m_Url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(m_Url.userName());
    ui.passwordEdit->setText(m_Url.password());

    if (authenticationDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());

        // Save username and password for future use
        writeCredentials(authenticator->user(), authenticator->password());
    }
}
