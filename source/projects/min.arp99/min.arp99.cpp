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

    inlet<>  attr_in    { this, "(attr) attributes in" };
    inlet<>  pitch_in   { this, "(list) pitch in" };
    inlet<>  cc_in      { this, "(list) cc in" };

    outlet<> bang_out		{ this, "(bang) triggers at according to specified pattern" };
    outlet<> note_out	{ this, "(float) the note value for the current bang" };

    timer<> metro { this,
        MIN_FUNCTION {
            // TODO [bosgood] Figure out which note value to output here
            // int note_value = note_on_value();
            // note_out.send(note_value);
            bang_out.send("bang");

            // Calculate millisecond delay interval from current beats per minute value
            double ms_delay = m_tempo / (double)m_clock_div / 60.0 * 1000.0;
            int pattern_note_count = note_on_count();
            cout <<
              "arp tick" << " (" <<
              "tempo=" << m_tempo << ", " <<
              "clock_div=" << m_clock_div << ", " <<
              "delay=" << ms_delay << " ms" << ", " <<
              "note=" << m_index << "/" << pattern_note_count <<
              ")" << endl
            ;

            // Schedule the next metronome event after the time has elapsed
            metro.delay(ms_delay);

            // Rotate through the pattern
            if (pattern_note_count == 0) {
              m_index = 0;
            } else {
              m_index = (m_index + 1) % pattern_note_count;
            }

            return {};
        }
    };

    attribute<bool> on {this, "on", false,
        description {"Turn on/off the internal timer."},
        setter { MIN_FUNCTION {
            if (args[0] == true && !m_on) {
                cout << "Enabling internal timer" << endl;
                metro.delay(0.0);    // fire the first one straight-away
                m_on = true;
            } else if (args[0] == false && m_on) {
                cout << "Disabling internal timer" << endl;
                metro.stop();
                m_on = false;
            }
            return args;
        }}
    };

    attribute<int> clock_div {this, "clock_div", 16,
        description {"Clock division"},
        setter { MIN_FUNCTION {
            m_clock_div = args[0];
            cout << "Set clock div to " << m_clock_div << endl;
            return args;
        }}
    };

    attribute<number> tempo {this, "tempo", 120.0,
        description {"Tempo"},
        setter { MIN_FUNCTION {
            m_tempo = args[0];
            cout << "Set tempo to " << m_tempo << endl;
            return args;
        }}
    };

    message<> list { this, "list", "Pitch input",
      MIN_FUNCTION {
        if (inlet == 1) {
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
        } else if (inlet == 2) {
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

    int note_on_count() {
      int count = 0;
      for (int i = 0; i < m_notes.size(); i++) {
        if (m_notes[i] > 0) {
          count++;
        }
      }
      return count;
    }

private:
    // Whether the internal clock is enabled
    bool m_on { false };
    // Current tempo as measured in beats per minute
    double m_tempo { 0.0 };
    // Where in the current pattern we are
    int m_index	{ 0 };
    // The clock division
    int m_clock_div { 16 };
    // Which note values are on or off
    std::vector<int> m_notes = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
};

MIN_EXTERNAL(arp99);
