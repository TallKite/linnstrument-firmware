namespace MicroLinnV72A {

  struct MicroLinnDevice {
    byte MLversion;                                 // version = official version we forked from plus 56, MLversion = microLinn version
    byte scales[MICROLINN_ARRAY_SIZE];              // each byte is a bitmask for one note of the 8 scales, except bit 8 is unused
    byte rainbows[MICROLINN_ARRAY_SIZE];            // choose among the 10 colors
    byte dots[MICROLINN_ARRAY_SIZE];                // one bit per row, ignores column offsets except for lefty/righty
  };

  struct MicroLinnGlobal {
    byte EDO;                                  // ranges 5-55, plus 4 for OFF
    signed char octaveStretch;                 // ranges -120 to 120 cents, for non-octave tunings such as bohlen-pierce
    byte anchorCol;                            // ranges 1-25, setting to a number > 16 on a Linnstrument 128 is allowed
    byte anchorRow;                            // top row is 7, bottom row is 0, but the user sees top row as 1, bottom row as 8
    byte anchorNote;                           // any midi note 0-127, refers to a standard pitch of 12edo calibrated to A-440
    signed char anchorCents;                   // ranges -100 to +100 cents, even though 50 would do, for convenience
    short guitarTuning[MAXROWS];               // interval in edosteps from the bottom string, can be negative, independent of Global.guitarTuning
    boolean useRainbow;                        // if false, instead of the 9 colors, use only colorMain, and colorAccent for the tonic
    boolean sweeten;                           // adjust 41edo 5/4, 5/3 by 2¢ both top and bottom to make it 4¢ closer to just?
  };

  struct MicroLinnSplit {
    byte colOffset;                         // column offsets, 1 to 25
    signed char transposeEDOsteps;          // accessed not via displayMicroLinnConfig but via displayOctaveTranspose
    signed char transposeEDOlights;
    boolean rawMidiOutput;                  // output in edostep format (1 midi note = 1 edostep)
    unsigned short hammerOnWindow;          // maximum width in cents of a hammer-on before it becomes two simultaneous notes, 0 = off
    boolean hammerOnNewNoteOn;              // do hammer-ons send a new midi note or bend the old one? (guitar = yes, flute = no)
    byte pullOffVelocity;                   // 0 = 1st noteOn veloc, 1 = 2nd noteOn veloc, 2 = average them, 3 = 2nd note's noteOff velocity
  };

  struct DeviceSettings {
    byte version;                                   // the version of the configuration format
    boolean serialMode;                             // 0 = normal MIDI I/O, 1 = Arduino serial mode for OS update and serial monitor
    CalibrationX calRows[MAXCOLS+1][4];             // store four rows of calibration data
    CalibrationY calCols[9][MAXROWS];               // store nine columns of calibration data
    uint32_t calCrc;                                // the CRC check value of the calibration data to see if it's still valid
    boolean calCrcCalculated;                       // indicates whether the CRC of the calibration was calculated, previous firmware versions didn't
    boolean calibrated;                             // indicates whether the calibration data actually resulted from a calibration operation
    boolean calibrationHealed;                      // indicates whether the calibration data was healed
    unsigned short minUSBMIDIInterval;              // the minimum delay between MIDI bytes when sent over USB
    byte sensorSensitivityZ;                        // the scaling factor of the raw value of Z in percentage
    unsigned short sensorLoZ;                       // the lowest acceptable raw Z value to start a touch
    unsigned short sensorFeatherZ;                  // the lowest acceptable raw Z value to continue a touch
    unsigned short sensorRangeZ;                    // the maximum raw value of Z
    boolean sleepAnimationActive;                   // store whether an animation was active last
    boolean sleepActive;                            // store whether LinnStrument should go to sleep automatically
    byte sleepDelay;                                // the number of minutes it takes for sleep to kick in
    byte sleepAnimationType;                        // the animation type to use during sleep, see SleepAnimationType
    char audienceMessages[16][31];                  // the 16 audience messages that will scroll across the surface
    boolean operatingLowPower;                      // whether low power mode is active or not
    boolean otherHanded;                            // whether change the handedness of the splits
    byte splitHandedness;                           // see SplitHandednessType
    boolean midiThrough;                            // false if incoming MIDI should be isolated, true if it should be passed through to the outgoing MIDI port
    short lastLoadedPreset;                         // the last settings preset that was loaded
    short lastLoadedProject;                        // the last sequencer project that was loaded
    byte customLeds[LED_PATTERNS][LED_LAYER_SIZE];  // the custom LEDs that persist across power cycle
    MicroLinnDevice microLinn;                      // microtonal data
  };

  struct PresetSettings {
    GlobalSettings global;
    SplitSettings split[NUMSPLITS];
  };

  // per-split settings
  struct SplitSettings {
    byte midiMode;                          // 0 = one channel, 1 = note per channel, 2 = row per channel
    byte midiChanMain;                      // main midi channel, 1 to 16
    boolean midiChanMainEnabled;            // true when the midi main channel is enabled to send common data, false in not
    byte midiChanPerRow;                    // per-row midi channel, 1 to 16
    boolean midiChanPerRowReversed;         // indicates whether channel per row channels count upwards or downwards across the rows
    boolean midiChanSet[16];                // Indicates whether each channel is used.  If midiMode!=channelPerNote, only one channel can be set.
    BendRangeOption bendRangeOption;        // see BendRangeOption
    byte customBendRange;                   // 1 - 96
    boolean sendX;                          // true to send continuous X, false if not
    boolean sendY;                          // true to send continuous Y, false if not
    boolean sendZ;                          // true to send continuous Z, false if not
    boolean pitchCorrectQuantize;           // true to quantize pitch of initial touch, false if not
    byte pitchCorrectHold;                  // See PitchCorrectHoldSpeed values
    boolean pitchResetOnRelease;            // true to enable pitch bend being set back to 0 when releasing a touch
    TimbreExpression expressionForY;        // the expression that should be used for timbre
    unsigned short customCCForY;            // 0-129 (with 128 and 129 being placeholders for PolyPressure and ChannelPressure)
    unsigned short minForY;                 // 0-127
    unsigned short maxForY;                 // 0-127
    boolean relativeY;                      // true when Y should be sent relative to the initial touch, false when it's absolute
    unsigned short initialRelativeY;        // 0-127
    LoudnessExpression expressionForZ;      // the expression that should be used for loudness
    unsigned short customCCForZ;            // 0-127
    unsigned short minForZ;                 // 0-127
    unsigned short maxForZ;                 // 0-127
    boolean ccForZ14Bit;                    // true when 14-bit messages should be sent when Z CC is between 0-31, false when only 7-bit messages should be sent
    unsigned short ccForFader[8];           // each fader can control a CC number ranging from 0-128 (with 128 being placeholder for ChannelPressure)
    byte colorMain;                         // color for non-accented cells
    byte colorAccent;                       // color for accented cells
    byte colorPlayed;                       // color for played notes
    byte colorLowRow;                       // color for low row if on
    byte colorSequencerEmpty;               // color for sequencer low row step with no events
    byte colorSequencerEvent;               // color for sequencer low row step with events
    byte colorSequencerDisabled;            // color for sequencer low row step that's not being played
    byte playedTouchMode;                   // see PlayedTouchMode values
    byte lowRowMode;                        // see LowRowMode values
    byte lowRowCCXBehavior;                 // see LowRowCCBehavior values
    unsigned short ccForLowRow;             // 0-128 (with 128 being placeholder for ChannelPressure)
    byte lowRowCCXYZBehavior;               // see LowRowCCBehavior values
    unsigned short ccForLowRowX;            // 0-128 (with 128 being placeholder for ChannelPressure)
    unsigned short ccForLowRowY;            // 0-128 (with 128 being placeholder for ChannelPressure)
    unsigned short ccForLowRowZ;            // 0-128 (with 128 being placeholder for ChannelPressure)
    signed char transposeOctave;            // -60, -48, -36, -24, -12, 0, +12, +24, +36, +48, +60
    signed char transposePitch;             // transpose output midi notes. Range is -12 to +12
    signed char transposeLights;            // transpose lights on display. Range is -12 to +12
    boolean ccFaders;                       // true to activated 8 CC faders for this split, false for regular music performance
    boolean arpeggiator;                    // true when the arpeggiator is on, false if notes should be played directly
    boolean strum;                          // true when this split strums the touches of the other split
    boolean mpe;                            // true when MPE is active for this split
    boolean sequencer;                      // true when the sequencer of this split is displayed
    SequencerView sequencerView;            // see SequencerView
    MicroLinnSplit microLinn;               // microtonal data
  };


  struct Configuration {
    DeviceSettings device;
    PresetSettings settings;
    PresetSettings preset[NUMPRESETS];
    ::SequencerProject project;
  };
}
