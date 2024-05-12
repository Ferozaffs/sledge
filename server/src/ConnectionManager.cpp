#include "ConnectionManager.h"
#include "PlayerManager.h"

#pragma warning(push)
#pragma warning(disable: 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <string>
#include <functional>
#include <map>

using namespace Network;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection<websocketpp::config::asio> connection;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef server::message_ptr message_ptr;

std::unique_ptr<Impl> ConnectionManager::m_impl = nullptr;

class Network::Impl{
public:
	server endpoint;
	std::map<std::shared_ptr<connection>, std::shared_ptr<Gameplay::Player>> connections;
	
	Impl(Gameplay::PlayerManager* playerMgr)
		: playerManager(playerMgr)
	{
		thread = std::thread(&websocketThread, this);
	}

private:
	std::thread thread;
	Gameplay::PlayerManager* playerManager;

	static void websocketThread(Impl* impl)
	{
		auto& endpoint = impl->endpoint;

		endpoint.set_access_channels(websocketpp::log::alevel::all);
		endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);
		endpoint.init_asio();

		endpoint.set_message_handler(bind(&Impl::on_message, &endpoint, ::_1, ::_2));

		endpoint.listen(9002);
		endpoint.start_accept();

		endpoint.run();
	}

	static void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
		std::string message = msg->get_payload();
		if (message == "conreq") {
			s->send(hdl, "Pending connection", websocketpp::frame::opcode::text);

			ConnectionManager::m_impl->CompleteConnection(s->get_con_from_hdl(hdl));
		}
		else {
			s->send(hdl, "Unknown request", websocketpp::frame::opcode::text);
		}
	}
	
	void CompleteConnection(const std::shared_ptr<connection>& handle)
	{
		auto player = playerManager->CreatePlayer();
		connections.insert({ handle, player });
	
		endpoint.send(handle, "Connection established", websocketpp::frame::opcode::text);
	}

};


ConnectionManager::ConnectionManager(Gameplay::PlayerManager* playerManager)
{
	m_impl = std::make_unique<Impl>(playerManager);
}

ConnectionManager::~ConnectionManager()
{
}