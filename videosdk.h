#ifndef VIDEOSDK_H
#define VIDEOSDK_H

#include <QObject>
#include <QWebSocket>
#include <QDebug>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringLiteral>
#include <QMutex>
#include <QList>
#include <QTimer>

#define QUEUE_INTERVAL 50

#define OBJ_METHOD "method"
#define OBJ_EVENT "event"
#define OBJ_TYPE  "type"
#define OBJ_REQUEST_ID "requestId"
#define OBJ_ERROR "error"
#define OBJ_RESULT "result"
#define OBJ_CREDENTIALS "credentials"
#define OBJ_tokenForHttpServer "tokenForHttpServer"
#define OBJ_APP_STATE "appState"

#define V_AUTH "auth"
#define V_SECURED "secured"
#define V_GET_APP_STATE "getAppState"
#define V_APP_STATE_CHANGED "appStateChanged"

enum State
{
    none = 0,       // No connection to the server and TrueConf Room does nothing
    connect = 1,    // Tries to connect to the server
    login = 2,      // you need to login
    normal = 3,     // Connected to the server and logged in
    wait = 4,       // Pending: either it calls somebody or somebody calls it
    conference = 5, // In the conference
    close = 6       // Finishing the conference
};

typedef struct SocketData {
    QString tokenForHttpServer;
} SocketData;

class VideoSDK : public QObject
{
    Q_OBJECT

public:
    explicit VideoSDK(QObject *parent);
    ~VideoSDK();

public:
    bool started() const;
    static QString stateToText(const State state);

public:
    void open_session(const QString &host, const QString &pin);
    void close_session();
    void connectToServer(const QString& server, const int port = 4307);
    void connectToService();
    void login(const QString& callId, const QString& password);
    void logout();
    void call(const QString &peerId);
    void reject();
    void accept();
    void hangUp(const bool forAll = false);
    void moveVideoSlotToMonitor(const QString& callId, int monitorIndex);
    void sendCommand(QString& peerId, QString& cmd);
    void setPanPos(int pos);
    void setTiltPos(int pos);
    void setZoomPos(int pos);    
    State state() const;

protected:
    void send_command(const QString &data);
    void clear_queue();
    void API_send(const QString &data);
    void auth();
    bool processIncoming(const QString& data);
    bool processIncomingEvent(const QString &event, const QJsonObject &json_obj);
    /* Requests */
    void requestAppState();
    void requestSettings();
    void requestSystemInfo();
    void requestMonitorsInfo();
    void requestConferenceParticipants();

private:
    int API_send_direct(const QString &data);
    void now_ready();
    void now_error(QString &error);

signals:
    void opened();
    void closed();
    void error(QString text);
    void change_state(State state);
    void socketReceived(QString data);

private:
    QMutex m_mutex;
    bool m_started = false;
    QString m_host;
    QString m_pin;
    QWebSocket* m_socket = nullptr;
    SocketData m_SocketData;
    QList<QString*> m_queue;
    QTimer m_timer;
    State m_state = State(0);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    void onSocketReceived(const QString data);
    void onSocketDestroyed(QObject *obj = nullptr);
    /* For QTimer::quit signal */
    void queue_processing();

};

#endif // VIDEOSDK_H
