#include "ConnectionManager.h"
#include "PlayerManager.h"
#include "Player.h"
#include "LevelLoader.h"
#include "LevelAsset.h"

#pragma warning(push)
#pragma warning(disable: 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <nlohmann/json.hpp>

#include <string>
#include <functional>
#include <map>
#include <Windows.h>

using namespace Network;
using namespace Gameplay;

using json = nlohmann::json;
using namespace nlohmann::literals;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection<websocketpp::config::asio> connection;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef server::message_ptr message_ptr;

std::unique_ptr<Impl> ConnectionManager::m_impl = nullptr;

std::string guidToString(const GUID& guid) {
	wchar_t guidWStr[39]; // GUIDs are 38 characters plus null terminator
	StringFromGUID2(guid, guidWStr, 39);
	std::wstring wideStr(guidWStr);
	return std::string(wideStr.begin(), wideStr.end());
}

class Network::Impl{
public:
	server endpoint;
	std::map<std::shared_ptr<connection>, std::shared_ptr<Gameplay::Player>> connections;
	
	Impl()
	{
		thread = std::thread(&websocketThread, this);
	}

	void Send(const std::shared_ptr<Gameplay::Player>& player, std::string message)
	{
		for (const auto& connection : connections)
		{
			if (player == connection.second)
			{
				endpoint.send(connection.first, message.c_str(), websocketpp::frame::opcode::text);
			}
		}
	}

private:
	std::thread thread;

	static void websocketThread(Impl* impl)
	{
		auto& endpoint = impl->endpoint;

		endpoint.set_access_channels(websocketpp::log::alevel::all);
		endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);
		endpoint.init_asio();

		endpoint.set_close_handler(bind(&Impl::on_close, &endpoint, ::_1));
		endpoint.set_message_handler(bind(&Impl::on_message, &endpoint, ::_1, ::_2));

		endpoint.listen(9002);
		endpoint.start_accept();

		endpoint.run();
	}

	static void on_close(server* s, websocketpp::connection_hdl hdl) {
		ConnectionManager::m_impl->CloseConnection(s->get_con_from_hdl(hdl));
	}

	static void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
		try {
			json j = json::parse(msg->get_payload());

			if (j.contains("type"))
			{
				if (j["type"] == "conreq") {
					s->send(hdl, ConnectionManager::CreateStatusMessage("Pending connection"), websocketpp::frame::opcode::text);

					ConnectionManager::m_impl->CompleteConnection(s->get_con_from_hdl(hdl));
				}
				else if (j["type"] == "input")
				{
					ConnectionManager::m_impl->SetInput(s->get_con_from_hdl(hdl), j);
				}
			}
			else {
				s->send(hdl, ConnectionManager::CreateErrorMessage("Unknown request"), websocketpp::frame::opcode::text);
			}
		}
		catch (json::parse_error& e) {
			std::cerr << "Parse error: " << e.what() << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}	
	}

	void CloseConnection(const std::shared_ptr<connection>& handle)
	{
		auto it = connections.find(endpoint.get_con_from_hdl(handle));
		if (it != connections.end())
		{
			it->second->m_pendingRemove = true;
			connections.erase(it);
		}
	}
	
	void CompleteConnection(const std::shared_ptr<connection>& handle)
	{
		connections.insert({ handle, nullptr });
	
		endpoint.send(handle, ConnectionManager::CreateStatusMessage("Connection established"), websocketpp::frame::opcode::text);
	}

	void SetInput(const std::shared_ptr<connection>& handle, const json& j)
	{
		auto it = connections.find(endpoint.get_con_from_hdl(handle));
		if (it != connections.end())
		{
			auto input = j["input"];
			it->second->SetInputs(input["sledge"], input["move"], input["jump"]);
		}
	}

};


ConnectionManager::ConnectionManager(PlayerManager* playerManager, Gameplay::LevelLoader* levelLoader)
	: m_playerManager(playerManager)
	, m_levelLoader(levelLoader)
{
	m_impl = std::make_unique<Impl>();
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::Update(float deltaTime)
{
	for (auto& connection : m_impl->connections)
	{
		if (connection.second == nullptr)
		{
			auto player = m_playerManager->CreatePlayer();
			connection.second = player;

			SendLevelAssets(player, m_levelLoader->GetLevelAssets());
		}
	}
}

void ConnectionManager::SendLevelAssets(const std::shared_ptr<Player>& player, std::vector<std::shared_ptr<LevelAsset>> assets)
{
	json j = {
		{"type", "addData"}
	};
	
	json blockArray = json::array();

	for (const auto& asset : assets)
	{
		json block = {
		   {"id", guidToString(asset->GetId())},
		   {"alias", "block_basic"},
		   {"x", asset->GetX() * 10.0f},
		   {"y", asset->GetY() * 10.0f}
		};

		blockArray.push_back(block);
	}


	j["blocks"] = blockArray;

	m_impl->Send(player, j.dump());
}


std::string ConnectionManager::CreateStatusMessage(std::string message)
{
	json j = {
		{"type", "status"},
		{"status", message},
	};

	return j.dump();
}

std::string ConnectionManager::CreateErrorMessage(std::string message)
{
	json j = {
		{"type", "error"},
		{"error", message},
	};

	return j.dump();
}
