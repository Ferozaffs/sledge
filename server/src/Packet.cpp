#include "Packet.h"

using namespace Network;

Packet::Packet(Type type, unsigned int size) : m_type(type), m_size(size)
{
}

Packet::~Packet()
{
}

size_t Packet::GameAsset::Fill(AssetCommand command, std::vector<unsigned char> &outData) const
{
    auto append = [&](const void *data, size_t size) -> size_t {
        const unsigned char *bytes = static_cast<const unsigned char *>(data);
        outData.insert(outData.end(), bytes, bytes + size);
        return size;
    };

    size_t size = 0;
    switch (command)
    {
    case AssetCommand::Create: {
        size += append(&id, sizeof(id));
        size += append(&x, sizeof(x));
        size += append(&y, sizeof(y));
        size += append(&rot, sizeof(rot));
        size += append(&sizeX, sizeof(sizeX));
        size += append(&sizeY, sizeof(sizeY));
        size += append(&tint, sizeof(tint));
        size += append(&aliasLength, sizeof(aliasLength));
        size += append(alias.data(), aliasLength);

        return size;
    }
    case AssetCommand::Remove: {
        size += append(&id, sizeof(id));

        return size;
    }
    case AssetCommand::Update: {
        size += append(&id, sizeof(id));
        size += append(&x, sizeof(x));
        size += append(&y, sizeof(y));
        size += append(&rot, sizeof(rot));

        return size;
    }
    }

    return 0;
}

size_t Packet::GameScore::Fill(std::vector<unsigned char> &outData) const
{
    auto append = [&](const void *data, size_t size) -> size_t {
        const unsigned char *bytes = static_cast<const unsigned char *>(data);
        outData.insert(outData.end(), bytes, bytes + size);
        return size;
    };

    size_t size = 0;

    size += append(&id, sizeof(id));
    size += append(&score, sizeof(score));

    return size;
}

size_t Packet::GetSize() const
{
    return (sizeof(unsigned char) + sizeof(unsigned int) + m_size);
}

Packet Packet::CreateErrorPacket(Error error)
{
    auto packet = Packet(Type::Error, sizeof(unsigned char));
    packet.m_data.emplace_back(static_cast<unsigned char>(error));
    return std::move(packet);
}

Packet Packet::CreateStatusPacket(Status status)
{
    auto packet = Packet(Type::Status, sizeof(unsigned char));
    packet.m_data.emplace_back(static_cast<unsigned char>(status));
    return std::move(packet);
}

Packet Packet::CreateAssetPacket(AssetCommand command, const std::vector<GameAsset> assets)
{
    auto packet = Packet(Type::Asset, sizeof(unsigned char));
    packet.m_data.emplace_back(static_cast<unsigned char>(command));
    packet.m_data.reserve(assets.size() * sizeof(GameAsset));
    for (const auto &asset : assets)
    {
        packet.m_size += asset.Fill(command, packet.m_data);
    }

    return std::move(packet);
}

Packet Packet::CreateScorePacket(const std::vector<GameScore> scores)
{
    auto packet = Packet(Type::Score);
    packet.m_data.reserve(scores.size() * sizeof(GameScore));
    for (const auto &score : scores)
    {
        packet.m_size += score.Fill(packet.m_data);
    }

    return std::move(packet);
}
