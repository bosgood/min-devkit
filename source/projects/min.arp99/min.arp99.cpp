/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include <chrono>
#include <ctime>

#include "c74_min.h"

using namespace c74::min;

struct Note {
  int note;
  int velocity;
};

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
    outlet<> note_out	{ this, "(list) the note and velocity value for the current bang" };

    timer<> metro { this,
        MIN_FUNCTION {
            // Given the current pattern index, send the note which is on
            auto notes = notes_on();
            auto velocity_value = 0;
            if (notes.size() > 0) {
              velocity_value = notes[m_index].velocity;
            }
            atoms note_msg = { m_index, velocity_value };
            note_out.send(note_msg);
            bang_out.send("bang");

            // Calculate millisecond delay interval from current beats per minute value
            auto ms_delay = m_tempo / (double)m_clock_div / 60.0 * 1000.0;
            auto pattern_note_count = notes.size();
            cout <<
              "arp tick" << " (" <<
              "tempo=" << m_tempo << ", " <<
              "clock_div=" << m_clock_div << ", " <<
              "delay=" << ms_delay << " ms" << ", " <<
              "note=" << m_index << "/" << pattern_note_count <<
              ")" << endl
            ;

            // Schedule the next metronome event after the time has elapsed.
            // 0 ms delay is probably never what we want, and indicative of a
            // bug or bad environment, and spikes CPU load, so let's not do that.
            if (ms_delay > 0) {
              metro.delay(ms_delay);
            }

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

    message<> list { this, "list", "Note input",
      MIN_FUNCTION {
        if (inlet == 1) {
          // Args are pairs of (Note, Velocity)
          for (auto i = 0; i < args.size(); i += 2) {
            auto note = args[i];
            auto velocity = (int)args[i+1];
            if (velocity > 0) {
              cout << "NOTEON " << note << endl;
              m_notes[note] = velocity;
            } else {
              cout << "NOTEOFF " << note << endl;
              m_notes[note] = 0;
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

    // Return the MIDI note and velocity values of the notes which are enabled
    std::vector<Note> notes_on() {
      auto on = std::vector<Note>();
      for (int i = 0; i < m_notes.size(); i++) {
        if (m_notes[i] > 0) {
          on.push_back(Note{ i, m_notes[i] });
        }
      }
      return on;
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
    // Which note values are on or off, and their velocity.
    // The zero value is equivalent to NOTE_OFF
    std::vector<int> m_notes = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
};

MIN_EXTERNAL(arp99);
