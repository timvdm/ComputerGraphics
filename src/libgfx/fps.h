#ifndef GFX_FPS_H
#define GFX_FPS_H

#include <chrono>

namespace GFX {

  class FramesPerSecond
  {
    public:
      FramesPerSecond() : m_millisecs(0.0), m_frames(0)
      {
      }

      void startRender()
      {
        m_start = std::chrono::high_resolution_clock::now();
      }

      void stopRender()
      {
        std::chrono::time_point<std::chrono::high_resolution_clock> stop = std::chrono::high_resolution_clock::now();
        ++m_frames;
        std::cout << "# millisecs: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_start).count() << std::endl;
        m_millisecs += std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_start).count();
      }

      double fps() const
      {
        return 1000.0 * m_frames / m_millisecs;
      }

    private:
      std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
      double m_millisecs;
      int m_frames;
  };

}

#endif
