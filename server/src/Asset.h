#pragma once
#include <guiddef.h>
#include <string>

class b2Body;

namespace Gameplay
{
class Asset
{
  public:
    Asset(b2Body *body, const std::string &alias);
    ~Asset();

    const GUID GetId() const;
    const std::string &GetAlias() const;
    const float GetX() const;
    const float GetY() const;
    const float GetSizeX() const;
    const float GetSizeY() const;
    const float GetRot() const;
    const unsigned int GetTint() const;

    b2Body *GetBody();

    void SetTint(unsigned int tint);

    void UpdateSize();

  private:
    GUID m_id;
    b2Body *m_body;
    std::string m_alias;
    unsigned int m_tint;
    float m_sizeX;
    float m_sizeY;
};
} // namespace Gameplay
