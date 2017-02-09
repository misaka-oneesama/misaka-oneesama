/*****************************************************************************
 * 御坂ーお姉さま
 * Copyright (C) 2017 マギルゥ一ベルベット
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BOTMANAGER_HPP
#define BOTMANAGER_HPP

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QDBusVariant>

#include <QDiscord>
#include <Bot/DiscordEventHandler.hpp>
#include <Core/PluginInterface.hpp>

#include <memory>

class BotManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString token MEMBER m_token WRITE setOAuthToken)

public:
    BotManager(QObject *parent = nullptr);
    ~BotManager();

    void setOAuthToken(const QString &token);
    const QString &token() const;

    bool isConnected() const;

    // Returns a pointer to the local QDiscord object (NEVER nullptr)
    QDiscord *discord();

public slots:
    void login(); // always Type::Bot
    void login(const QString &token, const QDiscordToken::Type &type = QDiscordToken::Type::Bot);
    void login(const QDiscordToken &token);
    void logout();

    void stop();
    void reload();

    void sendMessage(const QString &content, const QDiscordID &channel, bool tts = false);
    void deleteMessage(const QDiscordMessage &message);

private slots:
    void internal_loginSuccess();
    void internal_loginFailed();
    void internal_loggedOut();
    void internal_disconnected();

signals:
    void loginSuccess();
    void loginFailed();
    void loggedOut();
    void disconnected();

    void stopped();
    void reloaded();

private:
    QMutex m_mutex;
    QList<PluginInterface*> m_plugins;

    std::unique_ptr<QDiscord> m_discord;
    std::unique_ptr<DiscordEventHandler> m_eventHandler;

    QString m_token;
    bool m_isStopping = false;
    bool m_isReloading = false;
};

class BotManagerDBusAdapter : public QObject
{
    Q_OBJECT

public:
    BotManagerDBusAdapter(BotManager *botManager, QObject *parent = nullptr);
    ~BotManagerDBusAdapter();

public slots:
    Q_NOREPLY void start();   // aka login
    Q_NOREPLY void login();   // alias to .start
    Q_NOREPLY void stop();    // aka stop process
    Q_NOREPLY void logout();  // just logout keeping the process alive
    Q_NOREPLY void reload();  // re-login

    Q_SCRIPTABLE bool isConnected();

    Q_NOREPLY void setOAuthToken(const QString &token);
    //Q_SCRIPTABLE QString OAuthToken(); <-- ATTENTION! NEVER IMPLEMENT THIS FOR SECURITY REASONS!!

    Q_NOREPLY void sendMessage(const quint64 &channel, const QString &message);
    Q_NOREPLY void editMessage(const quint64 &channel, const quint64 &messageId, const QString &content);
    Q_NOREPLY void deleteMessage(const quint64 &channel, const quint64 &messageId);
    //Q_NOREPLY void deleteMessages(const quint64 &channel, const QList<quint64> &messageIds);

private:
    QMutex m_mutex;
    BotManager *d = nullptr;
};

#endif // BOTMANAGER_HPP
