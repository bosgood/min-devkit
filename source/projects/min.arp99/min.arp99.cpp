/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;


class arp99 : public object<arp99> {
public:
    MIN_DESCRIPTION	{ "Arpeggiator deluxe" };
    MIN_TAGS	    	{ "time" };
    MIN_AUTHOR	  	{ "bosgood" };
    MIN_RELATED	    { "min.beat.random, link.beat, metro, tempo, drunk" };

    inlet<>  input			{ this, "(toggle) on/off" };
    inlet<>  clock_in		{ this, "(bang) clock signal input" };
    outlet<> bang_out		{ this, "(bang) triggers at according to specified pattern" };
    outlet<> pitch_out	{ this, "(float) the pitch value for the current bang" };
    outlet<> tempo_out  { this, "(float) the current tempo (BPM)"};

    // inlet<> clock       { this, "(bang) clock signal input" };
    // inlet<> pitch1_in   { this, "(float) note 1 pitch" };
    // inlet<> velocity1_in { this, "(float) note 1 velocity" };
    // inlet<> pitch2_in { this, "(float) note 2 pitch" };

    timer<> metro { this,
        MIN_FUNCTION {
            // double pitch = m_sequence[m_index];
            pitch_out.send(0);
            bang_out.send("bang");
            tempo_out.send(m_tempo);

            // Calculate millisecond delay interval from current beats per minute value
            double ms_delay = m_tempo / m_clock_div / 60 * 1000;
            cout << "arp tick (delay: " << ms_delay << "ms) " << endl;
            metro.delay(ms_delay);

            // m_index += 1;

            // if (m_index == m_sequence.size())
            //   m_index = 0;
            return {};
        }
    };

    attribute<bool> on {this, "on", false,
        description {"Turn on/off the internal timer."},
        setter { MIN_FUNCTION {
            if (args[0] == true)
                metro.delay(0.0);    // fire the first one straight-away
            else
                metro.stop();
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

    message<> toggle { this, "int", "Turn on/off the internal timer.",
        MIN_FUNCTION {
            on = args[0];
            cout << "Toggled arp: " << on << endl;
            return {};
        }
    };

    message<> clock { this, "bang", "Receive an incoming clock signal.",
        MIN_FUNCTION {
          cout << "Clock tick received" << endl;
          // TODO [bosgood] Store datetimes of received signals
          return {};
        }
    };

private:
    // Current tempo as measured in beats per minute
    double m_tempo { 120.0 };
    int m_index	{ 0 };
    int m_clock_div { 16 };
    std::vector<int> m_clock_signals { 0, 0, 0 };
    atoms m_sequence	{ 250.0, 250.0, 250.0, 250.0, 500.0, 500.0, 500.0, 500.0 };
};

MIN_EXTERNAL(arp99);
