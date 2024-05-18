#pragma once
#include <guiddef.h>

class b2Body;

namespace Gameplay
{
class Asset
{
  public:
    Asset(b2Body *body);
    ~Asset();

    const GUID GetId() const;

    const float GetX() const;
    const float GetY() const;
    const float GetSizeX() const;
    const float GetSizeY() const;
    const float GetRot() const;

    b2Body *GetBody();
    void SetBody();

    void UpdateSize();

  private:
    GUID m_id;
    b2Body *m_body;
    float m_sizeX;
    float m_sizeY;
};
} // namespace Gameplay
