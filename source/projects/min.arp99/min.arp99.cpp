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

    outlet<> bang_out		{ this, "(bang) triggers at according to specified pattern" };
    outlet<> note_out	{ this, "(list) the note and velocity value for the current bang" };

    timer<> metro { this,
        MIN_FUNCTION {
            // Calculate millisecond delay interval from current beats per minute value
            auto ms_delay = m_tempo / (double)m_clock_div / 60.0 * 1000.0;

            // Schedule the next metronome event after the time has elapsed.
            // 0 ms delay is probably never what we want, and indicative of a
            // bug or bad environment, and spikes CPU load, so let's not do that.
            if (ms_delay > 0) {
              metro.delay(ms_delay);
            }

            // Given the current pattern index, send the note which is on
            auto notes = notes_on();
            auto pattern_note_count = notes.size();

            // Send NOTE_OFF messages for any notes still on
            for (auto note_value : m_notes_on) {
              note_out.send({ note_value, 0 });
            }
            m_notes_on.clear();

            // No messages to send if no notes are on
            if (pattern_note_count == 0) {
              m_index = 0;
              return {};
            } else {
              // Rotate through the pattern
              m_index = (m_index + 1) % pattern_note_count;
            }

            auto note_value = notes[m_index].note;
            auto velocity_value = notes[m_index].velocity;
            note_out.send({ note_value, velocity_value });
            m_notes_on.push_back(note_value);
            bang_out.send("bang");

            cout <<
              "arp tick" << " (" <<
              "tempo=" << m_tempo << ", " <<
              "clock_div=" << m_clock_div << ", " <<
              "delay=" << ms_delay << " ms" << ", " <<
              "note=" << note_value << ", " <<
              "vel=" << velocity_value << ", " <<
              "seq=" << m_index << "/" << pattern_note_count <<
              ")" << endl
            ;

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

    attribute<int> gate_length {this, "gate_length", 16,
        description {"Clock division"},
        setter { MIN_FUNCTION {
            m_gate_length = args[0];
            cout << "Set clock div to " << m_gate_length << endl;
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
        // Args are pairs of (Note, Velocity)
        for (auto i = 0; i < args.size(); i += 2) {
          auto note = args[i];
          auto velocity = (int)args[i+1];
          if (velocity > 0) {
            m_notes[note] = velocity;
            auto notes = notes_on();
            cout << "NOTEON " << note << "(" << notes.size() << " total)"<< endl;
          } else {
            m_notes[note] = 0;
            auto notes = notes_on();
            cout << "NOTEOFF " << note << "(" << notes.size() << " total)"<< endl;
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
    // The number of ticks that each note will last for
    int m_gate_length { 1 };
    // Which note values are on or off, and their velocity.
    // The zero value is equivalent to NOTE_OFF
    std::vector<int> m_notes = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    // The notes which are still in a NOTE_ON state, MIDI-wise
    std::vector<int> m_notes_on = { };
};

MIN_EXTERNAL(arp99);
