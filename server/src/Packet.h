#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace Network
{
static unsigned int packetId;

class Packet
{
  public:
    enum class Type : unsigned char
    {
        Error = 0,
        Status = 1,
        Asset = 2,
        Score = 3,
    };

    enum class Error : unsigned char
    {
        UnknownRequest = 0,
    };

    enum class Status : unsigned char
    {
        Empty = 0,
        Good = 1,
        PendingConnection = 2,
        ConnectionEstablished = 3,
    };

    enum class AssetCommand : unsigned char
    {
        Create = 0,
        Remove = 1,
        Update = 2,
    };

    struct GameAsset
    {
        int id;
        float x;
        float y;
        float rot;
        float sizeX;
        float sizeY;
        unsigned int tint;
        short aliasLength;
        std::string alias;

      public:
        size_t Fill(AssetCommand command, std::vector<unsigned char> &outData) const;
    };

    struct GameScore
    {
        int id;
        short score;

      public:
        size_t Fill(std::vector<unsigned char> &outData) const;
    };

    Packet(Type type, unsigned int size = 0);
    ~Packet();

    size_t GetSize() const;

    static Packet CreateErrorPacket(Error error);
    static Packet CreateStatusPacket(Status status);
    static Packet CreateAssetPacket(AssetCommand command, const std::vector<GameAsset> assets);
    static Packet CreateScorePacket(const std::vector<GameScore> scores);

    Type m_type;
    unsigned int m_size;
    std::vector<unsigned char> m_data;
};
} // namespace Network
