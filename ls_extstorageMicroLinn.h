// This file preserves the older versions of the microLinn data structures, for version migration by ls_extstorage.ino
// When microLinn 72.1 comes out, change the structures in Linnstrument-firmware.ino, not these
// When microLinn 72.2 comes out, add MicroLinnV72_1 to this file in its own namespace
// When a new official version comes out, say V17, 
//    Merge the official changes into the microLinn version (this will include V16 code)
//    Change all the Vlatest structs from the V16 version to the V17 version
//    Update the ls_extstorage.ino file

// settingsVersion = the old version, read from the 1st byte of the data structures
// Device.Version = new version, explicitly set in ls_settings.ino, e.g. official fork has "Device.version = 16;"
// the microLinn fork has "Device.version = MICROLINN_VERSION_OFFSET + 16;" which is 72
// restoreNonMicroLinn takes us from 72.n to 16

namespace MicroLinnV72_0 {

  struct MicroLinnDevice {
    byte MLversion;                            // current version of the microLinn data structures, 0 in namespace MicroLinnV72_0
    byte padding;                              // makes the importing code easier
    byte scales[MICROLINN_ARRAY_SIZE];         // each byte is a bitmask for one note of the 8 scales, except bit 8 is unused
    byte rainbows[MICROLINN_ARRAY_SIZE];       // choose among the 10 colors
    byte fretboards[MICROLINN_ARRAY_SIZE];     // one byte per fret, one bit per row, transposable, lefthandedness reverses it, ignores column offsets
  };

  struct MicroLinnGlobal {
    byte EDO;                                  // ranges 5-55, plus 4 for OFF
    boolean useRainbow;                        // if false, instead of the 9 colors, use only colorMain, and colorAccent for the tonic
    short guitarTuning[MAXROWS];               // interval in edosteps from the bottom string, can be negative, [0] is unused, independent of Global.guitarTuning
    byte anchorCol;                            // ranges 1-25, setting to a number > 16 on a Linnstrument 128 is allowed
    byte anchorRow;                            // top row is 7, bottom row is 0, but the user sees top row as 1, bottom row as 8
    byte anchorNote;                           // any midi note 0-127, refers to a standard pitch of 12edo calibrated to A-440
    signed char anchorCents;                   // ranges -60 to +60 cents, even though 50 would do, for convenience
    signed char octaveStretch;                 // ranges -120 to 120 cents, for non-octave tunings such as bohlen-pierce
    byte sweeten;                              // in tenths of a cent, adjust 41edo 5/4, 5/3 by this amt both top and bottom to make it closer to just
  };

  struct MicroLinnSplit {
    byte colOffset;                         // column offset, 0 to 8, 1 = OFF
    byte hammerOnWindow;                    // maximum width in tens of cents of a hammer-on before it becomes two simultaneous notes, 0..240, 0 = off
    boolean hammerOnNewNoteOn;              // do hammer-ons send a new midi note or bend the old one? (guitar = yes, flute = no)
    byte pullOffVelocity;                   // 0 = 2nd note's noteOff velocity, 1 = 1st noteOn veloc, 2 = 2nd noteOn veloc, 3 = average them
    signed char transposeEDOsteps;          // accessed via displayOctaveTranspose
    signed char transposeEDOlights;
    byte tuningTable;                       // 0..2 = OFF/ON/RCH, output in edostep format (1 midi note = 1 edostep), lowest note is always note 0
  };

  struct DeviceSettings {
    byte version;                                   // the version of the configuration format, currently 16, but the microLinn version is 16 + 56 = 72
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

  struct GlobalSettings {
    void setSwitchAssignment(byte, byte, boolean);
  
    byte splitPoint;                           // leftmost column number of right split (0 = leftmost column of playable area)
    byte currentPerSplit;                      // controls which split's settings are being displayed
    byte activeNotes;                          // controls which of the 12 collections of note lights presets is active
    int mainNotes[12];                         // 12 bitmasks that determine which notes receive "main" lights, mainNotes[0] is for the 1st scale, [9-11] no longer used
    int accentNotes[12];                       // 12 bitmasks that determine which notes receive accent lights (octaves, white keys, black keys, etc.)
    byte rowOffset;                            // interval between rows, 0 = no overlap, 3-7 = interval, 12 = custom, 13 = guitar, 127 = zero offset
    signed char customRowOffset;               // the custom row offset that can be configured at the location of the octave setting
    byte guitarTuning[MAXROWS];                // the notes used for each row for the guitar tuning, 0-127
    VelocitySensitivity velocitySensitivity;   // See VelocitySensitivity values
    unsigned short minForVelocity;             // 1-127
    unsigned short maxForVelocity;             // 1-127
    unsigned short valueForFixedVelocity;      // 1-127
    PressureSensitivity pressureSensitivity;   // See PressureSensitivity values
    boolean pressureAftertouch;                // Indicates whether pressure should behave like traditional piano keyboard aftertouch or be continuous from the start
    byte switchAssignment[5];                  // The element values are ASSIGNED_*.  The index values are SWITCH_*.
    boolean switchBothSplits[5];               // Indicate whether the switches should operate on both splits or only on the focused one
    unsigned short ccForSwitchCC65[5];         // 0-127
    unsigned short ccForSwitchSustain[5];      // 0-127
    unsigned short customSwitchAssignment[5];  // ASSIGNED_TAP_TEMPO, ASSIGNED_LEGATO, ASSIGNED_LATCH, ASSIGNED_PRESET_UP, ASSIGNED_PRESET_DOWN, ASSIGNED_REVERSE_PITCH_X, ASSIGNED_SEQUENCER_PLAY, ASSIGNED_SEQUENCER_PREV, ASSIGNED_SEQUENCER_NEXT, ASSIGNED_STANDALONE_MIDI_CLOCK and ASSIGNED_SEQUENCER_MUTE
    byte midiIO;                               // 0 = MIDI jacks, 1 = USB
    ArpeggiatorDirection arpDirection;         // the arpeggiator direction that has to be used for the note sequence
    ArpeggiatorStepTempo arpTempo;             // the multiplier that needs to be applied to the current tempo to achieve the arpeggiator's step duration
    signed char arpOctave;                     // the number of octaves that the arpeggiator has to operate over: 0, +1, or +2
    SustainBehavior sustainBehavior;           // the way the sustain pedal influences the notes
    boolean splitActive;                       // false = split off, true = split on
    MicroLinnGlobal microLinn;                 // microtonal data
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

  struct PresetSettings {
    GlobalSettings global;
    SplitSettings split[NUMSPLITS];
  };

  struct Configuration {
    DeviceSettings device;
    PresetSettings settings;
    PresetSettings preset[NUMPRESETS];
    ::SequencerProject project;             // double colon = use the globally defined version, since microLinn didn't change it
  };
}
