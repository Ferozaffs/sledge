#include "WebsocketHelper.h"
#include "ConnectionManager.h"
#include "Packet.h"
#include "Player.h"

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using namespace Network;

std::unique_ptr<WebSocketHelper> WebSocketHelper::m_instance = nullptr;
std::mutex WebSocketHelper::connectionMutex;

WebSocketHelper::WebSocketHelper()
{
    m_thread = std::thread(&WebsocketThread);
}

WebSocketHelper &WebSocketHelper::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = std::make_unique<WebSocketHelper>();
    }

    return *m_instance;
}

void WebSocketHelper::SendAll(const Packet &packet)
{
    auto buffer = packet.Serialize();

    for (const auto &connection : m_connections)
    {
        Send(connection.first, buffer);
    }
}

bool WebSocketHelper::HasPendingConnections()
{
    for (auto &connection : m_connections)
    {
        if (connection.second.lock() == nullptr)
        {
            return true;
        }
    }

    return false;
}

void WebSocketHelper::InsertPlayerHandle(std::weak_ptr<Gameplay::Player> player)
{
    for (auto &connection : m_connections)
    {
        if (connection.second.lock() == nullptr)
        {
            connection.second = player;
            return;
        }
    }
}

void WebSocketHelper::Send(const std::shared_ptr<ws_connection> &connection, std::vector<unsigned char> buffer)
{
    if (connection->get_state() == websocketpp::session::state::open)
    {
        m_endpoint.send(connection, buffer.data(), buffer.size(), websocketpp::frame::opcode::binary);
    }
}

void WebSocketHelper::WebsocketThread()
{
    auto &endpoint = GetInstance().m_endpoint;

    endpoint.clear_access_channels(websocketpp::log::alevel::all);
    endpoint.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
    endpoint.init_asio();

    endpoint.set_close_handler(bind(&OnClose, &endpoint, ::_1));
    endpoint.set_message_handler(bind(&OnMessage, &endpoint, ::_1, ::_2));

    endpoint.listen(9002);
    endpoint.start_accept();

    endpoint.run();
}

void WebSocketHelper::OnClose(ws_server *s, websocketpp::connection_hdl hdl)
{
    m_instance->CloseConnection(s->get_con_from_hdl(hdl));
}

void WebSocketHelper::OnMessage(ws_server *s, websocketpp::connection_hdl handle, ws_server::message_ptr msg)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(msg->get_payload());

        if (j.contains("type"))
        {
            if (j["type"] == "conreq")
            {
                auto packet = Packet::CreateStatusPacket(Packet::Status::PendingConnection);
                auto buffer = packet.Serialize();
                s->send(handle, buffer.data(), buffer.size(), websocketpp::frame::opcode::binary);

                m_instance->CompleteConnection(s->get_con_from_hdl(handle));
            }
            if (j["type"] == "statusreq")
            {
                int numConnection = 0;
                connectionMutex.lock();
                numConnection = GetInstance().m_connections.size();
                connectionMutex.unlock();

                auto packet =
                    Packet::CreateStatusPacket(numConnection > 0 ? Packet::Status::Good : Packet::Status::Empty);

                auto buffer = packet.Serialize();
                s->send(handle, buffer.data(), buffer.size(), websocketpp::frame::opcode::binary);
            }
            else if (j["type"] == "input")
            {
                GetInstance().SetInput(s->get_con_from_hdl(handle), j);
            }
        }
        else
        {
            auto packet = Packet::CreateErrorPacket(Packet::Error::UnknownRequest);
            auto buffer = packet.Serialize();
            s->send(handle, buffer.data(), buffer.size(), websocketpp::frame::opcode::binary);
        }
    }
    catch (nlohmann::json::parse_error &e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void WebSocketHelper::CloseConnection(const std::shared_ptr<ws_connection> &handle)
{
    connectionMutex.lock();
    auto it = m_connections.find(m_endpoint.get_con_from_hdl(handle));
    if (it != m_connections.end())
    {
        if (auto p = it->second.lock())
        {
            p->m_pendingRemove = true;
            m_connections.erase(it);
        }
    }
    connectionMutex.unlock();
}

void WebSocketHelper::CompleteConnection(const std::shared_ptr<ws_connection> &handle)
{
    connectionMutex.lock();
    m_connections.insert({handle, std::weak_ptr<Gameplay::Player>()});

    auto packet = Packet::CreateStatusPacket(Packet::Status::ConnectionEstablished);
    m_endpoint.send(handle, static_cast<void *>(&packet), packet.GetSize(), websocketpp::frame::opcode::binary);
    connectionMutex.unlock();
}

void WebSocketHelper::SetInput(const std::shared_ptr<ws_connection> &handle, const nlohmann::json &j)
{
    connectionMutex.lock();
    auto it = m_connections.find(m_endpoint.get_con_from_hdl(handle));
    if (it != m_connections.end())
    {
        if (auto p = it->second.lock())
        {
            auto input = j["input"];
            p->SetInputs(input["sledge"], input["move"], input["jump"]);
        }
    }
    connectionMutex.unlock();
}
