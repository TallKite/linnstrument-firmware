/**************************************** MICROLINN ****************************************

Depending on the context, "microLinn" sometimes refers to the entire fork, and sometimes only the microtonal part of it

This fork mostly augments the official code and does not replace it. It mostly only runs if a microLinn feature is turned on
  Exceptions: sequencer next/prev/play/mute, program change messages sent on release not new touch, PCH footswitch, double volume faders
  CC faders use 2 colors, doubly-used midi channels warning, per-split midi channel view follows per-split midi mode
The code that actually handles playing in real time is mostly unchanged, so there should be no increases in latency
  Exceptions: condense to scale with variable pitch bend, mono mode, hammer-ons, sweetening

When the user releases a column 0 button, there is about an 1/8 second delay before one can play
When microLinn is on (when the edo is set), there's a slight extra delay of about 8 milliseconds while caculating tuning tables
But getNoteNumber() will use these tables, which might reduce latency while playing

All new functions and almost all new global vars contain "microLinn" in the name, to avoid conflicts with future updates from Roger
A function that starts with "microLinn" is only called locally in this .ino file, e.g. microLinnTriIndex()
But a function that has "microLinn" not at the beginning is called elsewhere, e.g. isMicroLinnOn() (and perhaps called locally too)

The microLinn software version number is the usual 6-digit version number plus our 3-digit number
This is the number that the Global Settings OS VERS button shows, e.g. 234.072.001
The microLinn data structure version number is Device.version (16 as of late 2025) plus an offset of 56, making 72,
plus Device.microLinn.MLversion = 1, making 72.1
This is the number ls_extstorage.ino uses to migrate user settings from one OS version to the next

MicroLinn includes KVR forum member teknico's channel pressure fix, many thanks to him! Search for "teknico" to see his code
Unlike his original modification, microLinn does not maximize the Timbre/Y data (needlessly prevents playing small Y values)
See https://www.kvraudio.com/forum/viewtopic.php?t=591770
https://github.com/rogerlinndesign/linnstrument-firmware/compare/master...teknico:linnstrument-firmware:less-jumpy-z-and-y-axes

Device.microLinn.uninstall is never initialized to false, because no matter where we put the initializing code,
  the updater app runs the code and re-initializes the flag to false, thus uninstalling never happens
(However, importing allUserSettings does set it to false, as does updating to microLinn from the official firmware)
Fortunately, the compiler automatically initializes the flag to false
Fortunately, a setting of the flag to true does not persist over power cycles, for some mysterious reason

In mono mode, a trill on 2 pads on different rows that have the same note creates multiple noteOns for the same midi note 
followed by a single noteOff for that note. The single noteOff should silence that note, but some synths may possibly leave it sounding
If so, use lastValueMidiNotesOn to count up the noteOns and send a series of matching noteOffs

Misc small bug fixes, not in the readme file, line numbers are for the official firmware:
  ls_fonts.ino         line 587   "static Character small_Q = { 4,"          4 -> 5
  ls_midi.ino          line 1045  "if (inRange(value, 1, 7)) {"              1 -> 0      (allow setting arp tempo to quarter notes)
                       line 1814  "preSendTimbre(split, 0, note, ch);"       ch -> ch+1
                       line 1815  "preSendLoudness(split, 0, 0, note, ch);"  ch -> ch+1
                       line 900   "if (inRange(value, 0, 14)) {"             14 -> 15
  ls_settings.ino      line 722   "midiPreset[0] = 0;"                       [0] -> [s]
                       line 2154  "handleNumericDataReleaseCol(true);"       true -> false   (microLinn issue #5)
  midi.txt             line 127   limits should be 0-15, also in the list playedCell is missing
  various files, search for "avoid transposeLights working backwards when lefty"

https://www.kvraudio.com/forum/viewtopic.php?t=591770 teknico's maximize-Z code
https://www.kvraudio.com/forum/viewtopic.php?t=570851 send program change messages on release, send bank select messages too
https://www.kvraudio.com/forum/viewtopic.php?t=603578 per-split custom LED patterns, for use with CC faders

Current beta testers: Kite, Gordon, Austin, Max, (Ira)

======================= TO DO ==============================

sample comparisons, add ".diff" to the url to get a diff file
https://github.com/jimsnow/linnstrument-firmware-ml/compare/main...TallKite:linnstrument-firmware:main
https://github.com/TallKite/linnstrument-firmware/compare/cf85bb7...TallKite:linnstrument-firmware:main
https://github.com/rogerlinndesign/linnstrument-firmware/compare/master...TallKite:linnstrument-firmware:Brightness-control.patch


  16    72.1
11692          sizeof(config)
       11692   sizeof(ConfigurationVLatest)
 2956   7548   sizeof(Device)
        4592   sizeof(MicroLinnDevice)
        2956   sizeof(Device) - sizeof(MicroLinnDevice)
  168    364   sizeof(GlobalSettings)
         198   sizeof(MicroLinnGlobal)
         166   sizeof(GlobalSettings) - sizeof(MicroLinnGlobal)      <--------- 166 != 168
   96    114   sizeof(SplitSettings)  (one side only)
          18   sizeof(MicroLinnSplit)
          96   sizeof(SplitSettings) - sizeof(MicroLinnSplit)

export type 17 on microLinn 72.1 with no explicit padding:
  Device = 1 (not 0, reaper's midi editor counts bytes 1-based)
     5 = Device.CalibrationX = 12 bytes per cell = 12*27*4 = 1296
  1301 = Device.CalibrationY = 7 bytes per cell = 7*9*8 = 504
  1805 = Device.calCrc
  1813 = Device.minUSBMIDIInterval
       = Device.audienceMessages = 16*31 bytes = 496
       = Device.customLeds = 3*26*8 = 624 bytes
  2957 = Device.microLinn.scales
  4487 = Device.microLinn.rainbows
  6017 = Device.microLinn.fretboards
  7549 = Global
  7715 = Global.microLinn
  7913 = Split[LEFT]
  8008 = Split[LEFT].microLinn

video desc:
link to harmonic series videos
explain JI breifly
explain the kite guitar breifly


MUST-DO #2

use virtual edosteps to get the midi to work
done for playing
started for sequencer
not started for arpeggiator and strummer
test all low row and special functions
doesn't work with legato footswitch

make all midi notes edosteps, increase from byte to short
for all midiSend functions that have notes as inputs, change from edosteps to midi notes just before calling

for the release notes: https://www.kvraudio.com/forum/viewtopic.php?t=528045
also https://www.kvraudio.com/forum/viewtopic.php?t=607214


FIX/TEST/DEBUG

RAM usage: config vars and runtime vars both use up free ram, const arrays don't

original software:         RAM dynamic = 0, static = 20744, free ram = 11908   (11816 via global display dump, secret switch [5] uses 96 bytes)
microLinn build #193                     0           30176              2376
microLinn build #194                     0           94764              3452


POSSIBLE MEMORY USAGE OPTIMIZATIONS

  DeviceSettingsV12 = 2204 bytes
  MicroLinnDevice = 4591 bytes
  DeviceSettings  = 7549 bytes, so pre-microLinn it was 2958
  PresetSettings = 400 bytes (Global plus both splits)
  thus the 6 presets will be 2400 bytes
  total is 10349 bytes
  microLinn sizeof(config) = 16564 bytes

reduce Global.mainNotes and Global.accentNotes from [12] to [9] and change from int to short? 
  96 bytes becomes 36 bytes, saves 60 bytes, saves 420 bytes with 6 presets, 840 bytes if 12 presets

(Device struct, cut 8 audience messages, saves 248 bytes)
MicroLinnDevice, pack the rainbows array to 4-bit values saves 765 bytes, but don't pack the constant array
Split struct, pack midiChanSet[16] saves 14 bytes, replace all shorts w bytes saves 23 bytes, total is 2 * 37 = 74 bytes x 7 = 518 bytes
Global struct, change shorts to bytes saves 19 bytes x 7 = 133 bytes
MicroLinnGlobal, pack rainbow, saves 27 bytes x 7 = 189 bytes
runtime var, pack lastValueMidiNotesOn, bytes to nybbles, saves 1K
runtime var, boolean switchCCEnabled[2][128], bytes to bits, saves 224 bytes
 420   Global.mainNotes and Global.accentNotes
 765   Device.microLinn.rainbows array
 518   Split.midiChanSet[16] and various shorts -> bytes
 133   Global various shorts -> bytes
 189   Global.microLinn.rainbow
1024   runtime var lastValueMidiNotesOn byte to nibble
 224   runtime var switchCCEnabled bytes to bits
====
3269 bytes

pack struct SequencerPattern, save 3 bytes = 12 bytes in struct SequencerProject, 12x17 = 204 bytes

runtime vars
  in ls_touchanim, converting both to unsigned short saves 800 bytes
  unsigned long touchAnimationLastMoment[MAXCOLS][MAXROWS];
  unsigned long touchAnimationSpeed[MAXCOLS][MAXROWS];
  
  boolean switchTargetEnabled[2][21], pack to 1-bit values saves 36 bytes
  boolean userFirmwareSlideMode[MAXROWS]
  boolean userFirmwareXActive[MAXROWS];
  boolean userFirmwareYActive[MAXROWS];
  boolean userFirmwareZActive[MAXROWS];  pack to 1-bit, saves 7 x 4 = 28 bytes
  StepSequencerState __attribute__ ((packed)) saves about 14 bytes, x 2 = 28 bytes in seqState[2]


struct SplitSettings {
  byte midiMode;                          // 0 = one channel, 1 = note per channel, 2 = row per channel
  boolean midiChanMainEnabled;            // true when the midi main channel is enabled to send common data, false in not
  boolean midiChanPerRowReversed;         // indicates whether channel per row channels count upwards or downwards across the rows
  byte midiChanMain;                      // main midi channel, 1 to 16
  byte midiChanPerRow;                    // per-row midi channel, 1 to 16
  boolean midiChanSet[16];                // Indicates whether each channel is used
}

struct SplitSettings {
  // 1 byte = 8 bits example
  inline byte    midiMode()                         {return (midiFlags & B11000000) >> 6;}
  inline boolean midiChanMainEnabled()              {return bitRead(midiFlags, 5);}
  inline boolean midiChanPerRowReversed()           {return bitRead(midiFlags, 4);}
  inline void setMidiMode(byte i)                   {midiFlags &= B00111111; midiFlags |= i << 6;}
  inline void setMidiChanMainEnabled(boolean b)     {bitwrite(midiFlags, 5, b);}
  inline void setMidiChanPerRowReversed(boolean b)  {bitwrite(midiFlags, 4, b);}
  // 1 byte = 2 nybbles example
  inline byte midiChanMain()                        {return (midiChan >> 4) + 1;}
  inline byte midiChanPerRow()                      {return (midiChan & 15) + 1;}
  inline void setMidiChanMain(byte i)               {midiChan &= x0F; midiChan |= (i-1) << 4;}
  inline void setMidiChanPerRow(byte i)             {midiChan &= xF0; midiChan |= i-1;}
  // 1 short = 16 bits example
  inline boolean midiChanSet(byte i)                {return bitRead(midiChanSet, i);}
  inline void setMidiChanSet(byte i, boolean b)     {bitWrite(midiChanSet, i, b);}

  byte midiFlags;
//byte midiMode;                          // 0 = one channel, 1 = note per channel, 2 = row per channel
//boolean midiChanMainEnabled;            // true when the midi main channel is enabled to send common data, false in not
//boolean midiChanPerRowReversed;         // indicates whether channel per row channels count upwards or downwards across the rows
  byte midiChan;
//byte midiChanMain;                      // main midi channel, 1 to 16
//byte midiChanPerRow;                    // per-row midi channel, 1 to 16
  short midiChanSet;
//boolean midiChanSet[16];                // Indicates whether each channel is used
}

"if (Split[side].midiMode == 1)"  ->  "if (Split[side].midiMode() == 1)"
"Split[side].midiMode = 1;"       ->  "Split[side].setMidiMode(1);"

"if (Split[side].midiChanSet[i])"  ->  "if (Split[side].midiChanSet(i))"
"Split[side].midiChanSet = true;"  ->  "Split[side].setMidiChanSet(true);"

HARDWARE ISSUES
low row joystick xDelta is off
RAM usage: why so stable? what's a safe number?
does packing a preset work?

Add Quantize Hold to list of Foot/Panel Switch assignments, sez Roger
https://www.kvraudio.com/forum/viewtopic.php?t=588669 
my QNT± currently toggles Split[split].pitchCorrectQuantize
rename QNT± to QNT, toggle between current and previous settings
byte Split.prevQuantSettings[NUMSPLITS] holds all 8 settings, defaults to all off
pressing the switch swaps current settings with previous settings, A <-> B
if user changes settings while off, that's the new "B" state
user can switch between any 2 settings, e.g. quant fast/slow
need to save prev settings in the presets
readme: warn user that if prev=curr, switch does nothing


volume up/down options (like preset +/-) for the two panel/foot switches, Roger agrees
https://www.kvraudio.com/forum/viewtopic.php?t=587851

microLinn menu, rowOffsets linking works backwards, holding both sets old to new, not new to old

test unninstalling more

the import/export all settings code is broken until largeEDOoffsets is uncommented

scales screen: in full rainbow mode, let the user tap the 2 walls to change their color?
  search for "orangish-yellow borders", make a new global var Global.microLinn.borderColor

check that microLinnCalcLowestEdostepEtc() and others take into account condensing

on the note lights screen, when long-pressing a scale button to reset the scale, it should blink

tuning table modes: 55ed(4/3-50c) = 440/3 notes per octave, 1024 total notes means 1024x3/440 = 384/55 = 7 octaves

decide, should tuning table notes be transposable?

after bulk importing, or after unplugging, bend slope is sometimes too big, going to global menu fixes it

when bulk importing/exporting, clear screen and display "IMP" or "EXP"?

color-code the menu buttons? makes them more memorable

import types 9-11: warning excess data
import type 14, 15: various data failures

fix bug with anchor row = 0 and guitar tuning, strings preview note are off by one

ask on the forum about the unplugging bug for NRPN send/receive

colorAccent can be set to 0, if so my code should use some fixed color

test if changing splits always triggers a calctuning call, if so, just calc the displayed split(s)

implement showCustomLEDs via getCombinedLedData() instead?
see https://github.com/rogerlinndesign/linnstrument-firmware/compare/master...cmhoneycutt343:linnstrument-firmware-chuckb:master
fix custom LED bugs: completelyRefreshLeds, startBufferedLeds, finishBufferedLeds, see paintSequencerDisplay()
  see NRPN 247
  see https://www.kvraudio.com/forum/viewtopic.php?t=519090, checkRefreshLedColumn(micros())

test octave up/down footswitches while playing and sliding, does the calcTuning call make it glitchy?
  if so, copy how the original code does transposing

debug same/blink applying to the other side for note-offs, search for resetPossibleNoteCells
bug, red playedSame dots are wrong when the number of virtual edosteps is > 128, needs the group number
bug, condensing messes up the red dots
try implementing getMicroLinnNoteNumColumn()
try code that mimics receiving a note-on from the DAW from a channel for the other split

OLD WAY
The colors use the rainbow metaphor, red-yellow-green-blue = sharp-to-flat = supermajor-to-subminor. There's a rainbow of 2nds, a rainbow of 3rds, etc.
* white = 12-edo-ish = 3-limit
* yellow / green = downmajor / upminor =  5-over / 5-under (examples: 5/4 and 5/3 are 5-over, 6/5 and 16/15 are 5-under)
* blue / red = downminor / upmajor = 7-over / 7-under (examples: 7/4 and 7/6 are 7-over, 8/7 and 9/7 are 7-under)
* purple = neutral = 11-over or 11-under (e.g. 11/9 or 12/11) or 13-over or 13-under (e.g. 13/8 or 18/13)
* pink is reserved for C in certain edos
* cyan / orange = a catch-all pair, e.g. 41edo 7/5 and 10/7, cyan is also for "outside" notes aka interordinals e.g. 24edo
* orange is used in 55edo for upmid and downmid notes

POSSIBLE NEW FEATURES

warning: RAM is limited, can cause Arduino IDE triple compile error

col 12) Microtonal scale aka note lights
  col 3, row 6) Fine-tuning editor

col 16) Global microtonal settings
  row 6) Sweetening amount in tenths of cents (OFF, 0.1 to 6.0)
  row 7) Large EDO for fine tuning (OFF, various 56-311, 1200)

improve XYZ behavior with Nathan Kopp's code?
  https://github.com/nathankopp/linnstrument-firmware/wiki
  https://github.com/rogerlinndesign/linnstrument-firmware/compare/master...nathankopp:linnstrument-firmware:63bd3af
  https://www.kvraudio.com/forum/viewtopic.php?t=537336

qanun-style mandals: set one split to CC faders and set the 1st CC to 129 = MND = mandals
  condense, the faders alter the scale notes by edosteps
  top row = tonic, 2nd row = 2nd, etc.
  write directly to Device.microLinn.scales using microLinnScale array, then call calcCondensedTuning()
  if one scale note is moved to be on top of another, they merge into one
  if one scale note is moved past another, the notes are reordered

make the red dot follow a slide to the next pad? see transferFromSameRowCell() and handleSlideTransferCandidate()
  It already follows if played mode is CELL
  editing handleSlideTransferCandidate to use inRange(playedTouchMode, playedCell, playedBlink) sort of works, 
  but it leaves stray dots behind and the twin dots don't follow

Punch -- avoid a sluggish attack, scale the first 50 ms of Z to the velocity, stop scaling if Z > V
  T = time in ms since note-on, Z = (T/50)*Z + (1-T/50)*V, or with wet/dry W = 0..1, W*(T/50)*Z + W*(1-T/50)*V + (1-W)*Z
  code it up in jsfx first for testing, vars are Tmax and W
  OR, layer the sound with a 2nd "clicky" synth that has very fast attack and decay and doesn't listen to Z-data at all
  https://www.kvraudio.com/forum/viewtopic.php?t=584971
  https://www.kvraudio.com/forum/viewtopic.php?t=522304&hilit=vcv 

Roger's idea for a transpose function for the low row
https://www.kvraudio.com/forum/viewtopic.php?t=594760 
https://www.kvraudio.com/forum/viewtopic.php?t=596619

midi looper?

add a few custom colors? user can set the rgb values themselves. use jay sullivan's code:
https://github.com/rogerlinndesign/linnstrument-firmware/commit/5ea10702f9f98afa2af1cda266b8d4e90e5483ea
add new color between lime and green for neutral intervals?

drum pad mode, besides ON and OFF, add MRMB/MRB for marimba mode
  this adds an 8th column and gets the midi notes from the current scale, transposable
  if microLinn is off, the scale starts at lo-C = 48
  if on, it starts at the anchor note (needs to include tuning bends)
  if custom light pattern or if microLinn fretboard, use the previous scale or else the 1st scale

hammerons/pulloffs screen, select for each split,
  see colsInRowsTouched[MAXROWS]
  play D C E, lift up E, go to D or C? (closest vs. latest) closest
  play C E D, lift up D, go to E or C? both are equally close (highest vs lowest vs latest) latest
  option to send or not send a new noteon when hammering or pulling off? (guitar = yes, wind instrument = no)
  if new noteOn, send the noteOn on the same channel (same as the current strumming behavior)
  if no new noteOn, send a big pitch bend instead
  thus the cents zone cant't be bigger than the pitchbend range if no new noteOns
  pulloffs: old note's velocity = the new note's velocity or the original old velocity? (pull-off vs lift-off)
  byte pullOffVelocity; 0 = 2nd note's noteOff velocity, 1 = 1st noteOn veloc, 2 = 2nd noteOn veloc, 3 = average them
  make 2 hammer-on zones, one for ascending and one for descending?
    setting downward to 0¢ lets one play two notes by flamming, leading with the higher note
    but flamming leading with the lower note makes a hammer-on
see also https://www.kvraudio.com/forum/viewtopic.php?t=545602&sid=fc3c835f43d57ea031ef86720ccf6827
"Love my Linnstrument but one feature missing from seaboard" Roger discusses mono legato

sweeten 41edo: widen 5/4 by shifting the top note up 2¢ and the bottom note down 2¢
  ratios to widen: 5/4 5/3 5/2 10/3 5/1 20/3 10/1     (maybe 15/8 15/4 15/2?)  (maybe 10/7?)
  ratios to narrow: 6/5 8/5 12/5 16/5 24/5 32/5 48/5  (maybe 9/5 18/5 36/5?)   (maybe 7/5?)
  pitch shifts of 2¢ are unnoticeable, worst case is Cv followed by C^m making C and G shift by 4¢, which is OK
  v(vv#5) chord's 3rd is pulled both ways, so no change, even in an add8 voicing that pulls up harder than down
    Struct Pull {boolean sharp, flat;};
    Pull pull[MICROLINN_MAX_EDOSTEPS];
    if (pull[cell].sharp && !pull[cell].flat) sharpen(cell);
    if (pull[cell].flat && !pull[cell].sharp) flatten(cell);
  issues: in dim2 chord or v(b5) chord, 7/5 is 2¢ sharper = 5¢ sharp (add 7/5 to the narrowing list?)
  in a v9 chord, 9th is 2¢ sharper = 3¢ sharp, and in an ^9 chord, root is 3¢ flat (add 9/5 to the narrowing list?)
  in a vM7no3 or vM7(4) chord, the root-5th interval is 2¢ flat (widen 15/8?)
  would 11-limit chords become worse?
   
Large EDO subset of up to 55 notes:
  for an N-note scale, select N notes of small edo = S-edo then select large edo = L-edo, runs 56..311 (or N+1..311?)
  it's a good idea to pad a very uneven scale with a few extra notes, makes bends work better
  byte Global.microLinn.largeEDO, signed char largeEDOoffset[55], short largeEDOscale[55]
  the S-edo scale gets mapped to L-edo by cents
  tap the rainbow enabler button to cycle it to white and enter fine-tuning mode
    cycle gywg means fine-tuning always has the rainbow enabled, cycle gwywg means rainbow can be off
    "col 3, row 2) Rainbow enabler (yellow = ON, green = OFF, white = fine-tune)"
  tap any note and get a big blue number, edosteps in L-edo or cents in JI, swipe it to adjust ±127
  fine-tuning a note sounds the note, the pitch is calced on the spot
  tap the col 12 scales button to get back to the scales screen to tap another note
  if largeEDO is changed, this scale is adjusted to the new large edo by direct scaling
  if S-edo is changed, load L-edo with N evenly spaced notes 
  show largeEDOscale as notes in condensed ups and downs notation (vG = .G, vvG = :G, Gb = G.)
    edostep 0 is named as the 12edo anchor note plus arrows from any transposition (ignore anchor cents?)
    note format: cyan arrows - blue letter - cyan accidentals (no octave number), same font as anchor notes
    arrows/accidentals are a column going up from bottom row or down from top row, right next to the letter
    letters are 6 rows high, so max 5 arrows/accidentals per column (quip & quid!), use multiple cols if needed
    the scale buttons take up 4 cols, so for Linn 128 there's only 12 cols for the note name
    letters are 3-4 cols wide, so room for 8 columns of arrows/accidentals = 36-40 total
      311edo is sharp-30, pentasharp-23, max arrows = max(A1,m2)/2 = up to 3 cols of arrows and 1 col for #/b
      301edo is 28 23, up to 3 cols of arrows and 1 col for #/b
      note name switches from F to F# to Gb following the fewest-arrows rule, in a sharp-5 edo dudsharp beats trup
        in 72edo, vF# not vGb because the note is closer to F than to G
      1200edo shows a 12edo note plus cents, red if negative, F# is 4 cols wide, digits are 3 cols wide + 1 col spacer
      1200edo's black keys are named C# Eb F# Ab Bb, works musically, avoids the too-wide G
  transpose by S-edo edosteps? or L-edo edosteps?
    to transpose in 311edo, 311edo maj 2nd = 53\311, the M2 is divided into 15 small steps (transpose by -7..7 small steps),
    round(53/15) = 4, so transpose by edosteps is really 4 edosteps = 14¢. 1200edo --> 204/15 = 14¢
  if the equave is non-octave, which are the best large edos to use?
    when the edos get huge, any EDONOI becomes an edo with only a few cents of stretch, enter it as such

add new played mode rainbow (RNBW or RNB) that reveals the rainbow color even on the fretboard or custom LED pattern
  or just have it kick in when cell color = off?

write code for 6 new memories/presets, 6 new buttons next to the 6 standard ones (column 15 or column 22)?
  for the Linn 128, move the program change number one pad to the left to make room for the new buttons
  update NRPN 243 inrange code

when microLinn is installed, move the 16 sequencer projects around so that the row of the 4x4 array doesn't change?
  the uninstall code moves them back?
SEQUENCER PROJECTS LOAD/SAVE SCREEN: When exporting/importing projects to/from your computer, the updater app refers to 
projects by number. These project numbers now run top to bottom, with the top row being 1-4 not 13-16. This is more 
intuitive because it matches how people read, for example how you are reading this very paragraph right now.

TO DECIDE

implement namespace? search for [^a-z]microLinn[a-z], change to [^a-z]ML[a-z]
   fix sizeof(ML) by hand

POSSIBILTIES

Add the ability to set the split point to col 1? enables single-tap mode to switch splits
  settings.ino handleSplitPointNewTouch() comment out first line
  handleControlButtonNewTouch()
    case SPLIT_ROW:  (doubleTap || Global.splitPoint == 1)
    maybe set Global.splitActive == false;

make system reset be a long press not a short press?

run the arpeggiator and the sequencer both at once, so that the arpeggiator uses the sequencer's rhythm & velocity?


POSSIBLE PULL REQUESTS (every non-microtonal feature that doesn't require a new user setting, test thoroughly first)
  sequencer: foot pedal fix, pattern-chaining feature, 4x4 array fix
  DONE: long-press low power affects brightness only
  double volume faders
  add 8VE± footswitch function

note to self: user can enter/exit microLinn only by going to microLinnConfig and changing the edo from 4 
or by loading a preset (directly or via NRPN), the EDO+ and EDO- switches/pedals don't let you enter or exit

note to self: to use multi-channel output with my non-MPE s90es synth, deselect the main channel

WHEN DONE:
change microLinnOSVersion from 000 to 001
comment out microLinnSetupKitesPersonalPrefs() call
delete CC41 send and timer code in calcTuning
remove updater diagnostics
remove calbration importing
comment out DEBUG_ENABLED in ls_debug.h
cleanup: search for "delete", "uncomment" and "bug"

********************************************************************************************************************/


// virtual edosteps needed = 7 * maxRowOffset + 24 * maxColOffset + 1 = 368 max, but up it to 512 to give the guitar tuning more leeway
const byte MICROLINN_MAX_ROW_OFFSET = 25;                  // if this is changed, change it in initMicroLinnData() too
const byte MICROLINN_MAX_COL_OFFSET = 10;                  // 10 is the M2 of 54edo, for 53 and 55 it's 9
//const short MICROLINN_MAX_EDOSTEPS = 7 * 25 + 24 * 8 + 1;  // 175 + 192 + 1 = 368
const short MICROLINN_MAX_EDOSTEPS = 200;                  // for testing. delete later
//const short MICROLINN_MAX_EDOSTEPS = 1024;               // virtual edosteps use 8 midi groups and run 0..1023
const short MICROLINN_MAX_GUITAR_RANGE = 196; // 320;      // 512 - 192, 192 = max row length = numCols * maxColOffset = 24 * 8

const byte MICROLINN_NUM_LARGE_EDOS = 54;
const short MICROLINN_LARGE_EDO[MICROLINN_NUM_LARGE_EDOS] = {
  0,  56,  58,  60,  63,    65,  68,  72,  77,  80,        // extended list of absolute zeta peak edos, for Global.microLinn.largeEDO
 84,  87,  89,  94,  96,    99, 103, 106, 111, 113,        // en.xen.wiki/w/The_Riemann_zeta_function_and_tuning#Extended_list_of_absolute_zeta_peak_edos
118, 121, 125, 130, 137,   140, 145, 149, 152, 159,   
161, 166, 171, 176, 183,   190, 193, 198, 202, 212, 
217, 224, 229, 239, 243,   248, 255, 270, 277, 282,   
289, 301, 311, 1200
};

const byte MICROLINN_MIN_2ND[MICROLINN_MAX_EDO+1] = {      // used for Bosanquet 2 layout
  0,  0,  0,  0,  1,   0,  0,  0,  1,  2,    // 0-9        // to ensure coprime-ness, divide by GCD(M2,m2)
  1,  3,  1,  2,  1,   1,  3,  1,  3,  2,    // 10-19
  1,  1,  1,  4,  1,   1,  3,  1,  1,  2,    // 20-29
  1,  3,  1,  4,  1,   1,  1,  1,  1,  2,    // 30-39
  5,  3,  1,  4,  1,   5,  3,  6,  1,  2,    // 40-49
  5,  1,  3,  4,  1,   5                     // 50-55
};

const byte MICROLINN_MAJ_2ND[MICROLINN_MAX_EDO+1] = {      // defined not as best 9/8 but as best 3/2 minus best 4/3
  0,  0,  0,  0,  2,   1,  1,  1,  2,  1,    // 0-9        // used for Bosanquet & Wicki-Hayden layouts
  2,  1,  2,  3,  2,   3,  2,  3,  4,  3,    // 10-19      // 13 and 18, not 13b and 18b
  4,  3,  4,  3,  4,   5,  4,  5,  4,  5,    // 20-29 
  6,  5,  6,  5,  6,   5,  6,  7,  6,  7,    // 30-39
  6,  7,  8,  7,  8,   7,  8,  7,  8,  9,    // 40-49
  8,  9,  8,  9, 10,   9                     // 50-55
};

const byte MICROLINN_MAJ_2ND_TR[MICROLINN_MAX_EDO+1] = {   // defined not as best 9/8 but as best 3/2 minus best 4/3
  0,  0,  0,  0,  1,   1,  1,  1,  1,  1,    // 0-9        // but it's actually just a single edostep for edos 6, 8, 10 & 12
  1,  1,  1,  3,  2,   3,  2,  3,  4,  3,    // 10-19      // 13 and 18, not 13b and 18b
  4,  3,  4,  3,  4,   5,  4,  5,  4,  5,    // 20-29      // used for transposePitch
  6,  5,  6,  5,  6,   5,  6,  7,  6,  7,    // 30-39
  6,  7,  8,  7,  8,   7,  8,  7,  8,  9,    // 40-49
  8,  9,  8,  9, 10,   9                     // 50-55
};

const byte MICROLINN_PERF_4TH[MICROLINN_MAX_EDO+1] = {     // defined as best 4/3
  0,  0,  0,  0,  5,   2,  2,  3,  3,  4,    // 0-9        // used to test if microLinn.guitarTuning is F#BEADGBE
  4,  5,  5,  5,  6,   6,  7,  7,  7,  8,    // 10-19      // if it isn't, the guitar tuning light is light blue
  8,  9,  9, 10, 10,  10, 11, 11, 12, 12,    // 20-29      // 13b = 6666626 and 18b = 8888848 are also allowed
 12, 13, 13, 14, 14,  15, 15, 15, 16, 16,    // 30-39      // also used for Wicki-Hayden & Array Mbira layouts
 17, 17, 17, 18, 18,  19, 19, 20, 20, 20,    // 40-49
 21, 21, 22, 22, 22,  23                     // 50-55
};

// MICROLINN_SCALEROWS: for each of the 7 degrees plus the octave, the last note in each row of the note lights display

/********************** this table was generated with the following C program **********************
  for (byte edo = 5; edo <= MICROLINN_MAX_EDO; ++edo) {
    byte M2 = MICROLINN_MAJ_2ND[edo];                     // actually just a single edostep for edos 6, 8, 10 & 12
    byte m2 = floor ((edo - 5 * M2) / 2.0);               // 6edo and 8edo need floor
    byte half2nd = floor (min (M2, m2) / 2.0);            // include half the in-between-degrees notes on each row
    if (edo == 14 || edo == 28) half2nd -= 1;             // * looks better

    MICROLINN_SCALEROWS[edo][0] = half2nd;                // unisons
    MICROLINN_SCALEROWS[edo][1] = half2nd + M2;           // 2nds
    MICROLINN_SCALEROWS[edo][2] = half2nd + 2*M2;         // 3rds
    MICROLINN_SCALEROWS[edo][3] = half2nd + 2*M2 + m2;    // 4ths
    MICROLINN_SCALEROWS[edo][4] = half2nd + 3*M2 + m2;    // 5ths
    MICROLINN_SCALEROWS[edo][5] = half2nd + 4*M2 + m2;    // 6ths
    MICROLINN_SCALEROWS[edo][6] = half2nd + 5*M2 + m2;    // 7ths
    MICROLINN_SCALEROWS[edo][7] = half2nd + edo;          // octaves

    if (m2 == 0) {                                        // ** the pentatonic edos 5, 10, 15, 20, 25 and 30
      MICROLINN_SCALEROWS[edo][6] -= M2;                  // no 7ths, makes them all 8ves, since it's 1200¢
    }
  }
  // *** supersharp edos 8, 13, and 18 and superflat edos 9, 11 and 23 must be manually adjusted
  ***************************/

const byte MICROLINN_SCALEROWS[MICROLINN_MAX_EDO+1][8] = { 
  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  1,  2,  2,  3,  4,  4,  5,  // 5edo **
  0,  1,  2,  2,  3,  4,  5,  6,  // 6edo
  0,  1,  2,  3,  4,  5,  6,  7,  // 7edo
  0,  2,  4,  4,  5,  7,  7,  8,  // 8edo ***
  0,  1,  2,  4,  5,  6,  7,  9,  // 9edo ***
  0,  2,  4,  4,  6,  8,  8, 10,  // 10  **
  0,  1,  2,  5,  6,  7,  8, 11,  // 11  ***
  0,  2,  4,  5,  7,  9, 11, 12,  // 12
  0,  3,  6,  6,  8, 11, 11, 13,  // 13  ***
  0,  2,  4,  6,  8, 10, 12, 14,  // 14  *
  0,  3,  6,  6,  9, 12, 12, 15,  // 15  **
  1,  3,  5,  8, 10, 12, 14, 17,  // 16
  0,  3,  6,  7, 10, 13, 16, 17,  // 17
  1,  5,  9,  9, 12, 16, 16, 19,  // 18  ***
  1,  4,  7,  9, 12, 15, 18, 20,  // 19
  0,  4,  8,  8, 12, 16, 16, 20,  // 20  **
  1,  4,  7, 10, 13, 16, 19, 22,  // 21
  0,  4,  8,  9, 13, 17, 21, 22,  // 22
  1,  4,  7, 11, 14, 17, 20, 24,  // 23  ***
  1,  5,  9, 11, 15, 19, 23, 25,  // 24
  0,  5, 10, 10, 15, 20, 20, 25,  // 25  **
  1,  5,  9, 12, 16, 20, 24, 27,  // 26
  0,  5, 10, 11, 16, 21, 26, 27,  // 27
  1,  5,  9, 13, 17, 21, 25, 29,  // 28  *
  1,  6, 11, 13, 18, 23, 28, 30,  // 29
  0,  6, 12, 12, 18, 24, 24, 30,  // 30  **
  1,  6, 11, 14, 19, 24, 29, 32,  // 31
  0,  6, 12, 13, 19, 25, 31, 32,  // 32
  2,  7, 12, 16, 21, 26, 31, 35,  // 33
  1,  7, 13, 15, 21, 27, 33, 35,  // 34
  2,  7, 12, 17, 22, 27, 32, 37,  // 35
  1,  7, 13, 16, 22, 28, 34, 37,  // 36
  0,  7, 14, 15, 22, 29, 36, 37,  // 37
  2,  8, 14, 18, 24, 30, 36, 40,  // 38
  1,  8, 15, 17, 24, 31, 38, 40,  // 39
  2,  8, 14, 19, 25, 31, 37, 42,  // 40
  1,  8, 15, 18, 25, 32, 39, 42,  // 41
  0,  8, 16, 17, 25, 33, 41, 42,  // 42
  2,  9, 16, 20, 27, 34, 41, 45,  // 43
  1,  9, 17, 19, 27, 35, 43, 45,  // 44
  2,  9, 16, 21, 28, 35, 42, 47,  // 45
  1,  9, 17, 20, 28, 36, 44, 47,  // 46
  3, 10, 17, 23, 30, 37, 44, 50,  // 47
  2, 10, 18, 22, 30, 38, 46, 50,  // 48
  1, 10, 19, 21, 30, 39, 48, 50,  // 49
  2, 10, 18, 23, 31, 39, 47, 52,  // 50
  1, 10, 19, 22, 31, 40, 49, 52,  // 51
  3, 11, 19, 25, 33, 41, 49, 55,  // 52
  2, 11, 20, 24, 33, 42, 51, 55,  // 53
  1, 11, 21, 23, 33, 43, 53, 55,  // 54
  2, 11, 20, 25, 34, 43, 52, 57   // 55
};

const byte MICROLINN_RAINBOWS[MICROLINN_ARRAY_SIZE] = {
//  lime (10) is used for yellow because the official yellow (2) is too orangish
//  8     = white         = 12-edo-ish           = 3-limit
//  10/3  = yellow/green  = submajor/superminor  = 5-over/5-under
//  5/1   = blue/red      = subminor/supermajor  = 7-over/7-under
//  6     = magenta       = neutral              = 11-over or 11-under or 13-over or 13-under
//  11    = pink          = 600c, a half-8ve     = 12-edo-ish but not quite 3-limit, "off-white"
//  4     = cyan          = outside notes aka interordinals (e.g. 19edo and 24edo)
//  9     = orange        = used in 55edo for superneutral and subneutral, to avoid 3 magentas in a row
//          cyan/orange   = used in 41edo for 7/5 and 10/7

// Bosanquet / Wicki-Hayden colors
//                         for 5n and 7n edos
//   8   white = natural   pink = tonic
//   3   green = b         down
//  10  yellow = #         up
//   4    cyan = bb        dud
//   9  orange = ##        dup
//   5    blue = bbb       trud/trup
//  11    pink = x#
//   6  purple = bbbb
//   1     red = 
//   2  yellow/orange = 

//8, 1, 8, 8, 5, // 5edo = white red white white blue
 11, 8, 8, 8, 8, // 5edo = pink white white white white
//8,10,10,11, 3, 3, // 6edo
 11, 8, 8, 8, 8, 8, // 6edo
//8,10, 6, 8, 8, 6, 3, // 7edo
 11, 8, 8, 8, 8, 8, 8, // 7edo
//8, 6, 3, 5,11, 1,10, 6, // 8
  8, 3, 8, 3, 8, 8, 3, 8, // 8
//8, 3, 5,10, 6, 6, 3, 1,10, // 9
  8, 8, 8, 3, 8, 8, 8, 8, 3, // 9
//8, 3, 1, 6, 8,11, 8, 6, 5,10, // 10
 11, 3, 8, 3, 8, 3, 8, 3, 8, 3, // 10
//8, 3, 1, 3, 1, 6, 6, 5,10, 5,10, // 11
  8, 8, 8, 3, 3, 8, 8, 8, 8, 3, 3, // 11
//8, 3,10, 3,10, 8,11, 8, 3,10, 3,10, // 12
  8, 3, 8, 3, 8, 8, 3, 8, 3, 8, 3, 8, // 12
//8, 5,10, 5,10, 5, 6, 6, 1, 3, 1, 3, 1, // 13
  8,10, 3, 8,10, 3, 8, 3, 8,10, 3, 8, 3, // 13
//8, 5,10, 5, 6, 1, 8,11, 8, 5, 6, 1, 3, 1, // 14
 11, 3, 8, 3, 8, 3, 8, 3, 8, 3, 8, 3, 8, 3, // 14
//8, 3,10, 1, 3,10, 8, 6, 6, 8, 3,10, 5, 3,10, // 15
 11, 3,10, 8, 3,10, 8, 3,10, 8, 3,10, 8, 3,10, // 15
//8, 6, 3,10, 3,10, 6, 8,11, 8, 6, 3,10, 3,10, 6, // 16
  8,10, 8, 3, 8, 3,10, 8,10, 8, 3, 8, 3, 8, 3,10, // 16
//8, 5, 6, 1, 5, 6, 1, 8, 6, 6, 8, 5, 6, 1, 5, 6, 1, // 17
  8, 3,10, 8, 3,10, 8, 8, 3,10, 8, 3,10, 8, 3,10, 8, // 17
//8, 5, 3,10, 5, 3,10, 5, 6,11, 6, 1, 3,10, 1, 3,10, 1, // 18
//8,10, 8,10, 8,10, 3,10, 8,10, 8,10, 8,10, 8,10, 3,10, // 18b
  8,10, 4, 3, 8,10, 4, 3, 8,10, 3, 8,10, 4, 3, 8,10, 3, // 18
//8, 4, 3,10, 4, 3,10, 4, 8, 6, 6, 8, 4, 3,10, 4, 3,10, 4, // 19
  8,10, 3, 8,10, 3, 8, 4, 8,10, 3, 8,10, 3, 8,10, 3, 8, 4, // 19
//8, 6, 3,10, 1, 3,10, 1, 8, 6,11, 6, 8, 5, 3,10, 5, 3,10, 6, // 20
 11, 3, 4,10, 8, 3, 4,10, 8, 3, 4,10, 8, 3, 4,10, 8, 3, 4,10, // 20
//8, 6, 3,10, 1, 5, 6,10, 5, 8, 6, 6, 8, 1, 3, 6, 1, 5, 3,10, 6, // 21
 11,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, 8,10, 3, // 21
//8, 5, 3,10, 1, 5, 3,10, 1, 8, 6,11, 6, 8, 5, 3,10, 1, 5, 3,10, 1, // 22
  8, 3, 4,10, 8, 3, 4,10, 8, 8, 3, 4,10, 8, 3, 4,10, 8, 3, 4,10, 8, // 22
//8, 4, 3, 6, 1, 5, 3, 6, 1, 5, 8, 6, 6, 8, 1, 5, 6,10, 1, 5, 6,10, 4, // 23
  8, 4,10, 8, 3, 4, 8, 3, 4,10, 8, 4,10, 8, 3,10, 8, 3, 4, 8, 3, 4,10, // 23
//8, 4, 3, 6,10, 4, 3, 6,10, 4, 8, 6,11, 6, 8, 4, 3, 6,10, 4, 3, 6,10, 4, // 24
  8, 4, 3,10, 8, 4, 3,10, 8, 4, 8, 4, 3,10, 8, 4, 3, 10, 8, 4, 3,10, 8, 4, // 24
//8, 5, 3, 6,10, 1, 5, 3,10, 1, 8, 6,10, 3, 6, 8, 5, 3,10, 1, 5, 3, 6,10, 1, // 25
 11, 4, 3,10, 9, 8, 4, 3,10, 9, 8, 4, 3,10, 9, 8, 4, 3,10, 9, 8, 4, 3,10, 9, // 25
//8, 1, 5, 3,10, 1, 5, 3,10, 1, 5, 8, 6,11, 6, 8, 1, 5, 3,10, 1, 5, 3,10, 1, 5, // 26
  8,10, 4, 3, 8,10, 4, 3, 8,10, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 3, // 26
//8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 8, 6, 3,10, 6, 8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, // 27 -- swap the 2 tritones?
  8, 3, 4, 9,10, 8, 3, 4, 9,10, 8, 8, 3, 4, 9,10, 8, 3, 4, 9,10, 8, 3, 4, 9,10, 8, // 27
//8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, 8, 6,11, 6, 8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, // 28
 11,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, 8,10, 4, 3, // 28
//8, 4, 8, 3,10, 8, 4, 8, 3,10, 8, 4, 8, 6,10, 3, 6, 8, 4, 8, 3,10, 8, 4, 8, 3,10, 8, 4, // 29
  8, 9, 3,10, 4, 8, 9, 3,10, 4, 8, 9, 8, 9, 3,10, 4, 8, 9, 3,10, 4, 8, 9, 3,10, 4, 8, 9, // 29
//8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 8, 3, 6,11, 6,10, 8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, // 30
 11,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, // 30
//8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, // 31
  8, 4,10, 3, 9, 8, 4,10, 3, 9, 8, 4, 9, 8, 4,10, 3, 9, 8, 4,10, 3, 9, 8, 4,10, 3, 9, 8, 4, 9, // 31
//8, 4, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 4, 8, 3, 6,11, 6,10, 8, 4, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 4, // 32
  8, 3, 4, 5, 9,10, 8, 3, 4, 5, 9,10, 8, 8, 3, 4, 5, 9,10, 8, 3, 4, 5, 9,10, 8, 3, 4, 5, 9,10, 8, // 32
//8, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, // 33
  8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 3, // 33
//8, 4, 5, 3, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,11, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,10, 1, 4, // 34
  8, 4, 3, 5,10, 9, 8, 4, 3, 5,10, 9, 8, 4, 8, 4, 3, 5,10, 9, 8, 4, 3, 5,10, 9, 8, 4, 3, 5,10, 9, 8, 4, // 34
//8, 1, 5, 3, 6,10, 8, 1, 5, 3, 6,10, 8, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 8, 3, 6,10, 1, 5, 8, 3, 6,10, 1, 5, // 35
 11,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 4, 3, // 35
//8, 1, 5, 3, 6, 6,10, 1, 5, 3, 6, 6,10, 1, 5, 8, 6, 6,11, 6, 6, 8, 1, 5, 3, 6, 6,10, 1, 5, 3, 6, 6,10, 1, 5, // 36
  8, 4, 5, 3,10, 9, 8, 4, 5, 3,10, 9, 8, 4, 9, 8, 4, 5, 3,10, 9, 8, 4, 5, 3,10, 9, 8, 4, 5, 3,10, 9, 8, 4, 9, // 36
//8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,10, 3, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, // 37
  8, 3, 4, 5,11, 9,10, 8, 3, 4, 5,11, 9,10, 8, 8, 3, 4, 5,11, 9,10, 8, 3, 4, 5,11, 9,10, 8, 3, 4, 5,11, 9,10, 8, // 37
//8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, 8, 6,10,11, 3, 6, 8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, // 38
  8, 9,10, 5, 3, 4, 8, 9,10, 5, 3, 4, 8, 9,10, 4, 8, 9,10, 5, 3, 4, 8, 9,10, 5, 3, 4, 8, 9,10, 5, 3, 4, 8, 9,10, 4, // 38
//8, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 8, 3, 6, 3,10, 6,10, 8, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, // 39 -- swap the 2 tritones?
  8,11, 3, 9, 4,10, 5, 8,11, 3, 9, 4,10, 5, 8,11, 8,11, 3, 9, 4,10, 5, 8,11, 3, 9, 4,10, 5, 8,11, 3, 9, 4,10, 5, 8,11, // 39
//8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 6,10,11, 3, 6, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 40
  8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 5, 4, 3, 8,10, 9, 4, 3, // 40
  8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6, 4, 9, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 41
//8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10,11, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 42
  8, 3, 4, 5, 6,11, 9,10, 8, 3, 4, 5, 6,11, 9,10, 8, 8, 3, 4, 5, 6,11, 9,10, 8, 3, 4, 5, 6,11, 9,10, 8, 3, 4, 5, 6,11, 9,10, 8, // 42
//8, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 8, 6, 6,10, 3, 6, 6, 8, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, // 43
  8, 4,11,10, 3, 5, 9, 8, 4,11,10, 3, 5, 9, 8, 4,11, 9, 8, 4,11,10, 3, 5, 9, 8, 4,11,10, 3, 5, 9, 8, 4,11,10, 3, 5, 9, 8, 4,11, 9, // 43
  8, 3, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 8, 3, 6,10,11, 3, 6,10, 8, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1,10, // 44
//8, 1, 4, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 4, 5, // 45
  8, 5,10, 4, 9, 3,11, 8, 5,10, 4, 9, 3,11, 8, 5,10, 3,11, 8, 5,10, 4, 9, 3,11, 8, 5,10, 4, 9, 3,11, 8, 5,10, 4, 9, 3,11, 8, 5,10, 3,11, // 45
  8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, // 46

//8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 47
  8,10, 9,11, 5, 4, 3, 8,10, 9,11, 5, 4, 3, 8,10, 9,11, 4, 3, 8,10, 9,11, 5, 4, 3, 8,10, 9,11, 5, 4, 3, 8,10, 9,11, 5, 4, 3, 8,10, 9,11, 4, 3, // 47
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10,11, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 48
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 49
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 50
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 51
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 52
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 53
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10,11, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 54
  8, 1, 4, 5, 8, 3, 9, 6, 9,10, 8, 1, 5, 8, 3, 9, 6, 9,10, 8, 1, 4, 5, 8, 9, 6, 9,10, 3, 9, 6, 9, 8, 1, 4, 5, 8, 3, 9, 6, 9,10, 8, 1, 5, 8, 3, 9, 6, 9,10, 8, 1, 4, 5  // 55
};

const byte MICROLINN_SCALES[MICROLINN_ARRAY_SIZE] = {
  // 5edo    1L1s(3:2), 2L1s(2:1)
  B1111111,  // P1
  B0000000,  // M2/m3
  B0000010,  // P4
  B0000011,  // P5
  B0000000,  // M6/m7
  // 6edo    2L, 3L, 2L2s(2:1)
  B1111111,  // P1
  B0000000,  // M2
  B0000110,  // M3
  B0000101,  // A4/d5
  B0000010,  // m6
  B0000100,  // m7
  // 7edo    1L1s(4:3), 1L2s(3:2), 3L1s(2:1), 2L3s(2:1)
  B1111111,  // P1
  B0000000,  // P2
  B0001110,  // P3
  B0001000,  // P4
  B0001111,  // P5
  B0000000,  // P6
  B0001100,  // P7
  // 8edo    2L1s(3:2), 4L, 3L2s(2:1), 2L4s(2:1)
  B1111111,  // P1
  B0000000,  // 
  B0001110,  // m3
  B0001001,  // 
  B0001110,  // A4/d5
  B0000100,  // 
  B0001011,  // M6
  B0001100,  //   
  // 9edo    1L3s(3:2), 4L1s(2:1), 3L3s(2:1), 2L5s(2:1)
  B1111111,  // P1
  B0000000,  // M2
  B0001110,  // m2/M3
  B0001101,  // m3
  B0001010,  // P4
  B0001111,  // P5
  B0000100,  // M6
  B0001011,  // m6/M7
  B0001100,  // m7
  // 10edo   2L2s(3:2), 5L, 4L2s(2:1), 3L4s(2:1)
  B1111111,  // P1
  B0000100,  // 
  B0001010,  // M2/m3
  B0001101,  // 
  B0000010,  // P4
  B0001101,  // 
  B0001110,  // P5
  B0000000,  // 
  B0001111,  // M6/m7
  B0001000,  // 
  // 11edo   3L1s(3:2), 1L4s(3:2), 5L1s(2:1), 4L3s(2:1)
  B1111111,  // P1
  B0000000,  // M2
  B0001110,  // 
  B0001001,  // 
  B0000110,  // m3
  B0001000,  // P4
  B0001111,  // P5
  B0000000,  // M6
  B0001100,  // 
  B0000011,  // 
  B0001100,  // m7
  // 12edo   major, minor, major pentatonic, minor pentatonic
  B1111111,  // P1
  B0000000,  // m2
  B0000111,  // M2
  B0001010,  // m3
  B0000101,  // M3
  B0001011,  // P4
  B0000000,  // A4/d5
  B0001111,  // P5
  B0000010,  // m6
  B0000101,  // M6
  B0001010,  // m7
  B0000001,  // M7
  // 13edo   3L2s(3:2), 1L5s(3:2), 5L3s(2:1), 4L5s(2:1)
  B1111111,  // P1
  B0000000,  //
  B0001110,  // m3
  B0001001,  //
  B0000110,  //
  B0001101,  // P4
  B0001010,  //
  B0000100,  //
  B0001111,  // P5
  B0001000,  //
  B0000100,  //
  B0001011,  // M6
  B0001100,  //
  // 14edo   4L1s(3:2), 2L4s(3:2), 7L, 5L4s(2:1)
  B1111111,  // P1
  B0000000,  //
  B0001100,  // P2
  B0000011,  //
  B0001100,  // P3
  B0001010,  //
  B0000101,  // P4
  B0001010,  //
  B0001101,  // P5
  B0000000,  //
  B0001110,  // P6
  B0001001,  //
  B0000110,  // P7
  B0001000,  //
  // 15edo   3L3s(3:2), 1L6s(3:2), 5L5s(2:1), 3L9s(2:1)
  B1111111,  // P1
  B0001000,  //
  B0000110,  //
  B0001101,  // M2/m3
  B0001010,  //
  B0001101,  //
  B0001110,  // P4
  B0000000,  //
  B0001101,  //
  B0001110,  // P5
  B0001001,  //
  B0001110,  //
  B0000100,  // M6/m7
  B0001011,  //
  B0001100,  //
  // 16edo   4L2s(3:2), 2L5s(3:2), 7L2s(2:1), 4L8s(2:1)
  B1111111,  // P1
  B0001000,  //
  B0000110,  //
  B0001001,  //
  B0001110,  //
  B0001000,  // m3
  B0000101,  //
  B0001110,  // P4
  B0001001,  //
  B0001110,  // P5
  B0000000,  //
  B0001111,  // M6
  B0001000,  //
  B0001110,  //
  B0000001,  //
  B0001100,  //
  // 17edo   5L1s(3:2), 3L4s(3:2), 1L7s(3:2), 5L7s(2:1)
  B1111111,  // P1
  B0001000,  // m2
  B0000100,  //
  B0001011,  // M2
  B0001100,  // m3
  B0000010,  //
  B0001101,  // M3
  B0001010,  // P4
  B0001100,  //
  B0000001,  //
  B0001110,  // P5
  B0001000,  // m6
  B0000011,  //
  B0001100,  // M6
  B0001000,  // m7
  B0000111,  //
  B0001000,  // M7 
  // 18edo   3L2s(4:3), 4L3s(3:2), 2L6s(3:2), 6L6s(2:1)
  B1111111,  // P1
  B0000000,  //  
  B0001100,  //
  B0001010,  // m3
  B0000001,  //
  B0001100,  //
  B0001010,  //
  B0000101,  // P4
  B0001010,  //
  B0001100,  //
  B0000000,  //
  B0001111,  // P5
  B0001000,  //
  B0000010,  //
  B0001100,  //
  B0001001,  // M6
  B0000110,  //
  B0001000,  // 
  // 19edo   1L5s(4:3), 3L5s(3:2), 1L8s(3:2), 7L5s(2:1)
  B1111111,  // P1
  B0000000,  //  
  B0001100,  // m2
  B0001010,  // M2
  B0000101,  //
  B0001010,  // m3
  B0001100,  // M3
  B0000001,  //
  B0001110,  // P4
  B0000000,  //
  B0001011,  //
  B0001100,  // P5
  B0000010,  //
  B0001101,  // m6
  B0001000,  // M6
  B0000110,  //
  B0001001,  // m7
  B0001110,  // M7
  B0000000,  //


  // 20edo   2L4s(4:3), 6L1s(3:2), 4L4s(3:2), 2L7s(3:2)
  B1111111,  // P1
  B0000000,  //
  B0000100,  //
  B0001010,  //
  B0000001,  // M2/m3
  B0001100,  //
  B0000010,  //
  B0001101,  //
  B0000000,  // P4
  B0001010,  //
  B0000101,  //
  B0000000,  //
  B0001110,  // P5
  B0000000,  //
  B0001001,  //
  B0000110,  //
  B0001000,  // M6/m7
  B0000101,  //
  B0001010,  //
  B0000000,  //
  // 21edo   1L4s(5:4), 5L3s(3:2), 3L6s(3:2), 1L9s(3:2)
  B1111111,  // P1
  B0000000,  //  
  B0001100,  //
  B0000010,  // P2
  B0001001,  //
  B0000100,  //
  B0001010,  // P3
  B0000100,  //
  B0001011,  //
  B0000100,  // P4
  B0001000,  //
  B0000010,  //
  B0001101,  // P5
  B0000000,  //
  B0000110,  //
  B0001000,  // P6
  B0000110,  //
  B0001001,  //
  B0000000,  // P7
  B0001110,  //
  B0000000,  //
  // 22edo   1L6s(4:3), 6L2s(3:2), 4L5s(3:2), 2L8s(3:2), needs 5L7s(3:1)
  B1111111,  // P1
  B0000000,  // m2
  B0001010,  //
  B0000101,  //
  B0001000,  // M2
  B0000110,  // m3
  B0000001,  //
  B0001000,  //
  B0000110,  // M3
  B0001001,  // P4
  B0000100,  //
  B0001010,  //
  B0000000,  //
  B0001111,  // P5
  B0000000,  // m6
  B0001100,  //
  B0000011,  //
  B0000000,  // M6
  B0001100,  // m7
  B0000011,  //
  B0001100,  //
  B0000000,  // M7
  // 23edo   5L1s(4:3), 7L1s(3:2), 5L4s(3:2), 3L7s(3:2)
  B1111111,  // P1
  B0000000,  //  
  B0001000,  //
  B0000110,  // M2
  B0001001,  //
  B0000100,  //
  B0000010,  //
  B0001000,  // m3
  B0000101,  //
  B0001010,  //
  B0000100,  // P4
  B0001000,  //
  B0000011,  //
  B0001100,  // P5
  B0000000,  //
  B0000010,  //
  B0001101,  // M6
  B0000000,  //
  B0001110,  //
  B0000000,  //
  B0001001,  // m7
  B0000110,  //  
  B0000000,  //  
  // 24edo   3L4s(4:3), 6L3s(3:2), 4L6s(3:2), 2L9s(3:2)
  B1111111,  // P1
  B0000000,  //  
  B0000100,  // m2
  B0001010,  //
  B0000001,  // M2
  B0001100,  //
  B0000010,  // m3
  B0001101,  //
  B0000010,  // M3
  B0001000,  //
  B0000101,  // P4
  B0001010,  //
  B0000100,  // A4/d5
  B0000000,  //
  B0001111,  // P5
  B0000000,  //
  B0001010,  // m6
  B0000101,  //
  B0001000,  // M6
  B0000110,  //
  B0001000,  // m7
  B0000001,  //  
  B0001110,  // M7
  B0000000,  //  
  
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0,64,64, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 25
  // 26: #3 = sagugu halberstadt
  127, 0,68,64,71,64,68,66,68,65, 0,71, 0,64, 0,71, 0,70,64,69,64,68,66,68,65, 0, // 26
  127,64,64, 0,67,64,64,66, 0,65,64,67, 0,64,64, 0,67,64,66, 0,65,64,64,66, 0,65,64, // 27
  127,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0,64, 0,67, 0,64,66, 0,65,64,64,66, 0,65,64, // 28
  127, 0,64,64,64,67, 0,64,66,65,64, 0,67, 0,64,64, 0,67, 0,64,66,65,64, 0,64,66,65,64, 0, // 29
  127, 0,64,64, 0,67,64,64,66, 0,65,64,67, 0, 0,64, 0, 0,67,64,66, 0,65,64,64,66, 0,65,64, 0, // 30
  127, 0,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0,64,64, 0,67,64,64,66, 0,65,64,64,66, 0,65,64, 0, // 31
  127, 0,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0, 0,64, 0, 0,67, 0,64,66, 0,65,64,64,66, 0,65,64, 0, // 32
  127, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0,67, 0,64,64, 0,67, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, // 33
  // 34: #3 = sagugu halberstadt
  127, 0,64,68, 0,67,68, 0,64,70, 0,65,68, 0,71, 0, 0,68, 0, 0,71, 0,64,70, 0,65,68, 0,64,70, 0,65,68, 0, // 34
  127, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0, 0,67, 0,64,64, 0,67, 0, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, // 35
  127, 0,64,64, 0, 0,67,64,64,66, 0, 0,65,64, 0,67, 0, 0,64, 0, 0,67, 0,64,66, 0, 0,65,64,64,66, 0, 0,65,64, 0, // 36
  127, 0, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0,67, 0, 0,64,64, 0, 0,67, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, 0, // 37
  127, 0, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0, 0,67, 0, 0,64, 0, 0,67, 0, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, 0, // 38
  127, 0,64,64, 0, 0,64,67, 0,64,66, 0, 0,65,64, 0,67, 0, 0,64,64, 0, 0,67, 0,64,66, 0, 0,65,64, 0,64,66, 0, 0,65,64, 0, // 39
  127, 0, 0,64,64,64,64,67, 0, 0,66,64,64,65,64, 0, 0,67, 0, 0,64, 0, 0,67, 0, 0,64,66,64,64,65, 0, 0,66,64,64,65,64, 0, 0, // 40
  127, 0,64, 0,64, 0,64,67,64,64, 0,66, 0,65, 0,64, 0,67, 0,64,64,64,64, 0,67, 0,64, 0,66, 0,65, 0,64,64,64,66, 0,65, 0,64, 0, // 41
  127, 0, 0,64,64,64,64,67, 0, 0,64,66,64,64,65, 0, 0,67, 0, 0,64,64,64, 0, 0,67, 0, 0,64,66,64,64,65, 0, 0,66,64,64,65,64, 0, 0, // 42
  127, 0, 0,64,64, 0, 0,67,64, 0,64,66, 0, 0,65,64, 0, 0,67, 0, 0,64,64, 0, 0,67, 0, 0,64,66, 0, 0,65,64, 0,64,66, 0, 0,65,64, 0, 0, // 43
  127, 0, 0,64, 0,64, 0, 0,67, 0,64, 0,66, 0,65, 0,64, 0,67, 0, 0,64, 0,64, 0, 0,67, 0,64, 0,66, 0,65, 0,64, 0,64, 0,66, 0,65, 0,64, 0, // 44
  127, 0, 0,64,64, 0, 0,67, 0, 0,64, 0,66, 0,65, 0,64, 0, 0,67, 0, 0,64,64, 0, 0,67, 0, 0,64, 0,66, 0,65, 0,64,64, 0,66, 0,65, 0,64, 0, 0, // 45
  127, 0,64, 0,64, 0, 0,64,67,64,64, 0,66, 0, 0,65, 0,64, 0,67, 0, 0,64,64,64, 0, 0,67, 0,64, 0,66, 0, 0,65, 0,64,64,64,66, 0, 0,65, 0,64, 0, // 46
  127, 0, 0,64,64, 0, 0,67, 0, 0,64, 0,66, 0, 0,65, 0,64, 0, 0,67, 0, 0,64,64, 0, 0,67, 0, 0,64, 0,66, 0, 0,65, 0,64, 0,66, 0, 0,65, 0,64, 0, 0, // 47
  127, 0, 0, 0,64, 0, 0, 0,67, 0,64, 0,66, 0, 0, 0,65, 0,64, 0,67, 0, 0, 0,64, 0, 0, 0,67, 0,64, 0,66, 0, 0, 0,65, 0,64, 0,66, 0, 0, 0,65, 0,64, 0, // 48
  127, 0, 0, 0,64, 0, 0, 0,67, 0, 0,64, 0,66, 0, 0,65, 0,64, 0,67, 0, 0, 0,64,64, 0, 0, 0,67, 0,64, 0,66, 0, 0,65, 0,64, 0,64, 0,66, 0, 0,65, 0,64, 0, // 49
  127, 0, 0,64, 0,64, 0, 0,67, 0,64,64, 0,66, 0, 0,65, 0,64, 0, 0,67, 0, 0, 0,64, 0, 0, 0,67, 0, 0,64, 0,66, 0, 0,65, 0,64,64, 0,66, 0, 0,65, 0,64, 0, 0, // 50
  127, 0, 0,64, 0,64, 0,64, 0,67, 0, 0,64, 0,66, 0,65, 0,64, 0, 0,67, 0, 0, 0,64,64, 0, 0, 0,67, 0, 0,64, 0,66, 0,65, 0,64, 0, 0,64, 0,66, 0,65, 0,64, 0, 0, // 51
  127, 0, 0, 0,64, 0, 0, 0,67, 0, 0,64, 0,66, 0, 0, 0,65, 0,64, 0, 0,67, 0, 0, 0,64, 0, 0, 0,67, 0, 0,64, 0,66, 0, 0, 0,65, 0,64, 0,66, 0, 0, 0,65, 0,64, 0, 0, // 52
  127, 0, 0,64, 0,64, 0, 0,64,67,64, 0,64, 0,66, 0, 0,65, 0,64, 0, 0,67, 0, 0,64,64,64,64, 0, 0,67, 0, 0,64, 0,66, 0, 0,65, 0,64, 0,64,64,66, 0, 0,65, 0,64, 0, 0, // 53
  127, 0, 0,64, 0,64, 0, 0,64, 3,64, 0,64, 0,64, 2, 0,65, 0,64, 0, 0,67, 0,64, 0,64, 0,64, 0,64, 0,67, 0, 0,64, 0,66, 0, 1,64, 0,64, 0,64, 0,64, 2, 0,65, 0,64, 0, 0, // 54
  127, 0, 0,64, 0,64, 0,64, 0,67, 0,64,64, 0,66, 0, 0, 0,65, 0,64, 0, 0,67, 0,64, 0,64,64, 0,64, 0,67, 0, 0,64, 0,66, 0, 0, 0,65, 0,64,64, 0,66, 0, 0, 0,65, 0,64, 0, 0  // 55
};

const byte MICROLINN_FRETBOARDS [6 * (MICROLINN_MAX_EDO - 4)] = { 
// for each edo, this array gives the location in edosteps of each fret marker, 0 means none
// all are single dots except the 5th column is the double dot for the 4th/5th, only doubled if there are more than 5 dots per octave
// all this is for the lower half of the octave only, the upper half gets filled in by symmetry
// the unison/octave always gets a triple dot, so there's a maximum of 13 dots per octave
  2,  0,  0,  0,  0,  0,  //  5edo ---- edos 5-9 mark P4 and P5
  2,  0,  0,  0,  0,  0,  //  6edo
  3,  0,  0,  0,  0,  0,  //  7edo
  3,  0,  0,  0,  0,  0,  //  8edo
  3,  0,  0,  0,  0,  0,  //  9edo (9b)
  2,  4,  0,  0,  0,  0,  // 10edo --- edos 10-24 mark (M2) m3 P4 P5 M6 (m7)
  2,  4,  0,  0,  0,  0,  // 11edo (11b)
  3,  5,  0,  0,  0,  0,  // 12edo
  2,  5,  0,  0,  0,  0,  // 13edo
  2,  4,  0,  0,  6,  0,  // 14edo
  3,  6,  0,  0,  0,  0,  // 15edo
  2,  5,  0,  0,  7,  0,  // 16edo
  4,  7,  0,  0,  0,  0,  // 17edo
  3,  7,  0,  0,  0,  0,  // 18edo
  3,  5,  0,  0,  8,  0,  // 19edo
  4,  8,  0,  0,  0,  0,  // 20edo
  3,  6,  0,  0,  9,  0,  // 21edo
  5,  9,  0,  0,  0,  0,  // 22edo
  3,  7,  0,  0, 10,  0,  // 23edo
  4,  6,  0,  0, 10,  0,  // 24edo
  2,  4,  6,  8, 10, 12,  // 25edo --- edos 25-55 approximate 12edo
  2,  4,  7,  9, 11, 13,  // 26edo
  2,  4,  7,  9, 11, 13,  // 27edo
  2,  5,  7, 10, 12, 14,  // 28edo
  2,  5,  7, 10, 12, 14,  // 29edo
  3,  6,  9,  0, 12, 15,  // 30edo (exception -- 15edo dots doubled)
  3,  5,  8, 10, 13, 15,  // 31edo
  3,  5,  8, 10, 13, 16,  // 32edo
  3,  6,  8, 11, 14, 16,  // 33edo
  3,  6,  8, 11, 14, 17,  // 34edo
  3,  6,  9, 12, 15, 17,  // 35edo
  3,  6,  9, 12, 15, 18,  // 36edo
  3,  6,  9, 12, 15, 18,  // 37edo
  3,  6, 10, 13, 16, 19,  // 38edo
  3,  6, 10, 13, 16, 19,  // 39edo
  3,  7, 10, 14, 17, 20,  // 40edo
  0,  0,  0,  0,  0,  0,  // 41edo (exception -- kite guitar dots)
  3,  7, 10, 14, 17, 21,  // 42edo
  4,  7, 11, 14, 18, 21,  // 43edo
  4,  7, 11, 14, 18, 22,  // 44edo
  4,  8, 11, 15, 19, 22,  // 45edo
  4,  8, 11, 15, 19, 23,  // 46edo
  4,  8, 12, 16, 20, 23,  // 47edo
  4,  8, 12, 16, 20, 24,  // 48edo
  4,  8, 12, 16, 20, 24,  // 49edo
  4,  8, 13, 17, 21, 25,  // 50edo
  4,  8, 13, 17, 21, 25,  // 51edo
  4,  9, 13, 18, 22, 26,  // 52edo
  4,  9, 13, 18, 22, 26,  // 53edo
  5,  9, 14, 18, 23, 27,  // 54edo
  5,  9, 14, 18, 23, 27   // 55edo
};

////////////  runtime vars  ///////////////////////////

short microLinnEdostep[NUMSPLITS][MAXCOLS][MAXROWS];                // each cell's edosteps relative to the anchor cell
signed char microLinnMidiNote[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];   // the midi note number for each untransposed virtual edostep, -1 = unused
short microLinnTuningBend[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];       // the deviation from 12edo as a pitch bend number from -8192 to 8191

byte edo;                                                       // these 2 are the only microLinn vars that don't have "microLinn" in the name
float edostepSize;                                              // edo's step size in 12edo semitones

short microLinnColOffsetCents[NUMSPLITS];                       // row/col offset cents are set only when user sets an offset directly...
float microLinnSemitonesPerPad[NUMSPLITS];                      // number of 12edo semitones in a pitch bend one pad wide, positive even when lefty
short microLinnRowOffsetCents[NUMSPLITS];                       // ...used to avoid the offset drifting too wide or too narrow when switching edos
short microLinnGlobalRowOffsetCents;                            // used to adjust either Global.rowOffset or Global.customRowOffset when changing the edo
// delete microLinnGuitarTuning? slower but 16 bytes more RAM
short microLinnGuitarTuning[MAXROWS];                           // cumulative row offsets, unlike Global.microLinn.guitarTuning, for a faster calcTuning()
short microLinnGuitarTuningCents[MAXROWS];                      // cents between open strings, can be negative
short microLinnHighestGuitarNote;                               // the highest/lowest note in the anchor column, as edosteps from the anchor cell...
short microLinnLowestGuitarNote;                                // ...used to find lowestEdostep and numGroups, and to limit guitar range when changing edos
short microLinnLowestEdostep[NUMSPLITS];                        // what not where, lowest edostep when the anchor cell is 0, usually negative
byte microLinnChannelCount[NUMSPLITS];                          // used for rechannelling
byte microLinnNumGroups[NUMSPLITS];                             // 1 if highestEdostep - lowestEdostep < 128, 2 if < 256, 3 if < 384, 4 if < 512... up to 8
short microLinnSweetener[NUMSPLITS];                            // 2¢ (or whatever) as a zero-centered midi pitch bend value
char microLinnAnchorString[6] = "R C  ";                        // row and column of the anchor cell, e.g. "R3C12", top row is row #1
short microLinnHammerOnEdosteps[NUMSPLITS];                     // convert Split[side].microLinn.hammerOnZone from cents to edosteps
struct MicroLinnHammerOn {signed char note, channel, col, row;};      // -1 means no note or channel or col or row
MicroLinnHammerOn microLinnHammerOns[9];                        // for each hammer-on (10 fingers = 9 max), store the midi channel and note
byte microLinnNumHammerOns = 0;                                 // pointer into microLinnHammerOns[9]
byte microLinnScale[MICROLINN_MAX_EDO+1];                       // condensed scale e.g. 12edo major scale --> [0 2 4 5 7 9 11 12 0 0 0 0]
byte microLinnScaleLookup[MICROLINN_MAX_EDO+1];                 // look up scale step by edostep          --> [0 0 1 0 2 3 0 4 0 5 0 6 7]
byte microLinnNumNotes;                                         // number of notes in the condensed scale, 1..55
byte microLinnLargestStep = 1;                                  // largest step in the condensed scale, 1..55

// not yet used
//struct MicroLinnPull {boolean sharp, flat;};                    // used to sweeten 41edo
//MicroLinnPull microLinnPull[MICROLINN_MAX_EDOSTEPS];
//short microLinnLargeEDOscale[MICROLINN_MAX_EDO];                // LargeEDO edosteps

// menu vars
byte microLinnConfigColNum = 0;                                 // active col number in microLinnConfig display, 0 means nothing active
byte microLinnPerSplitSettingsRowNum = 0;
byte microLinnGlobalSettingsRowNum = 0;
byte microLinnPerSplitXenSettingsRowNum = 0;
byte microLinnGlobalXenSettingsRowNum = 0;
byte microLinnGuitarTuningRowNum = NUMROWS + 1;                 // active row number 0..7, NUMROWS+1 means no row selected, +1 so that row 7 isn't lit
boolean microLinnConfigNowScrolling = false;
boolean microLinnPerSplitSettingsNowScrolling = false;
boolean microLinnGlobalSettingsNowScrolling = false;
boolean microLinnPerSplitXenSettingsNowScrolling = false;
boolean microLinnGlobalXenSettingsNowScrolling = false;
boolean microLinnColOffsetLinked;                               // true if column offset screen adjusts both splits at once
boolean microLinnRowOffsetLinked; 
boolean microLinnPerSplitSettingsLinked[MAXROWS];               // each setting has a separate flag
boolean microLinnPerSplitXenSettingsLinked[MAXROWS];
byte microLinnPrevScale;                                        // for backtracking to the previous scale easily
boolean microLinnCanBacktrack = false;                          // true if newly touching an already selected scale button

// importing vars
byte microLinnImportType = 0;                                   // 0 means no import is currently happening
short microLinnImportSize = 0;                                  // size = the # of data bytes, 2 data bytes per 3-byte polypressure msg
short microLinnImportCounter = 0;
boolean microLinnImportXen = true;                              // don't import xen data for most presets if rawEDO == 4
boolean microLinnImportingOn = false;
unsigned long microLinnLastMomentMidiPP;                        // for timeout calculations, not yet used

// debugging vars
unsigned long MLcalcTuningTimer;  // delete later
//int MLdebug[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/************** math functions ************************/

signed char microLinnSign (short num) {
  if (num > 0) return 1;
  if (num < 0) return -1;
  return 0;
}

int microLinnMod (int num, int base) {          // -13 % 10 = -3, but microLinnMod (-13, 10) = 7
  num %= base;
  if (num < 0) num += base;
  return num;
}

short microLinnGCD (short x, short y) {         // returns the greatest common divisor of x & y
  x = abs (x); y = abs (y);                     // the GCD is always positive
  if (x == 0) return y;                         // ensures GCD (0, y) = y
  if (y == 0) return x;                         // ensures GCD (x, 0) = x
  short z;
  while (x > 0) {
    if (x < y) {z = x; x = y; y = z;}           // swap x and y
    x %= y;
  }
  return max (x, y);                            // GCD (0, 0) = 0
}

short microLinnLCM (short x,  short y) {        // returns the least common multiple of x & y
  float z = microLinnGCD (x, y);
  return (z == 0 ? 0 : abs (x * y) / z);        // LCM (x, 0) = x, LCM (0, y) = y, LCM (0, 0) = 0
}

// unused, delete?
short microLinnRound (float x) {
  return floor (x + 0.5);                       // round towards positive infinity
}

/************** lookup functions called only from ls_microLinn.ino ************************/


/********** unused, delete?
struct __attribute__ ((packed)) MicroLinn2nybbles {   // packed byte
  byte get (byte index);
  void set (byte index, byte data);
  byte nybble1:4;
  byte nybble2:4;
};

struct __attribute__ ((packed)) MicroLinn2signedNybbles {   // packed byte
  signed char get (byte index);
  void set (byte index, signed char data);
  signed char nybble1:4;
  signed char nybble2:4;
};

struct __attribute__ ((packed)) MicroLinn8bits {     // packed byte
  boolean get (byte index);
  void set (byte index, boolean data);
  boolean bit0:1;
  boolean bit1:1;
  boolean bit2:1;
  boolean bit3:1;
  boolean bit4:1;
  boolean bit5:1;
  boolean bit6:1;
  boolean bit7:1;
};

struct __attribute__ ((packed)) MicroLinn16bits {    // packed short
  boolean get (byte index);
  void set (byte index, boolean data);
  boolean bit0:1;
  boolean bit1:1;
  boolean bit2:1;
  boolean bit3:1;
  boolean bit4:1;
  boolean bit5:1;
  boolean bit6:1;
  boolean bit7:1;
  boolean bit8:1;
  boolean bit9:1;
  boolean bit10:1;
  boolean bit11:1;
  boolean bit12:1;
  boolean bit13:1;
  boolean bit14:1;
  boolean bit15:1;
};

byte MicroLinn2nybbles::get(byte index) {
  if ((index & 1) == 0) return nybble1; 
  return nybble2;
}

void MicroLinn2nybbles::set(byte index, byte data) {
  if ((index & 1) == 0) nybble1 = data;
  else                  nybble2 = data;
}

signed char MicroLinn2signedNybbles::get(byte index) {
  if ((index & 1) == 0) return nybble1;
  return nybble2;
}

void MicroLinn2signedNybbles::set(byte index, signed char data) {
  if ((index & 1) == 0) nybble1 = data;
  else                  nybble2 = data;
}

boolean MicroLinn8bits::get(byte index) {
  switch(index & 7) {
    case 0: return bit0;
    case 1: return bit1;
    case 2: return bit2;
    case 3: return bit3;
    case 4: return bit4;
    case 5: return bit5;
    case 6: return bit6;
    case 7: return bit7;
    default: return false;
  }
}

void MicroLinn8bits::set(byte index, boolean data) {
  switch(index & 7) {
    case 0: bit0 = data;
    case 1: bit1 = data;
    case 2: bit2 = data;
    case 3: bit3 = data;
    case 4: bit4 = data;
    case 5: bit5 = data;
    case 6: bit6 = data;
    case 7: bit7 = data;
  }
}

boolean MicroLinn16bits::get(byte index) {
  switch(index & 15) {
    case 0:  return bit0;
    case 1:  return bit1;
    case 2:  return bit2;
    case 3:  return bit3;
    case 4:  return bit4;
    case 5:  return bit5;
    case 6:  return bit6;
    case 7:  return bit7;
    case 8:  return bit8;
    case 9:  return bit9;
    case 10: return bit10;
    case 11: return bit11;
    case 12: return bit12;
    case 13: return bit13;
    case 14: return bit14;
    case 15: return bit15;
    default: return false;
  }
}

void MicroLinn16bits::set(byte index, boolean data) {
  switch(index & 15) {
    case 0:  bit0  = data;
    case 1:  bit1  = data;
    case 2:  bit2  = data;
    case 3:  bit3  = data;
    case 4:  bit4  = data;
    case 5:  bit5  = data;
    case 6:  bit6  = data;
    case 7:  bit7  = data;
    case 8:  bit8  = data;
    case 9:  bit9  = data;
    case 10: bit10 = data;
    case 11: bit11 = data;
    case 12: bit12 = data;
    case 13: bit13 = data;
    case 14: bit14 = data;
    case 15: bit15 = data;
  }
}
*********************/

/*********************
// unused, delete?
boolean get(byte array[], short n) {                        // read from the nth boolean of a packed array of booleans
  return (array[n >> 3] & (1 << (n & 7))) != 0;
}

// unused, delete?
void set(byte array[], short n, boolean value) {            // write to the nth boolean of a packed array of booleans
  if (value) {
    array[n >> 3] |= 1 << (n & 7);
  } else {
    array[n >> 3] &= 255 - (1 << (n & 7));
  }
}
****************************/

short microLinnTriIndex(byte edo, short edostep) {     // index into a triangular array
  edostep = microLinnMod (edostep, edo);
  return (edo * (edo - 1)) / 2 + edostep - 10;         // -10 to skip over edos 1-4
}

byte microLinnDots(byte edo, short fret) {
  return Device.microLinn.fretboards[microLinnTriIndex(edo, fret)];
}

byte microLinnRainbowColor(byte edo, short edostep) {
  return Device.microLinn.rainbows[microLinnTriIndex(edo, edostep)];
}

/*************************
// not yet used, delete microLinnRainbowColor()
byte microLinnGetRainbowColor(byte edo, byte edostep) {
  short index = microLinnTriIndex(edo, edostep);
  byte twoColors = Device.microLinn.rainbows[index >> 1];
  if (index % 2 == 0) return twoColors & 15; 
  return twoColors >> 4;
}
//not yet used
void microLinnSetRainbowColor(byte edo, byte edostep, byte color) {
  short index = microLinnTriIndex(edo, edostep);
  byte twoColors = Device.microLinn.rainbows[index >> 1];
  twoColors = index % 2 == 0 ? (twoColors & 240) | color : (twoColors & 15) | (color << 4);
  Device.microLinn.rainbows[index >> 1] = twoColors;
}
****************************/

boolean microLinnIsScaleStep(byte edo, short edostep) {
  return microLinnIsScaleStep(edo, edostep, Global.activeNotes);
}

boolean microLinnIsScaleStep(byte edo, short edostep, byte scale) {
  if (scale == 7) return true;         // full rainbow
  if (scale >= 8) return false;        // fretboard or custom LED pattern
  return Device.microLinn.scales[microLinnTriIndex(edo, edostep)] & (1 << scale);
}

byte microLinnGetFret(byte side, byte col) {
  // returns the distance from the "nut" of the dot pattern to the column, octave-reduced
  short fret = (isLeftHandedSplit(LEFT) && isLeftHandedSplit(RIGHT)) ? NUMCOLS - col : col;
  signed char direction = isLeftHandedSplit(side) ? -1 : 1;
  fret -= direction * Split[side].transposeLights;                                     // transposeLights shifts by columns not edosteps
  byte modBase = (edo == 41 && Split[side].microLinn.colOffset == 2 ? 12 : edo);       // 12 because 41edo kites repeat every 12 frets
  return microLinnMod (fret - 1, modBase);
}

/************** lookup functions called from other .ino files ************************/

boolean isLinn200() {          // only call this function for text displays! not calibration, scan order, Z-bias, etc.!
//return false;                // uncomment this line to test linn128 text displays on a linn200
  return LINNMODEL == 200;
}

boolean isMicroLinnOn() {
  return Global.microLinn.EDO >= 5;
}

boolean isMicroLinnXfixOn() {
  if (Global.microLinn.drumPadMode > 0) return false;
  // include hammer-ons since they are so similar
  return Split[sensorSplit].microLinn.monoFixes % 2 == 1 || isMicroLinnHammeringOn();
}

boolean isMicroLinnZmaxOn() {
  return Split[sensorSplit].microLinn.monoFixes >= 2;
}

boolean isMicroLinnHammeringOn() {
  if (displayMode != displayNormal) return false;
  if (Split[sensorSplit].ccFaders  || Split[sensorSplit].arpeggiator ||
      isStrummedSplit(sensorSplit) || Split[sensorSplit].sequencer) return false;
  if (Global.microLinn.drumPadMode > 0) return false;
  if (Split[sensorSplit].midiMode == oneChannel) return false;
  return Split[sensorSplit].microLinn.hammerOnMode > 0;
}

boolean isMicroLinnImportingOn() {
  return microLinnImportingOn;
}

// called by scalePitch() in ls_midi.ino
boolean isMicroLinnColOffsetOn(byte side) {
  return Split[side].microLinn.colOffset != 1;
}

// a cell's edostep = the distance in edosteps from the anchor cell, plus transposition
// a cell's virtual edostep = the distance in edosteps from the cell with the lowest pitch, not transposed
// virtual edostep is always >= 0, virtual edostep = edostep - microLinnLowestEdostep[side]
// the lowest edostep is usually in one of the corners, but in guitar tuning it can be in any row of col 1 or 25
// there are often more than 128 edosteps, so midi notes come in groups of 128, up to 8 groups for a total of 1024 notes
// VirtualTouchInfo, StepEventState have note as a short 0..1023 plus -1 for no note
// TouchInfo, VirtualTouchInfo have note as a signed char -1..127, also have microLinnGroup 0..7
// all the sequencer functions treat the note as a short because getNote and setNote unpack data[0] and data[5]

// to do: ls_arpeggiator.ino: playingArpNote, stepArpNote, also low row strumming, restrike

// called from transposedNote(), getNoteNumber() and determineRowOffsetNote() in ls_handleTouches.ino
//    transposedNote() is called from handleTouches/cellTransposedNote() and handleStrummedOpenRow()
//       (also paintNormalDisplayCell(), but only when microLinn is off)
//    getNoteNumber() is called from handleTouches/transposedNote()
//       and StepSequencerState::findNoteSequencerCoordinate() and ::getSensorNotesNoteNum()
//       (also paintNormalDisplayCell(), but only when microLinn is off)
//    determineRowOffsetNote() is called from midi/getNoteNumColumn()
//       (also getNoteNumber(), but only when microLinn is off)
short getMicroLinnVirtualEdostep(byte side, byte col, byte row) {
  return microLinnEdostep[side][col][row] - microLinnLowestEdostep[side];   // both are transposed so transposing cancels out
}                                                                           // even a dead pad returns a number >= 0

void applySameAndMicroLinnBlinkToOtherSplit(boolean calledOnNoteOn, short note) {
  if (!Global.splitActive) return;
  if (!Global.microLinn.dotsCarryOver) return;
  byte otherSide = sensorSplit - 1;
  if (Split[otherSide].playedTouchMode != playedSame &&
      Split[otherSide].playedTouchMode != playedBlink) return;
  if (Split[otherSide].ccFaders || Split[otherSide].strum || Split[otherSide].sequencer) return;

  if (calledOnNoteOn) {
    highlightPossibleNoteCells(otherSide, note);
  }
  else resetPossibleNoteCells(otherSide, note);            // if called on noteOff
}

/****************************************
// not yet used. possible usage: in ls_midi.ino, line 1789, 1st line of getNoteNumColumn()
//  if (isMicroLinnOn()) return getMicroLinnNoteNumColumn(split, notenum, sensorCell->microLinnGroup, row);

byte getMicroLinnNoteNumColumn(byte side, byte note, byte group, byte row) {
  return getMicroLinnNoteNumColumn(side, note + (group << 7), row);
}

byte getMicroLinnNoteNumColumn(byte side, short edostep, byte row) {
  if (edostep < 0) return -1;
  for (byte col = 1; col < NUMCOLS; ++col) {
    if (microLinnEdostep[side][col][row] == edostep) return col;
  }
  return -1;
}
******************************************/

// called by handleXYZupdate(), sendNewNote(), sendReleasedNote() and handleTouchRelease() in ls_handleTouches.ino
signed char getMicroLinnMidiNote(byte side, signed char note, byte group) {
  if (note < 0) return -1;
  return getMicroLinnMidiNote(side, note + (group << 7));
}

// called by StepEventState::sendNoteOn() and StepEventState::sendNoteOff() in ls_sequencer.ino and getOctaveNote() in ls_arpeggiator.ino
signed char getMicroLinnMidiNote(byte side, short edostep) {
  if (edostep < 0) return -1;
  if (Split[side].microLinn.tuningTable > 0) {
    if (edostep > 127 && Split[side].microLinn.tuningTable == 1) return -1;       // bug: should this be transposed?
    // discard higher bits, the DAW should use midi grouping CCs or rechannelling to deduce them
    return microLinnMod(edostep + microLinnTransposition(sensorSplit), 128);      // bug: should this be transposed?
  }
  return microLinnMidiNote[side][edostep];
}

// called by handleXYZupdate(), sendNewNote(), sendReleasedNote() and handleTouchRelease() in ls_handleTouches.ino
signed char getMicroLinnDrumPadMidiNote() {
  byte wid = Global.microLinn.drumPadMode + 1;
  if (sensorCol < 2 || sensorCol > 7*wid+1 || sensorRow < 1 || sensorRow > 6) return -1;
  byte padNum = floor((sensorCol - 2) / wid) + 7 * floor(sensorRow / 4);
  return Project.sequencer[sensorSplit].seqDrumNotes[padNum];
}

// called by handleXYZupdate(), sendNewNote() and handleTouchRelease() in ls_handleTouches.ino
short getMicroLinnTuningBend(byte side, signed char note, byte group) {
  if (note < 0 || Split[side].microLinn.tuningTable > 0) return 0;
  return microLinnTuningBend[side][note + (group << 7)];
}
short getMicroLinnTuningBend2(byte side, signed char note, byte group) {
  // first draft of implementing sweetening, not yet used
  if (note < 0 || Split[side].microLinn.tuningTable > 0) return 0;
  short edostep = note + (group << 7);
  short bend = microLinnTuningBend[side][edostep];
  if (edo == 41) {
    //if (microLinnPull[edostep].sharp && !microLinnPull[edostep].flat) bend += microLinnSweetener[side];
    //if (microLinnPull[edostep].flat && !microLinnPull[edostep].sharp) bend -= microLinnSweetener[side];
  }
  return bend;
}

// called by StepEventState::sendNoteOn() in ls_sequencer.ino and getOctaveNote() in ls_arpeggiator.ino
short getMicroLinnTuningBend(byte side, short edostep) {
  if (edostep < 0 || Split[side].microLinn.tuningTable > 0) return 0;
  return microLinnTuningBend[side][edostep];
}

// called by scalePitch() in ls_midi.ino
float getMicroLinnSemitonesPerPad(byte side, int pitchValue) {
  if (Split[side].microLinn.condensedBendPerPad != 1) return microLinnSemitonesPerPad[side];
  // calc the scaling factor for the variable bend slopes used with condensed scales
  float pads = getBendRange(side) * abs(pitchValue) / 8192.0;                // the physical length of the slide in pads
  float pads2 = pads;                                                        // a copy we can destructively edit
  short octaves = floor (pads2 / microLinnNumNotes);
  pads2 -= octaves * microLinnNumNotes;
  short steps = floor (pads2);
  pads2 -= steps;
  short startingStep = microLinnEdostep[side][sensorCell->initialColumn][sensorRow];     // edosteps from the anchor cell
  startingStep = microLinnScaleLookup[microLinnMod(startingStep, edo)];                  // convert to scale steps from tonic
  short edostepsLo, edostepsHi;
  if (pitchValue >= 0) {
    edostepsLo =  microLinnScaleStepsToEdoSteps( steps,   startingStep);
    edostepsHi =  microLinnScaleStepsToEdoSteps( steps+1, startingStep);
  } else {
    edostepsLo = -microLinnScaleStepsToEdoSteps(-steps,   startingStep);
    edostepsHi = -microLinnScaleStepsToEdoSteps(-steps-1, startingStep);
  }
  float edosteps = octaves * edo + edostepsLo + pads2 * (edostepsHi - edostepsLo);       // interpolate between hi and lo
  float semitones = edosteps * edostepSize;
  return Split[side].microLinn.colOffset * semitones / pads;
}

/********************** OLD WAY, should work identically, but seemed inaccurate on descending slides, delete?
float getMicroLinnSemitonesPerPad_OLD (byte side, int pitchValue) {
  if (Split[side].microLinn.condensedBendPerPad != 1) return microLinnSemitonesPerPad[side];
  // calc the scaling factor for the variable bend slopes used with condensed scales
  float pads = getBendRange(side) * pitchValue / 8192.0;                // the physical length of the slide in pads
  float pads2 = pads;                                                   // a copy we can destructively edit
  short octaves = floor (pads2 / microLinnNumNotes);
  pads2 -= octaves * microLinnNumNotes;
  short steps = floor (pads2);
  pads2 -= steps;
  short startingStep = microLinnEdostep[side][sensorCell->initialColumn][sensorRow];     // edosteps from the anchor cell
  startingStep = microLinnScaleLookup[microLinnMod(startingStep, edo)];                  // convert to scale steps from tonic
  short edostepsLo = microLinnScaleStepsToEdoSteps(steps,   startingStep);
  short edostepsHi = microLinnScaleStepsToEdoSteps(steps+1, startingStep);
  float edosteps = octaves * edo + edostepsLo + pads2 * (edostepsHi - edostepsLo);       // interpolate between hi and lo
  float semitones = edosteps * edostepSize;
  return Split[side].microLinn.colOffset * abs(semitones / pads);
}************************************/

// called by determineRowOffsetNote() in ls_handleTouches.ino, duplicates the original determineRowOffsetNote function 
short getMicroLinnRowOffsetNote(byte side, byte row) {
  signed char offset = Split[side].microLinn.rowOffset;
  if (offset <= -12) return 18 + (row - 7) * offset;      // ensure upper left cell is 18 = F#-1 (middle-C is C3)
  if (offset < 0)    return 65 + row * offset;            // 65 = F3 = 1st string of an all-4ths guitar
  if (offset < 12)   return 30 + row * offset;            // 30 = F#0 = 8th string of a guitar
  if (offset <= 25)  return 18 + row * offset;            // 18 = F#-1 = 8th string of a guitar, down an octave
  offset = microLinnGetSplitWidth(side);                  // row offset must be NOVR = no overlap
  if (Global.splitActive && side == RIGHT) {
    return 30 - (Global.splitPoint - 1) + row * offset;   // ensure right split's lower left cell is 30 = F#0
  }
  return 30 + row * offset;
}

// called by sendNewNote() and sendReleasedNote() in ls_handleTouches.ino and getOctaveNote() in ls_arpeggiator.ino
// also StepEventState::sendNoteOn() and StepEventState::sendNoteOff() in ls_sequencer.ino
// if there's more than 128 virtual edosteps, send higher midi groups to other channel blocks
// this gives every pad a unique combo of midi note and midi channel
signed char rechannelMicroLinnGroup(byte side, signed char chan, byte group) {
  if (group == 0) return chan;
  if (Split[side].microLinn.tuningTable < 3) return chan;
  if (side == RIGHT) group *= -1;                         // right split adds new blocks below not above
  chan += group * microLinnChannelCount[side];
  if (chan < 1 || chan > 16) return -1;                   // bad channel = dead pad
  return chan;
}

// called by showPerNoteMidiChannels() in ls_displayModes.ino
void showMicroLinnExtraMidiChannels(byte side) {
  if (!isMicroLinnOn()) return;
  if (Split[side].microLinn.tuningTable < 3) return;
  byte n = microLinnChannelCount[side];
  byte numExtraChannels = n * (microLinnNumGroups[side] - 1);
  byte firstChannel = (side == LEFT ? 2 + n : 16 - n - numExtraChannels);
  for (byte chan = firstChannel; chan < firstChannel + numExtraChannels; ++chan) {
    if (!Split[side].midiChanSet[chan-1]) setMidiChannelLed(chan, Split[side].colorAccent);
  }
}

// called by setMidiChannelLed() in ls_displayModes.ino
boolean isMicroLinnDupeChannel(byte chan) {                  // chan runs 1-16
  boolean isUsed;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    isUsed  = Split[side].midiMode == 0 || Split[side].midiChanMainEnabled;
    isUsed &= Split[side].midiChanMain == chan;
    isUsed |= Split[side].midiMode == 1 && Split[side].midiChanSet[chan - 1];
    isUsed |= Split[side].midiMode == 2 && (chan - Split[side].midiChanPerRow + 16) % 16 < 8;
    if (!isUsed) return false;
  }
  return true;
}

// called by StepEvent::getFaderMax() in ls_sequencer.ino
byte getMicroLinnSequencerBendRange() {
  if (edo >= 12) return 50;
  return ceil(600.0 / edo);                               // larger edos have a smaller bend range
}

void setMidiChannelSelect() {
  // show the user the channels that are actually in use, user can override this by tapping Split Settings col 2
  midiChannelSelect = 7 - Split[Global.currentPerSplit].midiMode;
}

/**
// called by takeChannel() in ls_handleTouches.ino
signed char prepareMicroLinnHammerOnOLD(byte side, byte row) {
  // look for a touched pad on sensorRow within maxCols of sensorCol, starting 1 col away, then 2 cols, etc.
  // if found, temporarily mute that pad with a noteOff, which has the same velocity as the new noteOn
  // return that pad's channel so the new note can steal it (unless in ChanPerRow mode)
  // keep a list of the muted notes, because they can be hard to find later because of slides
  if (Split[side].ccFaders  || Split[side].arpeggiator ||
      isStrummedSplit(side) || Split[side].sequencer) return -1;
  if (Global.microLinn.drumPadMode > 0) return -1;
  if (displayMode != displayNormal) return -1;

  short sensorEdostep = microLinnEdostep[side][sensorCol][row];
  short edostep;
  short note = -1;
  signed char channel = -1;
  signed char ch;
  // maxCols is the range to search in, condensing decreases this, so this is sometimes overly large
  short maxCols = microLinnHammerOnEdosteps[side] / Split[side].microLinn.colOffset;
  unsigned short wait = 10 * Split[side].microLinn.hammerOnWait;
  int32_t colsInRowTouched = colsInRowsTouched[row];
  if (Global.splitActive) {
    if (side == LEFT)  colsInRowTouched &=  ((int32_t)(1 << Global.splitPoint) - 1);              // exclude the other split
    if (side == RIGHT) colsInRowTouched &= ~((int32_t)(1 << Global.splitPoint) - 1);
  }

  byte col = 1;
  while (channel == -1 && col <= maxCols) {

    // search to the right for older notes for leftwards hammer-ons TO the sensorCell
    edostep = microLinnEdostep[side][sensorCol + col][row];
    if (Split[sensorSplit].microLinn.hammerOnMode >= 2 &&
        colsInRowTouched & (1 << (sensorCol + col)) && (sensorCol + col) < NUMCOLS &&
        abs(edostep - sensorEdostep) <= microLinnHammerOnEdosteps[side] &&
        calcTimeDelta(millis(), touchInfo[sensorCol + col][row].lastTouch) > wait) {
      note = touchInfo[sensorCol + col][row].note;            // + (touchInfo[sensorCol + col][row].microLinnGroup << 7);
      channel = ch = touchInfo[sensorCol + col][row].channel;
      microLinnHammerOns[microLinnNumHammerOns].note = note;
      microLinnHammerOns[microLinnNumHammerOns].channel = channel;
      microLinnNumHammerOns += 1;
      if (isMicroLinnOn()) {
        note = getMicroLinnMidiNote(side, note, sensorCell->microLinnGroup);         // bug delete group
        ch = rechannelMicroLinnGroup(side, ch, sensorCell->microLinnGroup);
      }
      midiSendNoteOffWithVelocity(side, note, sensorCell->velocity, ch);
    }

    // search to the left the same way
    edostep = microLinnEdostep[side][sensorCol - col][row];
    if (Split[sensorSplit].microLinn.hammerOnMode % 2 == 1 &&
        colsInRowTouched & (1 << (sensorCol - col)) && (sensorCol - col) > 0 &&
        abs(edostep - sensorEdostep) <= microLinnHammerOnEdosteps[side] &&
        calcTimeDelta(millis(), touchInfo[sensorCol - col][row].lastTouch) > wait) {
      note = touchInfo[sensorCol - col][row].note;   // + (touchInfo[sensorCol - col][row].microLinnGroup << 7);
      // if no pad was found on the right, proceed the same way
      if (channel == -1) { 
        channel = touchInfo[sensorCol - col][row].channel;
        microLinnHammerOns[microLinnNumHammerOns].note = note;
        microLinnHammerOns[microLinnNumHammerOns].channel = channel;
        microLinnNumHammerOns += 1;
      }
      // else sensorCell is equidistant between 2 sounding pads and in the zones of each,
      // but the 2 sounding pads are not in each other's zones and have different channels
      // mute them both and steal the channel from the more recent pad
      else if (calcTimeDelta(millis(), touchInfo[sensorCol - col][row].lastTouch) <
               calcTimeDelta(millis(), touchInfo[sensorCol + col][row].lastTouch)) {
        channel = touchInfo[sensorCol - col][row].channel;
        microLinnHammerOns[microLinnNumHammerOns - 1].note = note;                 // subtract 1 to overwrite the righthand data
        microLinnHammerOns[microLinnNumHammerOns - 1].channel = channel;
      }
      ch = touchInfo[sensorCol - col][row].channel;
      if (isMicroLinnOn()) {
        note = getMicroLinnMidiNote(side, note, sensorCell->microLinnGroup);
        ch = rechannelMicroLinnGroup(side, ch, sensorCell->microLinnGroup);
      }
      midiSendNoteOffWithVelocity(side, note, sensorCell->velocity, ch);
    }

    ++col;
  }

  return channel;
}
*/


// called by takeChannel() in ls_handleTouches.ino
signed char prepareMicroLinnHammerOn(byte side, byte row) {
  // look for touched pads on sensorRow within maxCols of sensorCol
  // temporarily mute those pads with a noteOff, which has the same velocity as the new noteOn (which is sent later)
  // return that pad's channel so if in ChanPerNote mode, the new note can steal it
  // keep a list of the muted notes, because they can be hard to find later because of slides
  short maxEdosteps = microLinnHammerOnEdosteps[side];
  if (Split[side].microLinn.hammerOnZone == 121) maxEdosteps = SHRT_MAX;                          // 121 = ALL
  short sensorEdostep = microLinnEdostep[side][sensorCol][row];
  unsigned short wait = 10 * Split[side].microLinn.hammerOnWait;

  // int32_t colsInRowTouched = colsInRowsTouched[row];
  // colsInRowTouched = colsInRowTouched & ~(1 << sensorCol);                                        // exclude the current cell
  // if (Global.splitActive) {
  //   if (side == LEFT)  colsInRowTouched &=  ((int32_t)(1 << Global.splitPoint) - 1);              // exclude the other split
  //   if (side == RIGHT) colsInRowTouched &= ~((int32_t)(1 << Global.splitPoint) - 1);
  // }

  // maxCols is the range to search in, condensing decreases this, so maxCols is sometimes overly large
  short maxCols = maxEdosteps / Split[side].microLinn.colOffset;
  // hammering on to the right means searching to the left of the hammering note for notes to be muted
  signed char startCol = (Split[sensorSplit].microLinn.hammerOnMode % 2 == 1) ? sensorCol - maxCols : sensorCol + 1;
  signed char   endCol = (Split[sensorSplit].microLinn.hammerOnMode >= 2)     ? sensorCol + maxCols : sensorCol - 1;
  // bug, should constrain to current split
  startCol = constrain(startCol, 1, NUMCOLS-1);
    endCol = constrain(  endCol, 1, NUMCOLS-1);

  // if play a note in between 2 notes more than a zone apart but less than 2 zones apart, it mutes both but steals only 1 channel
  // the 1st pass finds the latest valid touch and what channel it's on, this is the channel to steal
  unsigned long lastTouch;
  unsigned long latestTouchSoFar = 0;
  signed char latestChannel = -1;
  for (byte col = startCol; col <= endCol; col++) {
    lastTouch = touchInfo[col][row].lastTouch;
    if (touchInfo[col][row].touched == touchedCell &&
        calcTimeDelta(millis(), lastTouch) > wait &&
        lastTouch > latestTouchSoFar &&
        //colsInRowTouched & (1 << (col)) &&
        abs(microLinnEdostep[side][col][row] - sensorEdostep) <= maxEdosteps) {
      latestTouchSoFar = lastTouch;
      latestChannel = touchInfo[col][row].channel;
    }
  }
  if (latestChannel == -1) return -1;

  // the 2nd pass mutes up to 2 notes and keeps a list of notes on the latest channel for later un-muting
  short note = -1;
  signed char channel = -1;
  for (byte col = startCol; col <= endCol; col++) {
    if (touchInfo[col][row].touched == touchedCell &&
        //colsInRowTouched & (1 << (col)) &&
        calcTimeDelta(millis(), touchInfo[col][row].lastTouch) > wait &&
        abs(microLinnEdostep[side][col][row] - sensorEdostep) <= maxEdosteps) {
      touchInfo[col][row].touched = ignoredCell;
      note = touchInfo[col][row].note;            // + (touchInfo[col][row].microLinnGroup << 7);
      channel = touchInfo[col][row].channel;
      { //if (channel == latestChannel) {
        microLinnHammerOns[microLinnNumHammerOns].note = note;
        microLinnHammerOns[microLinnNumHammerOns].channel = channel;
        microLinnHammerOns[microLinnNumHammerOns].col = col;
        microLinnHammerOns[microLinnNumHammerOns].row = row;
        microLinnNumHammerOns += 1;
        DEBUGPRINT((0,"prepareMicroLinnHammerOn"));
        DEBUGPRINT((0,"  add note="));              DEBUGPRINT((0,(int)note));
        DEBUGPRINT((0,"  microLinnNumHammerOns=")); DEBUGPRINT((0,(int)microLinnNumHammerOns));
        DEBUGPRINT((0,"\n"));      
      }
      resetPossibleNoteCells (side, note);
      if (isMicroLinnOn()) {
        note = getMicroLinnMidiNote(side, note, sensorCell->microLinnGroup);         // bug delete group
        channel = rechannelMicroLinnGroup(side, channel, sensorCell->microLinnGroup);
      }
      midiSendNoteOffWithVelocity(side, note, sensorCell->velocity, channel);
      //setLed(col, row, COLOR_OFF, cellOff, LED_LAYER_PLAYED);
    }
  }
  return channel;
}

/*************
ideas:
in R or L mode, pull-off to closest note, acts more like a guitar string
but in R+L mode, pull-off to latest note in the zone
if you set zone size = ALL, there's only 1 note per string, like ChanPerRow mode and a mono synth w highest note priority
send note-off just after note-on, for single-trigger synths? hard to do
OFF, R, R', L, L', R+L
R  = nearby notes to the left don't sound (priority = highest)
R' = nearby notes to the left do sound (priority = newest)
use midiSendNoteOffForAllTouches() code to find notes in the zone

tests:
slide and then hammer
play C, slide up to D, hammer-on C, release C and D in either order
play C C# D (or B C D) in all 6 orders and release them in all 6 orders:   123 132 213  231 312 321

bugs: 


    // handle taking over an already active touch
    for (byte col = lowCol; col < highCol; ++col) {
      if (col != sensorCol && cell(col, 0).velocity) {
        transferFromSameRowCell(col);
        return;
      }
    }

    // handle taking over an already active touch, the highest already active touch wins
    for (byte col = highCol-1; col >= lowCol; --col) {
      if (col != sensorCol && cell(col, 0).touched == touchedCell) {
        transferToSameRowCell(col);
        return;
      }
    }    

****************/

void microLinnDeleteHammeredNote (byte i) {
  microLinnNumHammerOns -= 1;
  memcpy(&microLinnHammerOns[i], &microLinnHammerOns[i + 1], 4 * (microLinnNumHammerOns - i));
  microLinnHammerOns[microLinnNumHammerOns].note = -1;
  microLinnHammerOns[microLinnNumHammerOns].channel = -1;
  microLinnHammerOns[microLinnNumHammerOns].col = -1;
  microLinnHammerOns[microLinnNumHammerOns].row = -1;
}

// called by sendReleasedNote() in ls_handleTouches.ino
void sendMicroLinnPullOff() {
  // if the note is focused = not muted, send a noteOff and send a pull-off noteOn for the most recent note
  // if the note is not focused = muted, don't send a noteOff or a pull-off and delete it from the list
  signed char note = sensorCell->note;
  signed char channel = sensorCell->channel;

  if (!isFocusedCell()) {
    for (signed char i = microLinnNumHammerOns - 1; i >= 0; --i) {
      if (microLinnHammerOns[i].note == note && 
          microLinnHammerOns[i].channel == channel) {
        microLinnDeleteHammeredNote(i);
        DEBUGPRINT((0,"sendMicroLinnPullOff"));
        DEBUGPRINT((0,"  unfocused delete i="));    DEBUGPRINT((0,(int)i));
        DEBUGPRINT((0,"  microLinnNumHammerOns=")); DEBUGPRINT((0,(int)microLinnNumHammerOns));
        DEBUGPRINT((0,"\n"));
        break;
      }
    }
    return;
  }

  if (isMicroLinnOn()) {
    note = getMicroLinnMidiNote(sensorSplit, note, sensorCell->microLinnGroup);
    channel = rechannelMicroLinnGroup(sensorSplit, channel, sensorCell->microLinnGroup);
    if (note == -1 || channel == -1) return;
  }
  midiSendNoteOffWithVelocity(sensorSplit, note, sensorCell->velocity, channel);

  // send the pull-off note, which is a note-on for the old note, using the released note's noteOff velocity
  channel = sensorCell->channel;
  for (signed char i = microLinnNumHammerOns - 1; i >= 0; --i) {
    // find the most recently muted pad on this channel (the other hand might be doing hammer-ons on another channel)
    signed char col = microLinnHammerOns[i].col;
    signed char row = microLinnHammerOns[i].row;
    if (microLinnHammerOns[i].channel == channel && touchInfo[col][row].touched == ignoredCell) {
      touchInfo[col][row].touched = touchedCell;
      //focus(sensorSplit, channel).col = col;                      // reassign focus to this cell
      //focus(sensorSplit, channel).row = row;
      note = microLinnHammerOns[i].note;
      highlightPossibleNoteCells(sensorSplit, note);
      if (isMicroLinnOn()) {
        note = getMicroLinnMidiNote(sensorSplit, note, sensorCell->microLinnGroup);
        channel = rechannelMicroLinnGroup(sensorSplit, channel, sensorCell->microLinnGroup);
      }
      midiSendNoteOn(sensorSplit, note, sensorCell->velocity, channel);
      microLinnDeleteHammeredNote(i);
      DEBUGPRINT((0,"sendMicroLinnPullOff"));
      DEBUGPRINT((0,"    focused delete i="));    DEBUGPRINT((0,(int)i));
      DEBUGPRINT((0,"  microLinnNumHammerOns=")); DEBUGPRINT((0,(int)microLinnNumHammerOns));
      DEBUGPRINT((0,"\n"));
      break;
    }
  }
}

/********** maybe not needed, delete? 
FocusCell microLinnFIndEdostep(byte side, short edostep) {
  byte row = 0;
  boolean success = false;
  while (!success && row < NUMROWS) {
    short col = edostep - microLinnEdostep[side][1][row];
    if (col % Split[side].microLinn.colOffset == 0) {                              // if this row doesn't skip this note
      col = 1.0 * col / Split[side].microLinn.colOffset + 1;
      success = (col > 0 && col < NUMCOLS);
    }
    if (!success) ++row;
  }
  FocusCell c;
  c.col = success ? col : 0; c.row = row;
  return c;
} **********************/

/************** initialization functions ************************/

void initializeMicroLinn() {
  // called in reset(), runs when microLinn firmware is first installed or when the user does a global reset
  // DO NOT initialize Device.microLinn.uninstall here, because the updater app seems to re-initialize it before using it
  microLinnImportingOn = false;
  Device.microLinn.MLversion = 1;     // the 1 in 72.1
  memcpy (&Device.microLinn.scales,   &MICROLINN_SCALES,   MICROLINN_ARRAY_SIZE);
  memcpy (&Device.microLinn.rainbows, &MICROLINN_RAINBOWS, MICROLINN_ARRAY_SIZE);
  for (byte EDO = 5; EDO <= MICROLINN_MAX_EDO; ++EDO) microLinnResetFretboard(EDO);

  Global.microLinn.drumPadMode = 0;
  Global.microLinn.dotsCarryOver = false;
  Global.microLinn.locatorCC1 = -1;
  Global.microLinn.locatorCC2 = -1;
  Global.microLinn.EDO = 4;                       // 4 = OFF
  memset(&Global.microLinn.rainbow,   0, MICROLINN_MAX_EDO);
  memset(&Global.microLinn.fretboard, 0, MICROLINN_MAX_EDO);
  Global.microLinn.useRainbow = true;
  Global.microLinn.anchorCol = 11;
  Global.microLinn.anchorRow = 4;                 // 4th row from the top
  Global.microLinn.anchorNote = 60;               // middle-C
  Global.microLinn.anchorCents = 0;
  Global.microLinn.equaveSemitones = 12;
  Global.microLinn.equaveStretch = 0;
  Global.microLinn.sweeten = 0; 
  Global.microLinn.largeEDO = 0;
//memset(&Global.microLinn.largeEDOoffset, 0, sizeof(Global.microLinn.largeEDOoffset)); uncomment later
  Global.microLinn.guitarTuning[0] = 0;           // default to DIA not CHRO
  Global.microLinn.guitarTuning[1] = 5;           // 12edo F# B E A D G B E
  Global.microLinn.guitarTuning[2] = 5;           // row offset from the string below it
  Global.microLinn.guitarTuning[3] = 5;
  Global.microLinn.guitarTuning[4] = 5;
  Global.microLinn.guitarTuning[5] = 5;
  Global.microLinn.guitarTuning[6] = 4;
  Global.microLinn.guitarTuning[7] = 5;

  for (byte side = 0; side < NUMSPLITS; ++side) {
    Split[side].microLinn.colOffset = 1;
    Split[side].microLinn.rowOffset = -MICROLINN_MAX_ROW_OFFSET - 1;       // -26 = OFF
    Split[side].microLinn.monoFixes = 0;
    Split[side].microLinn.hammerOnMode = 0;
    Split[side].microLinn.hammerOnZone = 10;
    Split[side].microLinn.hammerOnWait = 0;
    Split[side].microLinn.showCustomLEDs = 0;
    Split[side].microLinn.condensedBendPerPad = 0;
    Split[side].microLinn.defaultLayout = 0;
    Split[side].microLinn.tuningTable = 0;
    Split[side].microLinn.midiGroupCC = -1;
    Split[side].microLinn.transposeEDOsteps = 0;
    Split[side].microLinn.ccForLowRowW = 255;
    Split[side].microLinn.ccForLowRowX = 255;
    Split[side].microLinn.ccForLowRowY = 255;
    Split[side].microLinn.flags = 0;                        // prev sendX/quant/hold = OFF, joystick W=0, X=0, Y=0
  }

  for (byte p = 0; p < NUMPRESETS; ++p) {
    memcpy (&config.preset[p].global.microLinn,       &Global.microLinn,       sizeof(MicroLinnGlobal));
    memcpy (&config.preset[p].split[LEFT].microLinn,  &Split[LEFT].microLinn,  sizeof(MicroLinnSplit));
    memcpy (&config.preset[p].split[RIGHT].microLinn, &Split[RIGHT].microLinn, sizeof(MicroLinnSplit));
  }

  // preset 5 (5th from the top) is 41edo Kite guitar (the presets run 3 2 1 0 5 4)
  byte p = 5;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    config.preset[p].split[side].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
    config.preset[p].split[side].bendRangeOption = bendRange24;
    config.preset[p].split[side].customBendRange = 24;
    config.preset[p].split[side].expressionForZ = loudnessChannelPressure;
    config.preset[p].split[side].playedTouchMode = playedBlink;
  }
  config.preset[p].split[LEFT].midiChanMain = 1;
  config.preset[p].split[LEFT].midiChanSet[0] = false;
  config.preset[p].split[RIGHT].midiChanMain = 16;
  config.preset[p].split[RIGHT].midiChanSet[15] = false;
  config.preset[p].global.microLinn.EDO = 41;
  short i = microLinnTriIndex(41, 0); 
  memcpy(&config.preset[p].global.microLinn.rainbow,   &Device.microLinn.rainbows[i],   41); 
  memcpy(&config.preset[p].global.microLinn.fretboard, &Device.microLinn.fretboards[i], 41); 
  config.preset[p].global.microLinn.anchorRow = 3;                // 5th row from top, places the two rainbows nicely
  config.preset[p].global.microLinn.anchorCol = 6;
  config.preset[p].global.microLinn.anchorNote = 54;              // ^F#2, Kite guitar standard tuning with D = 0 cents
  config.preset[p].global.microLinn.anchorCents = 39;             // ^M3 = 439c
  config.preset[p].global.microLinn.sweeten = 20;                 // 20 = 2¢
  config.preset[p].global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
  config.preset[p].global.customRowOffset = 13;                   // 41-equal downmajor 3rds
  config.preset[p].split[LEFT].microLinn.colOffset = 2;
  config.preset[p].split[RIGHT].microLinn.colOffset = 2;
  config.preset[p].global.activeNotes = 6;                        // fit 2 rainbow zones on a Linn200
  config.preset[p].global.microLinn.guitarTuning[1] = 13;         // alternating downmajor and upminor 3rds
  config.preset[p].global.microLinn.guitarTuning[2] = 11;         // row offset from the string below it
  config.preset[p].global.microLinn.guitarTuning[3] = 13;
  config.preset[p].global.microLinn.guitarTuning[4] = 11;
  config.preset[p].global.microLinn.guitarTuning[5] = 13;
  config.preset[p].global.microLinn.guitarTuning[6] = 11;
  config.preset[p].global.microLinn.guitarTuning[7] = 13;

  // preset 0 (4th from the top) is 31edo Bosanquet
  p = 0;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    config.preset[p].split[side].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
    config.preset[p].split[side].bendRangeOption = bendRange24;
    config.preset[p].split[side].customBendRange = 24;
    config.preset[p].split[side].expressionForZ = loudnessChannelPressure;
    config.preset[p].split[side].playedTouchMode = playedBlink;
  }
  config.preset[p].split[LEFT].midiChanMain = 1;
  config.preset[p].split[LEFT].midiChanSet[0] = false;
  config.preset[p].split[RIGHT].midiChanMain = 16;
  config.preset[p].split[RIGHT].midiChanSet[15] = false;
  config.preset[p].global.microLinn.EDO = 31;
  i = microLinnTriIndex(31, 0); 
  memcpy(&config.preset[p].global.microLinn.rainbow,   &Device.microLinn.rainbows[i],   31);
  memcpy(&config.preset[p].global.microLinn.fretboard, &Device.microLinn.fretboards[i], 31);
  config.preset[p].global.activeNotes = 6;                        // partial rainbow
  for (byte side = 0; side < NUMSPLITS; ++side) {
    config.preset[p].split[side].microLinn.colOffset = 5;         // 31-equal major 2nd
    config.preset[p].split[side].microLinn.rowOffset = 3;         // 31-equal minor 2nd
    config.preset[p].split[side].microLinn.defaultLayout = 2;     // Bosanquet #2
  }
  config.preset[p].global.microLinn.guitarTuning[1] = 13;         // F# B E A D G B E
  config.preset[p].global.microLinn.guitarTuning[2] = 13;
  config.preset[p].global.microLinn.guitarTuning[3] = 13;
  config.preset[p].global.microLinn.guitarTuning[4] = 13;
  config.preset[p].global.microLinn.guitarTuning[5] = 13;
  config.preset[p].global.microLinn.guitarTuning[6] = 10;
  config.preset[p].global.microLinn.guitarTuning[7] = 13;

  microLinnSetupKitesPersonalPrefs();   // delete later
  setupMicroLinn();
}

void storeMicroLinnRainbowAndFretboard() {         // called from storeSettingsToPreset()
  byte rawEDO = Global.microLinn.EDO;
  if (rawEDO == 4) return;
  short i = microLinnTriIndex(rawEDO, 0);  
  memcpy(&Global.microLinn.rainbow,   &Device.microLinn.rainbows[i],   rawEDO);
  memcpy(&Global.microLinn.fretboard, &Device.microLinn.fretboards[i], rawEDO); 
}

void loadMicroLinnRainbowAndFretboard() {          // called from loadSettingsFromPreset()
  byte rawEDO = Global.microLinn.EDO;
  if (rawEDO == 4) return;
  short i = microLinnTriIndex(rawEDO, 0);
  memcpy(&Device.microLinn.rainbows[i],   &Global.microLinn.rainbow,   rawEDO);
  memcpy(&Device.microLinn.fretboards[i], &Global.microLinn.fretboard, rawEDO); 
}

void microLinnSetupKitesPersonalPrefs() {          // speed up debugging cycle, delete later once things are more stable
  // the 2nd from bottom preset (#5) is the kite guitar preset
  config.preset[5].global.activeNotes = 8;                            // 41edo fretboard
  config.preset[5].global.microLinn.anchorRow = 5;                    // 3rd row from top
  config.preset[5].global.microLinn.anchorCol = 6;
  config.preset[5].global.microLinn.anchorNote = 62;                  // D = 0 cents
  config.preset[5].global.microLinn.anchorCents = 0;

  // the bottom preset (#4) is a 12edo preset
  config.preset[4].global.rowOffset = 4;                              // 12edo major 3rds
  
  for (byte p = 4; p <= 5; ++p) {
    config.preset[p].global.switchAssignment[SWITCH_SWITCH_1] = ASSIGNED_MICROLINN_EDO_UP;
    config.preset[p].global.switchAssignment[SWITCH_SWITCH_2] = ASSIGNED_MICROLINN_EDO_DOWN;
    for (byte side = 0; side < NUMSPLITS; ++side) {
      config.preset[p].split[side].midiMode = channelPerNote;         // overwrite the 1-channel defaults with MPE settings
      config.preset[p].split[side].bendRangeOption = bendRange24;
      config.preset[p].split[side].customBendRange = 24;
      config.preset[p].split[side].pitchCorrectHold = pitchCorrectHoldOff;
      config.preset[p].split[side].expressionForZ = loudnessChannelPressure;
      config.preset[p].split[side].playedTouchMode = playedSame;
      config.preset[p].split[side].microLinn.monoFixes = 3;            //  3 = X+Z
      config.preset[p].split[side].microLinn.hammerOnMode = 0;         //  3 = R+L
      config.preset[p].split[side].microLinn.hammerOnZone = 30;        // 15 = 150¢
      config.preset[p].split[side].microLinn.hammerOnWait = 0;         // 20 = 200 milliseconds
      config.preset[p].split[side].microLinn.midiGroupCC = 20;
    }
    config.preset[p].split[LEFT].colorAccent = COLOR_ORANGE;
    config.preset[p].split[RIGHT].colorMain = COLOR_LIME;
    config.preset[p].split[RIGHT].colorLowRow = COLOR_GREEN;
    // set up the right side for SWAM instruments
    config.preset[p].split[RIGHT].midiMode = oneChannel;
    config.preset[p].split[RIGHT].lowRowMode = lowRowCCXYZ;
    config.preset[p].split[RIGHT].lowRowCCXYZBehavior = lowRowJoystick;
    config.preset[p].split[RIGHT].microLinn.ccForLowRowW = 20;
    config.preset[p].split[RIGHT].microLinn.ccForLowRowX = 15;
    config.preset[p].split[RIGHT].ccForLowRowX = 16;
    config.preset[p].split[RIGHT].microLinn.ccForLowRowY = 17;
    config.preset[p].split[RIGHT].ccForLowRowY = 18;
    config.preset[p].split[RIGHT].ccForLowRowZ = 19;
  }
  //debugLevel = 0;
  microLinnImportingOn = true;
}

boolean microLinnIsLinked (byte leftVal, byte rightVal, byte offVal) {      // don't auto-link two OFFs
  return (leftVal == rightVal) && (leftVal != offVal);
}

void setupMicroLinn() {                              // runs when the Linnstrument powers up or a preset is loaded
  edo = Global.microLinn.EDO;
  if (edo == 4) edo = 12;                            // because "OFF" is 4edo which is really 12edo
  microLinnPrevScale = Global.activeNotes;           // to disable backtracking
  if (isMicroLinnOn()) lightSettings = LIGHTS_ACTIVE;
  microLinnColOffsetLinked = microLinnIsLinked(Split[LEFT].microLinn.colOffset, Split[RIGHT].microLinn.colOffset, 1);
  microLinnRowOffsetLinked = microLinnIsLinked(Split[LEFT].microLinn.rowOffset, Split[RIGHT].microLinn.rowOffset, -MICROLINN_MAX_ROW_OFFSET-1);
  microLinnPerSplitSettingsLinked[7] = microLinnIsLinked(Split[LEFT].microLinn.monoFixes, Split[RIGHT].microLinn.monoFixes, 0);
  microLinnPerSplitSettingsLinked[4] = microLinnIsLinked(Split[LEFT].microLinn.hammerOnMode, Split[RIGHT].microLinn.hammerOnMode, 0);
  microLinnPerSplitSettingsLinked[6] = microLinnIsLinked(Split[LEFT].microLinn.hammerOnZone, Split[RIGHT].microLinn.hammerOnZone, 10);
  microLinnPerSplitSettingsLinked[5] = microLinnIsLinked(Split[LEFT].microLinn.hammerOnWait, Split[RIGHT].microLinn.hammerOnWait, 0);
  microLinnPerSplitSettingsLinked[3] = microLinnIsLinked(Split[LEFT].microLinn.showCustomLEDs, Split[RIGHT].microLinn.showCustomLEDs, 0);
  microLinnPerSplitXenSettingsLinked[7] = microLinnIsLinked(Split[LEFT].microLinn.condensedBendPerPad, Split[RIGHT].microLinn.condensedBendPerPad, 0);
  microLinnPerSplitXenSettingsLinked[6] = microLinnIsLinked(Split[LEFT].microLinn.defaultLayout, Split[RIGHT].microLinn.defaultLayout, 0);
  microLinnPerSplitXenSettingsLinked[5] = microLinnIsLinked(Split[LEFT].microLinn.tuningTable, Split[RIGHT].microLinn.tuningTable, 0);
  microLinnPerSplitXenSettingsLinked[4] = microLinnIsLinked(Split[LEFT].microLinn.midiGroupCC, Split[RIGHT].microLinn.midiGroupCC, -1);

  calcMicroLinnTuning();
  for (byte side = 0; side < NUMSPLITS; ++side) {
    microLinnCalcHammerOnEdosteps(side);
    microLinnCalcColOffsetCents(side);
    microLinnCalcRowOffsetCents(side);
  }
  calcMicroLinnGlobalRowOffsetCents();
  for (byte row = 1; row < NUMROWS; ++row) {
    microLinnCalcGuitarRowOffsetCents(row);
  }
  microLinnCalcGuitarTuning();
  microLinnSemitonesPerPad[LEFT]  = Split[LEFT].microLinn.colOffset;     // assumes 12edo, if not calcTuning will overwrite this
  microLinnSemitonesPerPad[RIGHT] = Split[RIGHT].microLinn.colOffset;
  microLinnImportType = 0;                                               // 0 means no import is currently happening
  microLinnUpdateAnchorString();
  updateDisplay();
}

void microLinnCalcHammerOnEdosteps(byte side) {
  short equave = 100 * Global.microLinn.equaveSemitones + Global.microLinn.equaveStretch;
  if (Global.microLinn.EDO == 4 && Split[side].microLinn.hammerOnZone < 10) Split[side].microLinn.hammerOnZone = 10;
  // use paretheses to ensure division happens last. 10 not 10.0 so that edosteps are automatically floored, so 180c = 1\12
  microLinnHammerOnEdosteps[side] = (edo * Split[side].microLinn.hammerOnZone * 10) / equave;
  // since the user is not playing atm, these should be nulls, but reset it here just to be safe
  memset (microLinnHammerOns, -1, sizeof(microLinnHammerOns));
  microLinnNumHammerOns = 0;
}

void microLinnResetFretboard (byte edo) {
  short start1 = microLinnTriIndex (edo, 0);                // index into microLinn.fretboards
  short start2 = 6 * (edo - 5);                             // index into MICROLINN_FRETBOARDS
  memset(&Device.microLinn.fretboards[start1], 0, edo);

  if (edo == 41) {                                          // kite guitar fretboard
    for (byte i = 0; i < 41; i += 12) {
      Device.microLinn.fretboards[start1 + i] = 16;         // single dot
      Device.microLinn.fretboards[start1 + i + 4] = 40;     // double dot
      if (i == 36) break;
      Device.microLinn.fretboards[start1 + i + 8] = 84;     // triple dot
    }
    return;
  }

  for (byte i = 0; i < 6; ++i) {
    byte fretNum = MICROLINN_FRETBOARDS[start2 + i];
    Device.microLinn.fretboards[start1 + fretNum] = (i == 4 ? 60 : 24);             // 5th column is for double dots
    if (fretNum > 0) {
      Device.microLinn.fretboards[start1 + edo - fretNum] = (i == 4 ? 60 : 24);     // fill upper octave symmetrically
    }
  }
  Device.microLinn.fretboards[start1] = 126;                                        // tonic/octave gets a triple dot
}

void microLinnUpdateAnchorString() {
  char digit1, digit2;
  if (Global.microLinn.anchorCol < 10) {
    digit1 = '0' + Global.microLinn.anchorCol;
    digit2 = ' ';
  } else {
    digit1 = '0' + floor (Global.microLinn.anchorCol / 10.0);
    digit2 = '0' + Global.microLinn.anchorCol % 10;
  }
  if (!isLinn200() && Global.microLinn.anchorCol >= 10) {            // "3 12 ", drop R and C to fit 3 digits in
    microLinnAnchorString[0] = '8' - Global.microLinn.anchorRow; 
    microLinnAnchorString[1] = ' ';
    microLinnAnchorString[2] = digit1;
    microLinnAnchorString[3] = digit2;
    microLinnAnchorString[4] = ' ';
  } else {
    microLinnAnchorString[0] = 'R';                                  // "R3C12"
    microLinnAnchorString[1] = '8' - Global.microLinn.anchorRow;
    microLinnAnchorString[2] = 'C';
    microLinnAnchorString[3] = digit1;
    microLinnAnchorString[4] = digit2;
  }
}

/************** functions to adjust the col/row offsets when the edo changes *********************/

short microLinnEDOstepsToCents (short edosteps) {
  return round((100.0 * Global.microLinn.equaveSemitones + Global.microLinn.equaveStretch) * edosteps / edo);
}

void microLinnCalcColOffsetCents(byte side) {
  microLinnColOffsetCents[side] = microLinnEDOstepsToCents(Split[side].microLinn.colOffset);
}

void microLinnCalcRowOffsetCents(byte side) {
  signed char rowOffset = Split[side].microLinn.rowOffset;
  if (abs(rowOffset) > MICROLINN_MAX_ROW_OFFSET) return;                           // if OFF or NOVR
  microLinnRowOffsetCents[side] = microLinnEDOstepsToCents(rowOffset);
}

// called from handleGlobalSettingNewTouch(), handleGlobalSettingHold(), 
// handleGlobalSettingRelease() and handleRowOffsetNewTouch()
void calcMicroLinnGlobalRowOffsetCents() {
  // if there's no rowOffset, use the customRowOffset, because that's what will get adjusted when changing edos
  if (Global.rowOffset == ROWOFFSET_ZERO ||
      Global.rowOffset == ROWOFFSET_GUITAR ||
      Global.rowOffset == ROWOFFSET_NOOVERLAP ||
      Global.rowOffset == ROWOFFSET_OCTAVECUSTOM) {
    if (Global.microLinn.EDO == 4 && Global.customRowOffset == -17) {
      microLinnGlobalRowOffsetCents = microLinnEDOstepsToCents(-5);                   // -GUI is mostly descending 4ths
    } else {
      microLinnGlobalRowOffsetCents = microLinnEDOstepsToCents(Global.customRowOffset);
    }
  } else {
    microLinnGlobalRowOffsetCents = microLinnEDOstepsToCents(Global.rowOffset);
  }
}

void microLinnCalcGuitarRowOffsetCents(byte row) {
  microLinnGuitarTuningCents[row] = microLinnEDOstepsToCents(Global.microLinn.guitarTuning[row]);
}

// true if all intervals are 4ths except that one major 3rd, the exact pitch doesn't matter
boolean isMicroLinnGuitarTuningStandard() {
  byte fourth = MICROLINN_PERF_4TH[edo];
  byte third = 2 * edo - 4 * fourth;                                       // ensure a double 8ve between 1st and 6th strings
  if (edo == 13 && Global.microLinn.guitarTuning[1] != fourth) {           // it will flunk 13edo, so test for 13b instead
    third = 2; fourth = 6;
  }
  else if (edo == 18 && Global.microLinn.guitarTuning[1] != fourth) {      // test for 18b instead
    third = 4; fourth = 8;
  }
  return Global.microLinn.guitarTuning[1] == fourth &&
         Global.microLinn.guitarTuning[2] == fourth &&
         Global.microLinn.guitarTuning[3] == fourth &&
         Global.microLinn.guitarTuning[4] == fourth &&
         Global.microLinn.guitarTuning[5] == fourth &&
         Global.microLinn.guitarTuning[6] == third &&
         Global.microLinn.guitarTuning[7] == fourth;
}

short microLinnCheckRowOffset(short rowOffset, short cents, short maxOffset) {
  // ensure coprime row/col offsets so that all notes of the edo are present
  // e.g. 12edo +5 +2 --> 24edo +10 +4 --> 24edo +9 +4
  // ideally if the col offset were bigger than the row offset, and non-guitar tuning, the col offset would get adjusted
  // but it might be hard to be coprime with all 5 kinds of row offsets
  short colOffsetLCM = microLinnLCM (Split[LEFT].microLinn.colOffset, Split[RIGHT].microLinn.colOffset); 
  if (colOffsetLCM <= 1) return rowOffset;                       // if both col offsets are 1 or 0, do nothing
  short equaveCents = 100 * Global.microLinn.equaveSemitones + Global.microLinn.equaveStretch;
  short zigzagBy = (rowOffset * equaveCents > cents * edo) ? -1 : 1;    // this is really (rowOffset/edo > cents/equaveCents)
  // this while loop exits when rowOffset is a prime number if not sooner
  while (microLinnGCD (rowOffset, colOffsetLCM) > 1 || abs(rowOffset) > maxOffset) {
    rowOffset += zigzagBy;
    zigzagBy += zigzagBy > 0 ? 1 : -1;                           // zigzag through the nearest numbers,
    zigzagBy *= -1;                                              // e.g. 9 --> 8 --> 10 --> 7
  }
  return rowOffset;
}

void microLinnCheckGuitarTuning() {                     // make sure it fits inside MICROLINN_MAX_GUITAR_RANGE
  microLinnCalcGuitarTuning();
  short overflow = microLinnHighestGuitarNote
                 - microLinnLowestGuitarNote
                 - MICROLINN_MAX_GUITAR_RANGE;
  while (overflow > 0) {                                // while not if, to handle weird edge cases
    for (byte row = 1; row < NUMROWS; ++row) {          // shrink all 7 offsets by 1
      Global.microLinn.guitarTuning[row] -= microLinnSign(Global.microLinn.guitarTuning[row]);
    }
    microLinnCalcGuitarTuning();
    overflow = microLinnHighestGuitarNote
             - microLinnLowestGuitarNote
             - MICROLINN_MAX_GUITAR_RANGE;
  }
}

void microLinnApplyDefaultLayout(byte side) {
  signed char row, col;
  byte m2 = MICROLINN_MIN_2ND[edo];
  byte M2 = MICROLINN_MAJ_2ND[edo];
  byte A1 = abs(M2 - m2);                         // use abs so that the row offset is > 0 even with superflat edos like 16 
  byte P4 = MICROLINN_PERF_4TH[edo];
  switch (Split[side].microLinn.defaultLayout) {
    case 1:  {row = A1;       col = M2;       break;}      // A1, M2 = Bosanquet
    case 2:  {row = m2;       col = M2;       break;}      // m2, M2 = Bosanquet 2
    case 3:  {row = P4 - M2;  col = M2;       break;}      // m3, M2 = Accordion B/C
    case 4:  {row = P4;       col = M2;       break;}      // P4, M2 = Wicki-Hayden
    case 5:  {row = P4 + M2;  col = M2;       break;}      // P5, M2 = Wicki-Hayden 2
    case 6:  {row = P4;       col = P4 + M2;  break;}      // P4, P5 = Array mbira
    case 7:  {row = edo;      col = P4 + M2;  break;}      // P8, P5 = Array mbira 2
    default: {
      // swiping from BSQ to OFF sets the row/col offsets to OFF
      Split[side].microLinn.colOffset = 1;
      Split[side].microLinn.rowOffset = -MICROLINN_MAX_ROW_OFFSET - 1;
      return;
    }
  }
  row /= microLinnGCD(row, col);            // ensure coprime-ness, e.g. 24edo wicki-hayden
  Split[side].microLinn.colOffset = col;
  Split[side].microLinn.rowOffset = microLinnCheckRowOffset(row, 0, edo);
}

void microLinnAdjustColAndRowOffsets() {
  // called when the edo changes. going from e.g. 12edo to 19edo adjusts the row offset from +5 (12edo 4th) to +8 (19edo 4th)
  // adjust each row offset even if it's unselected, because the user might want to switch to it later
  // avoid offsets becoming zero, which can happen with a small offset and going from a large edo to a small one
  // but if the user sets a row offset to zero edosteps, it will remain zero when they change edos
  // ensure each row offset is copriime with both column offsets, to avoid missing notes (e.g. 12edo whole-tone scale)
  boolean wasStandardTuning = isMicroLinnGuitarTuningStandard();   // test using the old value of edo
  edo = Global.microLinn.EDO;
  if (edo == 4) edo = 12;                              // because "OFF" is 4edo which is really 12edo
  signed char newOffset;

  for (byte side = 0; side < NUMSPLITS; ++side) {
    microLinnCalcHammerOnEdosteps(side);
  }

  // adjust the 2 per-split column offsets if already skipping frets (don't start skip-fretting unasked, too confusing)
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.colOffset == 1) continue;
    if (Split[side].microLinn.defaultLayout > 0) continue;
    if (Split[side].microLinn.condensedBendPerPad > 0) continue;                      // don't adjust stepspans
    newOffset = round ((edo * microLinnColOffsetCents[side]) / 1200.0);
    newOffset = constrain (newOffset, 1, MICROLINN_MAX_COL_OFFSET);
    Split[side].microLinn.colOffset = newOffset;
  }

  // adjust the 2 per-split row offsets, unless they are OFF or NOVR
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (abs(Split[side].microLinn.rowOffset) > MICROLINN_MAX_ROW_OFFSET) continue;
    if (Split[side].microLinn.defaultLayout > 0) continue;
    newOffset = round ((edo * microLinnRowOffsetCents[side]) / 1200.0);
    newOffset = constrain (newOffset, -MICROLINN_MAX_ROW_OFFSET, MICROLINN_MAX_ROW_OFFSET);
    if (newOffset == 0) {
      newOffset = microLinnSign(Split[side].microLinn.rowOffset);
    }
    newOffset = microLinnCheckRowOffset(newOffset, microLinnRowOffsetCents[side], NUMCOLS);
    Split[side].microLinn.rowOffset = newOffset;
  }

  // apply any default layouts to the per-split column and row offsets
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.defaultLayout > 0) microLinnApplyDefaultLayout(side);
  }

  // adjust the 7 row offsets in the guitar tuning, preserving standard tunings
  if (wasStandardTuning) {
    // if using cents, 12edo 55545 -> 22edo 99979, but should be 99989 to get a double 8ve between outer strings
    for (byte row = 1; row < NUMROWS; ++row) {
      Global.microLinn.guitarTuning[row] = MICROLINN_PERF_4TH[edo];
    }
    Global.microLinn.guitarTuning[6] = 2 * edo - 4 * MICROLINN_PERF_4TH[edo];        // the third is 81/64 not 5/4
    microLinnCalcGuitarTuning();
  }
  else {
    // row = 1 because Global.microLinn.guitarTuning[0] is only used for the DIA/CHRO flag
    for (byte row = 1; row < NUMROWS; ++row) {
      newOffset = round ((edo * microLinnGuitarTuningCents[row]) / 1200.0);
      if (newOffset == 0) {
        newOffset = microLinnSign(Global.microLinn.guitarTuning[row]);
      }
      newOffset = microLinnCheckRowOffset(newOffset, microLinnGuitarTuningCents[row], MICROLINN_MAX_GUITAR_RANGE);
      Global.microLinn.guitarTuning[row] = newOffset;
    }
    microLinnCheckGuitarTuning();
  }

  // adjust the global row offset
  // if the global row offset becomes > 7 (e.g. 12edo +5 --> 22edo +9), it's stored as a custom row offset
  // but if the user later switches to a smaller edo and the row offset becomes <= 7 (e.g. 22edo +9 --> 17edo +7),
  // the row offset is stored as a global row offset, and the custom row offset becomes an 8ve/5th/4th (whichever is <= 25)
  newOffset = round ((edo * microLinnGlobalRowOffsetCents) / 1200.0);
  newOffset = constrain (newOffset, -MICROLINN_MAX_ROW_OFFSET, MICROLINN_MAX_ROW_OFFSET);
  if (newOffset == 0) {
    newOffset = microLinnSign(microLinnGlobalRowOffsetCents);
  }
  newOffset = microLinnCheckRowOffset(newOffset, microLinnGlobalRowOffsetCents, NUMCOLS);
  if (Global.rowOffset == ROWOFFSET_ZERO ||
      Global.rowOffset == ROWOFFSET_GUITAR ||
      Global.rowOffset == ROWOFFSET_NOOVERLAP) {
    Global.customRowOffset = newOffset;
  } else if (newOffset < 3 || newOffset > 7) {
    Global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
    Global.customRowOffset = newOffset;
  } else {
    Global.rowOffset = newOffset;
    if (edo <= 25)      {Global.customRowOffset = edo;}                             // octave, if it's <= 25
    else if (edo <= 43) {Global.customRowOffset = edo - MICROLINN_PERF_4TH[edo];}   // fifth,  if it's <= 25
    else                {Global.customRowOffset = MICROLINN_PERF_4TH[edo];}         // fourth
  }
}

/************** sweetening functions ************************/

// not yet called anywhere, will be called on note-ons and note-offs
// ratios to widen: 5/4 5/3 5/2 10/3 5/1 20/3 10/1     (maybe 15/8 15/4 15/2?)  (maybe 10/7?)
// ratios to narrow: 6/5 8/5 12/5 16/5 24/5 32/5 48/5  (maybe 9/5 18/5 36/5?)   (maybe 7/5?)
void sweetenMicroLinn41(short newNote) {
  if (edo != 41) return;
  //memset(&microLinnPull, false, sizeof(microLinnPull));
  for (byte side = 0; side < NUMSPLITS; ++side) {
    for (byte ch = 0; ch < 15; ++ch) {
      if (noteTouchMapping[side].musicalTouchCount[ch] != 1) continue;
      byte col = focusCell[side][ch].col;
      byte row = focusCell[side][ch].row;
      short edostep = getMicroLinnVirtualEdostep(side, col, row);
      for (byte side2 = 0; side2 < NUMSPLITS; ++side2) {
        for (byte ch2 = ch+1; ch2 < 16; ++ch2) {
          if (noteTouchMapping[side2].musicalTouchCount[ch2] != 1) continue;
          byte col2 = focusCell[side2][ch2].col;
          byte row2 = focusCell[side2][ch2].row;
          short edostep2 = getMicroLinnVirtualEdostep(side2, col2, row2);
          switch (microLinnMod(edostep - edostep2, 41)) {
            case 13:    // 13\41 = vM3 = 5/4
            case 30:    // 30\41 = vM6 = 5/3
              //microLinnPull[edostep].sharp = true;
              //microLinnPull[edostep2].flat = true;
              break;
            case 11:     // 11\41 = ^m3 = 6/5
            case 28:     // 28\41 = ^m6 = 8/5
              //microLinnPull[edostep].flat = true;
              //microLinnPull[edostep2].sharp = true;
              break;
          }
        }
      }
    }
  }
}

/************** calcTuning functions ************************/

// calc the cumulative row offsets by summing the individual row offsets, to speed up calcTuning()
// if Global.microLinn.guitarTuning = [0 7 5 5 5' 5 4 5] (5' = anchor) then microLinnGuitarTuning = [-22 -15 -10 -5 0 5 9 14]
// [0 7 5 5 5 5 4 5] -> [0 7 12 17 22 27 31 36]
// called not by calcTuning() but by handleMicroLinnGuitarTuningNewTouch(), so it's done ahead of time while not waiting to play
void microLinnCalcGuitarTuning() {
  byte anchorRow = Global.microLinn.anchorRow;
  for (byte row = 0; row < MAXROWS; ++row) {
    short sum = 0;
    for (byte i = anchorRow; i < row; ++i) {          // only one of these two for-loops will run
      sum += Global.microLinn.guitarTuning[i+1];      // add them if going up
    }
    for (byte i = row; i < anchorRow; ++i) {
      sum -= Global.microLinn.guitarTuning[i+1];      // subtract them if going down
    }
    microLinnGuitarTuning[row] = sum;                 // microLinnGuitarTuning[anchorRow] always equals 0
  }
  // find the highest and lowest notes
  microLinnHighestGuitarNote = microLinnGuitarTuning[0];
  microLinnLowestGuitarNote  = microLinnGuitarTuning[0];
  for (byte row = 1; row < MAXROWS; ++row) {
    if (microLinnGuitarTuning[row] > microLinnHighestGuitarNote) {
      microLinnHighestGuitarNote = microLinnGuitarTuning[row];
    }
    if (microLinnGuitarTuning[row] < microLinnLowestGuitarNote) {
      microLinnLowestGuitarNote = microLinnGuitarTuning[row];
    }
  }
}

// calcs what, not where, also calcs numGroups and channelCount for rechanneling
void microLinnCalcLowestEdostepEtc(byte side) {
  short lowestRow, lowestCol, distance, highestRow, highestEdostep;
  if (Split[side].microLinn.rowOffset >= 0) {                                       // use the per-split row offset if it's positive or NOVR
    lowestRow = 0;
  } else if (Split[side].microLinn.rowOffset >= -MICROLINN_MAX_ROW_OFFSET) {        // ...or if it's negative but not OFF
    lowestRow = 7;
  } else if (Global.rowOffset == ROWOFFSET_OCTAVECUSTOM) {
    lowestRow = Global.customRowOffset >= 0 ? 0 : 7;
  } else lowestRow = 0;

  if (Global.rowOffset == ROWOFFSET_GUITAR && Split[side].microLinn.rowOffset < -MICROLINN_MAX_ROW_OFFSET) {
    microLinnLowestEdostep[side] = microLinnLowestGuitarNote;
  } else {
    microLinnLowestEdostep[side] = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, lowestRow);
  }
  lowestCol = isLeftHandedSplit(side) ? NUMCOLS - 1 : 1;
  distance = abs(Global.microLinn.anchorCol - lowestCol);
  if (Split[side].microLinn.condensedBendPerPad > 0) {
    microLinnLowestEdostep[side] -= microLinnScaleStepsToEdoSteps(distance, 0);
  } else {
    microLinnLowestEdostep[side] -= Split[side].microLinn.colOffset * distance;
  }
  microLinnLowestEdostep[side] += microLinnTransposition(side);

  // calc microLinnNumGroups for grouping and rechannelling
  if (Split[side].microLinn.tuningTable < 2) return;
  if (Global.rowOffset == ROWOFFSET_GUITAR && Split[side].microLinn.rowOffset < -MICROLINN_MAX_ROW_OFFSET) {
    highestEdostep = microLinnHighestGuitarNote;
  } else {
    highestRow = 7 - lowestRow;
    highestEdostep = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, highestRow);
  }
  highestEdostep += Split[side].microLinn.colOffset * abs((NUMCOLS - lowestCol) - Global.microLinn.anchorCol);
  highestEdostep += microLinnTransposition(side);
  microLinnNumGroups[side] = ((highestEdostep - microLinnLowestEdostep[side]) >> 7) + 1;
  microLinnNumGroups[side] = constrain(microLinnNumGroups[side], 1, 8);

  // calc microLinnChannelCount for rechannelling
  if (Split[side].microLinn.tuningTable < 3) return;
  microLinnChannelCount[side] = 0;
  for (byte ch = 0; ch < 16; ++ch) {
    if (Split[side].midiChanSet[ch]) microLinnChannelCount[side] += 1;
  }
}

// called by toggleChannel() in ls_settings.ino
void calcMicroLinnNumChannels(byte side) {
  microLinnChannelCount[side] = 0;
  for (byte ch = 0; ch < 16; ++ch) {
    if (Split[side].midiChanSet[ch]) microLinnChannelCount[side] += 1;
  }
}

byte microLinnGetSplitWidth(byte side) {
  if (Global.splitActive) {
    if (side == LEFT)  return Global.splitPoint - 1;
    if (side == RIGHT) return NUMCOLS - Global.splitPoint;
  }
  return NUMCOLS - 1;
}

short microLinnSumOfRowOffsets(byte side, byte row1, byte row2) {      // edosteps from row1 to row2
  // use this split's row offset if it's not OFF
  if (Split[side].microLinn.rowOffset >   MICROLINN_MAX_ROW_OFFSET) return microLinnGetSplitWidth(side) * (row2 - row1);    // if NOVR
  if (Split[side].microLinn.rowOffset >= -MICROLINN_MAX_ROW_OFFSET) return Split[side].microLinn.rowOffset * (row2 - row1); // if not OFF
  // otherwise fallback to the global row offset
  switch (Global.rowOffset) {
    case ROWOFFSET_OCTAVECUSTOM: return Global.customRowOffset  * (row2 - row1);
    case ROWOFFSET_GUITAR:       return microLinnGuitarTuning[row2] - microLinnGuitarTuning[row1];
    case ROWOFFSET_NOOVERLAP:    return microLinnGetSplitWidth(side) * (row2 - row1);
    case ROWOFFSET_ZERO:         return 0;
    default:                     return Global.rowOffset * (row2 - row1);
  }
}

short microLinnTransposition(byte side) {                                  // # of edosteps to transpose by
  signed char direction = isLeftHandedSplit(side) ? -1 : 1;
  return (Split[side].transposeOctave / 12.0) * edo
        + Split[side].transposePitch  * MICROLINN_MAJ_2ND_TR[edo]          // semitones are interpreted as whole tones
        + Split[side].microLinn.transposeEDOsteps
        - Split[side].transposeLights * Split[side].microLinn.colOffset * direction;    // transposeLights shifts by columns not edosteps
}

void calcMicroLinnTuning() { 
  // calcs microLinnSemitonesPerPad, microLinnEdostep, microLinnMidiNote and microLinnTuningBend
  // the last three are tuning tables, different from tuning table mode which refers to loading a scala tuning file into a synth
  // called when any column 0 button is released and the user returns to normal playing mode
  // also called when entering microLinnConfig note lights display or leaving anchor cell display
  // also called for octave up/down or EDO+/EDO- via switch1/switch2 control buttons or footswitch presses
  // needed for any change to row offsets, col offsets, transpose, bendrange or anything xen on microLinnConfig
  // also needed for changes to the split point if in NO_OVERLAP mode

  if (!isMicroLinnOn()) return;

midiSendControlChange(41, 41, 1, true);    // for debugging, delete later
MLcalcTuningTimer = millis();

  if (Split[LEFT].microLinn.condensedBendPerPad > 0 ||
      Split[RIGHT].microLinn.condensedBendPerPad > 0) {
    microLinnCalcCondensedScale();
  }
  for (byte side = 0; side < NUMSPLITS; ++side) {
    microLinnCalcLowestEdostepEtc(side);
    microLinnSweetener[side] = round(Global.microLinn.sweeten * getBendRange(side) / 1000);  // 20 = 2¢ = 20 thousandths of a semitone
  }

  byte anchorCol = Global.microLinn.anchorCol;
  byte anchorRow = Global.microLinn.anchorRow;
  // anchorPitch = a standard 12edo midi note, but with 2 decimal places for anchorCents
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;
  // edostepSize = size of 1 edostep in 12edo semitones, e.g. 1\41 is 0.2927 semitones
  edostepSize = (Global.microLinn.equaveSemitones + Global.microLinn.equaveStretch / 100.0) / edo; 
  short distance, virtualEdostep, roundNote; float note;
  memset (&microLinnMidiNote, -1, sizeof(microLinnMidiNote));                       // -1 = this edostep is not used
  memset (&microLinnTuningBend, 0, sizeof(microLinnTuningBend));

  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.condensedBendPerPad > 0) {
      microLinnCalcCondensedTuning(side);
      continue;
    }
    /************ bug, JI tunings can be condensed
    if (Split[side].microLinn.tuningTable > 0) {
      microLinnCalcTuningTable(side);
      continue;
    }**********/
    short colOffset = Split[side].microLinn.colOffset;
    microLinnSemitonesPerPad[side] = colOffset * edostepSize;                       // e.g. kite tuning is 2\41 = 0.5854 semitones
    if (isLeftHandedSplit(side)) colOffset *= -1;
    float semitonesPerPad = colOffset * edostepSize;                                // microLinnSemitonesPerPad is used later, semitonesPerPad is used here
    short transpose = microLinnTransposition(side);
    float bendRange = 8192.0F / getBendRange(side);                                 // invert to avoid dividing 400 times
    for (byte row = 0; row < NUMROWS; ++row) {
      distance = microLinnSumOfRowOffsets(side, anchorRow, row);                    // distance in edosteps between any 2 rows
      distance += transpose - anchorCol * colOffset;                                // distance from the anchor cell, initialized to col 0
      note = anchorPitch + distance * edostepSize;                                  // convert distance from edosteps to a fractional 12edo midi note
      for (byte col = 1; col < NUMCOLS; ++col) {
        distance += colOffset;                                                      // increment rather than recalc each time, for speed
        microLinnEdostep[side][col][row] = distance;                                // the anchor cell is always edostep 0, unless transposed
        note += semitonesPerPad;                                                    // increment rather than recalc
        if (note < 0 || note > 127) continue;                                       // if so, midiNote will be -1 due to the earlier memsets
        virtualEdostep = distance - microLinnLowestEdostep[side];                   // virtual edosteps are always >= 0
        if (virtualEdostep < 0 || virtualEdostep >= MICROLINN_MAX_EDOSTEPS) continue;
        microLinnMidiNote[side][virtualEdostep] = roundNote = round(note);
        microLinnTuningBend[side][virtualEdostep] = round((note - roundNote) * bendRange);  // fine-tuning bend as a zero-centered signed 13-bit integer
      }
    }
  }
MLcalcTuningTimer = calcTimeDelta(millis(), MLcalcTuningTimer);
midiSendControlChange(41, MLcalcTuningTimer, 1, true);    // for debugging, delete later
}

/************** condensing functions *********************/

void microLinnCalcCondensedScale() {
  // calc microLinnNumNotes, microLinnLargestStep, microLinnScale and microLinnScaleLookup
  // for a 12edo major scale we get 7, 2, (0 2 4 5 7 9 11 12 0 0 0 0) and (0 0 1 0 2 3 0 4 0 5 0 6 7)
  byte currScale = Global.activeNotes;
  if (currScale >= 7) currScale = microLinnPrevScale;
  if (currScale >= 7) currScale = 0;  
  microLinnNumNotes = 1;                                                  // tonic is always part of the scale       
  microLinnLargestStep = 1;
  memset (&microLinnScale, 0, sizeof(microLinnScale));
  memset (&microLinnScaleLookup, 0, sizeof(microLinnScaleLookup));
  for (byte edostep = 1; edostep <= edo; ++edostep) {
    if (microLinnIsScaleStep(edo, edostep, currScale) || edostep == edo) {
      microLinnScale[microLinnNumNotes] = edostep;
      microLinnScaleLookup[edostep] = microLinnNumNotes;
      byte currStep = edostep - microLinnScale[microLinnNumNotes - 1];
      microLinnLargestStep = max(microLinnLargestStep, currStep);
      microLinnNumNotes += 1;
    }
  }
  microLinnNumNotes -= 1;                                                 // don't count the octave
}

short microLinnScaleStepsToEdoSteps(short scaleSteps, short startingStep) {
  // startingStep determines the mode, 0 = major, 1 = dorian, 2 = phrygian, etc.
  // 2 scalesteps (a 3rd) starting at 0 (the tonic) -->  4 - 0 = 4 edosteps
  // 2 scalesteps (a 3rd) starting at 1 (the 2nd)   -->  5 - 2 = 3 edosteps
  startingStep = microLinnMod(startingStep, microLinnNumNotes);
  short endStep = microLinnMod(startingStep + scaleSteps, microLinnNumNotes);
  short octaves = (startingStep + scaleSteps - endStep) / microLinnNumNotes;
  return edo * octaves + microLinnScale[endStep] - microLinnScale[startingStep];
}

void microLinnCalcCondensedTuning(byte side) {                                    // first condense, then skip frets
  byte anchorCol = Global.microLinn.anchorCol; 
  byte anchorRow = Global.microLinn.anchorRow;
  // anchorPitch = a standard 12edo midi note, but with 2 decimal places for anchorCents
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;
  short colOffset = Split[side].microLinn.colOffset;
  if (isLeftHandedSplit(side)) colOffset *= -1;
  short transpose = microLinnTransposition(side);
  float bendRange = 8192.0F / getBendRange(side);                                 // invert to avoid dividing 400 times
  boolean isGuitarTuning = (Global.rowOffset == ROWOFFSET_GUITAR && Split[side].microLinn.rowOffset < -MICROLINN_MAX_ROW_OFFSET);
  short rowOffset, stepspan, anchorColumnNote, distance, virtualEdostep, roundNote; float note;

  microLinnSemitonesPerPad[side] = edostepSize * (Split[side].microLinn.condensedBendPerPad - 1);

  for (byte row = 0; row < NUMROWS; ++row) {
    if (!isGuitarTuning) {
      // diatonic/condensed row offsets ensure all notes are in the scale
      // condensing row offsets before summing ensures a consistent stepspan between rows
      rowOffset = microLinnSumOfRowOffsets(side, 0, 1);                           // uncondensed edosteps between any 2 rows
      stepspan = round (1.0 * rowOffset * microLinnNumNotes / edo);               // condense the edosteps to scalesteps
      stepspan *= row - anchorRow;                                                // sum the scalesteps
      anchorColumnNote = microLinnScaleStepsToEdoSteps(stepspan, 0);              // convert the scalesteps back to edosteps
    } else if (Global.microLinn.guitarTuning[0] == 0) {
      // diatonic/condensed row offsets ensure all notes are in the scale
      // condense the row offsets before summing them, +6 tritone offset makes stacked fifths
      stepspan = 0;
      for (byte i = anchorRow; i < row; ++i) {                                    // only one of these two for-loops will run
        rowOffset = Global.microLinn.guitarTuning[i+1];
        stepspan += round (1.0 * rowOffset * microLinnNumNotes / edo);
      }
      for (byte i = row; i < anchorRow; ++i) {
        rowOffset = Global.microLinn.guitarTuning[i+1];
        stepspan -= round (1.0 * rowOffset * microLinnNumNotes / edo);
      }
      anchorColumnNote = microLinnScaleStepsToEdoSteps(stepspan, 0);              // convert the scalesteps back to edosteps
    /******************** alternative method for diatonic row offsets ************************************
    } else if (Global.microLinn.guitarTuning[0] == 0) {
      // diatonic/condensed row offsets ensure all notes are in the scale
      // summing row offsets before condensing them ensures the scalesteps between distant rows...
      // ...don't diverge too much from the edosteps between distant rows
      // for example a +6 tritone offset becomes alternating 5ths and 4ths
      rowOffset = microLinnSumOfRowOffsets(side, anchorRow, row);                 // uncondensed edosteps from the anchor cell
      stepspan = floor (0.5 + rowOffset * microLinnNumNotes / edo);               // condense the edosteps to scalesteps
      anchorColumnNote = microLinnScaleStepsToEdoSteps(stepspan, 0);              // convert the scalesteps back to edosteps
    ******************************************************************************************************/
    } else {
      // Global.microLinn.guitarTuning[0] == 1, chromatic mode
      // chromatic/uncondensed row offsets control the exact edostepspan between rows 
      rowOffset = microLinnSumOfRowOffsets(side, anchorRow, row);                 // uncondensed edosteps from the anchor cell
      stepspan = 0;                                                               // all rows use the same mode, so rows go off-key
      anchorColumnNote = rowOffset;                                               // condensing doesn't alter the anchor column 
    }
    anchorColumnNote += transpose;
    for (byte col = 1; col < NUMCOLS; ++col) {
      distance = anchorColumnNote + microLinnScaleStepsToEdoSteps((col - anchorCol) * colOffset, stepspan);
      microLinnEdostep[side][col][row] = distance;                                // the anchor cell is always edostep 0, unless transposed
      note = anchorPitch + distance * edostepSize;                                // convert distance from edosteps to a fractional 12edo midi note
      if (note < 0 || note > 127) continue;                                       // if so, midiNote will be -1 due to the earlier memsets
      virtualEdostep = distance - microLinnLowestEdostep[side];                   // virtual edosteps are always >= 0
      if (virtualEdostep < 0 || virtualEdostep >= MICROLINN_MAX_EDOSTEPS) continue;
      microLinnMidiNote[side][virtualEdostep] = roundNote = round(note);
      microLinnTuningBend[side][virtualEdostep] = round((note - roundNote) * bendRange);   // fine-tuning bend as a zero-centered signed 13-bit integer
    }
  }
}

// needs work, delete?
void microLinnCalcTuningTable(byte side){
  short colOffset = Split[side].microLinn.colOffset;
  short colWidth = 0;
  if (isLeftHandedSplit(side)) {
    colWidth = (NUMCOLS - 2) * colOffset;
    colOffset *= -1;
  }
  short virtualEdostep;
  for (byte row = 0; row < NUMROWS; ++row) {
    short rowOffset = microLinnSumOfRowOffsets(side, 0, row);                // edosteps from the lowest row
    for (byte col = 1; col < NUMCOLS; ++col) {
      virtualEdostep = rowOffset + (col-1) * colOffset + colWidth;           // the lower left cell is always edostep 0, unless lefty
      microLinnEdostep[side][col][row] = virtualEdostep;
      microLinnMidiNote[side][virtualEdostep] = virtualEdostep;
      microLinnTuningBend[side][virtualEdostep] = 0;                         // not actually needed
    }
  }
}

/************** footswitch functions called in ls_switches.ino ************************/

void changeMicroLinnEDO(signed char delta) {
  if (!isMicroLinnOn()) return;
  Global.microLinn.EDO += delta;
  if (Global.microLinn.EDO < 5) Global.microLinn.EDO = MICROLINN_MAX_EDO;        // wrap around
  if (Global.microLinn.EDO > MICROLINN_MAX_EDO) Global.microLinn.EDO = 5;
  microLinnAdjustColAndRowOffsets();
  calcMicroLinnTuning();
  updateDisplay();
}

/************** misc painting functions ************************/

void paintMicroLinnNormalDisplayCell(byte split, byte col, byte row) {
  // by default clear the cell color
  byte colour = COLOR_OFF;
  CellDisplay cellDisplay = cellOff;
  byte currScale = Global.activeNotes;
  short edostep = microLinnEdostep[split][col][row];
  short virtualEdostep = edostep - microLinnLowestEdostep[split];
  signed char midiNote = microLinnMidiNote[split][virtualEdostep];
  edostep -= Split[split].transposePitch * MICROLINN_MAJ_2ND_TR[edo]     // transposing changes the sound but not the note lights
           + Split[split].microLinn.transposeEDOsteps;
  edostep = microLinnMod(edostep, edo);

  // unless the note is out of MIDI note range, show it
  if (midiNote >= 0 && !customLedPatternActive) {
    // either an anchor cell being chosen...
    if (displayMode == displayMicroLinnAnchorChooser &&
        col == Global.microLinn.anchorCol && 
        row == Global.microLinn.anchorRow) {
      colour = microLinnRainbowColor(edo, 0);
      if (colour == COLOR_OFF) colour = revealMicroLinnRainbowColor(split, col, row);
      cellDisplay = cellSlowPulse;
    }
    // ...or a fretboard...
    else if (currScale == 8) {
      byte fret = microLinnGetFret(split, col);
      if (microLinnDots(edo, fret) & (1 << row)) {
        colour = Split[split].colorMain;
        cellDisplay = cellOn;
      }
    } 
    // ...or a rainbow
    else if (microLinnIsScaleStep(edo, edostep)) {
      if (Global.microLinn.useRainbow) {
        colour = microLinnRainbowColor(edo, edostep);
      } else {
        colour = (edostep == 0 ? Split[split].colorAccent : Split[split].colorMain);
      }
      cellDisplay = cellOn;
    }
    else if (cell(col, row).touched != untouchedCell && Split[split].colorPlayed == COLOR_OFF) {  // bug: touched cells aren't found
      colour = revealMicroLinnRainbowColor(split, col, row);
      cellDisplay = cellOn;
    }
    // show pulsating anchor cell if in the octave centered on middle-C
    byte halfEquave = Global.microLinn.equaveSemitones >> 1;
    if (blinkMiddleRootNote && edostep == 0 && 
        midiNote >= 60 - halfEquave && 
        midiNote <  60 + halfEquave + (Global.microLinn.equaveSemitones % 2)) {       // F#3 to F4 in 12edo, G3 to F4 in 7edo
      colour = Split[split].colorAccent;
      cellDisplay = cellFastPulse;
    }
  }

  // if the low row is anything but normal, set it to the appropriate color
  if (row == 0 && Split[split].lowRowMode != lowRowNormal) {
    if ((Split[split].lowRowMode == lowRowCCX   && Split[sensorSplit].lowRowCCXBehavior   == lowRowCCFader) ||
        (Split[split].lowRowMode == lowRowCCXYZ && Split[sensorSplit].lowRowCCXYZBehavior == lowRowCCFader)) {
      colour = COLOR_BLACK;
      cellDisplay = cellOff;
    }
    else {
      colour = lowRowJoystickLatched[split] ? Split[split].colorPlayed : Split[split].colorLowRow;
      cellDisplay = cellOn;
    }
    // actually set the cell's color
    setLed(col, row, colour, cellDisplay, LED_LAYER_LOWROW);
  }
  else {
    // actually set the cell's color
    if (row == 0) {
      clearLed(col, row, LED_LAYER_LOWROW);
    }
    // custom LEDs shows up underneath the note lights
    byte pattern = Split[split].microLinn.showCustomLEDs;
    if (pattern > 3 && cellDisplay == cellOff) {
      colour = Device.customLeds[pattern - 4][col + MAXCOLS * row] >> 3;
      cellDisplay = cellOn;
    }
    setLed(col, row, colour, cellDisplay, LED_LAYER_MAIN);
  }
}

// used for blank cells in playedBlink mode and in playedCell mode when colorPlayed is blank
// also used for the blinking anchor cell
byte revealMicroLinnRainbowColor(byte split, short col, byte row) {
  if (Global.activeNotes > 7 || !Global.microLinn.useRainbow) {
    if (displayMode == displayMicroLinnAnchorChooser) {
      return Split[split].colorAccent;
    } else return Split[split].colorPlayed;
  }
  return microLinnRainbowColor(edo, microLinnEdostep[split][col][row]);
}

byte microLinnWarningColor (byte side) {
  return (Split[side].colorMain == COLOR_RED ? COLOR_MAGENTA : COLOR_RED);
}

void paintMicroLinnPlusMinus() {                                   // for the "8VE±" footswitch
  if (isLinn200()) {
    bigfont_draw_string(18,  2, "+", globalColor, false);          // true/false = erase
    bigfont_draw_string(18, -2, "-", globalColor, false);
  } else {
    smallfont_draw_string(13,  1, "+", globalColor, false);
    smallfont_draw_string(13, -2, "-", globalColor, false);
  }
}

void paintMicroLinnMinusPlus() {                                   // for the "8VE-+" footswitch
  if (isLinn200()) {
    bigfont_draw_string(18,  3, "-", globalColor, false);          // true/false = erase
    bigfont_draw_string(18, -1, "+", globalColor, false);
  } else {
    smallfont_draw_string(13,  2, "-", globalColor, false);
    smallfont_draw_string(13, -1, "+", globalColor, false);
  }
}

byte getMicroLinnRowOffsetColor(short rowOffset) {                            // for most Global displays
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.rowOffset >= -MICROLINN_MAX_ROW_OFFSET) {       // per-split offset overrides it
      if (side == Global.currentPerSplit) return COLOR_PINK;
      continue;
    }
    if (microLinnGCD(rowOffset, Split[side].microLinn.colOffset) != 1) return COLOR_RED;
  }
  return globalColor;
}

byte getMicroLinnRowOffsetColorNoOverlap() {                                  // for Global NO OVERLAP button
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.rowOffset >= -MICROLINN_MAX_ROW_OFFSET) {       // per-split offset overrides it
      if (side == Global.currentPerSplit) return COLOR_PINK;
      continue;      
    }
    if (microLinnGCD(microLinnGetSplitWidth(side), Split[side].microLinn.colOffset) != 1) return COLOR_RED;
  }
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar1() {                                    // for Global GUITAR button
  short rowOffset;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.rowOffset >= -MICROLINN_MAX_ROW_OFFSET) {       // per-split offset overrides it
      if (side == Global.currentPerSplit) return COLOR_PINK;
      continue;      
    }
    for (byte row = 1; row < NUMROWS; ++row) {                                // skip row 0 = DIA/CHRO flag
      if (isMicroLinnOn()) {
        rowOffset = Global.microLinn.guitarTuning[row];
      } else {
        rowOffset = Global.guitarTuning[row] - Global.guitarTuning[row - 1];
      }
      if (microLinnGCD(rowOffset, Split[side].microLinn.colOffset) != 1) return COLOR_RED;
    }
  }
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar2(byte row, boolean isSideButton) {      // for non-microtonal guitar tuning display
  if (isSideButton && row == guitarTuningRowNum) {                            // side buttons are in col 1
    return Split[Global.currentPerSplit].colorAccent;
  }
  if (row > 0) {
    signed char rowOffset = Global.guitarTuning[row] - Global.guitarTuning[row - 1];      // lower neighbor
    if (getMicroLinnRowOffsetColor(rowOffset) == COLOR_RED) return COLOR_RED;
  }
  if (row < NUMROWS - 1) {
    signed char rowOffset = Global.guitarTuning[row + 1] - Global.guitarTuning[row];      // upper neighbor
    if (getMicroLinnRowOffsetColor(rowOffset) == COLOR_RED) return COLOR_RED;
  }
  if (isSideButton) return COLOR_GREEN;
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar3(byte row) {                               // for microtonal guitar tuning display, col 1
  if (row < Global.microLinn.anchorRow) row += 1;
  if (row == 0) return COLOR_GREEN;
  if (getMicroLinnRowOffsetColor(Global.microLinn.guitarTuning[row]) == COLOR_RED) return COLOR_RED;
  return COLOR_GREEN;
}

byte getMicroLinnRowOffsetColorGuitar4(byte row) {                               // for microtonal guitar tuning display, numbers
  if (row == 0) return globalColor;
  if (getMicroLinnRowOffsetColor(Global.microLinn.guitarTuning[row]) == COLOR_RED) return COLOR_RED;
  return globalColor;
}

byte getMicroLinnGUIcolor() {                                                   // for custom row offset of -17 = -GUI
  if (getMicroLinnRowOffsetColor(4) == COLOR_RED) return COLOR_RED;             // 4 = M3 and 5 = P4
  return getMicroLinnRowOffsetColor(5);
}

byte getMicroLinnRowOffsetColor() {                                              // for PerSplit row display
  byte side = Global.currentPerSplit;
  signed char rowOffset = Split[side].microLinn.rowOffset;
  if (rowOffset < -MICROLINN_MAX_ROW_OFFSET) return Split[side].colorMain;               // OFF is never red
  if (rowOffset >  MICROLINN_MAX_ROW_OFFSET) rowOffset = microLinnGetSplitWidth(side);   // NOVR
  if (microLinnGCD(rowOffset, Split[side].microLinn.colOffset) != 1) {
    return microLinnWarningColor(side);
  }
  return Split[side].colorMain;
}

byte getMicroLinnColOffsetColor() {                                              // for PerSplit col display
  byte side = Global.currentPerSplit;
  byte colOffset = Split[side].microLinn.colOffset;
  if (colOffset == 1) return Split[side].colorMain;
  byte badColor = microLinnWarningColor(side);

  signed char rowOffset = Split[side].microLinn.rowOffset;
  if (rowOffset > MICROLINN_MAX_ROW_OFFSET) {rowOffset = microLinnGetSplitWidth(side);}  // NOVR
  else if (rowOffset < -MICROLINN_MAX_ROW_OFFSET) {
    rowOffset = Global.rowOffset;
    if (rowOffset == ROWOFFSET_ZERO) {rowOffset = 0;}
    else if (rowOffset == ROWOFFSET_NOOVERLAP) {rowOffset = microLinnGetSplitWidth(side);}
    else if (rowOffset == ROWOFFSET_OCTAVECUSTOM && Global.customRowOffset == -17 && !isMicroLinnOn()) {   // -GUI guitar
      if (microLinnGCD(colOffset, 4) != 1 || microLinnGCD(colOffset, 5) != 1) return badColor;             // 4 = M3 and 5 = P4
      return Split[side].colorMain;
    }
    else if (rowOffset == ROWOFFSET_OCTAVECUSTOM) {
      rowOffset = Global.customRowOffset;
    }
    else if (rowOffset == ROWOFFSET_GUITAR) {
      if (isMicroLinnOn()) {
        for (byte row = 1; row < NUMROWS; ++row) {
          if (microLinnGCD(colOffset, Global.microLinn.guitarTuning[row]) != 1) return badColor;
        }
        return Split[side].colorMain;
      } else {
        for (byte row = 1; row < NUMROWS; ++row) {
          signed char stringGap = Global.guitarTuning[row] - Global.guitarTuning[row - 1];
          if (microLinnGCD(colOffset, stringGap) != 1) return badColor;
        }
        return Split[side].colorMain;
      }
    }
  }

  if (microLinnGCD(colOffset, rowOffset) != 1) return badColor;
  return Split[side].colorMain;
}

void paintMicroLinnDebugDump() {     // delete later from here and from ls_displayModes
  if (sensorRow < 4 && sensorCol <= 16) return;

  //byte* arrayPtr1 = (byte *) &config.device.minUSBMIDIInterval;
  //byte* arrayPtr2 = (byte *) &config.project;
  //int allSettingsSize = arrayPtr2 - arrayPtr1;
  //arrayPtr2 = (byte *) &Global;
  //int deviceSize = (byte *) &Global - arrayPtr1;
  //int MLversion  = (byte *) &Device.microLinn.MLversion - arrayPtr1;
  //int scales     = (byte *) &Device.microLinn.scales - arrayPtr1;
  //int rainbows   = (byte *) &Device.microLinn.rainbows - arrayPtr1;
  //int fretboards = (byte *) &Device.microLinn.fretboards - arrayPtr1;

  byte sp = Global.currentPerSplit;
  if (sensorRow == 6) {
    //if (sensorCol == 4)  paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][25][7], 0, false);
    //if (sensorCol == 16) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportType, 0, false);
    //if (sensorCol == 17) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportSize, 0, false);
    //if (sensorCol == 18) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportCounter, 0, false);
    //if (sensorCol == 20) paintNumericDataDisplay(Split[sp].colorMain, allSettingsSize, 0, false);
    //if (sensorCol == 21) paintNumericDataDisplay(Split[sp].colorMain, deviceSize, 0, false);
    //if (sensorCol == 22) paintNumericDataDisplay(Split[sp].colorMain, MLversion, 0, false);
    //if (sensorCol == 23) paintNumericDataDisplay(Split[sp].colorMain, scales, 0, false);
    //if (sensorCol == 24) paintNumericDataDisplay(Split[sp].colorMain, rainbows, 0, false);
    //if (sensorCol == 25) paintNumericDataDisplay(Split[sp].colorMain, fretboards, 0, false);
  } else if (sensorRow == 5) {
    //byte side = sensorCol % 2;
    //if (sensorCol <= 2)  paintNumericDataDisplay(Split[1-side].colorMain, Split[1-side].microLinn.colOffset, 0, false);
    //if (sensorCol == 3)  paintNumericDataDisplay(Split[sp].colorMain, Global.microLinn.useRainbow, 0, false);
    //if (sensorCol >= 4 && sensorCol <= 15) 
    //                     paintNumericDataDisplay(Split[side].colorMain, config.preset[(sensorCol-4)/2].split[side].microLinn.colOffset, 0, false);
    //if (sensorCol >= 16 && sensorCol <= 21) 
    //                     paintNumericDataDisplay(Split[sp].colorMain, config.preset[sensorCol-16].global.microLinn.useRainbow, 0, false);
  } else if (sensorRow == 4) {
    //if (sensorCol == 1)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(Device), 0, true);
    //if (sensorCol == 2)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(MicroLinnDevice), 0, true);
    //if (sensorCol == 3)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(Device) - sizeof(MicroLinnDevice), 0, true);
    //if (sensorCol == 4)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(GlobalSettings), 0, true);
    //if (sensorCol == 5)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(MicroLinnGlobal), 0, true);
    //if (sensorCol == 6)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(GlobalSettings) - sizeof(MicroLinnGlobal), 0, true);
    //if (sensorCol == 7)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(SplitSettings), 0, true);
    //if (sensorCol == 8)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(MicroLinnSplit), 0, true);
    //if (sensorCol == 9)  paintNumericDataDisplay(Split[sp].colorMain, sizeof(SplitSettings) - sizeof(MicroLinnSplit), 0, true);

    //if (sensorCol == 1)  paintNumericDataDisplay(Split[sp].colorMain, mi.uordblks, 0, false);
    //if (sensorCol == 2)  paintNumericDataDisplay(Split[sp].colorMain, &_end - ramstart, 0, false);
    //if (sensorCol == 3)  paintNumericDataDisplay(Split[sp].colorMain, stack_ptr - heapend + mi.fordblks, 0, false);    

    if (sensorCol == 12) paintNumericDataDisplay(Split[sp].colorMain, updaterVersion,             0, false);
    if (sensorCol == 13) paintNumericDataDisplay(Split[sp].colorMain, updaterMicroLinnVersion,    0, false);
    if (sensorCol == 14) paintNumericDataDisplay(Split[sp].colorMain, updaterSettingsSize,        0, false);
    if (sensorCol == 15) paintNumericDataDisplay(Split[sp].colorMain, updaterImpliedSettingsSize, 0, false);
    if (sensorCol == 16) paintNumericDataDisplay(Split[sp].colorMain, updaterBadBatchNum,         0, false);
    if (sensorCol == 17) paintNumericDataDisplay(Split[sp].colorMain, Device.microLinn.uninstall ? 1 : 0, 0, false);
  }
}

/************** microLinn config functions ************************/

void enterMicroLinnConfig() {                             // called from Global Settings, col 1 row 0 or col 17 row 1
  microLinnConfigNowScrolling = false;
  cellTouched(ignoredCell);
  resetNumericDataChange();
  setDisplayMode(displayMicroLinnConfig);
  updateDisplay();
}

void handleMicroLinnScaleHold() {                         // long-press one of the 9 scale buttons in Global Settings
  if (!isMicroLinnOn()) return;
  if (sensorRow <= 2) {
    calcMicroLinnTuning();                                // note lights display needs to know the current tuning
    setDisplayMode(displayMicroLinnConfig);
    updateDisplay();
    if (microLinnConfigColNum > 0 && microLinnConfigColNum != 12)
      setLed(microLinnConfigColNum, 0, microLinnGetColor(false), cellOn);     // turn off old button
    microLinnConfigColNum = 12;
    setLed(12, 0, microLinnGetColor(true), cellOn);                           // turn on new button
    microLinnPaintNoteLights();
  }
}

byte microLinnGetColor (boolean selected) {
  return selected ? COLOR_CYAN : COLOR_GREEN;
}

void microLinn_draw_string (byte col, byte row, const char* str, byte color, boolean erase) {
  if (isLinn200()) smallfont_draw_string(col, row, str, color, erase);
  else              condfont_draw_string(col, row, str, color, erase);
}

void microLinn_draw_string (byte col, byte row, const char* str, const char* str2, byte color, boolean erase) {
  if (isLinn200()) smallfont_draw_string(col, row, str,  color, erase);
  else              condfont_draw_string(col, row, str2, color, erase);
}

void microLinnScrollSmall (String text) {
  if (displayMode == displayMicroLinnConfig &&
      microLinnConfigColNum > 10 &&
      !isMicroLinnOn()) {
    text = "FIRST SELECT AN EDO";
  }
  char tripleText[200];                                      // 200 to fit the layout message, the longest message
  // leading spaces ensure the string first appears on the right edge, not on the left edge
  snprintf(tripleText, sizeof(tripleText), "      %s     %s     %s", text.c_str(), text.c_str(), text.c_str());
  small_scroll_text(tripleText, microLinnGetColor(false));
}

void microLinnPaintConfigButtons() {
  for (byte col = 2; col <= 16; col += 2) {                              // skip over odd columns
    byte color = (col <= 8 ? COLOR_GREEN : COLOR_LIME);
    if (col == microLinnConfigColNum) color = COLOR_CYAN;
    setLed(col, 0, color, cellOn);
  }
}

void paintMicroLinnConfig() {
  clearDisplay();
  microLinnPaintConfigButtons();

  if (microLinnConfigNowScrolling) {
    switch (microLinnConfigColNum) {
      case  2: microLinnScrollSmall("PER-SPLIT COLUMN OFFSET"); break;
      case  4: microLinnScrollSmall("PER-SPLIT ROW OFFSET"); break;
      case  6: microLinnScrollSmall("PER-SPLIT SETTINGS"); break;
      case  8: microLinnScrollSmall("GLOBAL SETTINGS"); break;
      case 10: microLinnScrollSmall("EDO (NOTES PER OCTAVE)"); break;
      case 12: microLinnScrollSmall("MICROTONAL NOTE LIGHTS"); break;
      case 14: microLinnScrollSmall("PER-SPLIT MICROTONAL SETTINGS"); break;
      case 16: microLinnScrollSmall("GLOBAL MICROTONAL SETTINGS"); break;
    }
    microLinnPaintConfigButtons();
  }

  switch (microLinnConfigColNum) {
    case  2: microLinnPaintColOffset(); break;
    case  4: microLinnPaintRowOffset(); break;
    case  6: microLinnPaintPerSplitSettings(); break;
    case  8: microLinnPaintGlobalSettings(); break;
    case 10: microLinnPaintEDO(); break;
    case 12: if (isMicroLinnOn()) {microLinnPaintNoteLights();} break; 
    case 14: if (isMicroLinnOn()) {microLinnPaintPerSplitXenSettings();} break;
    case 16: if (isMicroLinnOn()) {microLinnPaintGlobalXenSettings();} break;
  }
}

boolean microLinnIsConfigButton() {
  return (sensorRow == 0 && sensorCol <= 16 && sensorCol % 2 == 0);
}

// called from handleNonPlayingTouch() in ls_handleTouches.ino
void handleMicroLinnConfigNewTouch() {
  // start tracking the touch duration to enable hold functionality
  sensorCell->lastTouch = millis();

  microLinnConfigNowScrolling = false;
  if (microLinnIsConfigButton()) {                                             // mostly handled on hold or release
    if (microLinnConfigColNum > 0 && microLinnConfigColNum != sensorCol) {
      setLed(microLinnConfigColNum, 0, microLinnGetColor(false), cellOn);      // turn off old button
    }
    microLinnConfigColNum = sensorCol;
    setLed(microLinnConfigColNum, 0, microLinnGetColor(true), cellOn);         // turn on new button
    updateDisplay();
    return;
  }
  // tap the lower left corner to clear the menu row, handy to remind yourself everything is off
  if (sensorRow == 0 && sensorCol > 1) return;
  
  switch (microLinnConfigColNum) {
    case  2: microLinnHandleColOffsetNewTouch(); break;
    case  4: microLinnHandleRowOffsetNewTouch(); break;
    case  6: microLinnHandlePerSplitSettingsNewTouch(); break;
    case  8: microLinnHandleGlobalSettingsNewTouch(); break;
    case 10: microLinnHandleEdoNewTouch(); break;
    case 12: if (isMicroLinnOn()) microLinnHandleNoteLightsNewTouch(); break;
    case 14: if (isMicroLinnOn()) microLinnHandlePerSplitXenSettingsNewTouch(); break;
    case 16: if (isMicroLinnOn()) microLinnHandleGlobalXenSettingsNewTouch(); break;
  }
  updateDisplay();
}

boolean microLinnIsDisabledConfigButton() {                // a button that doesn't work because we're not in an edo
  return !isMicroLinnOn() && sensorRow == 0 && (sensorCol == 12 || sensorCol == 14 || sensorCol == 16);
}

void handleMicroLinnConfigHold() {
  if (microLinnIsConfigButton() && !microLinnConfigNowScrolling &&            // long-press bottom row (or short-press when no edo is set)
     (isCellPastSensorHoldWait() || microLinnIsDisabledConfigButton())) {     
    microLinnConfigNowScrolling = true;
    paintMicroLinnConfig();                                                   // scroll the name of the button
  }
  if (sensorRow == 0) return;
  switch (microLinnConfigColNum) {
    case  6: microLinnHandlePerSplitSettingsHold(); break;
    case  8: microLinnHandleGlobalSettingsHold(); break;
    case 12: if (isMicroLinnOn()) microLinnHandleNoteLightsHold(); break;
    case 14: if (isMicroLinnOn()) microLinnHandlePerSplitXenSettingsHold(); break;
    case 16: if (isMicroLinnOn()) microLinnHandleGlobalXenSettingsHold(); break;
  }
}

void handleMicroLinnConfigRelease() {
  if (microLinnIsConfigButton() && !isCellPastSensorHoldWait()) {     // short-press bottom row
    if (microLinnConfigColNum == 12 && isMicroLinnOn()) {
      calcMicroLinnTuning();                                          // note lights display needs to know the current tuning
      updateDisplay();
    }
  }
  switch (microLinnConfigColNum) {
    case  6: microLinnHandlePerSplitSettingsRelease(); break;
    case  8: microLinnHandleGlobalSettingsRelease(); break;
    case 12: if (isMicroLinnOn()) microLinnHandleNoteLightsRelease(); break;
    case 14: if (isMicroLinnOn()) microLinnHandlePerSplitXenSettingsRelease(); break;
    case 16: if (isMicroLinnOn()) microLinnHandleGlobalXenSettingsRelease(); break;
  }

  if (sensorRow > 0 && cellsTouched == 0) {
    handleNumericDataReleaseCol(false);
    resetNumericDataChange();
    return;
  }
}

void microLinnPaintShowSplitSelection(boolean linked) {
  if (Global.currentPerSplit == LEFT || linked) {
    setLed(15, 7, Split[LEFT].colorMain, cellOn);
  }
  if (Global.currentPerSplit == RIGHT || linked) {
    setLed(16, 7, Split[RIGHT].colorMain, cellOn);
  }
}

void microLinnHandleShowSplit(boolean& linked) {                         // adds the ability to affect both splits at once
  if (sensorRow == 7 && (sensorCol == 15 || sensorCol == 16)) {          // modelled after handleOctaveTransposeNewTouch()
    linked = cell(15, 7).touched == touchedCell && cell(16, 7).touched == touchedCell;
    handleShowSplit();
    updateDisplay();
  }
}

void microLinnPaintColOffset() {
  byte side = Global.currentPerSplit;
  byte color = getMicroLinnColOffsetColor();
  microLinnPaintShowSplitSelection (microLinnColOffsetLinked);
  if (Split[side].microLinn.colOffset == 1) {
    smallfont_draw_string(1, 1, "OFF", color, false);
  } else {
    paintNumericDataDisplay(color, Split[side].microLinn.colOffset, -4, false);
  }
}

void microLinnHandleColOffsetNewTouch() {
  byte side = Global.currentPerSplit;
  microLinnHandleShowSplit(microLinnColOffsetLinked);
  handleNumericDataNewTouchCol(Split[side].microLinn.colOffset, 1, MICROLINN_MAX_COL_OFFSET, true);
  microLinnSemitonesPerPad[side] = Split[side].microLinn.colOffset;      // assumes 12edo, if not calcTuning() will overwrite this
  microLinnCalcColOffsetCents(side);
  if (microLinnColOffsetLinked) {
    byte otherSide = 1 - side;
    Split[otherSide].microLinn.colOffset = Split[side].microLinn.colOffset;
    microLinnSemitonesPerPad[otherSide] = microLinnSemitonesPerPad[side];
    microLinnCalcColOffsetCents(otherSide);
    updateDisplay();
  }
}

void microLinnPaintRowOffset() {
  byte side = Global.currentPerSplit;
  signed char rowOffset = Split[side].microLinn.rowOffset;
  byte color = getMicroLinnRowOffsetColor();
  if (rowOffset < -MICROLINN_MAX_ROW_OFFSET) {
    smallfont_draw_string(1, 1, "OFF", Split[side].colorMain, false);
  }
  else if (rowOffset > MICROLINN_MAX_ROW_OFFSET) {
    microLinn_draw_string(0, 1, "NOVR", color, false);
  }
  else {
    if (rowOffset != 0) setLed(1, 7, globalColor, cellOn);
    paintNumericDataDisplay(color, rowOffset, -3, false);
  }
  microLinnPaintShowSplitSelection(microLinnRowOffsetLinked);              // paint this last or else linn128 NOVR covers it up
}

void microLinnHandleRowOffsetNewTouch() {
  microLinnHandleShowSplit(microLinnRowOffsetLinked);

  byte side = Global.currentPerSplit;
  signed char rowOffset = Split[side].microLinn.rowOffset;                           // stored as -26=OFF, -25..25, 26=NOVR 
  if (sensorCol == 1 && sensorRow == 7 && abs(rowOffset) <= MICROLINN_MAX_ROW_OFFSET) {
    Split[side].microLinn.rowOffset *= -1;
  } else {
    // convert from "stored as" format to "appears as" format
    boolean isPositive = inRange(rowOffset, 0, MICROLINN_MAX_ROW_OFFSET);
         if (rowOffset == -MICROLINN_MAX_ROW_OFFSET - 1) {rowOffset = -2;}             // OFF
    else if (rowOffset ==  MICROLINN_MAX_ROW_OFFSET + 1) {rowOffset = -1;}             // NOVR = no overlap
    else rowOffset = abs(rowOffset);
    // handle touches
    handleNumericDataNewTouchCol(rowOffset, -2, MICROLINN_MAX_ROW_OFFSET, true);       // appears as OFF, NOVR, 0, ±(1..25)
    // convert back
         if (rowOffset == -2) {rowOffset = -MICROLINN_MAX_ROW_OFFSET - 1;}             // OFF
    else if (rowOffset == -1) {rowOffset =  MICROLINN_MAX_ROW_OFFSET + 1;}             // NOVR
    else if (!isPositive)     {rowOffset *= -1;}
    Split[side].microLinn.rowOffset = rowOffset;
    // handleNumericDataNewTouchCol() called updateDisplay() which called microLinnPaintRowOffset() which painted microLinn.rowOffset
    // but that was before converting back, so microLinn.rowOffset was out of date, so we must call updateDisplay() again
    updateDisplay();
  }
  
  microLinnCalcRowOffsetCents(side);
  if (microLinnRowOffsetLinked) {
    byte otherSide = 1 - side;
    Split[otherSide].microLinn.rowOffset = Split[side].microLinn.rowOffset;
    microLinnCalcRowOffsetCents(otherSide);
    updateDisplay();
  }
}

void microLinnPaintPerSplitSettingsButtons() {
  for (byte row = 3; row <= 7; ++row) {
    byte color = (row >= 4 && row <= 6) ? COLOR_LIME : COLOR_GREEN;
    if (row == microLinnPerSplitSettingsRowNum) color = COLOR_CYAN;
    setLed(1, row, color, cellOn);
  }
}

void microLinnPaintPerSplitSettings() {
  byte side = Global.currentPerSplit;
  byte color = Split[side].colorMain;
  signed char offset;
  microLinnPaintShowSplitSelection(microLinnPerSplitSettingsLinked[microLinnPerSplitSettingsRowNum]);
  microLinnPaintPerSplitSettingsButtons();
  if (microLinnPerSplitSettingsNowScrolling) {
    switch (microLinnPerSplitSettingsRowNum) {
      case 7: microLinnScrollSmall ("MONO FIXES"); break;
      case 6: microLinnScrollSmall ("HAMMER-ON DIRECTION"); break;
      case 5: microLinnScrollSmall ("HAMMER-ON ZONE IN SEMITONES"); break;
      case 4: microLinnScrollSmall ("HAMMER-ON WAIT IN MILLISECONDS"); break;
      case 3: microLinnScrollSmall ("SHOW CUSTOM LIGHT PATTERN"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintShowSplitSelection(microLinnPerSplitSettingsLinked[microLinnPerSplitSettingsRowNum]);
    microLinnPaintPerSplitSettingsButtons();
  }

  switch (microLinnPerSplitSettingsRowNum) {
    case 7:
      switch (Split[side].microLinn.monoFixes) {
        case 0: smallfont_draw_string(2, 1, "OFF", color, false); break;
        case 1: smallfont_draw_string(7, 1, "X",   color, false); break;
        case 2: smallfont_draw_string(7, 1, "Z",   color, false); break;
        case 3: smallfont_draw_string(2, 1, "X+Z", color, false); break;
      }
      break;
    case 6:
      switch (Split[side].microLinn.hammerOnMode) {    
        case 0: smallfont_draw_string(2, 1, "OFF", color, false); break;
        case 1: smallfont_draw_string(7, 1, "R",   color, false); break;
        case 2: smallfont_draw_string(7, 1, "L",   color, false); break;
        case 3: smallfont_draw_string(2, 1, "R+L", color, false); break;
      }
      break;
    case 5:
      if (Split[side].microLinn.hammerOnZone == 121) {
        smallfont_draw_string(2, 1, "ALL", color, false);
      } else if (!isMicroLinnOn()) {
        paintNumericDataDisplay(color, Split[side].microLinn.hammerOnZone / 10, -2, false);      // 12edo displays whole semitones
      } else {
        offset = -2;
        if ((Split[side].microLinn.hammerOnZone / 100)     == 1) offset += 1;      // tens digit
        if ((Split[side].microLinn.hammerOnZone / 10) % 10 == 1) offset += 1;      // ones digit
        paintNumericDataDisplay(color, Split[side].microLinn.hammerOnZone / 10, offset, true);
        setLed(10, 2, color, cellOn);                                              // decimal point
        paintNumericDataDisplay(color, Split[side].microLinn.hammerOnZone % 10, 4, true);
      }
      break;
    case 4:
      offset = 1;
      if (Split[side].microLinn.hammerOnWait < 20) offset = 0;
      if (Split[side].microLinn.hammerOnWait < 10) offset = -2;
      paintNumericDataDisplay(color, 10 * Split[side].microLinn.hammerOnWait, offset, false);
      break;
    case 3:
      switch (Split[side].microLinn.showCustomLEDs) {    
        case 0:         smallfont_draw_string(2, 1, "OFF", color, false); break;
        case 1: case 4: smallfont_draw_string(2, 1, "A",   color, false); break;
        case 2: case 5: smallfont_draw_string(2, 1, "A#",  color, false); break;
        case 3: case 6: smallfont_draw_string(2, 1, "B",   color, false); break;
      }
      if      (Split[side].microLinn.showCustomLEDs == 5) smallfont_draw_string(12, 1, "'", color, false);     // apostrophe
      else if (Split[side].microLinn.showCustomLEDs >= 4) smallfont_draw_string( 6, 1, "'", color, false);
      break;
  }
}

void microLinnHandlePerSplitSettingsNewTouch() {
  microLinnPerSplitSettingsNowScrolling = false;
  if (sensorCol == 1) {
    if (sensorRow >= 3 || sensorRow == 0) {
      microLinnPerSplitSettingsRowNum = sensorRow;
      updateDisplay(); 
    }
    return;
  }

  byte side = Global.currentPerSplit;
  byte otherSide = 1 - side;
  microLinnHandleShowSplit(microLinnPerSplitSettingsLinked[microLinnPerSplitSettingsRowNum]);
  if (microLinnPerSplitSettingsRowNum < 3)
    microLinnPerSplitSettingsLinked[microLinnPerSplitSettingsRowNum] = false;        // don't link if there's nothing to link

  switch (microLinnPerSplitSettingsRowNum) {
    case 7:
      handleNumericDataNewTouchCol(Split[side].microLinn.monoFixes, 0, 3, true);
      if (microLinnPerSplitSettingsLinked[7])
        Split[otherSide].microLinn.monoFixes = Split[side].microLinn.monoFixes;
      break;
    case 6:
      handleNumericDataNewTouchCol(Split[side].microLinn.hammerOnMode, 0, 3, true);
      if (microLinnPerSplitSettingsLinked[6])
        Split[otherSide].microLinn.hammerOnMode = Split[side].microLinn.hammerOnMode;
      break;
    case 5:
      if (isMicroLinnOn()) {
        handleNumericDataNewTouchCol(Split[side].microLinn.hammerOnZone, 1, 121, true);
      } else {
        byte semitones = max(Split[side].microLinn.hammerOnZone / 10, 1);
        if (Split[side].microLinn.hammerOnZone == 121) semitones = 13;
        handleNumericDataNewTouchCol(semitones, 1, 13, true);
        Split[side].microLinn.hammerOnZone = semitones == 13 ? 121 : 10 * semitones;
        // update the display to show the newly-calculated hammerOnZone value
        updateDisplay(); 
      }
      microLinnCalcHammerOnEdosteps(side);
      if (microLinnPerSplitSettingsLinked[5]) {
        Split[otherSide].microLinn.hammerOnZone = Split[side].microLinn.hammerOnZone;
        microLinnCalcHammerOnEdosteps(otherSide);
      }
      break;
    case 4:
      handleNumericDataNewTouchCol(Split[side].microLinn.hammerOnWait, 0, 50, true);       // in hundredths of a second
      if (microLinnPerSplitSettingsLinked[4]) 
        Split[otherSide].microLinn.hammerOnWait = Split[side].microLinn.hammerOnWait;
      break;
    case 3:
      handleNumericDataNewTouchCol(Split[side].microLinn.showCustomLEDs, 0, 6, true);
      if (microLinnPerSplitSettingsLinked[3]) 
        Split[otherSide].microLinn.showCustomLEDs = Split[side].microLinn.showCustomLEDs;
      break;
  }
  updateDisplay(); 
}

void microLinnHandlePerSplitSettingsHold() {
  if (sensorCol == 1 && sensorRow >= 3 &&                                      // long-press col 1
      isCellPastSensorHoldWait() && 
      !microLinnPerSplitSettingsNowScrolling) {     
    microLinnPerSplitSettingsNowScrolling = true;
    microLinnPaintPerSplitSettings();                                          // scroll the name of the button
  }
}

void microLinnHandlePerSplitSettingsRelease() {                                // short-press col 1
  if (sensorCol == 1 && sensorRow >= 3) updateDisplay();
}

void microLinnPaintGlobalSettingsButtons() {
  for (byte row = (isLinn200() ? 3 : 4); row <= 7; ++row) {
    byte color = (row <= 4) ? COLOR_LIME : COLOR_GREEN;
    if (row == microLinnGlobalSettingsRowNum) color = COLOR_CYAN;
    setLed(1, row, color, cellOn);
  }
}

boolean microLinnIsLinnCC(byte CC) {
  // locator and grouping CCs should avoid CCs that the LinnStrument uses for other purposes
  switch (CC) {case 0: case 1: case 7: case 11: case 16: case 17: case 18: case 64: case 65: case 74: return true;}
  // also avoid CCs used by a CC FADERS split
  if (CC >= 1 && CC <= 8) return true;
  // also avoid CCs used for NRPNs
  switch (CC) {case 6: case 38: case 98: case 99: case 100: case 101: return true;}
  // also avoid channel mode CCs
  if (CC >= 120) return true;
  /********** unfinished, probably not worth doing since the official firmware doesn't do any error checking
   * needs Split.ccFaders, ccForFader[8], Global.switchAssignment[5], ccForSwitchCC65[5], ccForSwitchSustain[5]
  if (Split[side].expressionForY == timbreCC1    && CC == 1) return true;
  if (Split[side].expressionForY == timbreCC74   && CC == Split[side].customCCForY) return true;
  if (Split[side].expressionForZ == loudnessCC11 && CC == Split[side].customCCForZ) return true;
  if (Split[side].expressionForZ == loudnessCC11 && Split[side].ccForZ14Bit
                                                 && Split[side].customCCForZ < 32 
                                                 && CC == Split[side].customCCForZ + 32) return true;
  if (Split[side].lowRowMode == lowRowCCX   &&  CC == Split[side].ccForLowRow) return true;    // defaults to CC1
  if (Split[side].lowRowMode == lowRowCCXYZ && (CC == Split[side].ccForLowRowX ||              // defaults to CC16-18
                                                CC == Split[side].ccForLowRowY ||
                                                CC == Split[side].ccForLowRowZ)) return true;
  ******************/
  return false;
}

void microLinnPaintGlobalSettings() {
  microLinnPaintGlobalSettingsButtons();
  if (microLinnGlobalSettingsNowScrolling) {
    switch (microLinnGlobalSettingsRowNum) {
      case 7: microLinnScrollSmall("DRUM PAD MODE"); break;
      case 6: microLinnScrollSmall("SAME/BLINK CARRY OVER"); break;
      case 5: microLinnScrollSmall("ALLOW IMPORTING"); break;
      case 4: microLinnScrollSmall("LOCATOR CC #1"); break;
      case 3: microLinnScrollSmall("LOCATOR CC #2"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintGlobalSettingsButtons();
  }

  byte locatorColor = globalColor;
  boolean matchingCCs = Global.microLinn.locatorCC1 == Global.microLinn.locatorCC2 &&
                        Global.microLinn.locatorCC2 != -1;                                  // locatorCC2 is always -1 on a Linn128
  switch (microLinnGlobalSettingsRowNum) {
    case 7:
      if (Global.microLinn.drumPadMode == 0) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        boolean big = Global.microLinn.drumPadMode == 2;
        smallfont_draw_string(2, 1, big ? "3" : "2", globalColor, false);
        // draw a tiny "x"
        setLed( 8, 3, globalColor, cellOn);
        setLed( 8, 5, globalColor, cellOn);
        setLed( 9, 4, globalColor, cellOn);
        setLed(10, 3, globalColor, cellOn);
        setLed(10, 5, globalColor, cellOn);
        smallfont_draw_string(11, 1, "3", globalColor, false);
      }
      break;
    case 6:
      smallfont_draw_string(2, 1, Global.microLinn.dotsCarryOver ? "ON" : "OFF", globalColor, false);
      break;
    case 5:
      smallfont_draw_string(2, 1, microLinnImportingOn ? "IMP" : "OFF", globalColor, false);
      break;
    case 4:
      if (Global.microLinn.locatorCC1 == -1) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        if (matchingCCs || microLinnIsLinnCC (Global.microLinn.locatorCC1)) locatorColor = COLOR_RED;
        paintNumericDataDisplay(locatorColor, Global.microLinn.locatorCC1, 0, false);
      }
      break;
    case 3:
      if (Global.microLinn.locatorCC2 == -1) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        if (matchingCCs || microLinnIsLinnCC (Global.microLinn.locatorCC2)) locatorColor = COLOR_RED;
        paintNumericDataDisplay(locatorColor, Global.microLinn.locatorCC2, 0, false);
      }
      break;
  }
}

void microLinnHandleGlobalSettingsNewTouch() {
  microLinnGlobalSettingsNowScrolling = false;
  if (sensorCol == 1) {
    byte row = isLinn200() ? 3 : 4;
    if (sensorRow >= row || sensorRow == 0) {
      microLinnGlobalSettingsRowNum = sensorRow;
      updateDisplay(); 
    }
    return;
  }

  byte prevDrumPadMode = Global.microLinn.drumPadMode;
  switch (microLinnGlobalSettingsRowNum) {
    case 7:
      handleNumericDataNewTouchCol(Global.microLinn.drumPadMode, 0, 2, true);
      if (Global.microLinn.drumPadMode > 0 && prevDrumPadMode == 0) {
        // clear all previous pitch bends so all drum hits sound the same
        for (byte ch = 0; ch < 16; ++ch) midiSendPitchBend(0, ch);
      }  
      break;
    case 6:
      handleNumericDataNewTouchCol(Global.microLinn.dotsCarryOver);
      break;
    case 5:
      handleNumericDataNewTouchCol(microLinnImportingOn);
      break;
    case 4:
      handleNumericDataNewTouchCol(Global.microLinn.locatorCC1, -1, 119, true);           // -1 means OFF
      break;
    case 3:
      handleNumericDataNewTouchCol(Global.microLinn.locatorCC2, -1, 119, true);
      break;
  }
  updateDisplay(); 
}

void microLinnHandleGlobalSettingsHold() {
  byte row = isLinn200() ? 3 : 4;
  if (sensorCol == 1 && sensorRow >= row &&                                  // long-press col 1
      isCellPastSensorHoldWait() && 
      !microLinnGlobalSettingsNowScrolling) {     
    microLinnGlobalSettingsNowScrolling = true;
    microLinnPaintGlobalSettings();                                          // scroll the name of the button
  }
}

void microLinnHandleGlobalSettingsRelease() {                                // short-press col 1
  byte row = isLinn200() ? 3 : 4;
  if (sensorCol == 1 && sensorRow >= row) updateDisplay(); 
}

void microLinnPaintEDO() {
  if (Global.microLinn.EDO == 4) {smallfont_draw_string(2, 1, "OFF", globalColor, false);}
  else {paintNumericDataDisplay(globalColor, Global.microLinn.EDO, 0, false);}
}

void microLinnHandleEdoNewTouch() {
  byte prevEDO = Global.microLinn.EDO;
  handleNumericDataNewTouchCol(Global.microLinn.EDO, 4, MICROLINN_MAX_EDO, true);           // 4 means OFF
  if (Global.microLinn.EDO != prevEDO) {
    if (prevEDO == 4) lightSettings = LIGHTS_ACTIVE;
    microLinnAdjustColAndRowOffsets();
  }
}

void microLinnPaintNoteLights() {
  clearDisplay();
  microLinnPaintConfigButtons();

  byte col = 12;       
  byte stepspan = 0;                                                     // the degree (2nd, 3rd, etc.) minus 1
  byte currScale = Global.activeNotes;
  byte color;

  if (currScale <= 7) {                                                  // paint the scale 
    for (byte edostep = 0; edostep < edo; ++edostep) {
      while (edostep > MICROLINN_SCALEROWS[edo][stepspan]) {             // while, not if, to skip over the empty rows of 5n edos
        stepspan += 1;
        col -= MICROLINN_SCALEROWS[edo][stepspan] - MICROLINN_SCALEROWS[edo][stepspan - 1];
      }
      if (microLinnIsScaleStep(edo, edostep)) {
        if (Global.microLinn.useRainbow) {
          color = microLinnRainbowColor(edo, edostep);
        } else {
          color = (edostep == 0 ? Split[Global.currentPerSplit].colorAccent : Split[Global.currentPerSplit].colorMain);
        }
        setLed(col, 7 - (stepspan % 7), color, cellOn);
      }
      col += 1; 
    }
    col = MICROLINN_SCALEROWS[edo][0];                                                // orangish-yellow borders
    for (stepspan = 0; stepspan < 7; ++stepspan) {
      setLed(col + 13, 7 - stepspan, COLOR_YELLOW, cellOn);
    }
    col = MICROLINN_SCALEROWS[edo][1] - MICROLINN_SCALEROWS[edo][0];                  // 2nd - 1sn = major 2nd
    col = max (col, MICROLINN_SCALEROWS[edo][3] - MICROLINN_SCALEROWS[edo][2]);       // 4th - 3rd = minor 2nd
    col -= MICROLINN_SCALEROWS[edo][0];
    for (stepspan = 0; stepspan < 7; ++stepspan) {
      setLed(12 - col, 7 - stepspan, COLOR_YELLOW, cellOn);
    }
  } else if (currScale == 8) {
    paintMicroLinnFretboardEditor(false);                                                 // blue dots
  } 

  for (byte scaleNum = 0; scaleNum < 7; ++scaleNum) {                                     // the 7 scale buttons
    setLed(1, 7 - scaleNum, microLinnGetColor(scaleNum == currScale), cellOn);
  }
  setLed(3, 7, microLinnGetColor(currScale == 7), cellOn);                                // color editor
  if (Global.activeNotes <= 7) 
    setLed(3, 6, Global.microLinn.useRainbow ? COLOR_LIME : COLOR_GREEN, cellOn);         // rainbow enabler
  setLed(3, 4, microLinnGetColor(currScale == 8), cellOn);                                // fretboard selector
//if (Global.microLinn.largeEDO > 0 && Global.activeNotes <= 7)
//  setLed(3, 2, COLOR_LIME, cellOn);                                                     // fine-tuning editor
}

void microLinnCycleRainbow(byte& color) {
  switch (color) {                   // cycle through the colors in rainbow order
    case 8:  color = 1;  break;      // white to red
    case 1:  color = 9;  break;      // red to orange
    case 9:  color = 2;  break;      // orange
    case 2:  color = 10; break;      // yellow (not used)
    case 10: color = 3;  break;      // lime (used as yellow)
    case 3:  color = 4;  break;      // green
    case 4:  color = 5;  break;      // cyan
    case 5:  color = 6;  break;      // blue
    case 6:  color = 11; break;      // magenta
    case 11: color = 8;  break;      // pink
    default: color = 8;  break;      // bad data? start over at white
  }
}

void microLinnHandleNoteLightsNewTouch() {
  if ((sensorCol == 1 && sensorRow > 0) ||                                    // scale selectors or
      (sensorCol == 3 && (sensorRow == 7 || sensorRow == 4))) {               // rainbow editor or fretboard selector
    byte currScale = 7 - sensorRow;                                           // currScale = what the user just touched
    if (sensorCol == 3) currScale = (sensorRow == 7 ? 7 : 8);
    if (Global.activeNotes != currScale) {                                    // did user touch a green button?
      if (Global.activeNotes > 8) {
        loadCustomLedLayer(getActiveCustomLedPattern());
      }
      microLinnPrevScale = Global.activeNotes;
      Global.activeNotes = currScale;
      microLinnCanBacktrack = false;
      updateDisplay(); 
    } else {
      microLinnCanBacktrack = microLinnPrevScale <= 8;                        // can't backtrack to a custom light pattern
    }
    setLed(sensorCol, sensorRow, microLinnGetColor(true), cellSlowPulse);
    return;
  }

  if (sensorCol == 3 && sensorRow == 6 && Global.activeNotes <= 7) {          // rainbow enabler
    Global.microLinn.useRainbow = !Global.microLinn.useRainbow;
    updateDisplay(); 
    return;
  }
  
  byte stepspan = 7 - sensorRow;                                              // the degree (2nd, 3rd, etc.) minus 1
  short edostep = MICROLINN_SCALEROWS[edo][stepspan] 
                - MICROLINN_SCALEROWS[edo][0] + sensorCol - 12;
  if (edostep <= MICROLINN_SCALEROWS[edo][stepspan] &&                        // did we touch a note?
     (edostep >  MICROLINN_SCALEROWS[edo][stepspan-1] ||
     (stepspan == 0 && edostep > MICROLINN_SCALEROWS[edo][6] - edo))) {
    edostep = microLinnMod (edostep, edo);
    if (Global.activeNotes == 7) {                                            // rainbow editor
      if (!Global.microLinn.useRainbow) return;                               // don't let the user make changes they can't see
      short i = microLinnTriIndex(edo, edostep);
      microLinnCycleRainbow(Device.microLinn.rainbows[i]);
    } else if (Global.activeNotes < 7) {
      Device.microLinn.scales[microLinnTriIndex(edo, edostep)] ^= (1 << Global.activeNotes);      // xor to toggle the bit
      if (Device.microLinn.scales[microLinnTriIndex(edo, edostep)] & (1 << Global.activeNotes)) {
        if (edostep > MICROLINN_SCALEROWS[edo][6]) {
          // adjust notes in the first row below the tonic
          edostep -= edo; 
        }
        microLinnSendPreviewNote(edostep);
      }
    }
    updateDisplay(); 
  }
}

void microLinnHandleNoteLightsHold() {
  if (!isCellPastConfirmHoldWait()) return;                                       // long-press 800 ms

  short start = microLinnTriIndex(edo, 0);
  byte currScale = Global.activeNotes;
  // scale selector button, reset the scale
  if (currScale == 7 - sensorRow && sensorCol == 1 && sensorRow > 0) {
    for (byte edostep = 0; edostep < edo; ++edostep) {
      bitWrite (Device.microLinn.scales[start + edostep], currScale, 
               bitRead(MICROLINN_SCALES[start + edostep], currScale));
    }
    updateDisplay();
    return;
  }
  // color editor button, reset the rainbow
  if (currScale == 7 && sensorCol == 3 && sensorRow == 7) {
    memcpy (&Device.microLinn.rainbows[start], &MICROLINN_RAINBOWS[start], edo); 
    updateDisplay();
    return;
  }
  // fretboard selector button, reset the fretboard
  if (currScale == 8 && sensorCol == 3 && sensorRow == 4) {
    microLinnResetFretboard(edo);
    updateDisplay();
  }
}

void microLinnHandleNoteLightsRelease() {
  if (!isCellPastConfirmHoldWait()) {                                           // short-press

    byte currScale = Global.activeNotes;
    if ((sensorCol == 1 && sensorRow >= 1 && currScale == 7 - sensorRow) ||     // scale selector
        (sensorCol == 3 && sensorRow == 7 && currScale == 7) ||                 // rainbow editor
        (sensorCol == 3 && sensorRow == 4 && currScale == 8)) {                 // fretboard selector
      if (microLinnCanBacktrack) {
        Global.activeNotes = microLinnPrevScale;                                // backtrack
        microLinnPrevScale = currScale;
        updateDisplay();
      } else {
        setLed(sensorCol, sensorRow, microLinnGetColor(true), cellOn);          // stop pulsing
      }
    }

    if (sensorCol > 3 && sensorRow > 0 && Global.activeNotes == 8) {            // enter fretboard editor
      setDisplayMode(displayMicroLinnFretboardEditor);
      updateDisplay();
      return;
    }
  }
  if (cellsTouched == 0) {
    ensureGuitarTuningPreviewNoteRelease();
  }
}

void paintMicroLinnFretboardEditor(boolean fullScreen) {             // fullscreen is true only when actually editing
  if (fullScreen) clearDisplay();
  for (byte col = (fullScreen ? 1 : 4); col < NUMCOLS; ++col) {
    byte fret = microLinnGetFret(Global.currentPerSplit, col);
    for (byte row = (fullScreen ? 0 : 1); row < MAXROWS; ++row) {
      if (microLinnDots(edo, fret) & (1 << row)) {
        setLed(col, row, globalColor, cellOn);
      }
    }
  }
}

// called from handleNonPlayingTouch() in ls_handleTouches.ino
void handleMicroLinnFretboardEditorNewTouch() {
  byte fret = microLinnGetFret(Global.currentPerSplit, sensorCol);
  Device.microLinn.fretboards[microLinnTriIndex(edo, fret)] ^= (1 << sensorRow);          // xor to toggle the bit
  updateDisplay();
}

void microLinnPaintPerSplitXenSettingsButtons() {
  for (byte row = 4; row <= 7; ++row) { 
    byte color = (row == 4 || row == 5) ? COLOR_LIME : COLOR_GREEN;
    if (row == microLinnPerSplitXenSettingsRowNum) color = COLOR_CYAN;
    setLed(1, row, color, cellOn);
  }
}

void microLinnPaintPerSplitXenSettings() {
  byte side = Global.currentPerSplit;
  byte color = Split[side].colorMain;
  microLinnPaintShowSplitSelection(microLinnPerSplitXenSettingsLinked[microLinnPerSplitXenSettingsRowNum]);
  microLinnPaintPerSplitXenSettingsButtons();
  if (microLinnPerSplitXenSettingsNowScrolling) {
    switch (microLinnPerSplitXenSettingsRowNum) {
      case 7: microLinnScrollSmall("CONDENSE TO SCALE"); break;
      case 6: microLinnScrollSmall("DEFAULT LAYOUT BOSANQUET ACCORDION WICKI-HAYDEN ARRAY-MBIRA"); break;
      case 5: microLinnScrollSmall("TUNING TABLE + GROUPING + RECHANNELING"); break;
      case 4: microLinnScrollSmall("GROUPING CC"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintShowSplitSelection(microLinnPerSplitXenSettingsLinked[microLinnPerSplitXenSettingsRowNum]);
    microLinnPaintPerSplitXenSettingsButtons();
  }

  switch (microLinnPerSplitXenSettingsRowNum) {
    case 7:
      switch (Split[side].microLinn.condensedBendPerPad) {
        case 0: smallfont_draw_string(2, 1, "OFF", color, false); break;
        case 1: microLinn_draw_string(2, 1, "VAR", color, false); break;
        default: paintNumericDataDisplay(color, Split[side].microLinn.condensedBendPerPad - 1, -2, false);
      }
      break;    
    case 6:
      switch (Split[side].microLinn.defaultLayout) {
        case 0: smallfont_draw_string(2, 1, "OFF",         color, false); break;
        case 1: microLinn_draw_string(2, 1, "BSQ",         color, false); break;
        case 2: microLinn_draw_string(2, 1, "BSQ2", "BS2", color, false); break; 
        case 3: microLinn_draw_string(2, 1, "ACC",         color, false); break;
        case 4: microLinn_draw_string(2, 1, "W-H",  "WH",  color, false); break;
        case 5: microLinn_draw_string(2, 1, "W-H2", "WH2", color, false); break;    // the hyphen keeps the tall 2 away from the split buttons
        case 6: microLinn_draw_string(2, 1, "ARR",         color, false); break;
        case 7: microLinn_draw_string(2, 1, "ARR2", "AR2", color, false); break;
      }
      break;
    case 5:
      if ((Split[side].microLinn.tuningTable == 1 && microLinnNumGroups[side] > 1) ||               // in ON mode, but more than 128 notes?
          (Split[side].microLinn.tuningTable == 2 && Split[side].microLinn.midiGroupCC == -1) ||    // in CC mode, but no CC chosen?
          (Split[side].microLinn.tuningTable == 3 && microLinnNumGroups[side] > 4)) {               // in RCH mode, but too many groups?
        color = microLinnWarningColor(side);
      }
      switch (Split[side].microLinn.tuningTable) {
        case 0: smallfont_draw_string(2, 1, "OFF", color, false); break;
        case 1: smallfont_draw_string(2, 1, "ON",  color, false); break;
        case 2: smallfont_draw_string(2, 1, "CC",  color, false); break;
        case 3: microLinn_draw_string(2, 1, "RCH", color, false); break;
      }
      if (Split[side].microLinn.tuningTable > 0) {                                          // if ON/CC/RCH, paint a column or two of dots
        for (byte i = 0; i < microLinnNumGroups[side]; ++i) {
          byte col = i < 4 ?  15 : 16;
          byte row = i < 4 ? i+2 : i-2;
          if (isLinn200() && Split[side].microLinn.tuningTable == 3) col += 5;
          setLed(col, row, color, cellOn);
        }
      }
      break;
    case 4:
      if (Split[side].microLinn.midiGroupCC == -1) {
        if (Split[side].microLinn.tuningTable == 2) {
          color = microLinnWarningColor(side);
        }
        smallfont_draw_string(2, 1, "OFF", color, false);
      } else {
        if (microLinnIsLinnCC(Split[side].microLinn.midiGroupCC)) color = microLinnWarningColor(side);
        paintNumericDataDisplay(color, Split[side].microLinn.midiGroupCC, isLinn200() ? 0 : 2, !isLinn200());
      }
      break;
  }
}

void microLinnHandlePerSplitXenSettingsNewTouch() {
  microLinnPerSplitXenSettingsNowScrolling = false;
  if (sensorCol == 1) {
    if (sensorRow >= 4 || sensorRow == 0) {
      microLinnPerSplitXenSettingsRowNum = sensorRow;
      updateDisplay(); 
    }
    return;
  }

  byte side = Global.currentPerSplit;
  byte otherSide = 1 - side;
  if (microLinnPerSplitXenSettingsRowNum == 5 && Split[side].microLinn.tuningTable == 3) {     // don't link while rechannelling
    handleShowSplit();
    updateDisplay();
  } else microLinnHandleShowSplit(microLinnPerSplitXenSettingsLinked[microLinnPerSplitXenSettingsRowNum]);
  if (microLinnPerSplitXenSettingsRowNum < 4)
    microLinnPerSplitXenSettingsLinked[microLinnPerSplitXenSettingsRowNum] = false;        // don't link if there's nothing to link

  byte colOffset = Split[side].microLinn.colOffset;
  boolean canRechannel = Split[otherSide].microLinn.tuningTable != 3;                  // user can only rechannel one side at a time
  switch (microLinnPerSplitXenSettingsRowNum) {
    case 7:
      microLinnCalcCondensedScale();
      handleNumericDataNewTouchCol(Split[side].microLinn.condensedBendPerPad, 0, colOffset * microLinnLargestStep + 1, true);
      // assumes 12edo, if not calcTuning() will overwrite this
      switch (Split[side].microLinn.condensedBendPerPad) {
        case 0:  microLinnSemitonesPerPad[side] = colOffset; break;                                       // OFF
        case 1:  break;     // microLinnSemitonesPerPad isn't used                                        // VAR
        default: microLinnSemitonesPerPad[side] = colOffset * (Split[side].microLinn.condensedBendPerPad - 1); break;
      }
      if (microLinnPerSplitXenSettingsLinked[7]) {
        Split[otherSide].microLinn.condensedBendPerPad = Split[side].microLinn.condensedBendPerPad;
        microLinnSemitonesPerPad[otherSide] = microLinnSemitonesPerPad[side];
      }
      break;
    case 6: 
      handleNumericDataNewTouchCol(Split[side].microLinn.defaultLayout, 0, 7, true); 
      microLinnApplyDefaultLayout(side);
      if (microLinnPerSplitXenSettingsLinked[6]) {
        Split[otherSide].microLinn.defaultLayout = Split[side].microLinn.defaultLayout;
        microLinnApplyDefaultLayout(otherSide);
      }
      break;
    case 5: 
      handleNumericDataNewTouchCol(Split[side].microLinn.tuningTable, 0, canRechannel ? 3 : 2, true);
      if (Split[side].microLinn.tuningTable == 3) {
        microLinnSetUpRechannelling(side);
        microLinnPerSplitXenSettingsLinked[5] = false;
      }
      if (microLinnPerSplitXenSettingsLinked[5]) {
        Split[otherSide].microLinn.tuningTable = Split[side].microLinn.tuningTable;
      }
      if (Split[side].microLinn.tuningTable > 0) {
        microLinnCalcLowestEdostepEtc(side);
        if (microLinnPerSplitXenSettingsLinked[5]) {
          microLinnCalcLowestEdostepEtc(otherSide);
        }
      }
      break;
    case 4: 
      handleNumericDataNewTouchCol(Split[side].microLinn.midiGroupCC, -1, 119, true);
      if (microLinnPerSplitXenSettingsLinked[4])
        Split[otherSide].microLinn.midiGroupCC = Split[side].microLinn.midiGroupCC;
      break;
  }
  updateDisplay(); 
}

void microLinnHandlePerSplitXenSettingsHold() {
  if (sensorCol == 1 && sensorRow >= 4 &&                                         // long-press col 1
      isCellPastSensorHoldWait() && 
      !microLinnPerSplitXenSettingsNowScrolling) {     
    microLinnPerSplitXenSettingsNowScrolling = true;
    microLinnPaintPerSplitXenSettings();                                          // scroll the name of the button
  }
}

void microLinnHandlePerSplitXenSettingsRelease() {                                // short-press col 1
  if (sensorCol == 1 && sensorRow >= 4) updateDisplay();
}

void microLinnSetUpRechannelling(byte side) {
  microLinnCalcLowestEdostepEtc(side);
  if (!inRange(microLinnNumGroups[side], 2, 4)) return;
  Split[side].midiMode = channelPerNote;
  Split[side].midiChanMain = side == LEFT ? 1 : 16;
  Split[side].midiChanMainEnabled = microLinnNumGroups[side] < 4;
  memset(&Split[side].midiChanSet, false, 16);
  if (side == LEFT) {
    if (microLinnNumGroups[side] == 2) memset(&Split[side].midiChanSet[1], true, 7);    // 2-8
    if (microLinnNumGroups[side] == 3) memset(&Split[side].midiChanSet[1], true, 5);    // 2-6
    if (microLinnNumGroups[side] == 4) memset(&Split[side].midiChanSet[0], true, 4);    // 1-4
  } else {
    if (microLinnNumGroups[side] == 2) memset(&Split[side].midiChanSet[8],  true, 7);   // 9-15
    if (microLinnNumGroups[side] == 3) memset(&Split[side].midiChanSet[10], true, 5);   // 11-15
    if (microLinnNumGroups[side] == 4) memset(&Split[side].midiChanSet[12], true, 4);   // 13-16
  }
}

void microLinnPaintGlobalXenSettingsButtons() {
  for (byte row = 3; row <= 7; ++row) {        // change 3 to 1 for sweetening and fine tuning
    byte color = (row == 4 || row == 3 || row == 1) ? COLOR_LIME : COLOR_GREEN;
    if (row == microLinnGlobalXenSettingsRowNum) color = COLOR_CYAN;
    setLed(1, row, color, cellOn);
  }
}

void microLinnPaintGlobalXenSettings() {
  microLinnPaintGlobalXenSettingsButtons();
  if (microLinnGlobalXenSettingsNowScrolling) {
    switch (microLinnGlobalXenSettingsRowNum) {
      case 7: microLinnScrollSmall("ANCHOR PAD"); break;
      case 6: microLinnScrollSmall("ANCHOR NOTE"); break;
      case 5: microLinnScrollSmall("ANCHOR CENTS"); break;
      case 4: microLinnScrollSmall("EQUAVE SEMITONES"); break;
      case 3: microLinnScrollSmall("EQUAVE STRETCH IN CENTS"); break;
    //case 2: microLinnScrollSmall("41EDO SWEETENING IN CENTS"); break;
    //case 1: microLinnScrollSmall("LARGE EDO FOR FINE TUNING"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintGlobalXenSettingsButtons();
  }

  signed char offset = 0;
  switch (microLinnGlobalXenSettingsRowNum) {
    case 7:
      offset = (!isLinn200() && inRange(Global.microLinn.anchorCol, 2, 9)) ? 1 : 2;
      microLinn_draw_string(offset, 1, microLinnAnchorString, globalColor, false);
      break;
    case 6:
      paintNoteDataDisplay(globalColor, Global.microLinn.anchorNote, 2);
      break;
    case 5:
      if (Global.microLinn.anchorCents <= -10) offset = -3;
      paintNumericDataDisplay(globalColor, Global.microLinn.anchorCents, offset, false);
      break;
    case 4:
      paintNumericDataDisplay(globalColor, Global.microLinn.equaveSemitones, 0, false);
      break;
    case 3: 
      if (Global.microLinn.equaveStretch <= -10) offset = -3;
      paintNumericDataDisplay(globalColor, Global.microLinn.equaveStretch, offset, false);
      break;
    /*********************
    case 2:
      if (Global.microLinn.sweeten == 0) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        offset = -5;
        if (inRange (Global.microLinn.sweeten, 10, 19)) offset = -3;
        paintNumericDataDisplay(globalColor, Global.microLinn.sweeten / 10, offset, false);
        setLed (9, 2, globalColor, cellOn);   // decimal point
        paintNumericDataDisplay(globalColor, Global.microLinn.sweeten % 10, 2, false);
      }
      break;
    case 1:
      if (MICROLINN_LARGE_EDO[Global.microLinn.largeEDO] >= 200) offset = 2;
      if (Global.microLinn.largeEDO == 0) {smallfont_draw_string(2, 1, "OFF", globalColor, false);}
      else if (Global.microLinn.largeEDO == 53) {microLinn_draw_string(2, 1, "1200",  globalColor, false);}
      else {paintNumericDataDisplay(globalColor, MICROLINN_LARGE_EDO[Global.microLinn.largeEDO], offset, false);}
      break;
    *******************/
  }
}

void microLinnHandleGlobalXenSettingsNewTouch() {
  microLinnGlobalXenSettingsNowScrolling = false;
  if (sensorCol == 1) {
    if (sensorRow >= 3 || sensorRow == 0) {       // change 3 to 1 for sweetening and fine tuning
      microLinnGlobalXenSettingsRowNum = sensorRow;
      updateDisplay(); 
    }
    return;
  }

  if (!isMicroLinnOn()) return;
  switch (microLinnGlobalXenSettingsRowNum) {
    case 7:
      // anchor cell is handled elsewhere
      break;
    case 6:
      handleNumericDataNewTouchCol(Global.microLinn.anchorNote, 0, 127, true);
      microLinnSendPreviewNote(0);
      break;
    case 5:
      handleNumericDataNewTouchCol(Global.microLinn.anchorCents, -60, 60, true); 
      microLinnSendPreviewNote(0);
      break;
    case 4:
      handleNumericDataNewTouchCol(Global.microLinn.equaveSemitones, 5, 36, true);
      microLinnCalcHammerOnEdosteps(LEFT);
      microLinnCalcHammerOnEdosteps(RIGHT);
      break;
    case 3:
      handleNumericDataNewTouchCol(Global.microLinn.equaveStretch, -50, 50, true);
      microLinnCalcHammerOnEdosteps(LEFT);
      microLinnCalcHammerOnEdosteps(RIGHT);
      break;
    /*******************
    case 2:
      handleNumericDataNewTouchCol(Global.microLinn.sweeten, 0, 60, true);       // in tenths of a cent
      break;
    case 1:
      handleNumericDataNewTouchCol(Global.microLinn.largeEDO, 0, MICROLINN_NUM_LARGE_EDOS, true);
      break;
    *******************/
  }
  updateDisplay(); 
}

void microLinnHandleGlobalXenSettingsHold() {
  if (sensorCol == 1 && sensorRow >= 3 &&        // change 3 to 1     // long-press col 1
      isCellPastSensorHoldWait() && 
      !microLinnGlobalXenSettingsNowScrolling) {     
    microLinnGlobalXenSettingsNowScrolling = true;
    microLinnPaintGlobalXenSettings();                                // scroll the name of the button
  }
}

void microLinnHandleGlobalXenSettingsRelease() {                      // short-press col 1
  if (sensorCol == 1 && sensorRow >= 3) {        // change 3 to 1 for sweetening and fine tuning
    updateDisplay(); 
  } else if (sensorRow > 0 && cellsTouched == 0) {
    handleNumericDataReleaseCol(false);
    resetNumericDataChange();
    if (microLinnGlobalXenSettingsRowNum == 7) {
      calcMicroLinnTuning();                                          // anchor cell chooser needs to know the current tuning
      setDisplayMode(displayMicroLinnAnchorChooser);
      updateDisplay();
    }
    else if (microLinnGlobalXenSettingsRowNum == 5 || microLinnGlobalXenSettingsRowNum == 6) {
      ensureGuitarTuningPreviewNoteRelease();
    }
  }
}

// called from handleNonPlayingTouch() in ls_handleTouches.ino
void handleMicroLinnAnchorChooserNewTouch() {
  Global.microLinn.anchorCol = sensorCol;
  Global.microLinn.anchorRow = sensorRow;
  calcMicroLinnTuning();                                   // update the edostep of each cell, and hence the color
  paintNormalDisplay();
  cellTouched(ignoredCell);                                // so that choosing a row 0 note won't trigger a scrolling message
  delayUsec(1000000);
  microLinnUpdateAnchorString();
  setDisplayMode(displayMicroLinnConfig); 
  updateDisplay(); 
}

/************** guitar tuning functions ************************/

void paintMicroLinnGuitarTuning() {
  byte anchor = Global.microLinn.anchorRow;
  byte color;
  clearDisplay();
  for (byte r = 0; r < NUMROWS; ++r) {
    if      (r == microLinnGuitarTuningRowNum)                     color = COLOR_CYAN;    // cyan hides red because the number will be red
    else if (getMicroLinnRowOffsetColorGuitar3(r) == COLOR_RED)    color = COLOR_RED;
    else if (r == microLinnGuitarTuningRowNum - 1 && r >= anchor)  color = COLOR_BLUE;
    else if (r == microLinnGuitarTuningRowNum + 1 && r <= anchor)  color = COLOR_BLUE;
    else                                                           color = COLOR_GREEN;
    setLed(1, r, color, cellOn);
    if (r == anchor) setLed(2, r, color, cellOn);
  }
  byte r = microLinnGuitarTuningRowNum;
  if (r >= NUMROWS) return;                                                          // r == NUMROWS+1 means the rownum hasn't been set yet
  if (r == anchor) {
    if (Split[LEFT].microLinn.condensedBendPerPad  > 0 ||
        Split[RIGHT].microLinn.condensedBendPerPad > 0) {
      byte col = 3;
      if (!isLinn200() && Global.microLinn.guitarTuning[0] == 1) col = 1;
      microLinn_draw_string(col, 0, Global.microLinn.guitarTuning[0] == 0 ? "DIA" : "CHRO", globalColor, false);
    }
    return;
  }
  if (r < anchor) r += 1;
  color = getMicroLinnRowOffsetColorGuitar4(r);
  paintNumericDataDisplay(color, Global.microLinn.guitarTuning[r], 0, false);
}

void handleMicroLinnGuitarTuningNewTouch() {
  if (sensorCol == 2 && sensorRow != Global.microLinn.anchorRow) return;
  if (sensorCol <= 2) {
    microLinnGuitarTuningRowNum = sensorRow;
    updateDisplay();
  }
  else if (microLinnGuitarTuningRowNum == Global.microLinn.anchorRow) {
    if (Split[LEFT].microLinn.condensedBendPerPad  > 0 ||
        Split[RIGHT].microLinn.condensedBendPerPad > 0) {
      handleNumericDataNewTouchCol(Global.microLinn.guitarTuning[0], 0, 1, true);
    }
  }
  else {
    byte row = microLinnGuitarTuningRowNum;
    if (row < Global.microLinn.anchorRow) row += 1;
    short loMax, loMin, hiMax, hiMin;
    // find max and min open string notes in lower part of the guitar, below the current string
    loMax = loMin = microLinnGuitarTuning[0];
    for (byte r = 1; r < row; ++r) {
      if (microLinnGuitarTuning[r] >= loMax) {
        loMax = microLinnGuitarTuning[r];
      }
      if (microLinnGuitarTuning[r] < loMin) {
        loMin = microLinnGuitarTuning[r];
      }
    }
    // find max and min open string notes in higher part of the guitar, above the current string
    hiMax = hiMin = microLinnGuitarTuning[row];
    for (byte r = row + 1; r < NUMROWS; ++r) {
      if (microLinnGuitarTuning[r] >= hiMax) {
        hiMax = microLinnGuitarTuning[r];
      }
      if (microLinnGuitarTuning[r] < hiMin) {
        hiMin = microLinnGuitarTuning[r];
      }
    }
    // wiggleRoom = MAX_RANGE - (max - min), ceiling = currVal + wiggleRoom, floor = currVal - wiggleRoom
    short ceiling = Global.microLinn.guitarTuning[row] + MICROLINN_MAX_GUITAR_RANGE - (hiMax - loMin);
    short floor   = Global.microLinn.guitarTuning[row] - MICROLINN_MAX_GUITAR_RANGE + (loMax - hiMin);
    handleNumericDataNewTouchCol(Global.microLinn.guitarTuning[row], floor, ceiling, true);
    microLinnCalcGuitarRowOffsetCents(row);
    microLinnCalcGuitarTuning();
  }

  if (microLinnGuitarTuningRowNum == Global.microLinn.anchorRow && sensorCol > 2) return;
  signed char direction = isLeftHandedSplit(Global.currentPerSplit) ? -1 : 1;
  short edostep = microLinnGuitarTuning[microLinnGuitarTuningRowNum]
                - direction * Split[Global.currentPerSplit].microLinn.colOffset * (Global.microLinn.anchorCol - 1);
  microLinnSendPreviewNote(edostep);
}

/************** octave transpose functions ************************/

void  paintMicroLinnEdostepTranspose(byte side) {
  if (edo <= 12) return;
  if (!doublePerSplit || Split[LEFT].microLinn.transposeEDOsteps == Split[RIGHT].microLinn.transposeEDOsteps) {
    paintTranspose(Split[Global.currentPerSplit].colorMain, SWITCH_2_ROW, Split[side].microLinn.transposeEDOsteps);
  }
  else if (doublePerSplit) {
    if (abs(Split[LEFT].microLinn.transposeEDOsteps) > abs(Split[RIGHT].microLinn.transposeEDOsteps)) {
      paintTranspose(Split[LEFT].colorMain,  SWITCH_2_ROW, Split[LEFT].microLinn.transposeEDOsteps);
      paintTranspose(Split[RIGHT].colorMain, SWITCH_2_ROW, Split[RIGHT].microLinn.transposeEDOsteps);
    }
    else {
      paintTranspose(Split[RIGHT].colorMain, SWITCH_2_ROW, Split[RIGHT].microLinn.transposeEDOsteps);
      paintTranspose(Split[LEFT].colorMain,  SWITCH_2_ROW, Split[LEFT].microLinn.transposeEDOsteps);
    }
  }
}

void paintMicroLinnOctaveTransposeButton() {
  if (!isMicroLinnOn()) return;
  // light the octave/transpose switch if the pitch is transposed by edosteps
  if ((Split[LEFT].microLinn.transposeEDOsteps < 0  && Split[RIGHT].microLinn.transposeEDOsteps < 0) ||
      (Split[LEFT].microLinn.transposeEDOsteps < 0  && Split[RIGHT].microLinn.transposeEDOsteps == 0) ||
      (Split[LEFT].microLinn.transposeEDOsteps == 0 && Split[RIGHT].microLinn.transposeEDOsteps < 0)) {
    setLed(0, OCTAVE_ROW, COLOR_RED, cellOn);
  }
  else if ((Split[LEFT].microLinn.transposeEDOsteps >  0 && Split[RIGHT].microLinn.transposeEDOsteps > 0) ||
           (Split[LEFT].microLinn.transposeEDOsteps >  0 && Split[RIGHT].microLinn.transposeEDOsteps == 0) ||
           (Split[LEFT].microLinn.transposeEDOsteps == 0 && Split[RIGHT].microLinn.transposeEDOsteps > 0)) {
    setLed(0, OCTAVE_ROW, COLOR_GREEN, cellOn);
  }
  else if (Split[LEFT].microLinn.transposeEDOsteps != 0 && Split[RIGHT].microLinn.transposeEDOsteps != 0) {
    setLed(0, OCTAVE_ROW, COLOR_YELLOW, cellOn);
  }
  else {
    clearLed(0, OCTAVE_ROW);
  }
}

/************** midi send/receive functions ************************/

// used for guitar tuning, anchor note, anchor cents and note lights
void microLinnSendPreviewNote (short edostep) {                                                // edosteps from the anchor cell
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;      // midi note with 2 decimal places for cents
  edostepSize = (Global.microLinn.equaveSemitones + Global.microLinn.equaveStretch / 100.0) / edo;     // edo's step size in 12edo semitones
  float note = anchorPitch + edostep * edostepSize;
  note += microLinnTransposition(Global.currentPerSplit);
  if (note < 0 || note > 127) return;
  ensureGuitarTuningPreviewNoteRelease();                                // re-use the standard guitar tuning functions & vars
  guitarTuningPreviewNote = round(note);
  note -= guitarTuningPreviewNote;
  note *= 8192.0 / getBendRange(Global.currentPerSplit);                 // fine-tuning bend as a zero-centered signed 13-bit integer
  short bend = round(note);
  guitarTuningPreviewChannel = takeChannel(Global.currentPerSplit, sensorRow);
  midiSendPitchBend(bend, guitarTuningPreviewChannel);
  midiSendNoteOn(Global.currentPerSplit, guitarTuningPreviewNote, 96, guitarTuningPreviewChannel);
}

// called from sendNewNote() in ls_handleTouches.ino
void sendMicroLinnMidiGroupAndLocatorCCs(byte channel, byte side) {
  // indicates midi group when tuning table mode = CC, sent before every played (not arpeggiated, strummed or sequenced) note-on
  // channel differs from sensorCell->channel when in tuning table mode and rechanneling
  if (Split[side].microLinn.tuningTable == 2 && Split[side].microLinn.midiGroupCC >= 0) {
    midiSendControlChange(Split[side].microLinn.midiGroupCC, sensorCell->microLinnGroup + 1, channel, true); 
  }
  // indicates pad's row/column, sent before every played (not arpeggiated, strummed or sequenced) note-on
  // thanks to KVR forum member vorp40 for the idea!
  if (sensorCol <= 16 && Global.microLinn.locatorCC1 >= 0)
    midiSendControlChange(Global.microLinn.locatorCC1, (7 - sensorRow) + 8 * (sensorCol - 1), channel, true);
  if (sensorCol >= 17 && Global.microLinn.locatorCC2 >= 0)
    midiSendControlChange(Global.microLinn.locatorCC2, (7 - sensorRow) + 8 * (sensorCol - 17), channel, true);
}

// called when nrpn 299 with a value between 1000 and 2048 is received
void sendMicroLinnNrpnParameter(int parameter, int channel) {
  byte side = LEFT;
  int value = INT_MIN;
  int param = parameter;
  if (param >= 1100 && param < 1200) {
    param -= 100;
    side = RIGHT;
  }
  short n = MICROLINN_MAX_GUITAR_RANGE;
  switch (param) {
    case 1000: value = Split[side].microLinn.colOffset; break;
    case 1001: value = Split[side].microLinn.rowOffset + MICROLINN_MAX_ROW_OFFSET + 1; break;
    case 1002: value = Split[side].microLinn.monoFixes; break;
    case 1003: value = Split[side].microLinn.hammerOnMode; break;
    case 1004: value = Split[side].microLinn.hammerOnZone; break;
    case 1005: value = Split[side].microLinn.hammerOnWait; break;
    case 1006: value = Split[side].microLinn.showCustomLEDs; break;
    case 1007: value = Split[side].microLinn.ccForLowRowW; break;
    case 1008: value = Split[side].microLinn.ccForLowRowX; break;
    case 1009: value = Split[side].microLinn.ccForLowRowY; break;
    case 1010: value = Split[side].microLinn.WccCentered() ? 1 : 0; break;
    case 1011: value = Split[side].microLinn.XccCentered() ? 1 : 0; break;
    case 1012: value = Split[side].microLinn.YccCentered() ? 1 : 0; break;
    case 1050: value = Split[side].microLinn.condensedBendPerPad; break;
    case 1051: value = Split[side].microLinn.defaultLayout; break;
    case 1052: value = Split[side].microLinn.tuningTable; break;
    case 1053: value = Split[side].microLinn.midiGroupCC + 1; break;
    case 1054: value = Split[side].microLinn.transposeEDOsteps + 7; break;
    case 1200: value = Global.microLinn.drumPadMode; break;
    case 1201: value = Global.microLinn.dotsCarryOver ? 1 : 0; break;
    case 1202: value = Global.microLinn.locatorCC1 + 1; break;
    case 1203: value = Global.microLinn.locatorCC2 + 1; break;
    case 1250: value = Global.microLinn.EDO; break;
    case 1251: value = Global.microLinn.useRainbow ? 1 : 0; break;
    case 1252: value = Global.microLinn.anchorCol; break;
    case 1253: value = Global.microLinn.anchorRow; break;
    case 1254: value = Global.microLinn.anchorNote; break;
    case 1255: value = Global.microLinn.anchorCents + 60; break;
    case 1256: value = Global.microLinn.equaveSemitones; break;
    case 1257: value = Global.microLinn.equaveStretch + 50; break;
    case 1258: value = Global.microLinn.sweeten; break;
    case 1259: value = Global.microLinn.largeEDO; break;
    case 1260: value = Global.microLinn.guitarTuning[0]; break;
    case 1261: value = Global.microLinn.guitarTuning[1] + n; break;
    case 1262: value = Global.microLinn.guitarTuning[2] + n; break;
    case 1263: value = Global.microLinn.guitarTuning[3] + n; break;
    case 1264: value = Global.microLinn.guitarTuning[4] + n; break;
    case 1265: value = Global.microLinn.guitarTuning[5] + n; break;
    case 1266: value = Global.microLinn.guitarTuning[6] + n; break;
    case 1267: value = Global.microLinn.guitarTuning[7] + n; break;
  }
  if (value != INT_MIN) {
    midiSendNRPN(parameter, value, channel);
  }
}

// called when a nrpn with parameter >= 1000 is received
void receivedMicroLinnNrpn(int parameter, int value) {
  byte side = LEFT;
  if (parameter >= 1100 && parameter < 1200) {
    parameter -= 100;
    side = RIGHT;
  }
  short n = MICROLINN_MAX_GUITAR_RANGE;
  byte c = MICROLINN_MAX_COL_OFFSET;
  byte r = MICROLINN_MAX_ROW_OFFSET + 1;
  switch (parameter) {
    case 1000: if (inRange(value, 1, c))   Split[side].microLinn.colOffset = value; break;
    case 1001: if (inRange(value, 0, 2*r)) Split[side].microLinn.rowOffset = value - r; break;
    case 1002: if (inRange(value, 0, 3))   Split[side].microLinn.monoFixes = value; break;
    case 1003: if (inRange(value, 0, 3))   Split[side].microLinn.hammerOnMode = value; break;
    case 1004: if (inRange(value, 1, 121)) Split[side].microLinn.hammerOnZone = value; break;
    case 1005: if (inRange(value, 0, 50))  Split[side].microLinn.hammerOnWait = value; break;
    case 1006: if (inRange(value, 0, 6))   Split[side].microLinn.showCustomLEDs = value; break;
    case 1007: if (inRange(value, 0, 128)) Split[side].microLinn.ccForLowRowW = value; break;
    case 1008: if (inRange(value, 0, 128)) Split[side].microLinn.ccForLowRowX = value; break;
    case 1009: if (inRange(value, 0, 128)) Split[side].microLinn.ccForLowRowY = value; break;
    case 1010: if (inRange(value, 0, 1))   Split[side].microLinn.setWccCentered(value == 1); break;
    case 1011: if (inRange(value, 0, 1))   Split[side].microLinn.setXccCentered(value == 1); break;
    case 1012: if (inRange(value, 0, 1))   Split[side].microLinn.setYccCentered(value == 1); break;
    case 1050: if (inRange(value, 0, 55))  Split[side].microLinn.condensedBendPerPad = value; break;
    case 1051: if (inRange(value, 0, 7))   Split[side].microLinn.defaultLayout = value; break;
    case 1052: if (inRange(value, 0, 3))   Split[side].microLinn.tuningTable = value; break;
    case 1053: if (inRange(value, 0, 120)) Split[side].microLinn.midiGroupCC = value - 1; break;
    case 1054: if (inRange(value, 0, 14))  Split[side].microLinn.transposeEDOsteps = value - 7; break;
    case 1200: if (inRange(value, 0, 2))   Global.microLinn.drumPadMode = value; break;
    case 1201: if (inRange(value, 0, 1))   Global.microLinn.dotsCarryOver = (value == 1); break;
    case 1202: if (inRange(value, 0, 120)) Global.microLinn.locatorCC1 = value - 1; break;
    case 1203: if (inRange(value, 0, 120)) Global.microLinn.locatorCC2 = isLinn200() ? value - 1 : -1; break;
    case 1250: if (inRange(value, 4, 55))  Global.microLinn.EDO = value; break;
    case 1251: if (inRange(value, 0, 1))   Global.microLinn.useRainbow = (value == 1); break;
    case 1252: if (inRange(value, 1, 25))  Global.microLinn.anchorCol = value; break;
    case 1253: if (inRange(value, 0, 7))   Global.microLinn.anchorRow = value; break;
    case 1254: if (inRange(value, 0, 127)) Global.microLinn.anchorNote = value; break;
    case 1255: if (inRange(value, 0, 120)) Global.microLinn.anchorCents = value - 60; break;
    case 1256: if (inRange(value, 1, 36))  Global.microLinn.equaveSemitones = value; break;
    case 1257: if (inRange(value, 0, 100)) Global.microLinn.equaveStretch = value - 50; break;
    case 1258: if (inRange(value, 0, 60))  Global.microLinn.sweeten = value; break;
    case 1259: if (inRange(value, 0, 53))  Global.microLinn.largeEDO = value; break;
    case 1260: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[0] = value; break;
    case 1261: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[1] = value - n; break;
    case 1262: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[2] = value - n; break;
    case 1263: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[3] = value - n; break;
    case 1264: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[4] = value - n; break;
    case 1265: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[5] = value - n; break;
    case 1266: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[6] = value - n; break;
    case 1267: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[7] = value - n; break;
    case 1300: setupMicroLinn();
  }
  updateDisplay();
}

// called when nrpn 299 with a value > 2048 is received, uses polypressure not sysex because not all DAWs support sysexes,
// also there's not enough RAM to hold the larger imports in memory before overwriting user settings,
// so we must overwrite as soon as we receive each byte, so the sysex's CRC would be useless anyway
void exportMicroLinnData(int exportType) {
  exportType -= 2048;
//if (exportType < 1 || exportType > 16)                      {microLinnScrollSmall("INVALID EXPORT TYPE"); return;}
// 17 is just for testing, delete later
  if (exportType < 1 || exportType > 17)                      {microLinnScrollSmall("INVALID EXPORT TYPE"); return;}
  if (exportType == 1 && Global.activeNotes < 9)              {microLinnScrollSmall("FIRST SELECT A LIGHT PATTERN"); return;}
  if (exportType == 3 && audienceMessageToEdit == -1)         {microLinnScrollSmall("FIRST EDIT AN AUDIENCE MESSAGE"); return;}
  if (exportType >= 6 && exportType <= 8 && !isMicroLinnOn()) {microLinnScrollSmall("FIRST SELECT AN EDO"); return;}
  if (exportType == 13 && Device.lastLoadedPreset == -1)      {microLinnScrollSmall("FIRST LOAD A MEMORY"); return;}

if (exportType < 17) {        // for testing, delete this line later
  byte midiData = inRange(exportType, 5, 8) ? Global.microLinn.EDO : 0;             // the edo says where to load import types 5-8
  midiSendNRPN(300, (exportType << 7) + midiData, 1);
  microLinnSendPolyPressure(Device.version, Device.microLinn.MLversion, 9);         // export header, channel is 1-indexed
}
  
  byte* array;   // used to configure complex structs as a simple array of bytes
  short n = microLinnTriIndex(edo, 0);

  switch (exportType) {
    case 1:
      for (unsigned short i = 0; i < LED_LAYER_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.customLeds[Global.activeNotes - 9][i], 
                                  Device.customLeds[Global.activeNotes - 9][i+1]);
      }
      break;
    case 2:
      for (byte i = 0; i < 3; ++i) {
        for (unsigned short j = 0; j < LED_LAYER_SIZE; j += 2) {
          microLinnSendPolyPressure(Device.customLeds[i][j], 
                                    Device.customLeds[i][j+1]);
        }
      }
      break;
    case 3:
      for (byte i = 0; i < 30; i += 2) {                                        // don't send the final null-terminator
        microLinnSendPolyPressure(Device.audienceMessages[audienceMessageToEdit][i], 
                                  Device.audienceMessages[audienceMessageToEdit][i+1]);
      }
      break;
    case 4:
      for (byte i = 0; i < 16; ++i) {
        for (byte j = 0; j < 30; j += 2) {
          microLinnSendPolyPressure(Device.audienceMessages[i][j], 
                                    Device.audienceMessages[i][j+1]);
        }
      }
      break;
    case 5: 
      if (Global.microLinn.EDO == 4) {
        for (byte i = 0; i < 9; i += 1) {microLinnSendPolyPressure(Global.mainNotes[i]);}
        for (byte i = 0; i < 9; i += 1) {microLinnSendPolyPressure(Global.accentNotes[i]);}
      } else {
        for (byte i = 0; i < edo; i += 2) {
          microLinnSendPolyPressure(Device.microLinn.scales[n+i],
                                    Device.microLinn.scales[n+i+1]);
        }
      }
      break;
    case 6:
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.rainbows[n+i],
                                  Device.microLinn.rainbows[n+i+1]);
      }
      break;
    case 7:
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.fretboards[n+i],
                                  Device.microLinn.fretboards[n+i+1]);
      }
      break;
    case 8:   // all edo data for the current edo only  (large EDO offsets is not yet included)
      microLinnSendPolyPressure(Global.microLinn.EDO, 
                                Global.microLinn.useRainbow ? 1 : 0);
      microLinnSendPolyPressure(Global.microLinn.equaveSemitones, 
                                Global.microLinn.equaveStretch + 50);
      microLinnSendPolyPressure(Global.microLinn.sweeten, 
                                Global.microLinn.largeEDO);
      microLinnSendPolyPressure(Global.rowOffset, 
                                Global.customRowOffset + MICROLINN_MAX_ROW_OFFSET);
      for (byte side = 0; side < NUMSPLITS; ++side) {
        microLinnSendPolyPressure(Split[side].microLinn.colOffset,
                                  Split[side].microLinn.rowOffset + MICROLINN_MAX_ROW_OFFSET + 1);
      }
      microLinnSendPolyPressure(Global.microLinn.guitarTuning[0]);
      for (byte i = 1; i < MAXROWS; i += 1) {
        microLinnSendPolyPressure(Global.microLinn.guitarTuning[i] + MICROLINN_MAX_GUITAR_RANGE);
      }
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.scales[n+i], 
                                  Device.microLinn.scales[n+i+1]);
      }
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.rainbows[n+i], 
                                  Device.microLinn.rainbows[n+i+1]);
      }
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.fretboards[n+i], 
                                  Device.microLinn.fretboards[n+i+1]);
      }
      break;
    case 9: 
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.scales[i], 
                                  Device.microLinn.scales[i+1]);
      }
      break;
    case 10:
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.rainbows[i], 
                                  Device.microLinn.rainbows[i+1]);
      }
      break;
    case 11:
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.fretboards[i], 
                                  Device.microLinn.fretboards[i+1]);
      }
      break;
    case 12: 
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.scales[i], 
                                  Device.microLinn.scales[i+1]);
      }
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.rainbows[i], 
                                  Device.microLinn.rainbows[i+1]);
      }
      for (short i = 0; i < MICROLINN_ARRAY_SIZE; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.fretboards[i], 
                                  Device.microLinn.fretboards[i+1]);
      }
      break;
    case 13:   // 1 preset
      array = (byte *) &config.preset[Device.lastLoadedPreset];
      for (unsigned short i = 0; i < sizeof(PresetSettings); i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
      }
      break;
    case 14:   // all 6 presets
      array = (byte *) &config.preset[0];
      for (unsigned short i = 0; i < NUMPRESETS * sizeof(PresetSettings); i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
      }
      break;
    case 15:   // send All User Settings
      array = (byte *) &config.device.minUSBMIDIInterval;                // skip over version, serialMode, and all calibration data
      n = (byte *) &config.project - array;                              // stop short of the current sequencer project
      for (unsigned short i = 0; i < n; i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
      }
      break;
    case 16:   // send calibration data (experimental)
      array = (byte *) &Device.calRows;
      n = sizeof(Device.calRows) + sizeof(Device.calCols) + sizeof(Device.calCrc) + 3 * sizeof(boolean);  
      for (unsigned short i = 0; i < n; i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
      }
      break;
    case 17:   // full dump of config, for debugging the updater, only exported never imported, no NRPN or header, delete later
      array = (byte *) &Device.version;
      for (unsigned short i = 0; i < sizeof(config); i += 1) {
        microLinnSendPolyPressure(0, array[i]);
      }
      break;
  }
  microLinnSendPolyPressure(exportType, 0, 13);                          // footer
}

void microLinnSendPolyPressure(short shortInt) {
  microLinnSendPolyPressure(shortInt & 255, shortInt >> 8);              // LSB first (little-endian)
}

void microLinnSendPolyPressure(byte data1, byte data2) {
  microLinnSendPolyPressure(data1, data2, 1);
}

void microLinnSendPolyPressure(byte data1, byte data2, byte channel) {   // channel is 1-indexed
  if (data1 > 127) {data1 -= 128; channel += 1;}                         // convert 8 bits to 7, store the 8th bit in the channel
  if (data2 > 127) {data2 -= 128; channel += 2;} 
  delayUsec(1200);                                                       // don't overflow the midi queue, see handlePendingMidi()
  resetLastMidiPolyPressure(data1, channel);                             // to ensure it isn't screened out as redundant
  midiSendPolyPressure(data1, data2, channel);
}

// called when nrpn 300 is received
void importMicroLinnData(int value) {
  byte importType = value >> 7;
  byte EDO = value & 127;
  microLinnImportType = 0;

  if (!microLinnImportingOn)                           {microLinnScrollSmall("FIRST TURN ON IMPORTING"); return;}
  if (importType < 1 || importType > 16)               {microLinnScrollSmall("IMPORT FAILURE INVALID IMPORT TYPE " + String(importType)); return;}
  if (importType == 1 && Global.activeNotes < 9)       {microLinnScrollSmall("FIRST SELECT A LIGHT PATTERN"); return;}
  if (importType == 3 && audienceMessageToEdit == -1)  {microLinnScrollSmall("FIRST EDIT AN AUDIENCE MESSAGE"); return;}
  if (importType >= 5 && importType <= 8) {
    byte minEDO = importType == 5 ? 4 : 5;
    if (EDO < minEDO || EDO > MICROLINN_MAX_EDO)       {microLinnScrollSmall("IMPORT FAILURE INVALID EDO " + String(EDO)); return;}
    edo = Global.microLinn.EDO = EDO;                  // switch to the edo if it's valid
    if (edo == 4) edo = 12;
  }

  microLinnImportType = importType;
  microLinnImportSize = 0;
  microLinnImportCounter = 0;
}

void microLinnDeduceImportSize(byte version, byte MLversion) {
  // deduce the size from import type and version numbers
  byte* arrayPtr;
  switch (microLinnImportType) {
    case 1:  // 1 custom light pattern
      microLinnImportSize = LED_LAYER_SIZE;
      break;
    case 2:  // all 3 custom light patterns
      microLinnImportSize = 3 * LED_LAYER_SIZE;
      break;
    case 3:  // 1 audience message
      microLinnImportSize = 30;                                       // not including the null-terminator
      break;
    case 4:  // all 16 audience messages
      microLinnImportSize = 16 * 30;
      break;
    case 5: case 6: case 7:  // scales/rainbow/fretboard for 1 edo
      if (Global.microLinn.EDO == 4) microLinnImportSize = 36;        // EDO == 4 only for case 5, 9 scales, each is 2 shorts
      else microLinnImportSize = edo + (edo % 2);                     // round an odd edo up to an even number
      break;
    case 8:   // All edo data for the current edo only
      microLinnImportSize = 28 + 3 * edo + 3 * (edo % 2);             // 3 arrays are padded to double-bytes
      break;
    case 9: case 10: case 11:  // scales or rainbows or fretboards for all edos
      microLinnImportSize = MICROLINN_ARRAY_SIZE;
      break;
    case 12:  // 3 edo arrays for all edos
      microLinnImportSize = 3 * MICROLINN_ARRAY_SIZE;
      break;
    case 13:  // 1 preset
      if (version == Device.version && MLversion == Device.microLinn.MLversion)
        microLinnImportSize = sizeof(PresetSettings);
      break;
    case 14:  // all 6 presets
      if (version == Device.version && MLversion == Device.microLinn.MLversion)
        microLinnImportSize = NUMPRESETS * sizeof(PresetSettings);
      break;
    case 15:  // all user settings
      arrayPtr = (byte *) &config.device.minUSBMIDIInterval;          // skip over version, serialMode, and all calibration data
      microLinnImportSize = (byte *) &config.project - arrayPtr;      // stop short of the current sequencer project
      break;
    case 16:  // calibration data, 1803 bytes (experimental)
      microLinnImportSize = sizeof(Device.calRows) + sizeof(Device.calCols) + sizeof(Device.calCrc) + 3 * sizeof(boolean); 
      break;
  }
}

// called when a poly pressure message is received, assumed to be part of a bulk import (channel is 0-indexed)
void receiveMicroLinnPolyPressure(byte data1, byte data2, byte channel) {
  if (microLinnImportType == 0) return;
  microLinnLastMomentMidiPP = micros();                                         // for an eventual time-out function

  // handle the header message
  if (channel == 8) {
    if (data1 > Device.version || data2 > Device.microLinn.MLversion) {         // cancel imports from future versions
      microLinnImportType = 0;
      microLinnScrollSmall("IMPORT FAILURE UNKNOWN DATA VERSION " + String(data1) + " " + String(data2));
    } else if (microLinnImportType == 15 &&
              (data1 != Device.version || data2 != Device.microLinn.MLversion)) {
      microLinnImportType = 0;
      microLinnScrollSmall("IMPORT FAILURE DATA VERSION MISMATCH " + String(data1) + " " + String(data2));
    } else {
      microLinnDeduceImportSize(data1, data2);
      // eventually save data1 and data2 as global vars microLinnImportVersion and microLinnImportMLVersion
      // and use them to import data exported from an older version, much like the updater app does
    }
    return;
  }

  // handle the footer message
  if (channel == 12) {
    if (microLinnImportCounter < microLinnImportSize) {
      short c = microLinnImportCounter;                   // c = the counter, 0-indexed
      microLinnScrollSmall("IMPORT FAILURE NOT ENOUGH DATA " + String(c) + " " +  String(c/2+8));
    } else if (microLinnImportSize == 0) {
      microLinnScrollSmall("IMPORT FAILURE NO HEADER");
    } else if (data1 != microLinnImportType) { 
      microLinnScrollSmall("IMPORT WARNING FOOTER MISMATCH");
    } else {
      microLinnScrollSmall("IMPORT SUCCESS");
    }
    if (microLinnImportType > 4) setupMicroLinn();
    setMidiChannelSelect();
    microLinnImportType = 0;
    updateDisplay();
    return;
  }

  // handle data messages
  if (channel > 3) return;
  if (microLinnImportSize == 0) return;                       // missing header
  data1 += (channel & 1) << 7;                                // convert 7 bits to 8, read the 8th bit from the channel
  data2 += (channel & 2) << 6;
  unsigned short i = microLinnImportCounter;
  if (i >= microLinnImportSize) {                             // ignore excess bytes
    microLinnScrollSmall("IMPORT WARNING EXCESS DATA " + String(i) + " " +  String(i/2+8));
    if (microLinnImportType > 4) setupMicroLinn();
    setMidiChannelSelect();
    microLinnImportType = 0;
    updateDisplay();
    return;
  }

  byte* array;          // used to configure complex structs as a simple array of bytes
  byte* ptrStart;       // used to calculate the # of bytes exported from Device
  byte* ptrEnd;
  unsigned short m;
  short n = microLinnTriIndex(edo, 0);

  switch (microLinnImportType) {

    case 1:   // 1 custom light pattern
      m = Global.activeNotes - 9;
      if ((data1 & 7) > 2 || (data2 & 7) > 2) microLinnCancelImport();
      if (microLinnInRange(data1 >> 3, 0, COLOR_PINK)) Device.customLeds[m][i]   = data1;
      if (microLinnInRange(data2 >> 3, 0, COLOR_PINK)) Device.customLeds[m][i+1] = data2;
      break;

    case 2:   // all 3 custom light patterns
      m = i / LED_LAYER_SIZE;
      n = i % LED_LAYER_SIZE;
      if ((data1 & 7) > 2 || (data2 & 7) > 2) microLinnCancelImport();
      if (microLinnInRange(data1 >> 3, 0, COLOR_PINK)) Device.customLeds[m][n]   = data1;
      if (microLinnInRange(data2 >> 3, 0, COLOR_PINK)) Device.customLeds[m][n+1] = data2;
      break;

    case 3:   // 1 audience message
      m = audienceMessageToEdit;
      if (inRange(data1, 1, 31)) {
        Device.audienceMessages[m][i] = '\0';
        microLinnCancelImport();
      }
      if (microLinnInRange(data1, 0, '~')) Device.audienceMessages[m][i] = data1;
      if (inRange(data2, 1, 31)) {
        Device.audienceMessages[m][i+1] = '\0';
        microLinnCancelImport();
      }
      if (microLinnInRange(data2, 0, '~')) Device.audienceMessages[m][i+1] = data2;
      if (i == 28) {
        trimEditedAudienceMessage();
        storeSettings();
      }
      break;

    case 4:   // all 16 audience messages
      array = (byte *) &Device.audienceMessages;
      m = i + (i / 30);                                         // skip over the 31st byte of each message
      if (inRange(data1, 1, 31)) {
        array[m] = '\0';
        microLinnCancelImport();
      }
      if (microLinnInRange(data1, 0, '~')) array[m] = data1;
      if (inRange(data2, 1, 31)) {
        array[m+1] = '\0';
        microLinnCancelImport();
      }
      if (microLinnInRange(data2, 0, '~')) array[m+1] = data2;
      if (i % 30 == 28) {
        audienceMessageToEdit = i / 30;
        trimEditedAudienceMessage();
      }
      break;

    case 5:   // scales for 1 edo
      if (Global.microLinn.EDO == 4) {
        m = data1 | (data2 << 8);
        if (microLinnInRange(m, 0, (1 << 12) - 1)) {
          if (i < 18) Global.mainNotes[i/2] = m;
          else        Global.accentNotes[i/2 - 9] = m;
        }
      } else {
        if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[n+i]   = data1;
        if (i+1 >= edo) break;
        if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[n+i+1] = data2;
      }
      break;

    case 6:   // 1 rainbow
      if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[n+i]   = data1;
      if (i+1 >= edo) break;
      if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[n+i+1] = data2;
      break;

    case 7:   // 1 fretboard
      Device.microLinn.fretboards[n+i] = data1;
      if (i+1 >= edo) break;
      Device.microLinn.fretboards[n+i+1] = data2;
      break;

    case 8:   // all edo data for 1 edo only
      m = edo + (edo % 2);                                  // round an odd edo up to an even number
      if (i == 0) {
        if (data1 != Global.microLinn.EDO) microLinnCancelImport();       // edo was set by the nrpn
        if (microLinnInRange(data2, 0, 1)) Global.microLinn.useRainbow = (data2 == 1);
      } else if (i == 2) {
        if (microLinnInRange(data1, 1, 36))  Global.microLinn.equaveSemitones = data1;
        if (microLinnInRange(data2, 0, 100)) Global.microLinn.equaveStretch = data2 - 50;
      } else if (i == 4) {
        if (microLinnInRange(data1, 1, 60))  Global.microLinn.sweeten = data1;
        if (microLinnInRange(data2, 0, 53))  Global.microLinn.largeEDO = data2 - 60;
      } else if (i == 6) {
        if (data1 == ROWOFFSET_NOOVERLAP || data1 == 3 || data1 == 4 || data1 == 5 || data1 == 6 ||
            data1 == 7 || data1 == ROWOFFSET_OCTAVECUSTOM || data1 == ROWOFFSET_GUITAR || data1 == ROWOFFSET_ZERO) {
          Global.rowOffset = data1;
        } else microLinnCancelImport();
        n = MICROLINN_MAX_ROW_OFFSET;
        if (microLinnInRange(data2, 0, 2*n))   Global.customRowOffset = data2 - n;
      } else if (i == 8) {
        n = MICROLINN_MAX_COL_OFFSET;
        if (microLinnInRange(data1, 1, n))     Split[LEFT].microLinn.colOffset = data1;
        n = MICROLINN_MAX_ROW_OFFSET;
        if (microLinnInRange(data2, 0, 2*n+2)) Split[LEFT].microLinn.rowOffset = data2 - n - 1;
      } else if (i == 10) {
        n = MICROLINN_MAX_COL_OFFSET;
        if (microLinnInRange(data1, 1, n))     Split[RIGHT].microLinn.colOffset = data1;
        n = MICROLINN_MAX_ROW_OFFSET;
        if (microLinnInRange(data2, 0, 2*n+2)) Split[RIGHT].microLinn.rowOffset = data2 - n - 1;
      } else if (i == 12) {
        n = data1 | (data2 << 8);
        if (microLinnInRange(n, 0, 1)) Global.microLinn.guitarTuning[0] = n;
      } else if (i <= 26) {
        n = data1 | (data2 << 8);
        if (n > 2 * MICROLINN_MAX_GUITAR_RANGE) microLinnCancelImport();
        Global.microLinn.guitarTuning[(i - 12) >> 1] = n - MICROLINN_MAX_GUITAR_RANGE;
        if (i == 26) microLinnCheckGuitarTuning();
      } else if (i < 28 + m) {
        if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[n + i - 28] = data1;
        if (i - 27 >= edo) break;
        if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[n + i - 27] = data2;
      } else if (i < 28 + 2 * m) {
        if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[n + i - 28 - m] = data1;
        if (i - 27 - m >= edo) break;
        if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[n + i - 27 - m] = data2;
      } else {
        Device.microLinn.fretboards[n + i - 28 - 2 * m] = data1;
        if (i - 27 - 2 * m >= edo) break;
        Device.microLinn.fretboards[n + i - 27 - 2 * m] = data2;
      }
      break;

    case 9:   // all 51 sets of 7 scales
      if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[i]   = data1;
      if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[i+1] = data2;
      break;

    case 10:   // all 51 rainbows
      if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[i]   = data1;
      if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[i+1] = data2;
      break;

    case 11:   // all 51 fretboards
      Device.microLinn.fretboards[i]   = data1;
      Device.microLinn.fretboards[i+1] = data2;
      break;

    case 12:   // all 3 arrays for all 51 edos
      if (i < MICROLINN_ARRAY_SIZE) {
        if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[i]   = data1;
        if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[i+1] = data2;
      }
      else if (i < 2 * MICROLINN_ARRAY_SIZE) {
        if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[i]   = data1;
        if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[i+1] = data2;
      }
      else if (i < 3 * MICROLINN_ARRAY_SIZE) {
        Device.microLinn.fretboards[i]   = data1;
        Device.microLinn.fretboards[i+1] = data2;
      }
      break;

    case 13:   // 1 preset = 396 bytes for version 72.0
      if (i == 0) microLinnImportXen = true;
      if (i < sizeof(GlobalSettings)) {
        microLinnImportGlobal(data1, data2, i, 0);               // import into Global and Split
      } else {
        microLinnImportSplits(data1, data2, i - sizeof(GlobalSettings), 0);
      }
      break;

    case 14:   // all 6 presets = 2376 bytes
      m = i % sizeof(PresetSettings);                            // m = index into the preset
      n = (i - m) / NUMPRESETS;                                  // n = which preset
      if (m == 0) microLinnImportXen = true;                     // reset the flag for each new preset
      if (m < sizeof(GlobalSettings)) {
        microLinnImportGlobal(data1, data2, m, n+1);
      } else {
        microLinnImportSplits(data1, data2, m - sizeof(GlobalSettings), n+1);
      }
      break;

    case 15:   // all user settings = 8508 bytes for version 72.0
      ptrStart = (byte *) &Device.minUSBMIDIInterval;            // start at Device.minUSBMIDIInterval
      ptrEnd = (byte *) &Global;                                 // stop short of Global
      m = ptrEnd - ptrStart;                                     // m = size of Device settings in the import
      if (i < m) {
        microLinnImportDevice(data1, data2, i);
        break;
      }
      i -= m;
      m = i % sizeof(PresetSettings);                            // m = index into the preset
      n = (i - m) / NUMPRESETS;                                  // 0 = Global/Split, 1-6 = presets 0-5
      if (m == 0) microLinnImportXen = true;                     // reset the flag for each new preset
      if (m < sizeof(GlobalSettings)) {
        microLinnImportGlobal(data1, data2, m, n);
      } else {
        microLinnImportSplits(data1, data2, m - sizeof(GlobalSettings), n);
      }
      break;

    case 16:   // calibration data (experimental)
      array = (byte *) &Device.calRows;
      array[i] = data1; 
      if (i+1 < microLinnImportSize) array[i+1] = data2;
      break;
  }
  microLinnImportCounter += 2;
}

void microLinnCancelImport() {
  short i = microLinnImportCounter;                // i = the counter, 0-indexed
  short j = microLinnImportCounter / 2 + 8;        // j = which midi message, 1-indexed, include the headers
  microLinnScrollSmall("IMPORT FAILURE " + String(i) + " " +  String(j));
  if (microLinnImportType > 4) setupMicroLinn();
  microLinnImportType = 0;
  setMidiChannelSelect();
  updateDisplay();
}

boolean microLinnInRange (short data, short lo, short hi) {
  if (microLinnImportType == 0) return false;      // avoid loading 2nd polypressure byte if the 1st byte is invalid
  boolean result = data >= lo && data <= hi;
  if (!result) microLinnCancelImport();
  return result;
}

void microLinnImportDevice(byte data1, byte data2, unsigned short i) {               // don't import calibration data
  if (i >= 5736) return;                                                             // version 72.1 size is 5736 bytes
  unsigned short number = data1 | (data2 << 8);                                      // LSB first (little-endian)

  if (i == 0) {
    if (microLinnInRange(number, 0, 512)) Device.minUSBMIDIInterval = number;
  } else if (i == 2) {
    if (microLinnInRange(data1,  50, 200)) Device.sensorSensitivityZ = data1;        // ignore data2, it's just padding
  } else if (i == 4) {
    if (microLinnInRange(number, 100, 1024)) Device.sensorLoZ = number;
  } else if (i == 6) {
    if (Device.sensorLoZ < number) microLinnCancelImport();
    if (microLinnInRange(number, 65, min(1024, Device.sensorLoZ))) Device.sensorFeatherZ = number;
  } else if (i == 8) {
    if (microLinnInRange(number, 3 * 127, MAX_SENSOR_RANGE_Z - 127)) Device.sensorRangeZ = number;
  } else if (i == 10) {
    if (microLinnInRange(data1, 0, 1)) Device.sleepAnimationActive = (data1 == 1);
    if (microLinnInRange(data2, 0, 1)) Device.sleepActive = (data2 == 1);
  } else if (i == 12) {
    if (microLinnInRange(data1, 0, 30)) Device.sleepDelay = data1;
    if (microLinnInRange(data2, animationNone, animationChristmas)) Device.sleepAnimationType = data2;
  } else if (i <= 508) {
    byte* array = (byte *) &Device.audienceMessages;
    if (inRange(data1, 1, 31)) {
      array[i-14] = '\0';
      microLinnCancelImport();
    }
    if (microLinnInRange(data1, 0, '~')) array[i-14] = data1;
    if (inRange(data2, 1, 31)) {
      array[i-13] = '\0';
      microLinnCancelImport();
    }
    if (microLinnInRange(data2, 0, '~')) array[i-13] = data2;
    if ((i - 11) % 31 <= 1) {
      audienceMessageToEdit = (i / 31) - 1;
      trimEditedAudienceMessage();
      storeSettings();
    }
  } else if (i == 510) {
    if (microLinnInRange(data1, 0, 1)) Device.operatingLowPower = data1;
    if (microLinnInRange(data2, 0, 1)) Device.otherHanded = (data2 == 1);
  } else if (i == 512) {
    if (microLinnInRange(data1, 0, 2)) Device.splitHandedness = data1;
    if (microLinnInRange(data2, 0, 1)) Device.midiThrough = (data2 == 1);
  } else if (i == 514) {
    if (microLinnInRange(number, -1, NUMPRESETS-1)) Device.lastLoadedPreset = number;      // -1 = no recent load
  } else if (i == 516) {
    if (microLinnInRange(number, -1, 15)) Device.lastLoadedProject = number;
  } else if (i <= 724) {    // LED_LAYER_SIZE = 26 * 8 = 208
    if ((data1 & 7) > 2 || (data2 & 7) > 2) microLinnCancelImport();
    if (microLinnInRange(data1 >> 3, 0, COLOR_PINK)) Device.customLeds[0][i-518] = data1;
    if (microLinnInRange(data2 >> 3, 0, COLOR_PINK)) Device.customLeds[0][i-517] = data2;
  } else if (i <= 932) {
    if ((data1 & 7) > 2 || (data2 & 7) > 2) microLinnCancelImport();
    if (microLinnInRange(data1 >> 3, 0, COLOR_PINK)) Device.customLeds[1][i-726] = data1;
    if (microLinnInRange(data2 >> 3, 0, COLOR_PINK)) Device.customLeds[1][i-725] = data2;
  } else if (i <= 1140) {
    if ((data1 & 7) > 2 || (data2 & 7) > 2) microLinnCancelImport();
    if (microLinnInRange(data1 >> 3, 0, COLOR_PINK)) Device.customLeds[2][i-934] = data1;
    if (microLinnInRange(data2 >> 3, 0, COLOR_PINK)) Device.customLeds[2][i-933] = data2;
  } else if (i == 1142) {
    // ignore data1 = microLinn.MLversion and data2 = Device.microLinn.uninstall
    Device.microLinn.uninstall = false;
  } else if (i <= 2672) {   // MICROLINN_ARRAY_SIZE = 1530
    if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[i-1144] = data1;
    if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[i-1143] = data2;
  } else if (i <= 4202) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[i-2674] = data1;
    if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[i-2673] = data2;
  } else if (i <= 5732) {  
    Device.microLinn.fretboards[i-4204] = data1;
    Device.microLinn.fretboards[i-4203] = data2;
  } else if (i == 5734) {
    // ignore last 2 bytes = padding
  }
}

void microLinnImportGlobal(byte data1, byte data2, unsigned short i, byte presetNum) {     // version 72.1 size is 364 bytes
  if (i < 0 || i >= sizeof(GlobalSettings)) return;
  GlobalSettings *g = (presetNum == 0 ? &Global : &config.preset[presetNum - 1].global);
  if (i >= 170 && !microLinnImportXen) return;
  unsigned short number = data1 | (data2 << 8);

  if (i == 0) {
    if (microLinnInRange(data1, 2, 25)) g->splitPoint = data1;
    if (microLinnInRange(data2, 0, 1))  g->currentPerSplit = data2;
  } else if (i == 2) {
    if (microLinnInRange(data1, 0, 8))  g->activeNotes = data1;               // ignore data2 = padding
  } else if (i <= 50) {
    if (i % 4 == 2) {                                                         // upper 2 bytes
      if (number != 0) microLinnCancelImport(); 
    }
    else if (microLinnInRange(number, 0, (1 << 12) - 1)) g->mainNotes[(i-4)/4] = number;
  } else if (i <= 98) {
    if (i % 4 == 2) {
      if (number != 0) microLinnCancelImport();
    }
    else if (microLinnInRange(number, 0, (1 << 12) - 1)) g->accentNotes[(i-52)/4] = number;
  } else if (i == 100) {
    if (inRange (data1, 3, 7) || data1 == ROWOFFSET_GUITAR || data1 == ROWOFFSET_ZERO ||
        data1 == ROWOFFSET_NOOVERLAP || data1 == ROWOFFSET_OCTAVECUSTOM) {
      g->rowOffset = data1;
    } else microLinnCancelImport();
    signed char n = (signed char)data2;
    byte r = MICROLINN_MAX_ROW_OFFSET;
    if (microLinnInRange(abs(n), 0, r))   g->customRowOffset = n;         // assume microLinn is on, fix later if it isn't
  } else if (i <= 108) {
    if (microLinnInRange(data1,  0, 127)) g->guitarTuning[i-102] = data1;
    if (microLinnInRange(data2,  0, 127)) g->guitarTuning[i-101] = data2;
  } else if (i == 110) {
    if (microLinnInRange(data1,  0, 3))   g->velocitySensitivity = (VelocitySensitivity)data1;   // ignore data2 = padding
  } else if (i == 112) {
    if (microLinnInRange(number, 0, 127)) g->minForVelocity = number;
  } else if (i == 114) {
    if (microLinnInRange(number, 0, 127)) g->maxForVelocity = number;
  } else if (i == 116) {
    if (microLinnInRange(number, 0, 127)) g->valueForFixedVelocity = number; 
  } else if (i == 118) {
    if (microLinnInRange(data1,  0, 2))   g->pressureSensitivity = (PressureSensitivity)data1;
    if (microLinnInRange(data2,  0, 1))   g->pressureAftertouch = (data2 == 1);
  } else if (i == 120) {
    if (microLinnInRange(data1, ASSIGNED_OCTAVE_DOWN, MAX_ASSIGNED)) g->switchAssignment[0] = data1;
    if (microLinnInRange(data2, ASSIGNED_OCTAVE_DOWN, MAX_ASSIGNED)) g->switchAssignment[1] = data2;
  } else if (i == 122) {
    if (microLinnInRange(data1, ASSIGNED_OCTAVE_DOWN, MAX_ASSIGNED)) g->switchAssignment[2] = data1;
    if (microLinnInRange(data2, ASSIGNED_OCTAVE_DOWN, MAX_ASSIGNED)) g->switchAssignment[3] = data2;
  } else if (i == 124) {
    if (inRange(data1, ASSIGNED_OCTAVE_DOWN, MAX_ASSIGNED) || data1 == ASSIGNED_DISABLED) {
      g->switchAssignment[4] = data1;
    } else microLinnCancelImport();
    if (microLinnInRange(data2,  0, 1))   g->switchBothSplits[0] = (data2 == 1);
  } else if (i == 126) {
    if (microLinnInRange(data1,  0, 1))   g->switchBothSplits[1] = (data1 == 1);
    if (microLinnInRange(data2,  0, 1))   g->switchBothSplits[2] = (data2 == 1);
  } else if (i == 128) {
    if (microLinnInRange(data1,  0, 1))   g->switchBothSplits[3] = (data1 == 1);
    if (microLinnInRange(data2,  0, 1))   g->switchBothSplits[4] = (data2 == 1);
  } else if (i <= 138) {
    if (microLinnInRange(number, 0, 127)) g->ccForSwitchCC65[(i-130)/2] = number;
  } else if (i <= 148) {
    if (microLinnInRange(number, 0, 127)) g->ccForSwitchSustain[(i-140)/2] = number;
  } else if (i <= 158) {
    if (microLinnInRange(number, ASSIGNED_TAP_TEMPO, MAX_ASSIGNED)) g->customSwitchAssignment[(i-150)/2] = number;
  } else if (i == 160) {
    if (microLinnInRange(data1,  0, 1)) g->midiIO = data1;
    if (microLinnInRange(data2,  0, 4)) g->arpDirection = (ArpeggiatorDirection)data2;
  } else if (i == 162) {
    if (microLinnInRange(data1,  0, 8)) g->arpTempo = (ArpeggiatorStepTempo)data1;
    if (microLinnInRange(data2,  0, 2)) g->arpOctave = data2;
  } else if (i == 164) {
    if (microLinnInRange(data1,  0, 1)) g->sustainBehavior = (SustainBehavior)data1;
    if (microLinnInRange(data2,  0, 1)) g->splitActive = (data2 == 1);
  } else if (i == 166) {
    if (microLinnInRange(data1, 0, 2))  g->microLinn.drumPadMode = data1;
    signed char n = (signed char)data2;
    if (microLinnInRange(n, -1, 119))   g->microLinn.locatorCC1 = n;
  } else if (i == 168) {
    signed char n = (signed char)data1;
    if (!isLinn200()) n = -1;
    if (microLinnInRange(n, -1, 119))   g->microLinn.locatorCC2 = n;
    if (data2 >= 5) {
      microLinnImportXen = true;
    } else {
      if (g->customRowOffset < -17 || g->customRowOffset > 16) {       // limits are tighter when microLinn is off
        microLinnImportCounter -= 68;                                  // point to the problem, the customRowOffset byte
        microLinnCancelImport();
      }
      microLinnImportXen = (presetNum == 0 || presetNum == 5);         // 0 = global, 5 is really 4 = bottom row preset
      if (!microLinnImportXen) return;
    }
    if (microLinnInRange(data2, 4, 55)) g->microLinn.EDO = data1;
  } else if (i <= 222) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) g->microLinn.rainbow[i-170] = data1;
    if (microLinnInRange(data2, 0, COLOR_PINK)) g->microLinn.rainbow[i-169] = data2;
  } else if (i == 224) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) g->microLinn.rainbow[54] = data1;
    g->microLinn.fretboard[0] = data2;
  } else if (i <= 278) {
    g->microLinn.fretboard[i-225] = data1;
    g->microLinn.fretboard[i-224] = data2;
  } else if (i == 280) {
    if (microLinnInRange(data1, 0, 1))   g->microLinn.useRainbow = (data1 == 1);
    if (microLinnInRange(data2, 1, 25))  g->microLinn.anchorCol = data2;
  } else if (i == 282) {
    if (microLinnInRange(data1, 0, 7))   g->microLinn.anchorRow = data1;
    if (microLinnInRange(data2, 0, 127)) g->microLinn.anchorNote = data2;
  } else if (i == 284) {
    signed char n = (signed char)data1;
    if (microLinnInRange(n,    -60, 60)) g->microLinn.anchorCents = n;
    if (microLinnInRange(data2,  1, 36)) g->microLinn.equaveSemitones = data2;
  } else if (i == 286) {
    signed char n = (signed char)data1;
    if (microLinnInRange(n,    -50, 50)) g->microLinn.equaveStretch = n;
    if (microLinnInRange(data2,  0, 60)) g->microLinn.sweeten = data2;
  } else if (i == 288) {
    if (microLinnInRange(data1,  0, 53)) g->microLinn.largeEDO = data1;
    //g->microLinn.largeEDOoffset[0] = data2;  uncomment later
  } else if (i <= 342) {
    //g->microLinn.largeEDOoffset[i-289] = data1;  uncomment later
    //g->microLinn.largeEDOoffset[i-288] = data2;
  } else if (i == 344) {
    short n = data1 | (data2 << 8);
    if (microLinnInRange(n, 0, 1)) g->microLinn.guitarTuning[0] = n;
  } else if (i <= 358) {
    short n = data1 | (data2 << 8);
    if (abs(n) > MICROLINN_MAX_GUITAR_RANGE) microLinnCancelImport();
    g->microLinn.guitarTuning[(i-344)/2] = n;
    if (i == 358) microLinnCheckGuitarTuning();
  } else if (i == 360) {
    if (microLinnInRange(data1, 0, 1)) g->microLinn.dotsCarryOver = (data1 == 1);
    // ignore bytes 361-362 = reserved and byte 363 = padding
  }
}

void microLinnImportSplits(byte data1, byte data2, unsigned short i,  byte presetNum) {       // version 72.1 size is 114 bytes per side
  if (i < 0 || i >= 2 * sizeof(SplitSettings)) return;
  byte side = (i < sizeof(SplitSettings) ? LEFT : RIGHT);
  if (side == RIGHT) i -= sizeof(SplitSettings);
  SplitSettings *spl = (presetNum == 0 ? &Split[side] : &config.preset[presetNum - 1].split[side]);
  if (i > 102 && !microLinnImportXen) return;
  unsigned short number = data1 | (data2 << 8);

  if (i == 0) {
    if (microLinnInRange(data1, 0, 2))  spl->midiMode = data1;
    if (microLinnInRange(data2, 1, 16)) spl->midiChanMain = data2;
  } else if (i == 2) {
    if (microLinnInRange(data1, 0, 1))  spl->midiChanMainEnabled = (data1 == 1);
    if (microLinnInRange(data2, 1, 16)) spl->midiChanPerRow = data2;
  } else if (i == 4) {
    if (microLinnInRange(data1, 0, 1))  spl->midiChanPerRowReversed = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))  spl->midiChanSet[0] = (data2 == 1);
  } else if (i <= 18) {
    if (microLinnInRange(data1, 0, 1))  spl->midiChanSet[i-5] = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))  spl->midiChanSet[i-4] = (data2 == 1);
  } else if (i == 20) {
    if (microLinnInRange(data1, 0, 1))  spl->midiChanSet[15] = (data1 == 1);
    if (microLinnInRange(data2, 0, 3))  spl->bendRangeOption = (BendRangeOption)data2;
  } else if (i == 22) {
    if (microLinnInRange(data1, 1, 96)) spl->customBendRange = data1;
    if (microLinnInRange(data2, 0, 1))  spl->sendX = (data2 == 1);
  } else if (i == 24) {
    if (microLinnInRange(data1, 0, 1))  spl->sendY = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))  spl->sendZ = (data2 == 1);
  } else if (i == 26) {
    if (microLinnInRange(data1, 0, 1))  spl->pitchCorrectQuantize = (data1 == 1);
    if (microLinnInRange(data2, 0, 3))  spl->pitchCorrectHold = data2;
  } else if (i == 28) {
    if (microLinnInRange(data1, 0, 1))  spl->pitchResetOnRelease = (data1 == 1);
    if (microLinnInRange(data2, 0, 3))  spl->expressionForY = (TimbreExpression)data2;
  } else if (i == 30) {
    if (microLinnInRange(number, 0, 129)) spl->customCCForY = number;
  } else if (i == 32) {
    if (microLinnInRange(number, 0, 127)) spl->minForY = number;
  } else if (i == 34) {
    if (microLinnInRange(number, 0, 127)) spl->maxForY = number;
  } else if (i == 36) {
    if (microLinnInRange(data1,  0, 1))   spl->relativeY = (data1 == 1);        // ignore data2 = padding 
  } else if (i == 38) {
    if (microLinnInRange(number, 0, 127)) spl->initialRelativeY = number;
  } else if (i == 40) {
    if (microLinnInRange(data1,  0, 2))   spl->expressionForZ = (LoudnessExpression)data1;    // ignore data2 = padding 
  } else if (i == 42) {
    if (microLinnInRange(number, 0, 127)) spl->customCCForZ = number;
  } else if (i == 44) {
    if (microLinnInRange(number, 0, 127)) spl->minForZ = number;
  } else if (i == 46) {
    if (microLinnInRange(number, 0, 127)) spl->maxForZ = number;
  } else if (i == 48) {
    if (microLinnInRange(data1,  0, 1))   spl->ccForZ14Bit = (data1 == 1);      // ignore data2 = padding
  } else if (i <= 64) {
    if (microLinnInRange(number, 0, 128)) spl->ccForFader[(i-50)/2] = number;
  } else if (i == 66) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) spl->colorMain = data1;
    if (microLinnInRange(data2, 0, COLOR_PINK)) spl->colorAccent = data2;
  } else if (i == 68) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) spl->colorPlayed = data1;
    if (microLinnInRange(data2, 0, COLOR_PINK)) spl->colorLowRow = data2;
  } else if (i == 70) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) spl->colorSequencerEmpty = data1;
    if (microLinnInRange(data2, 0, COLOR_PINK)) spl->colorSequencerEvent = data2;
  } else if (i == 72) {
    if (microLinnInRange(data1, 0, COLOR_PINK)) spl->colorSequencerDisabled = data1;
    if (microLinnInRange(data2, 0, 16))   spl->playedTouchMode = data2;
  } else if (i == 74) {
    if (microLinnInRange(data1, 0, 7))    spl->lowRowMode = data1;
    if (microLinnInRange(data2, 0, 1))    spl->lowRowCCXBehavior = data2;
  } else if (i == 76) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRow = number;
  } else if (i == 78) {
    if (microLinnInRange(data1,  0, 1))   spl->lowRowCCXYZBehavior = data1;      // ignore data2 = padding
  } else if (i == 80) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowX = number;
  } else if (i == 82) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowY = number; 
  } else if (i == 84) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowZ = number;
  } else if (i == 86) {
    signed char n = (signed char)data1;
    if (n % 12 != 0) microLinnCancelImport();
    if (microLinnInRange(n, -60, 60))    spl->transposeOctave = n;
    n = (signed char)data2;
    if (microLinnInRange(n, -7, 7))      spl->transposePitch = n;
  } else if (i == 88) {
    signed char n = (signed char)data1;
    if (microLinnInRange(n, -7, 7))      spl->transposeLights = n;
    if (microLinnInRange(data2,  0, 1))  spl->ccFaders = (data2 == 1);
  } else if (i == 90) {
    if (microLinnInRange(data1, 0, 1))   spl->arpeggiator = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))   spl->strum = (data2 == 1);
  } else if (i == 92) {
    if (microLinnInRange(data1, 0, 1))   spl->mpe = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))   spl->sequencer = (data2 == 1);
  } else if (i == 94) {
    if (microLinnInRange(data1, 0, 2))   spl->sequencerView = (SequencerView)data1;   // ignore data2 = padding
  } else if (i == 96) {
    byte c = MICROLINN_MAX_COL_OFFSET;
    if (microLinnImportXen || data1 > 1) {
      if (microLinnInRange(data1, 1, c)) spl->microLinn.colOffset = data1;
    }
    signed char n = (signed char)data2;
    byte r = MICROLINN_MAX_ROW_OFFSET + 1;
    if (!microLinnImportXen && n == -r)  return;
    if (microLinnInRange(n, -r, r))      spl->microLinn.rowOffset = n;
  } else if (i == 98) {
    if (microLinnInRange(data1, 0, 3))   spl->microLinn.monoFixes = data1;    
    if (microLinnInRange(data2, 0, 3))   spl->microLinn.hammerOnMode = data2;
  } else if (i == 100) {
    if (microLinnInRange(data1, 1, 121)) spl->microLinn.hammerOnZone = data1;
    if (microLinnInRange(data2, 0, 50))  spl->microLinn.hammerOnWait = data2;
  } else if (i == 102) {
    if (microLinnInRange(data1, 0, 6))   spl->microLinn.showCustomLEDs = data1;
    if (!microLinnImportXen) return;
    if (microLinnInRange(data2, 0, 55))  spl->microLinn.condensedBendPerPad = data2;
  } else if (i == 104) {
    if (microLinnInRange(data1, 0, 7))   spl->microLinn.defaultLayout = data1;
    if (microLinnInRange(data2, 0, 3))   spl->microLinn.tuningTable = data2;
  } else if (i == 106) {
    signed char n = (signed char)data1;
    if (microLinnInRange(n, -1, 119))    spl->microLinn.midiGroupCC = n;
    n = (signed char)data2;
    if (microLinnInRange(n, -7, 7))      spl->microLinn.transposeEDOsteps = n;
  } else if (i == 108) {
    if (microLinnInRange(data1, 0, 128)) spl->microLinn.ccForLowRowW = data1;
    if (microLinnInRange(data2, 0, 128)) spl->microLinn.ccForLowRowX = data2;
  } else if (i == 110) {
    if (microLinnInRange(data1, 0, 128)) spl->microLinn.ccForLowRowY = data1;
    if (microLinnInRange(data2, 0, 255)) spl->microLinn.flags = data2;
  } else if (i == 112) {
    // ignore bytes 112-113 = reserved for future use
  }
}


/**************  OBSOLETE CODE, DELETE LATER  ************************


const byte MICROLINN_RAINBOWS[MICROLINN_ARRAY_SIZE] = {
// lime (10) is used for yellow because the official yellow (2) is too orangish
// 8     = white         = 12-edo-ish           = 3-limit
// 10/3  = yellow/green  = submajor/superminor  = 5-over/5-under
// 5/1   = blue/red      = subminor/supermajor  = 7-over/7-under
// 6     = magenta       = neutral              = 11-over or 11-under or 13-over or 13-under
// 11    = pink          = 600c, a half-8ve     = 12-edo-ish but not quite 3-limit, "off-white"
// 4     = cyan          = outside notes aka interordinals (e.g. 19edo and 24edo)
// 9     = orange        = used in 55edo for superneutral and subneutral, to avoid 3 magentas in a row
//         cyan/orange   = used in 41edo for 7/5 and 10/7

  8, 1, 8, 8, 5, // 5edo = white red white white blue
  8,10,10,11, 3, 3, // 6edo
  8,10, 6, 8, 8, 6, 3, // 7edo
  8, 6, 3, 5,11, 1,10, 6, // 8
  8, 3, 5,10, 6, 6, 3, 1,10, // 9
  8, 3, 1, 6, 8,11, 8, 6, 5,10, // 10
  8, 3, 1, 3, 1, 6, 6, 5,10, 5,10, // 11
  8, 3,10, 3,10, 8,11, 8, 3,10, 3,10, // 12
  8, 5,10, 5,10, 5, 6, 6, 1, 3, 1, 3, 1, // 13
  8, 5,10, 5, 6, 1, 8,11, 8, 5, 6, 1, 3, 1, // 14
  8, 3,10, 1, 3,10, 8, 6, 6, 8, 3,10, 5, 3,10, // 15
  8, 6, 3,10, 3,10, 6, 8,11, 8, 6, 3,10, 3,10, 6, // 16
  8, 5, 6, 1, 5, 6, 1, 8, 6, 6, 8, 5, 6, 1, 5, 6, 1, // 17
  8, 5, 3,10, 5, 3,10, 5, 6,11, 6, 1, 3,10, 1, 3,10, 1, // 18
  8, 4, 3,10, 4, 3,10, 4, 8, 6, 6, 8, 4, 3,10, 4, 3,10, 4, // 19
  8, 6, 3,10, 1, 3,10, 1, 8, 6,11, 6, 8, 5, 3,10, 5, 3,10, 6, // 20
  8, 6, 3,10, 1, 5, 6,10, 5, 8, 6, 6, 8, 1, 3, 6, 1, 5, 3,10, 6, // 21
  8, 5, 3,10, 1, 5, 3,10, 1, 8, 6,11, 6, 8, 5, 3,10, 1, 5, 3,10, 1, // 22
  8, 4, 3, 6, 1, 5, 3, 6, 1, 5, 8, 6, 6, 8, 1, 5, 6,10, 1, 5, 6,10, 4, // 23
  8, 4, 3, 6,10, 4, 3, 6,10, 4, 8, 6,11, 6, 8, 4, 3, 6,10, 4, 3, 6,10, 4, // 24
  8, 5, 3, 6,10, 1, 5, 3,10, 1, 8, 6,10, 3, 6, 8, 5, 3,10, 1, 5, 3, 6,10, 1, // 25
  8, 1, 5, 3,10, 1, 5, 3,10, 1, 5, 8, 6,11, 6, 8, 1, 5, 3,10, 1, 5, 3,10, 1, 5, // 26
  8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 8, 6, 3,10, 6, 8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, // 27 -- swap the 2 tritones?
  8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, 8, 6,11, 6, 8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, // 28
  8, 4, 8, 3,10, 8, 4, 8, 3,10, 8, 4, 8, 6,10, 3, 6, 8, 4, 8, 3,10, 8, 4, 8, 3,10, 8, 4, // 29
  8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 8, 3, 6,11, 6,10, 8, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, // 30
  8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 5, // 31
  8, 4, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 4, 8, 3, 6,11, 6,10, 8, 4, 5, 3, 6,10, 1, 5, 3, 6,10, 1, 4, // 32
  8, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, // 33
  8, 4, 5, 3, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,11, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,10, 1, 4, // 34
  8, 1, 5, 3, 6,10, 8, 1, 5, 3, 6,10, 8, 1, 5, 8, 6,10, 3, 6, 8, 1, 5, 8, 3, 6,10, 1, 5, 8, 3, 6,10, 1, 5, // 35
  8, 1, 5, 3, 6, 6,10, 1, 5, 3, 6, 6,10, 1, 5, 8, 6, 6,11, 6, 6, 8, 1, 5, 3, 6, 6,10, 1, 5, 3, 6, 6,10, 1, 5, // 36
  8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 8, 3, 6,10, 3, 6,10, 8, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, // 37
  8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, 8, 6,10,11, 3, 6, 8, 1, 4, 5, 3, 6,10, 1, 4, 5, 3, 6,10, 1, 4, 5, // 38
  8, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 8, 3, 6, 3,10, 6,10, 8, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, // 39 -- swap the 2 tritones?
  8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 6,10,11, 3, 6, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 40
  8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6, 4, 9, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 41
  8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10,11, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 5, // 42
  8, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 8, 6, 6,10, 3, 6, 6, 8, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, 3, 6, 6,10, 1, 4, 5, // 43
  8, 3, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 8, 3, 6,10,11, 3, 6,10, 8, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1,10, // 44
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 5, 8, 3, 6,10, 8, 1, 4, 5, // 45
  8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 5, // 46
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 47
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10,11, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 48
  8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6,10, 8, 1, 4, 5, // 49
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 50
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 51
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,11, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 52
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 53
  8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10,11, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, 8, 3, 6, 6,10, 8, 1, 4, 5, // 54
  8, 1, 4, 5, 8, 3, 9, 6, 9,10, 8, 1, 5, 8, 3, 9, 6, 9,10, 8, 1, 4, 5, 8, 9, 6, 9,10, 3, 9, 6, 9, 8, 1, 4, 5, 8, 3, 9, 6, 9,10, 8, 1, 5, 8, 3, 9, 6, 9,10, 8, 1, 4, 5  // 55
};



IMPORTANT: Later on when you update the OS to a different version, you'll be asked if you want to uninstall microLinn. 
Say no (tap the red button in the middle, turns orange) if you're updating to a newer version of microLinn. This avoids 
needlessly deleting your microtonal user settings. Say yes (tap the blue button on the left, turns light blue) otherwise, 
e.g. if you're going back to an official (non-microtonal) version of the firmware. This does delete your microtonal user 
settings, which is necessary in order to avoid deleting all your other user settings plus your calibration data. 
Once you've decided, tap the green button on the right to continue.

void enterMicroLinnUninstall() {
  setDisplayMode(displayMicroLinnUninstall);
  microLinnUninstallNowScrolling = true;
  paintMicroLinnUninstall();
}

void microLinnPaintUninstallButtons() {
  setLed(3, 0, microLinnUninstall ? COLOR_CYAN : COLOR_BLUE, cellOn);
  setLed(4, 0, microLinnUninstall ? COLOR_CYAN : COLOR_BLUE, cellOn);
  setLed(5, 0, microLinnUninstall ? COLOR_CYAN : COLOR_BLUE, cellOn);
  setLed(8, 0, !microLinnUninstall ? COLOR_ORANGE : COLOR_RED, cellOn);
  setLed(9, 0, !microLinnUninstall ? COLOR_ORANGE : COLOR_RED, cellOn);
  setLed(10, 0, !microLinnUninstall ? COLOR_ORANGE : COLOR_RED, cellOn);
  setLed(13, 0, COLOR_GREEN, cellOn);
  setLed(14, 0, COLOR_GREEN, cellOn);
  setLed(15, 0, COLOR_GREEN, cellOn);
}

void paintMicroLinnUninstall() {
  clearDisplay();
  microLinnPaintUninstallButtons();
  if (microLinnUninstallNowScrolling) {
    microLinnScrollSmall("UNINSTALL MICROLINN? BLUE = YES, RED = NO, GREEN = CONTINUE");
    microLinnPaintUninstallButtons();
  }
}

void handleMicroLinnUninstallNewTouch() {
  microLinnUninstallNowScrolling = false;
  if (sensorRow > 0) return;
  if (sensorCol >= 3 && sensorCol <= 5) {          // blue button
    microLinnUninstall = true;
    switchSerialMode(true);
    storeSettings();
    updateDisplay(); 
  }
  else if (sensorCol >= 8 && sensorCol <= 10) {    // red
    microLinnUninstall = false;
    switchSerialMode(true);
    storeSettings();
    updateDisplay(); 
  }
  else if (sensorCol >= 13 && sensorCol <= 15) {   // green
    setDisplayMode(displayGlobal);
    updateDisplay(); 
  }
}


void progressBarMicroLinn(float done, byte color, byte row) {
  setLed(1 + (int)(done*25.0f), row, color, cellOn);
}

int overwriteMicroLinnData () {
  short sizeMLglobal = sizeof(MicroLinnGlobal);
  short sizeGlobal = sizeof(GlobalSettings) - sizeMLglobal;
  short sizeMLsplit = sizeof(MicroLinnSplit);
  short sizeSplit = sizeof(SplitSettings) - sizeMLsplit;
  int sizeMLdata = sizeof(MicroLinnDevice);                     // a running total of what we're overwriting

  memcpy(&Global - sizeMLdata, &Global, sizeGlobal);
  sizeMLdata += sizeMLglobal;
  memcpy(&Split[LEFT] - sizeMLdata, &Split[LEFT], sizeSplit);
  sizeMLdata += sizeMLsplit;
  memcpy(&Split[RIGHT] - sizeMLdata, &Split[RIGHT], sizeSplit);
  sizeMLdata += sizeMLsplit;

  for (byte p = 0; p < NUMPRESETS; ++p) {
    memcpy (&config.preset[p].global - sizeMLdata, &config.preset[p].global, sizeGlobal);
    sizeMLdata += sizeMLglobal;
    memcpy (&config.preset[p].split[LEFT]  - sizeMLdata, &config.preset[p].split[LEFT],  sizeSplit);
    sizeMLdata += sizeMLsplit;
    memcpy (&config.preset[p].split[RIGHT] - sizeMLdata, &config.preset[p].split[RIGHT], sizeSplit);
    sizeMLdata += sizeMLsplit;
  }

  memcpy (&config.project - sizeMLdata, &config.project, sizeof(config.project));

  return sizeof(config) - sizeMLdata;                           // return the new size of config
}





  if (ignoreSlides && sensorCell->velocity) return;          // don't calc on releases caused by slides, causes flickering

note to self:  "if (sensorCell->velocity)" means if another touch is already down on the same row
according to the comment by handleFaderRelease in ls_faders.ino



// for testing only, not useful enough for regular usage
// ASSIGNED_MICROLINN_SCALE_UP and ASSIGNED_MICROLINN_SCALE_DOWN appear as "SCL+" and "SCL-"
void changeMicroLinnScale(int delta) {                                // called via switch1, switch2 or footswitch press, works with 12edo too
  signed char newScale = Global.activeNotes + delta;                  // use a signed char because we can't set a byte to -1
  if (newScale > 8) newScale = 0;                                     // wrap around
  if (newScale < 0) newScale = 8;
  Global.activeNotes = newScale;
  updateDisplay();
}
  
void microLinnSendDebugMidi (byte channel, byte CCnum, short data) {                 // channels are numbered 1-16
  if (data < 0) {channel += 1; data = -data;}
  byte databyte = min (data, 127);
  midiSendControlChange(CCnum, databyte, channel);
}

void microLinnSendDebugMidi2 (unsigned int data) {
  byte controlval = data & 127;   data = data >> 7;
  byte channel = (data & 15) + 1; data = data >> 4;              // channels are numbered 1-16
  byte controlnum = data & 127;
  midiSendControlChange(controlnum, controlval, channel);
}



// this used to be called in calcTunings() when edo == 4 
// the idea was to use tuning tables even when microLinn is off, but it was complicated and not necessary
float microLinnCalc12edoAnchorPitch (byte side) {     // mimics how determineRowOffsetNote() calcs the "lowest" variable
  // try this split's row offset first
  signed char offset = Split[side].microLinn.rowOffset; 
  if (offset == 26) {    // no overlap
    if (Global.splitActive && side == RIGHT) {        // if the right split is displayed, change the column so that the lower left starting
      return 30 - (Global.splitPoint - 1);            // point starts at the same point as the left split, behaving as if there were two independent
    }                                                 // LinnStruments next to each-other (the lower left cell of the right split will be 30)
    return 30;
  }
  if (offset >= 12)  return 18;                       // 8th string of a guitar, down an octave
  if (offset >= 0)   return 30;                       // 8th string of a guitar
  if (offset >= -11) return 65;
  if (offset >= -16) return 18 - 7 * offset;          // ensures lower left cell is 18

  // fall back to the global row offset
  offset = Global.rowOffset;
  if (offset == ROWOFFSET_ZERO) return 30;
  if (offset == ROWOFFSET_GUITAR) return Global.guitarTuning[0];
  if (offset == ROWOFFSET_NOOVERLAP) {
    if (Global.splitActive && side == RIGHT) {        // see above
      return 30 - (Global.splitPoint - 1);
    }
    return 30;
  }
  if (offset == ROWOFFSET_OCTAVECUSTOM) {
    offset = Global.customRowOffset;
  }
  if (offset >= 12)  return 18;
  if (offset >= 0)   return 30;
  if (offset >= -11) return 65;
  if (offset >= -16) return 18 - 7 * offset;
  return 64;                                          // offset must be -17 which is -GUI, lowest = guitar's high E string
}



byte patternChainOtherSplit (byte split, byte role) {  // probably won't work for midi NRPN messages
  if (Split[split].sequencer) return split;
  boolean switchFound = false;                   // switch won't be found only if switchBothSplits was false
  for (byte i = 0; i < 5; ++i) {
    if (Global.switchAssignment[i] == ASSIGNED_TAP_TEMPO 
     && Global.customSwitchAssignment[i] == role
     && Global.switchBothSplits[i]) {
        switchFound = true;
     }
  }
  if (switchFound) split = RIGHT - split;        // use the other split instead
  return split;
}



    ************** OLD WAY to initialize major and minor scales **************
    short comma = 2 * M2 - round (i * yo3rd);           // gu comma, to find the yo and gu scales

    microLinn.scales[i][0] |= 4;                         // set scale #2 to yo scale
    microLinn.scales[i][M2] |= 4;                        // w2
    microLinn.scales[i][2*M2 - comma] |= 4;              // y3
    microLinn.scales[i][2*M2 + m2] |= 4;                 // w4
    microLinn.scales[i][3*M2 + m2] |= 4;                 // w5
    microLinn.scales[i][4*M2 + m2 - comma] |= 4;         // y6
    microLinn.scales[i][i - m2 - comma] |= 4;            // y7

    microLinn.scales[i][0] |= 8;                         // set scale #3 to gu scale
    microLinn.scales[i][M2] |= 8;                        // w2
    microLinn.scales[i][M2 + m2 + comma] |= 8;           // g3
    microLinn.scales[i][2*M2 + m2] |= 8;                 // w4
    microLinn.scales[i][3*M2 + m2] |= 8;                 // w5
    microLinn.scales[i][3*M2 + 2*m2 + comma] |= 8;       // g6
    microLinn.scales[i][i - M2 + comma] |= 8;            // g7



    if (isFullLefty) {
      memcpy(&Device.customLeds[2][0], &CUSTOM_LEDS_PATTERN_RAINBOWS_LEFTY[0], LED_LAYER_SIZE);
    } else {
      memcpy(&Device.customLeds[2][0], &CUSTOM_LEDS_PATTERN_RAINBOWS[0], LED_LAYER_SIZE);
    }




// extend audience message #8 by 2 chars, to store the user's choice of skip fretting or not for each split as 2 pseudo-booleans
const char ASCII_FALSE = ' ';                                                      // ascii 32, the lowest ascii char allowed in audienceMessages
const char ASCII_TRUE  = '!';                                                      // ascii 33, the 2nd lowest, plus it looks good!
const byte skipFrettingMsg = 7;                                                    // "HELLO NEW YORK" becomes "HELLO NEW YORK!!"
char * skipFretting = (char *)Device.audienceMessages + 31 * skipFrettingMsg;      // the skipFretting[2] array overlaps the messages array

void checkSkipFrettingAudienceMessage() {                                          // called before reading or writing either pseudo-boolean
  byte len = strlen (Device.audienceMessages[skipFrettingMsg]);
  skipFretting = (char *)Device.audienceMessages + 31 * skipFrettingMsg + len - 2; // find the 2nd to last char of message #8
  if (!(skipFretting[0] == ASCII_TRUE || skipFretting[0] == ASCII_FALSE)           // if either trailing char is not valid,
   || !(skipFretting[1] == ASCII_TRUE || skipFretting[1] == ASCII_FALSE)) {        // (1st run of this fork, or the user edited the message)
    skipFretting += min (2, 30 - len);                                             // extend the message (if no room, overwrite the last 2 chars)
    skipFretting[LEFT] = skipFretting[RIGHT] = ASCII_FALSE;                        // if there were invalid chars, best to initialize to false
    skipFretting[RIGHT + 1] = '\0';                                                // this line shouldn't be needed, but do it anyway just in case
  }
}

boolean isSkipFrettingOLD (byte side) {                          
  checkSkipFrettingAudienceMessage();
  return skipFretting[side] == ASCII_TRUE;                       // an invalid char (not a space or exclamation point) is assumed to be = FALSE
}

struct SkipFrettingData {                           // used to keep track of transposing, which is done via CCs to the LinnstrumentMicrotonal app
  signed char transposeOctave;
  signed char transposeTone;
  signed char transposeArrow;
};
SkipFrettingData skipFrettingData[NUMSPLITS];





void paintOctaveTransposeDisplaySkipFretting(byte side) {     // alternate version of paintOctaveTransposeDisplay
  clearDisplay();                                             // see handleOctaveTransposeNewTouchSplit in ls_settings.ino
  blinkMiddleRootNote = true;

  // Paint the octave shift value
  if (!doublePerSplit || skipFrettingData[LEFT].transposeOctave == skipFrettingData[RIGHT].transposeOctave) {
    paintOctave(Split[Global.currentPerSplit].colorMain, 8, OCTAVE_ROW, 12 * skipFrettingData[side].transposeOctave);
  }
  else if (doublePerSplit) {
    if (abs(skipFrettingData[LEFT].transposeOctave) > abs(skipFrettingData[RIGHT].transposeOctave)) {
      paintOctave(Split[LEFT].colorMain,  8, OCTAVE_ROW, 12 * skipFrettingData[LEFT].transposeOctave);
      paintOctave(Split[RIGHT].colorMain, 8, OCTAVE_ROW, 12 * skipFrettingData[RIGHT].transposeOctave);
    }
    else {
      paintOctave(Split[RIGHT].colorMain, 8, OCTAVE_ROW, 12 * skipFrettingData[RIGHT].transposeOctave);
      paintOctave(Split[LEFT].colorMain,  8, OCTAVE_ROW, 12 * skipFrettingData[LEFT].transposeOctave);
    }
  }

  // Paint the whole tone transpose values
  if (!doublePerSplit || skipFrettingData[LEFT].transposeTone == skipFrettingData[RIGHT].transposeTone) {
    paintTranspose(Split[Global.currentPerSplit].colorMain, SWITCH_1_ROW, skipFrettingData[side].transposeTone);
  }
  else if (doublePerSplit) {
    if (abs(skipFrettingData[LEFT].transposeTone) > abs(skipFrettingData[RIGHT].transposeTone)) {
      paintTranspose(Split[LEFT].colorMain,  SWITCH_1_ROW, skipFrettingData[LEFT].transposeTone);
      paintTranspose(Split[RIGHT].colorMain, SWITCH_1_ROW, skipFrettingData[RIGHT].transposeTone);
    }
    else {
      paintTranspose(Split[RIGHT].colorMain, SWITCH_1_ROW, skipFrettingData[RIGHT].transposeTone);
      paintTranspose(Split[LEFT].colorMain,  SWITCH_1_ROW, skipFrettingData[LEFT].transposeTone);
    }
  }

  // Paint the arrow transpose values
  if (!doublePerSplit || skipFrettingData[LEFT].transposeArrow == skipFrettingData[RIGHT].transposeArrow) {
    paintTranspose(Split[Global.currentPerSplit].colorMain, SWITCH_2_ROW, skipFrettingData[side].transposeArrow);
  }
  else if (doublePerSplit) {
    if (abs(skipFrettingData[LEFT].transposeArrow) > abs(skipFrettingData[RIGHT].transposeArrow)) {
      paintTranspose(Split[LEFT].colorMain,  SWITCH_2_ROW, skipFrettingData[LEFT].transposeArrow);
      paintTranspose(Split[RIGHT].colorMain, SWITCH_2_ROW, skipFrettingData[RIGHT].transposeArrow);
    }
    else {
      paintTranspose(Split[RIGHT].colorMain, SWITCH_2_ROW, skipFrettingData[RIGHT].transposeArrow);
      paintTranspose(Split[LEFT].colorMain,  SWITCH_2_ROW, skipFrettingData[LEFT].transposeArrow);
    }
  }

  paintShowSplitSelection(side);
}

from ls_displayModes.ino
void paintOctaveTransposeDisplay(byte side) {

  if (isSkipFretting(side) && Global.rowOffset > 7) {       // rowOffset > 7 to exclude 12edo Wicki-Hayden users
    paintOctaveTransposeDisplaySkipFretting (side);         // microLinn
    return;
  }

*************************************************************************************


**********************************************************************************************

const byte MICROLINN_MSG = 0;          // "LINNSTRUMENT" will be truncated to make room for the user's settings
const byte MICROLINN_MSG_LEN = 21;     // 31 chars including the null, minus sizeOf(MicroLinn)

struct __attribute__ ((packed)) MicroLinnColRow {     // packed byte like FocusCell, ":5" means 5 bits long
  byte col:5;
  byte row:3;
};

struct __attribute__ ((packed)) MicroLinn2Nybbles {   // packed byte
  signed char EDOsteps:4;                             // ":4" means 4 bits long, actual amount ranges from -7 to 7
  signed char EDOlights:4;
};

struct MicroLinn {                                    // overlaps the audience messages array
  char nullTerminator;                                // essential, truncates the audience message
  byte EDO;                                           // limited to 5-55, plus 4 for OFF
  signed char octaveStretch;                          // limited to ± 100 cents, for non-octave tunings such as bohlen-pierce
  struct MicroLinnColRow anchorCell;
  byte anchorNote;                                    // any midi note 0-127
  signed char anchorCents;                            // limited to ± 100 cents
  signed char colOffset[NUMSPLITS];                   // 2 column offsets, -34 to 33
  struct MicroLinn2Nybbles transpose[NUMSPLITS];      // 2 packed bytes, accessed not via displayMicroLinnConfig but via displayOctaveTranspose
};  

MicroLinn* microLinn = (MicroLinn*)(Device.audienceMessages + 31 * MICROLINN_MSG + MICROLINN_MSG_LEN);


// Upon powering up, this code has to decide if the audience message is text the user entered pre-forking or valid post-fork user settings
// Each fork's data struct should have one byte that will never equal zero, to make a fool-proof test whether the struct needs initializing
// For microLinn, one such byte is the EDO. The user is not allowed to set it to zero, for obvious reasons
// In the official un-forked code, the unused part of the audience message char array is always padded with trailing nulls
// So it's impossible to have a null followed by a non-null
// So if the nullTerminator is zero and the EDO is non-zero, we know the string has already been truncated and initialized
// And if not, then we know we need to initialize the string to default values
// (could possibly instead put the init function in initializeDeviceSettings in ls_settings.ino)
void setupMicroLinn() {                      // called in setup(), runs every time the Linnstrument powers up
  if (microLinn.nullTerminator != '\0'       // if user had lengthened the audience message and we haven't truncated it yet,
   || microLinn.EDO == 0) {                  // or if user has never set the EDO, then this fork must be running for the very first time
    microLinn.nullTerminator = '\0';
    microLinn.EDO = 4;                      
    microLinn.octaveStretch = 0; 
    microLinn.anchorRow = 4;                 // 4th row from the top
    microLinn.anchorCol = 11;
    microLinn.anchorNote = 60;               // middle-C
    microLinn.anchorCents = 0;  
    microLinn.colOffset[LEFT] = 1;
    microLinn.colOffset[RIGHT] = 1;
    microLinn.transpose[LEFT].EDOsteps = 0;
    microLinn.transpose[RIGHT].EDOsteps = 0;
    microLinn.transpose[LEFT].EDOlights = 0;
    microLinn.transpose[RIGHT].EDOlights = 0;
  }
    memcpy (microLinn.dots, MICROLINN_DOTS, sizeof(microLinn.dots));
    memcpy (microLinn.rainbows, MICROLINN_RAINBOWS, sizeof(microLinn.rainbows));
    memcpy (Device.microLinn.scales, MICROLINN_SCALES, sizeof(Device.microLinn.scales));
    memset (microLinnCurrScale, 0, sizeof(microLinnCurrScale));
    Global.activeNotes = 0;
    // move the above 5 lines up into the if statement once data storage is figured out
    Global.setSwitchAssignment(3, ASSIGNED_MICROLINN_EDO_UP,   false);  // for debugging
    Global.setSwitchAssignment(2, ASSIGNED_MICROLINN_EDO_DOWN, false);  // for debugging
  microLinnSetGlobalView();
  microLinnInitializeScales();             // delete this line once those arrays are getting saved
  microLinnCalcRowOffsetCents();
  microLinnCalcColOffsetCents(LEFT);
  microLinnCalcColOffsetCents(RIGHT);
  microLinnPrevEDO = microLinn.EDO;
  calcMicroLinnTuning();
  updateDisplay();
  microLinnUpdateAnchorString();
}



void paintPatternChainDebug() {
  for (byte side = 0; side < 2; ++side) {
    for (byte pattern = 0; pattern < 4; ++pattern) {
      byte color;
      switch (patternChain[side][pattern]) {
        case -1: color =  8; break;   // white
        case  0: color =  1; break;   // red
        case  1: color = 10; break;   // yellow
        case  2: color =  3; break;   // green
        case  3: color =  5; break;   // blue
        default: color =  6; break;   // purple
      }
      setLed(SEQ_PATTERN_SELECTOR_LEFT - 3 + side, 7 - pattern, color, cellOn);
    }
  }
}


// old way with scales #2 = zo, #3 = ru and #4 = ila/tha
const byte MICROLINN_SCALES[MICROLINN_ARRAY_SIZE] = {
  // scale 0 = yo, 1 is gu, 2 = zo, 3 = ru, 4 = ila/tha, 5 = blank but for the tonic, 6 = yaza rainbow, 7 = full rainbow, bit 7 is unused
  //  1 y   2 g   4 z   8 r   16 l   32   64 ygzr
  127,12,76,76,12, // 5edo
  127,64,64, 0,64,64, // 6edo
  127,67, 3,67,67, 3,67, // 7edo
  127,64,64,64, 0,64,64,64, // 8edo
  127,64, 0,64,64,64,64, 0,64, // 9edo
  127, 0,64,64,64, 0,64,64,64, 0, // 10
  127,64,64,64,64, 0, 0,64,64,64,64, // 11
  127,64,67,66,65,67, 0,67,66,65,66,65, // 12
  127,64,64,64,64,64, 0, 0,64,64,64,64,64, // 13
  127,64,64,64, 0,64,64, 0,64,64, 0,64,64,64, // 14
  127,64,64, 3,66,65,67, 0, 0,67,66,65, 0,66,65, // 15
  127, 0,64,67,66,65, 0,67, 0,67, 0,66,65,66,65, 0, // 16
  127,64, 0,92,68,16,72,92, 0, 0,92,68,16,72,68,16,72, // 17
  127, 0,64,64,64,64,64,64,64, 0,64,64,64,64,64,64,64, 0, // 18
  127, 0,64,67, 0,66,65, 0,67, 0, 0,67, 0,66,65, 0,66,65, 0, // 19
  127, 0,64,67, 0,66,65, 0,67, 0,64, 0,67, 0,66,65, 0,66,65, 0, // 20
  127, 0,64,64,64,66,64,64, 0,64, 0, 0,64, 0,64,64,64,64,64,64, 0, // 21
  127,64,64,64,79,68,66,65,72,79, 0,64, 0,79,68,66,65,72,68,66,65,72, // 22
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 23
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0,64, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 24
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0,64,64, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 25
  127,64,64, 0,95,64,68,66,16,65,72,95, 0,64, 0,95,68,66,16,65,72,68,66,16,65,72, // 26
  127,64,64, 0,95,64,68,66,16,65,72,95, 0,64,64, 0,95,68,66,16,65,72,68,66,16,65,72, // 27
  127,64,64, 0,95,64,68,66,16,65,72, 0,95, 0,64, 0,95, 0,68,66,16,65,72,68,66,16,65,72, // 28
  127,64,64, 0,64,95,68,66,16,65,72, 0,95, 0,64,64, 0,95, 0,68,66,16,65,72,68,66,16,65,72, // 29
  127, 0,64,64, 0,95,64,68,66,16,65,72,95, 0, 0,64, 0, 0,95,68,66,16,65,72,68,66,16,65,72, 0, // 30
  127, 0,64,64, 0,95,64,68,66,16,65,72, 0,95, 0,64,64, 0,95, 0,68,66,16,65,72,68,66,16,65,72, 0, // 31
  127, 0,64,64, 0,95,64,68,66,16,65,72, 0,95, 0, 0,64, 0, 0,95, 0,68,66,16,65,72,68,66,16,65,72, 0, // 32
  127, 0,64,64, 0,95,64, 0,68,66,16,65,72, 0,95, 0,64,64, 0,95, 0,68,66,16,65,72, 0,68,66,16,65,72, 0, // 33
  127, 0,64,64, 0,95,64, 0,68,66,16,65,72, 0,95, 0, 0,64, 0, 0,95, 0,68,66,16,65,72, 0,68,66,16,65,72, 0, // 34
  127, 0,64,64, 0,95,64, 0,68,66,16,65,72, 0, 0,95, 0,64,64, 0,95, 0, 0,68,66,16,65,72, 0,68,66,16,65,72, 0, // 35
  127, 0,64,64, 0, 0,79,64,68,66, 0, 0,65,72, 0,79, 0, 0,64, 0, 0,79, 0,68,66, 0, 0,65,72,68,66, 0, 0,65,72, 0, // 36
  127, 0, 0,64,64, 0,95,64, 0,68,66,16,65,72, 0,95, 0, 0,64,64, 0, 0,95, 0,68,66,16,65,72, 0,68,66,16,65,72, 0, 0, // 37
  127, 0, 0,64,64, 0,95,64, 0,68,66,16,65,72, 0, 0,95, 0, 0,64, 0, 0,95, 0, 0,68,66,16,65,72, 0,68,66,16,65,72, 0, 0, // 38
  127, 0,64,64, 0, 0,64,79, 0,68,66, 0, 0,65,72, 0,79, 0, 0,64,64, 0, 0,79, 0,68,66, 0, 0,65,72, 0,68,66, 0, 1,72,64, 0, // 39
  127, 0, 0,64,64, 0, 0,79,64,68,66, 0, 0,65,72, 0, 0,79, 0, 0,64, 0, 0,79, 0, 0,68,66, 0, 0,65,72,68,66, 0, 0,65,72, 0, 0, // 40
  127, 0,64, 0,64, 0,64,95,64,68, 0,66,16,65, 0,72, 0,95, 0,64,64,64,64, 0,95, 0,68, 0,66,16,65, 0,72,68,64,66,16,65, 0,72, 0, // 41
  127, 0, 0,64,64, 0, 0,79,64, 0,68,66, 0, 0,65,72, 0,79, 0, 0, 0,64, 0, 0, 0,79, 0, 0,68,66, 0, 0,65,72,68,66, 0, 0,65,72, 0, 0, // 42
  127, 0, 0,64,64, 0, 0,79, 0,68, 0,66, 0, 0,65, 0,72, 0,79, 0, 0,64,64, 0, 0,79, 0,68, 0,66, 0, 0,65, 0,76, 0,66, 0, 0,65, 0,72, 0, // 43
  127, 0, 0,64, 0,64, 0, 0,95, 0,68, 0,66,16,65, 0,72, 0,95, 0, 0,64, 0,64, 0, 0,95, 0,68, 0,66,16,65, 0,72, 0,68, 0,66,16,65, 0,72, 0, // 44
  127, 0, 0,64,64, 0, 0,95, 0, 0,68, 0,66,16,65, 0,72, 0, 0,95, 0, 0,64,64, 0, 0,95, 0, 0,68, 0,66,16,65, 0,72,68, 0,66,16,65, 0,72, 0, 0, // 45
  127, 0, 0,64,64, 0, 0, 0,79, 0,68, 0,66, 0, 0,65, 0,72, 0,79, 0, 0, 0,64, 0, 0, 0,79, 0,68, 0,66, 0, 0,65, 0,72,68, 0,66, 0,65, 0,72, 0, 0, // 46
  127, 0, 0,64,64, 0, 0,79, 0, 0,68, 0,66, 0, 0,65, 0,72, 0, 0,79, 0, 0,64,64, 0, 0,79, 0, 0,68, 0,66, 0, 0,65, 0,76, 0,66, 0, 0,65, 0,72, 0, 0, // 47
  127, 0, 0, 0,64, 0, 0, 0,95, 0,68, 0,66, 0,16, 0,65, 0,72, 0,95, 0, 0, 0,64, 0, 0, 0,95, 0,68, 0,66, 0,16, 0,65, 0,76, 0,66, 0,16, 0,65, 0,72, 0, // 48
  127, 0, 0, 0,64, 0, 0, 0,79, 0, 0,68, 0,66, 0, 0,65, 0,72, 0,79, 0, 0, 0,64,64, 0, 0, 0,79, 0,68, 0,66, 0, 0,65, 0,72, 0,68, 0,66, 0, 0,65, 0,72, 0, // 49
  127, 0, 0,64, 0,64, 0, 0,95, 0,64,68, 0,66,16, 0,65, 0,72, 0, 0,95, 0, 0, 0,64, 0, 0, 0,95, 0, 0,68, 0,66,16, 0,65, 0,72,68, 0,66,16, 0,65, 0,72, 0, 0, // 50
  127, 0, 0,64, 0,64, 0,64, 0,95, 0, 0,68, 0,66,16,65, 0,72, 0, 0,95, 0, 0, 0,64,64, 0, 0, 0,95, 0, 0,68, 0,66,16,65, 0,72, 0, 0,68, 0,66,16,65, 0,72, 0, 0, // 51
  127, 0, 0, 0,64, 0, 0, 0,95, 0, 0,68, 0,66, 0,16, 0,65, 0,72, 0, 0,95, 0, 0, 0,64, 0, 0, 0,95, 0, 0,68, 0,66, 0,16, 0,65, 0,76, 0,66, 0,16, 0,65, 0,72, 0, 0, // 52
  127, 0, 0,64, 0,64, 0, 0,64,95,64, 0,68, 0,66,16, 0,65, 0,72, 0, 0,95, 0, 0,64,64,64,64, 0, 0,95, 0, 0,68, 0,66,16, 0,65, 0,72, 0,68,64,66,16, 0,65, 0,72, 0, 0, // 53
  127, 0, 0,64, 0,64, 0, 0,64,31,64, 0,64, 4,64, 2,16,65, 0,72, 0, 0,95, 0,64, 0,64, 0,64, 0,64, 0,95, 0, 0,68, 0,66,16, 1,64, 8,64, 0,64, 4,64, 2,16,65, 0,72, 0, 0, // 54
  127, 0, 0,64, 0,64, 0,64, 0,95, 0,64,68, 0,66, 0,80, 0,65, 0,72, 0, 0,95, 0,64, 0,64,64, 0,64, 0,95, 0, 0,68, 0,66, 0,80, 0,65, 0,72,68, 0,66, 0,80, 0,65, 0,72, 0, 0  // 55
};





Data compression gains:

           struct MicroLinnSplit {
8 - 4 = 4    byte colOffset;                         // column offset, 1 to 10, 1 = OFF
8 - 6 = 2    signed char rowOffset;                  // overrides the global row offset, range is ±25 plus -26 = OFF and +26 = NOVR (no overlap)
8 - 3 = 5 ?  byte hammerOnMode;                      // do hammer-ons send a new midi note or bend the old one? (guitar = yes, flute = no)
8 - 6 = 2    byte hammerOnZone;                      // maximum width in tens of cents between two note-ons to make a hammer-on, 0..50, 0 = OFF
8 - 6 = 2    byte hammerOnWait;                      // minimum time in tens of milliseconds between two note-ons to make a hammer-on, 0..50
8 - 3 = 5    byte showCustomLEDs;                    // 0 = OFF, 1-3 = the three patterns, 4-6 = the three patterns plus note lights on top
8 - 5 = 3 ?  byte condensedBendPerPad;               // width of a single-pad pitch bend in edosteps, 0 = OFF, 1 = VAR, 2..L+1 = 1..L (L = largest scale step),
8 - 3 = 5    byte defaultLayout;                     // 0 = OFF, 1-2 = Bosanquet, 3 = Accordion, 4-5 = Wicki-Hayden, 6-7 = Array mbira
8 - 2 = 6    byte tuningTable;                       // 0..3 = OFF/ON/CC/RCH, output in edostep format (1 midi note = 1 edostep), lowest note is always note 0
8 - 7 = 1    signed char midiGroupCC;                // sent with each note-on, ranges from 0 to 119, -1 = OFF
8 - 4 = 4    signed char transposeEDOsteps;          // accessed via displayOctaveTranspose screen, -7..7
8 - 0 = 8    byte reserved1;                         // reserved for future use, 1 byte per empty menu row
8 - 0 = 8    byte reserved2;                         //    "
8 - 0 = 8    byte reserved3;                         //    "
8 - 0 = 8    byte reserved4;                         //    "
8 - 0 = 8    byte reserved5;                         //    "
8 - 0 = 8    byte reserved6;                         //    "
           };
save 89 bits = 11 bytes x 14 = 154 bytes


            // per-split settings
            struct SplitSettings {
 8 - 2 = 6    byte midiMode;                          // 0 = one channel, 1 = note per channel, 2 = row per channel
 8 - 4 = 4    byte midiChanMain;                      // main midi channel, 1 to 16
 8 - 1 = 7    boolean midiChanMainEnabled;            // true when the midi main channel is enabled to send common data, false in not
 8 - 4 = 4    byte midiChanPerRow;                    // per-row midi channel, 1 to 16
 8 - 1 = 7    boolean midiChanPerRowReversed;         // indicates whether channel per row channels count upwards or downwards across the rows
128-16 =112   boolean midiChanSet[16];                // Indicates whether each channel is used.  If midiMode!=channelPerNote, only one channel can be set.
 8 - 2 = 6    BendRangeOption bendRangeOption;        // see BendRangeOption, 2/3/12/24
 8 - 7 = 1    byte customBendRange;                   // 1 - 96
 8 - 1 = 7    boolean sendX;                          // true to send continuous X, false if not
 8 - 1 = 7    boolean sendY;                          // true to send continuous Y, false if not
 8 - 1 = 7    boolean sendZ;                          // true to send continuous Z, false if not
 8 - 1 = 7    boolean pitchCorrectQuantize;           // true to quantize pitch of initial touch, false if not
 8 - 2 = 6    byte pitchCorrectHold;                  // See PitchCorrectHoldSpeed values
 8 - 1 = 7    boolean pitchResetOnRelease;            // true to enable pitch bend being set back to 0 when releasing a touch
 8 - 2 = 6    TimbreExpression expressionForY;        // the expression that should be used for timbre
16 - 7 = 9    unsigned short customCCForY;            // 0-129 (with 128 and 129 being placeholders for PolyPressure and ChannelPressure)
16 - 7 = 9    unsigned short minForY;                 // 0-127
16 - 7 = 9    unsigned short maxForY;                 // 0-127
 8 - 1 = 7    boolean relativeY;                      // true when Y should be sent relative to the initial touch, false when it's absolute
 8 - 0 = 8  //byte padding1;                          // added by the compiler, declaring it explicitly helps with the updating and importing code
16 - 7 = 9    unsigned short initialRelativeY;        // 0-127
 8 - 2 = 6    LoudnessExpression expressionForZ;      // the expression that should be used for loudness
 8 - 0 = 8  //byte padding2;                          // see padding1
16 - 7 = 9    unsigned short customCCForZ;            // 0-127
16 - 7 = 9    unsigned short minForZ;                 // 0-127
16 - 7 = 9    unsigned short maxForZ;                 // 0-127
 8 - 1 = 7    boolean ccForZ14Bit;                    // true when 14-bit messages should be sent when Z CC is between 0-31, false when only 7-bit messages should be sent
 8 - 0 = 8  //byte padding3;                          // see padding1
128-56 =72    unsigned short ccForFader[8];           // each fader can control a CC number ranging from 0-128 (with 128 being placeholder for ChannelPressure)
 8 - 4 = 4    byte colorMain;                         // color for non-accented cells
 8 - 4 = 4    byte colorAccent;                       // color for accented cells
 8 - 4 = 4    byte colorPlayed;                       // color for played notes
 8 - 4 = 4    byte colorLowRow;                       // color for low row if on
 8 - 4 = 4    byte colorSequencerEmpty;               // color for sequencer low row step with no events
 8 - 4 = 4    byte colorSequencerEvent;               // color for sequencer low row step with events
 8 - 4 = 4    byte colorSequencerDisabled;            // color for sequencer low row step that's not being played
 8 - 5 = 3 ?  byte playedTouchMode;                   // see PlayedTouchMode values
 8 - 3 = 5    byte lowRowMode;                        // see LowRowMode values
 8 - 1 = 7    byte lowRowCCXBehavior;                 // see LowRowCCBehavior values
16 - 7 = 7 ?  unsigned short ccForLowRow;             // 0-128 (with 128 being placeholder for ChannelPressure)
 8 - 1 = 7    byte lowRowCCXYZBehavior;               // see LowRowCCBehavior values
 8 - 0 = 8  //byte padding4;                          // see padding1
16 - 7 = 9 ?  unsigned short ccForLowRowX;            // 0-128 (with 128 being placeholder for ChannelPressure)
16 - 7 = 9 ?  unsigned short ccForLowRowY;            // 0-128 (with 128 being placeholder for ChannelPressure)
16 - 7 = 9 ?  unsigned short ccForLowRowZ;            // 0-128 (with 128 being placeholder for ChannelPressure)
 8 - 4 = 4    signed char transposeOctave;            // -60, -48, -36, -24, -12, 0, +12, +24, +36, +48, +60
 8 - 4 = 4    signed char transposePitch;             // transpose output midi notes. Range is -7 to +7
 8 - 4 = 4    signed char transposeLights;            // transpose lights on display. Range is -7 to +7
 8 - 3 = 5    boolean ccFaders;                       // true to activated 8 CC faders for this split, false for regular music performance
 8 - 0 = 8    boolean arpeggiator;                    // true when the arpeggiator is on, false if notes should be played directly
 8 - 0 = 8    boolean strum;                          // true when this split strums the touches of the other split
 8 - 1 = 7    boolean mpe;                            // true when MPE is active for this split
 8 - 0 = 8    boolean sequencer;                      // true when the sequencer of this split is enabled
 8 - 2 = 6    SequencerView sequencerView;            // see SequencerView
            MicroLinnSplit microLinn;               // microtonal data
 8 - 0 = 8  //byte padding5;                          // added by the compiler, makes the SplitSettings struct an even number of bytes
         };
// 527 bits = 68 bytes x 14 = 680 + 272 = 952 bytes


microlinnSplit = 154 bytes
split          = 952 bytes
device =         765 bytes

1858 bytes


















**********************************************************************************************/
