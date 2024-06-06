#include "WebsocketHelper.h"
#include "ConnectionManager.h"
#include "Player.h"

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using namespace Network;

std::unique_ptr<WebSocketHelper> WebSocketHelper::m_instance = nullptr;
std::mutex WebSocketHelper::connectionMutex;

WebSocketHelper::WebSocketHelper()
{
    m_thread = std::thread(&websocketThread);
}

WebSocketHelper &WebSocketHelper::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = std::make_unique<WebSocketHelper>();
    }

    return *m_instance;
}

void WebSocketHelper::SendAll(std::string message)
{
    for (const auto &connection : m_connections)
    {
        Send(connection.first, message);
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

void WebSocketHelper::Send(const std::shared_ptr<ws_connection> &connection, std::string message)
{
    if (connection->get_state() == websocketpp::session::state::open)
    {
        m_endpoint.send(connection, message.c_str(), websocketpp::frame::opcode::text);
    }
}

void WebSocketHelper::websocketThread()
{
    auto &endpoint = GetInstance().m_endpoint;

    endpoint.clear_access_channels(websocketpp::log::alevel::all);
    endpoint.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
    endpoint.init_asio();

    endpoint.set_close_handler(bind(&on_close, &endpoint, ::_1));
    endpoint.set_message_handler(bind(&on_message, &endpoint, ::_1, ::_2));

    endpoint.listen(9002);
    endpoint.start_accept();

    endpoint.run();
}

void WebSocketHelper::on_close(ws_server *s, websocketpp::connection_hdl hdl)
{
    m_instance->CloseConnection(s->get_con_from_hdl(hdl));
}

void WebSocketHelper::on_message(ws_server *s, websocketpp::connection_hdl hdl, ws_server::message_ptr msg)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(msg->get_payload());

        if (j.contains("type"))
        {
            if (j["type"] == "conreq")
            {
                s->send(hdl, CreateStatusMessage("Pending connection"), websocketpp::frame::opcode::text);

                m_instance->CompleteConnection(s->get_con_from_hdl(hdl));
            }
            if (j["type"] == "statusreq")
            {
                int numConnection = 0;
                connectionMutex.lock();
                numConnection = GetInstance().m_connections.size();
                connectionMutex.unlock();

                s->send(hdl, CreateStatusMessage(numConnection > 0 ? "good" : "empty"),
                        websocketpp::frame::opcode::text);
            }
            else if (j["type"] == "input")
            {
                GetInstance().SetInput(s->get_con_from_hdl(hdl), j);
            }
        }
        else
        {
            s->send(hdl, CreateErrorMessage("Unknown request"), websocketpp::frame::opcode::text);
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

    m_endpoint.send(handle, CreateStatusMessage("Connection established"), websocketpp::frame::opcode::text);
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

std::string WebSocketHelper::CreateStatusMessage(std::string message)
{
    nlohmann::json j = {
        {"type", "status"},
        {"status", message},
    };

    return j.dump();
}

std::string WebSocketHelper::CreateErrorMessage(std::string message)
{
    nlohmann::json j = {
        {"type", "error"},
        {"error", message},
    };

    return j.dump();
}
