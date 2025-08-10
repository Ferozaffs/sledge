#pragma once
#include <nlohmann/json.hpp>

#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <mutex>

typedef websocketpp::server<websocketpp::config::asio> ws_server;
typedef websocketpp::connection<websocketpp::config::asio> ws_connection;

namespace Gameplay
{
class Player;
}

namespace Network
{
class Packet;

class WebSocketHelper
{
  public:
    WebSocketHelper();

    static WebSocketHelper &GetInstance();

    void SendAll(const Packet &packet);

    bool HasPendingConnections();
    void InsertPlayerHandle(std::weak_ptr<Gameplay::Player> player);

  private:
    static void WebsocketThread();

    static void OnClose(ws_server *s, websocketpp::connection_hdl hdl);

    static void OnMessage(ws_server *s, websocketpp::connection_hdl hdl, ws_server::message_ptr msg);

    static std::unique_ptr<WebSocketHelper> m_instance;

    friend class ConnectionManager;
    static std::mutex connectionMutex;

  private:
    void CloseConnection(const std::shared_ptr<ws_connection> &handle);

    void CompleteConnection(const std::shared_ptr<ws_connection> &handle);

    void Send(const std::shared_ptr<ws_connection> &connection, std::vector<unsigned char> buffer);

    void SetInput(const std::shared_ptr<ws_connection> &handle, const nlohmann::json &j);

    ws_server m_endpoint;
    std::map<std::shared_ptr<ws_connection>, std::weak_ptr<Gameplay::Player>> m_connections;
    std::thread m_thread;
};

} // namespace Network
