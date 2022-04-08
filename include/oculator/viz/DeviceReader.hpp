#pragma once

#include <vlc/vlc.h>
#include <mutex>
#include <functional>

namespace oculator
{
  class DeviceReader
  {
  public:
    DeviceReader(std::string uri);
    ~DeviceReader();
    
    bool is_playing();
  private:
    libvlc_instance_t *m_vlc_instance;
    libvlc_media_player_t *m_vlc_player;
    bool m_is_initialized;
    std::mutex mutex;

    bool initializeVLC();
    std::function< void() > m_callback;
    void lock_output_(void *data, void **p_pixels);
    void update_(void *data, void *id);
    void unlock_output_(void *data, void *id, void *const *p_pixels);
  };
}