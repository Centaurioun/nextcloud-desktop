#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include <QDateTime>
#include <QStringList>
#include <QQuickImageProvider>
#include <QHash>

#include "activitylistmodel.h"
#include "accountfwd.h"
#include "accountmanager.h"
#include "folderman.h"
#include "userstatusselectormodel.h"
#include "userstatusconnector.h"
#include <chrono>

namespace OCC {
class UnifiedSearchResultsListModel;

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString server READ server CONSTANT)
    Q_PROPERTY(QColor headerColor READ headerColor NOTIFY headerColorChanged)
    Q_PROPERTY(QColor headerTextColor READ headerTextColor NOTIFY headerTextColorChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY accentColorChanged)
    Q_PROPERTY(bool serverHasUserStatus READ serverHasUserStatus CONSTANT)
    Q_PROPERTY(QUrl statusIcon READ statusIcon NOTIFY statusChanged)
    Q_PROPERTY(QString statusEmoji READ statusEmoji NOTIFY statusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)
    Q_PROPERTY(bool desktopNotificationsAllowed READ isDesktopNotificationsAllowed NOTIFY desktopNotificationsAllowedChanged)
    Q_PROPERTY(bool hasLocalFolder READ hasLocalFolder NOTIFY hasLocalFolderChanged)
    Q_PROPERTY(bool serverHasTalk READ serverHasTalk NOTIFY serverHasTalkChanged)
    Q_PROPERTY(QString avatar READ avatarUrl NOTIFY avatarChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY accountStateChanged)
    Q_PROPERTY(UnifiedSearchResultsListModel* unifiedSearchResultsListModel READ getUnifiedSearchResultsListModel CONSTANT)

public:
    User(AccountStatePtr &account, const bool &isCurrent = false, QObject *parent = nullptr);

    [[nodiscard]] AccountPtr account() const;
    [[nodiscard]] AccountStatePtr accountState() const;

    [[nodiscard]] bool isConnected() const;
    [[nodiscard]] bool isCurrentUser() const;
    void setCurrentUser(const bool &isCurrent);
    [[nodiscard]] Folder *getFolder() const;
    ActivityListModel *getActivityModel();
    [[nodiscard]] UnifiedSearchResultsListModel *getUnifiedSearchResultsListModel() const;
    void openLocalFolder();
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString server(bool shortened = true) const;
    [[nodiscard]] bool hasLocalFolder() const;
    [[nodiscard]] bool serverHasTalk() const;
    [[nodiscard]] bool serverHasUserStatus() const;
    [[nodiscard]] AccountApp *talkApp() const;
    [[nodiscard]] bool hasActivities() const;
    [[nodiscard]] QColor accentColor() const;
    [[nodiscard]] QColor headerColor() const;
    [[nodiscard]] QColor headerTextColor() const;
    [[nodiscard]] AccountAppList appList() const;
    [[nodiscard]] QImage avatar() const;
    void login() const;
    void logout() const;
    void removeAccount() const;
    [[nodiscard]] QString avatarUrl() const;
    [[nodiscard]] bool isDesktopNotificationsAllowed() const;
    [[nodiscard]] UserStatus::OnlineStatus status() const;
    [[nodiscard]] QString statusMessage() const;
    [[nodiscard]] QUrl statusIcon() const;
    [[nodiscard]] QString statusEmoji() const;
    void processCompletedSyncItem(const Folder *folder, const SyncFileItemPtr &item);

signals:
    void nameChanged();
    void hasLocalFolderChanged();
    void serverHasTalkChanged();
    void avatarChanged();
    void accountStateChanged();
    void statusChanged();
    void desktopNotificationsAllowedChanged();
    void headerColorChanged();
    void headerTextColorChanged();
    void accentColorChanged();
    void sendReplyMessage(const int activityIndex, const QString &conversationToken, const QString &message, const QString &replyTo);

public slots:
    void slotItemCompleted(const QString &folder, const SyncFileItemPtr &item);
    void slotProgressInfo(const QString &folder, const ProgressInfo &progress);
    void slotAddError(const QString &folderAlias, const QString &message, ErrorCategory category);
    void slotAddErrorToGui(const QString &folderAlias, SyncFileItem::Status status, const QString &errorMessage, const QString &subject = {});
    void slotNotificationRequestFinished(int statusCode);
    void slotNotifyNetworkError(QNetworkReply *reply);
    void slotEndNotificationRequest(int replyCode);
    void slotNotifyServerFinished(const QString &reply, int replyCode);
    void slotSendNotificationRequest(const QString &accountName, const QString &link, const QByteArray &verb, int row);
    void slotBuildNotificationDisplay(const ActivityList &list);
    void slotBuildIncomingCallDialogs(const ActivityList &list);
    void slotRefreshNotifications();
    void slotRefreshActivitiesInitial();
    void slotRefreshActivities();
    void slotRefresh();
    void slotRefreshUserStatus();
    void slotRefreshImmediately();
    void setNotificationRefreshInterval(std::chrono::milliseconds interval);
    void slotRebuildNavigationAppList();
    void slotSendReplyMessage(const int activityIndex, const QString &conversationToken, const QString &message, const QString &replyTo);
    void forceSyncNow() const;

private:
    void slotPushNotificationsReady();
    void slotDisconnectPushNotifications();
    void slotReceivedPushNotification(Account *account);
    void slotReceivedPushActivity(Account *account);
    void slotCheckExpiredActivities();

    void connectPushNotifications() const;
    [[nodiscard]] bool checkPushNotificationsAreReady() const;

    bool isActivityOfCurrentAccount(const Folder *folder) const;
    [[nodiscard]] bool isUnsolvableConflict(const SyncFileItemPtr &item) const;

    void showDesktopNotification(const QString &title, const QString &message, const long notificationId);

private:
    AccountStatePtr _account;
    bool _isCurrentUser;
    ActivityListModel *_activityModel;
    UnifiedSearchResultsListModel *_unifiedSearchResultsModel;
    ActivityList _blacklistedNotifications;

    QTimer _expiredActivitiesCheckTimer;
    QTimer _notificationCheckTimer;
    QHash<AccountState *, QElapsedTimer> _timeSinceLastCheck;

    QElapsedTimer _guiLogTimer;
    QSet<long> _notifiedNotifications;
    QMimeDatabase _mimeDb;

    // number of currently running notification requests. If non zero,
    // no query for notifications is started.
    int _notificationRequestsRunning;
};

class UserModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(User* currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(int currentUserId READ currentUserId WRITE setCurrentUserId NOTIFY currentUserChanged)
public:
    static UserModel *instance();
    ~UserModel() override = default;

    void addUser(AccountStatePtr &user, const bool &isCurrent = false);
    int currentUserIndex();

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QImage avatarById(const int id);

    [[nodiscard]] User *currentUser() const;

    int findUserIdForAccount(AccountState *account) const;

    Q_INVOKABLE int numUsers();
    Q_INVOKABLE QString currentUserServer();
    [[nodiscard]] int currentUserId() const;

    Q_INVOKABLE bool isUserConnected(const int id);

    Q_INVOKABLE std::shared_ptr<OCC::UserStatusConnector> userStatusConnector(int id);

    ActivityListModel *currentActivityModel();

    enum UserRoles {
        NameRole = Qt::UserRole + 1,
        ServerRole,
        ServerHasUserStatusRole,
        StatusIconRole,
        StatusEmojiRole,
        StatusMessageRole,
        DesktopNotificationsAllowedRole,
        AvatarRole,
        IsCurrentUserRole,
        IsConnectedRole,
        IdRole
    };

    [[nodiscard]] AccountAppList appList() const;

signals:
    void addAccount();
    void currentUserChanged();

public slots:
    void fetchCurrentActivityModel();
    void openCurrentAccountLocalFolder();
    void openCurrentAccountTalk();
    void openCurrentAccountServer();
    void setCurrentUserId(const int id);
    void login(const int id);
    void logout(const int id);
    void removeAccount(const int id);

protected:
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    static UserModel *_instance;
    UserModel(QObject *parent = nullptr);
    QList<User*> _users;
    int _currentUserId = 0;
    bool _init = true;

    void buildUserList();
};

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

class UserAppsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    static UserAppsModel *instance();
    ~UserAppsModel() override = default;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    enum UserAppsRoles {
        NameRole = Qt::UserRole + 1,
        UrlRole,
        IconUrlRole
    };

    void buildAppList();

public slots:
    void openAppUrl(const QUrl &url);

protected:
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    static UserAppsModel *_instance;
    UserAppsModel(QObject *parent = nullptr);

    AccountAppList _apps;
};
}
#endif // USERMODEL_H
