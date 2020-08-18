/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include <chrono>
#include <ctime>

#include "c74_min.h"

using namespace c74::min;


class arp99 : public object<arp99> {
public:
    MIN_DESCRIPTION	{ "Arpeggiator deluxe" };
    MIN_TAGS	    	{ "time" };
    MIN_AUTHOR	  	{ "bosgood" };
    MIN_RELATED	    { "min.beat.random, link.beat, metro, tempo, drunk" };

    inlet<>  tempo_in   { this, "(int) tempo input" };
    inlet<>  clock_in		{ this, "(float) clock signal input" };
    inlet<>  pitch_in   { this, "(list) pitch in" };
    inlet<>  cc_in      { this, "(list) cc in" };
    outlet<> bang_out		{ this, "(bang) triggers at according to specified pattern" };
    outlet<> pitch_out	{ this, "(float) the pitch value for the current bang" };
    outlet<> tempo_out  { this, "(float) the current tempo (BPM)"};

    // timer<> metro { this,
    //     MIN_FUNCTION {
    //         // double pitch = m_sequence[m_index];
    //         pitch_out.send(0);
    //         bang_out.send("bang");
    //         tempo_out.send(m_tempo);

    //         // Calculate millisecond delay interval from current beats per minute value
    //         double ms_delay = m_tempo / m_clock_div / 60 * 1000;
    //         cout << "arp tick (delay: " << ms_delay << "ms) " << endl;
    //         metro.delay(ms_delay);

    //         // m_index += 1;

    //         // if (m_index == m_sequence.size())
    //         //   m_index = 0;
    //         return {};
    //     }
    // };

    // attribute<bool> on {this, "on", false,
    //     description {"Turn on/off the internal timer."},
    //     setter { MIN_FUNCTION {
    //         if (args[0] == true)
    //             metro.delay(0.0);    // fire the first one straight-away
    //         else
    //             metro.stop();
    //         return args;
    //     }}
    // };

    attribute<int> clock_div {this, "clock_div", 16,
        description {"Clock division"},
        setter { MIN_FUNCTION {
            m_clock_div = args[0];
            cout << "Set clock div to " << m_clock_div << endl;
            return args;
        }}
    };

    // message<> toggle { this, "int", "Turn on/off the internal timer.",
    //     MIN_FUNCTION {
    //         on = args[0];
    //         cout << "Toggled arp: " << on << endl;
    //         return {};
    //     }
    // };

    // message<> clock { this, "bang", "Receive an incoming clock signal.",
    //     MIN_FUNCTION {
    //       auto now = std::chrono::system_clock::now();
    //       auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    //       auto epoch = now_ms.time_since_epoch();
    //       m_clock_signals[m_tick_index] = epoch;

    //       auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    //       cout << "Clock tick received at " << value.count() << endl;
    //       m_tick_index = (m_tick_index + 1) % 3;

    //       // Use the last few clock signals received to calculate
    //       // the current tempo

    //       // TODO [bosgood] Implement tempo calc

    //       return {};
    //     }
    // };

    message<> clock { this, "int", "Receive an incoming clock signal.",
        MIN_FUNCTION {
          cout << "Clock signal received" << endl;
          return {};
        }
    };

    message<> tempo { this, "float", "Tempo input",
      MIN_FUNCTION {
        if (inlet == 0) {
          m_tempo = args[0];
          cout << "Received tempo: " << m_tempo << endl;
        }
        return {};
      }
    };

    message<> list { this, "list", "Pitch input",
      MIN_FUNCTION {
        if (inlet == 2) {
          // Args are pairs of (Pitch, Velocity)
          for (auto i = 0; i < args.size(); i += 2) {
            auto pitch = args[i];
            auto velocity = (int)args[i+1];
            if (velocity > 0) {
              cout << "NOTEON " << pitch << endl;
              m_notes[pitch] = 1;
            } else {
              cout << "NOTEOFF " << pitch << endl;
              m_notes[pitch] = 0;
            }
          }
        } else if (inlet == 3) {
          // Args are pairs of (CC, Value)
          for (auto i = 0; i < args.size(); i += 2) {
            auto cc = args[i];
            auto val = args[i+1];
            cout << "CC " << cc << " val " << val << endl;
          }
        }
        return {};
      }
    };

private:
    // Current tempo as measured in beats per minute
    double m_tempo { 120.0 };
    int m_index	{ 0 };
    int m_clock_div { 16 };
    int m_tick_index { 0 };
    std::vector<int> m_notes = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    std::vector<std::chrono::milliseconds> m_clock_signals {
      std::chrono::milliseconds { 0 },
      std::chrono::milliseconds { 0 },
      std::chrono::milliseconds { 0 },
    };
    atoms m_sequence	{ 250.0, 250.0, 250.0, 250.0, 500.0, 500.0, 500.0, 500.0 };
};

MIN_EXTERNAL(arp99);
