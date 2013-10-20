#ifndef CANVASBACKEND_H
#define CANVASBACKEND_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QJsonObject>
#include <QVariantList>
#include <QByteArray>
#include <QPoint>

class CanvasBackend : public QObject
{
    Q_OBJECT
public:
    // id, name and count, where count can be use to sort
    typedef std::tuple<QString, QString, quint64, QPoint, qint64> MemberSection;
    enum MemberSectionIndex {
        Id = 0,
        Name,
        Count,
        Footprint,
        LastActiveStamp
    };

    enum BlockLevel {
        NONE = 0,
        LOW = 5,
        MEDIUM = 10,
        HIGH = 20
    };

    CanvasBackend(QObject *parent = nullptr);
    ~CanvasBackend();
    void commit();
    BlockLevel blockLevel() const;
public slots:
    void onDataBlock(const QVariantMap d);
    void onIncomingData(const QJsonObject &d);
    void requestMembers(MemberSectionIndex index);
    void clearMembers();
    void setBlockLevel(const BlockLevel le);
    void pauseParse();
    void resumeParse();
signals:
    void newDataGroup(const QByteArray& d);
    void remoteDrawPoint(const QPoint &point,
                         const QVariantMap &brushInfo,
                         const QString &layer,
                         const QString clientid,
                         const qreal pressure=1.0);
    void remoteDrawLine(const QPoint &start,
                        const QPoint &end,
                        const QVariantMap &brushInfo,
                        const QString &layer,
                        const QString clientid,
                        const qreal pressure=1.0);
    void membersSorted(QList<MemberSection> list);
    void archiveParsed();
protected:
    void timerEvent(QTimerEvent * event);
private:
    BlockLevel blocklevel_;
    QVariantList tempStore;
    QList<QJsonObject> incoming_store_;
    // Warning, access memberHistory_ across thread
    // via member functions is not thread-safe
    QHash<QString, MemberSection> memberHistory_;
    QString cached_clientid_;
    int send_timer_id_;
    int parse_timer_id_;
    bool archive_loaded_;
    bool is_parsed_signal_sent;
    bool pause_;
    void upsertFootprint(const QString& id, const QString& name, const QPoint &point);
    void upsertFootprint(const QString& id, const QString& name);
    QByteArray toJson(const QVariant &m);
    QVariant fromJson(const QByteArray &d);
    void parseIncoming();
    void onArchiveLoaded();
};

typedef CanvasBackend::MemberSection MS;
typedef CanvasBackend::MemberSectionIndex MSI;

#endif // CANVASBACKEND_H
