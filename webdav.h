#ifndef WEBDAV_H
#define WEBDAV_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include <QDialog>

class WebDAV : public QWidget
{
    Q_OBJECT
public:
    explicit WebDAV(QWidget *parent = nullptr);

public:
    void getRemoteDB();
    void storeRemoteDB();

private slots:
    void uploadFinished();
    void downloadFinished();
    void deleteFinished();
    void downloadReadyRead();
    void getRemoteDBReadyRead();
    void getRemoteDBFinished();
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);

signals:
    void syncFinished();
    void storeFinished();

private:
    void uploadDB();
    void downloadDB(QString dbName);
    void writeCredentials(QString user, QString password);
    void readCredentials();
    void writeDB(QString dbName);
    void readDB();
    void setupUrl(QString file = "");
    void deletePreviousRemoteDB(QString remoteDB);
    QUrl m_Url;
    QNetworkAccessManager m_qnam;
    QNetworkReply *m_Reply;
    QFile *m_File;
    QString m_User;
    QString m_Password;
    QString m_LocalDB;
    QString m_RemoteDB;
};

#endif // WEBDAV_H
