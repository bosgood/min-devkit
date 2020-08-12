/// @file
/// @ingroup  minexamples
/// @copyright  Copyright 2020 Brad Osgood. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class arp99 : public object<arp99> {
public:
  MIN_DESCRIPTION { "Pattern generator based on the Mutable Instruments Yarns module." };
  MIN_TAGS { };
  MIN_AUTHOR { "bosgood" };
  MIN_RELATED {};

  inlet<> clock { this, "(bang) clock signal input" };
  inlet<> pitch1_in { this, "(float) note 1 pitch" };
  // inlet<> velocity1_in { this, "(float) note 1 velocity" };
  // inlet<> pitch2_in { this, "(float) note 2 pitch" };

  outlet<> trig_out { this, "(bang) note trigger signal" };
  outlet<> pitch_out { this, "(float) note pitch" };

  timer<> metro { this,
    MIN_FUNCTION {

      return {};
    }
  };

private:
  // The timestamps of the few clocks signals received, used for
  // tempo calculation
  int clock_signals { 0, 0, 0 };
};

MAX_EXTERNAL(arp99);
