/* Autogenerated with Kurento Idl */

#include "WindowIn.hpp"
#include <JsonSerializer.hpp>

void
Serialize(kurento::WindowIn& event, JsonSerializer& s)
{
  s.Serialize("windowId", event.windowId);
  try {
    kurento::Media &parent = dynamic_cast<kurento::Media&> (event);

    Serialize(parent, s);
  } catch (std::bad_cast) {

  }
}