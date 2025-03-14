/**************************************** MICROLINN ****************************************

This code augments the official code and does not replace any of it. It only runs if 
  something in the microLinnConfig display is turned ON
  any sequencer patterns are chained
  a switch or footswitch is PLAY, PREV, NEXT or MUTE and the sequencer isn't visible
  the played mode is BLNK
  SAME or BLNK is enabled on both splits
  a footswitch is set to 8VE±
  etc.
The code that actually handles playing in real time is unchanged, so there should be no increases in latency

When the user releases a column 0 button, there is about a 1/8 second delay before one can play
When microLinn is on, there's a slight extra delay while caculating tuning tables (note to self: test again)
But getNoteNumber() will use these tables, which might speed up the playing experience

a function that starts with "microLinn" is only called in this .ino file, e.g. microLinnTriIndex()
as opposed to a function that contains "microLinn" but doesn't start with it, e.g. isMicroLinnOn()

The maximum row offset is increased from 16 to 25, so that one row begins where another one ends
The maximum column offset is 8 so that notes from the other 7 rows can fill in the gaps

note to self: user can enter/exit microLinn only by going to microLinnConfig and changing the edo from 4 
or by loading a preset (directly or via NRPN) 
the EDO+ and EDO- switches/pedals don't let you enter or exit

note to self: to use multi-channel output with my non-MPE s90es synth, deselect the main channel


======================= TO DO ==============================

MUST-DO #1

fix the user data overwriting issue by modifying ls_extstorage.ino
the uninstall code must adjust Split[side].playedTouchMode since no BLNK mode
also adjust Global.customSwitchAssignment[switchSelect] since no EDO or SCALE up/down
  change microLinnUninstall from a byte to a boolean? default to true for now, false after testing
  change 128 to microLinnBaseVersion, so that Device.Version = microLinnBaseVersion + 16
  change all (whatever)VLatest to microLinn(whatever)V16
  settingsVersion = the old version, read from the 1st byte of the data structures
  Device.Version = new version, explicitly set in ls_settings.ino, e.g. official fork has "Device.version = 16;"
  the microLinn fork will have "Device.version = microLinnBaseVersion + 16;" which is 144.0
  copyConfigurationVLatest takes us from 16 to 144.0 
  restoreNonMicroLinn takes us from 144.0 to 16 (rename to uninstallMicroLinn?)
  how does Device.microLinn.MLversion work?
  delete code for col 16 and/or 17 shortcuts (search for ifndef)
  V14 is used by firmware v2.1.0
    in DeviceSettings, new byte splitHandedness
    in GlobalSettings, new unsigned short arrays ccForSwitchSustain[4] and ccForSwitchCC65[4]
    in SplitSettings, new boolean midiChanPerRowReversed
    in SplitSettings, new unsigned short initialRelativeY
    other additions too
  V15 is used by firmware v2.2.0, v2.2.1, v2.2.2
    in GlobalSettings, new byte array guitarTuning[MAXROWS]
    in GlobalSettings, the five switch arrays were lengthened from [4] to [5] (virtual 3rd pedal)
    preset[4] was lengthened to preset[6]
  microLinnV16 is used by firmware v2.3.0, v2.3.3
    in DeviceSettings, new array of bytes customLeds[LED_PATTERNS][LED_LAYER_SIZE] (675 bytes)

  DeviceSettingsV12 = 2204 bytes
  MicroLinnDevice = 4591 bytes
  DeviceSettings  = 7549 bytes, so pre-microLinn it was 2958
  PresetSettings = 400 bytes (Global plus both splits)
  thus the 6 presets will be 2400 bytes
  total is 10349 bytes

  microLinn sizeof(config) = 16564 bytes

Once this is done:
make microLinnBendPerPad, microLinnRowOffset, drumPadMode and microLinnLocatorCCs permanent microLinn vars, not just runtime vars
add Global.microLinn.equaveSemitones 1..48 and Global.microLinn.equaveCents -50..+50
  setting edo to 36 and equave to 600c makes 72edo! 26edo and 240c = 130edo
reduce Global.mainNotes and Global.accentNotes from [12] to [9] and change from int to short? 96 bytes becomes 36 bytes
  saves 420 bytes with 6 presets, 840 bytes if 12 presets
write loadEdoData() and saveEdoData(), edo data = that edo's rainbow and dots (no scales because we already store 7)
  these functions are only called in loadSettingsFromPreset() and storeSettingsToPreset()
  byte Global.microLinn.rainbow[MICROLINN_MAX_EDO]      // one row of Device.microLinn.rainbow[MICROLINN_MAX_ARRAY_SIZE]
  byte Global.microLinn.dots[MICROLINN_MAX_EDO]         // also stores the scale of a large edo
when microLinn is installed, move the 16 sequencer projects around so that the row of the 4x4 array doesn't change
  the uninstall code moves them back

///////////////// Jim's progress bar code ///////////////////////

diff --git a/ls_serial.ino b/ls_serial.ino

@@ -204,6 +204,10 @@ int negotiateIncomingCRC(byte* buffer, uint8_t size) {
   return 1;
 }
 
+void progress(float done, byte color) {
+  setLed(1 + (int)(done*25.0f), 7, color, cellOn);
+}
+
 void serialSendSettings() {
   Serial.write(ackCode);
 
@@ -211,6 +215,7 @@ void serialSendSettings() {
   delayUsec(1000);
 
   int32_t confSize = sizeof(Configuration);
+  const int32_t confSizeOriginal = confSize;
 
   // send the size of the settings
   Serial.write((byte*)&confSize, sizeof(int32_t));
@@ -219,6 +224,7 @@ void serialSendSettings() {
   const uint8_t batchsize = 96;
   byte* src = (byte*)&config;
   lastSerialMoment = millis();
+  progress(0.0f, COLOR_WHITE);
   while (confSize > 0) {
     int actual = min(confSize, batchsize);
     Serial.write(src, actual);
@@ -226,12 +232,17 @@ void serialSendSettings() {
     if (!waitForSerialCheck()) return;
 
     int crc = negotiateOutgoingCRC(src, actual);
-    if (crc == -1)      return;
+    if (crc == -1)      {
+      progress(0.0f, COLOR_RED);
+      return;
+    }
     else if (crc == 0)  continue;
 
     confSize -= actual;
     src += actual;
+    progress(((float)(confSizeOriginal - confSize)) / (float)confSizeOriginal, COLOR_WHITE);
   }
+  progress(1.0f, COLOR_WHITE);
 
   Serial.write(ackCode);
 }
///////////////////////////////

    
MUST-DO #2

use virtual edosteps to get the midi to work
done for playing
started for sequencer
not started for arpeggiator and strummer
test all low row and special functions

make all midi notes edosteps, increase from byte to short
for all midiSend functions that have notes as inputs, change from edosteps to midi notes just before calling

a sequencer project needs the correct edo and equave, so the user should put that in the project name
for the release notes: https://www.kvraudio.com/forum/viewtopic.php?t=528045


FIX/TEST/DEBUG

prevent user from making a scale completely empty, will screw up collapse to scale

test all 5 of herman miller's 41edo lumatone layouts

test note-on, note-on, note-off with a mono synth, does tuning bend fail? what about in chanPerNote mode?

check: does using the updater app always reset the 3 custom LED patterns?

make anchor cents sound only on release, too obnoxious otherwise

test new switch function 8VE± with transposing, also make it not latch on a long press?

fix custom LED bugs: completelyRefreshLeds, startBufferedLeds, finishBufferedLeds, see paintSequencerDisplay()

fix all differing per-split values such as column offset to match as soon as double split is enabled
    when you hold R split and tap L split, the L value changes to the R value
    display the old L value in fast-blinking green for 0.25 seconds, then the new L value in non-blinking green
OR  when you hold R split and tap L split, the R value changes to the L value
    display the blinking yellow R value for 0.5 seconds, then the non-blinking yellow new L value

test PREV/NEXT footswitches with sequencer

test octave up/down footswitches while playing, does the calcTuning call make it glitchy?
  if so, copy how the original code does transposing

debug same/blink applying to the other side for note-offs, search for resetPossibleNoteCells


POSSIBLE NEW FEATURES

warning: RAM is limited, can cause Arduino IDE triple compile error

Collapse to scale:
  if microLinn is off, select one of the first 9 scales
  OR select an edo and select one of the first 7 scales
  (if display is custom LEDs or microlinn full rainbow or microlinn dots, the scale mode is stuck in OFF)
  change scale mode from OFF to set the width of a single-pad pitch bend in edosteps 1..L, L = largest scale step
  the last option is AVG = the average scale step = 1\N, smaller than L, e.g. if heptatonic AVG = 1\7 = 171¢
  all the unlit notes in each column go away, the left/anchor column stays much the same and the horizontal range increases

  the row offset becomes inconsistent edosteps but a consistent interval, stepspan = round (numNotes * rowOffset / edo)
  but if unrounded = N.5, row offsets alternate N and N+1, e.g. if 12edo heptatonic, +6\12 makes alternating 4ths and 5ths
  if you select the guitar tuning before or after collapsing, stepspans vary, follows the formula, but N.5 is rounded down
  if you select No Overlap, stepspan = width of the split
  
  you can still bend normally within a pad for vibrato, but sliding to a note often means overshooting or undershooting
  if in CDEFGABC' and 1 edostep, to slide from D to E, end on F. from C to G, end on C', from C to C', end on A' 
  if in CDEFGABC' and 2 edosteps, to slide from D to E, slide normally, but to slide from E to F, end midway between E and F
  if in any scale and AVG mode, you can slide to an octave normally, and slide to any note normally if Quantize Hold is on
  if you change scales while in scale mode, the L and AVG numbers are recalculated and applied to microLinnBendPerPad
  if you change to one of the custom LED patterns or to microlinn full rainbow or dots, the last scale selected is used
  
  Large EDO subset of up to 55 notes:
    for an N-note scale, select N-edo then select large EDO = L-edo, runs 56..440 (or N+1..440?)
    it's a good idea to pad a very uneven scale with a few extra notes, makes bends work better
    short Global.microLinn.largeEDO = 56..maxLargeEDO plus 55 = OFF
    byte Global.microLinn.scales[55] = repurposed from 7 scales to be for 1 scale, 55 x 8 = 440 max edo
      pick N-edo, pick L-edo, enter scales screen, see a large screen loaded with N-edo mapped evenly onto L-edo
        symmetrical mode, for (i = 0; i < N; ++i) set (largeEDO, round(i*L/N));
        if N is even and L is odd, rounding issues, oh well
      upper left corner is 0\L, upper right is 15 or 24, etc. (make two tables for the user from the Linnstrument pics)
      add notes by tapping unlit pads, remove notes by tapping lit pads, can't add unless you first remove
      adding a note sounds the note, the pitch is calced on the spot
      if user exits by unplugging the Linn or pressing global settings button with less than N notes, 
        stay in uncollapsed N-edo, no L-edo, and the scale is the tonic only, blinking
        adjust edo? add extra notes? send them back somehow? (do a return in handleControlButtonNewTouch?) 
        maybe all notes blink when the count is short? or all turn white?
      colors match the N-edo rainbow, removing a note changes the color of all higher notes, helps with counting notes
      if edo > 200 or 128, buttons on the lefthand column or bottom row select extra pages (make pics for this too)
      if largeEDO is changed, this scale is adjusted by direct scaling
      if N-edo is changed, load L-edo with N evenly spaced notes colored with the new edo's rainbow

add new color between lime and green for neutral intervals?
investigate lumatone/bosanquet-style color schemes

add new played mode rainbow (RNBW or RNB) that reveals the rainbow color even on the fretboard or custom LED pattern
  or just have it kick in when cell color = off?

hammerons/pulloffs screen, select for each split
  window of so many cents, 0 = off
  option to send or not send a new noteon when hammering or pulling off? (guitar = yes, wind instrument = no)
  pulloffs: old note's velocity = the new note's velocity or the original old velocity? (pull-off vs lift-off)
  byte pullOffVelocity; 0 = 1st noteOn veloc, 1 = 2nd noteOn veloc, 2 = average them, 3 = 2nd note's noteOff velocity

make doublePerSplit work better for volume
  doublePerSplit becomes doubleSplitVol and doubleSplitTranspose
  volume screen shows two long bars not one, paint in rows 6 and 2, handle in rows 5-7 and 0-3
  get rid of doublePerSplit display on preset screen, get rid of doublePerSplit reset in per-split screen

long-press the low power button to get a dimmer display but not a lower scan rate (in effect a crude brightness knob)
  change operatingLowPower from a boolean to a byte (both are the same size), 0 = normal, 1 = dim & slow, 2 = dim

Add the ability to set the split point to col 1? enables single-tap mode to switch splits
  settings.ino handleSplitPointNewTouch() comment out first line
  handleControlButtonNewTouch()
    case SPLIT_ROW:  (doubleTap || Global.splitPoint == 1)
    maybe set Global.splitActive == false;

write code for sending and receiving microLinn NRPNs (see midi.txt)

write code for 6 new memories/presets, 6 new buttons next to the 6 standard ones (column 15 or column 22)?
  for the Linn 128, move the program change number one pad to the left to make room for the new buttons

make rainbows, scales and dots exportable and importable via NRPNs or CC messages or sysexes or serial port transfer?
  also exportable custom lights? (they are currently importable only, via CC messages)
  make linnstrument importer/exporter: duplicate the updater,
  and add new copies of the sequencer project part for the other types of data

fix "yip", see https://www.kvraudio.com/forum/viewtopic.php?p=8885419#p8885419

incorporate the channel pressure fix (will be part of future official updates anyway)
  see https://www.kvraudio.com/forum/viewtopic.php?t=591770

cleanup: search for "delete", "uncomment" and "bug"

TO DECIDE

pack the rainbows array? 2 colors per byte = 765 bytes savings, but makes it much harder to edit
maybe don't pack the constant array?

byte microLinnGetRainbowColor(byte edo, byte edostep) {
  byte index = microLinnTriIndex(edo, edostep);
  byte twoColors = Device.microLinn.rainbows[index >> 1];
  if (index % 2 == 0) return twoColors & 15; 
  return twoColors >> 4;
}

void microLinnSetRainbowColor(byte edo, byte edostep, byte color) {
  byte index = microLinnTriIndex(edo, edostep);
  byte twoColors = Device.microLinn.rainbows[index >> 1];
  if (index % 2 == 0) {
    Device.microLinn.rainbows[index >> 1] = (twoColors & 240) | color;
  } else {
    Device.microLinn.rainbows[index >> 1] = (twoColors & 15) | (color << 4);
  }
}

organize config into Gnon = global non-xen, Snon = per-split non-xen, Gxen = global xen, Sxen = per-split xen
  __ Col Row __ Gnon __ Snon __ __ edo scales __ Gxen __ Sxen __
   2 colOffset
   3 rowOffset
   5 (6) Gnon: drumPad, locatorCC1, locatorCC2
   7 (8) Snon: (collapseToScale), (hammerOn), customLED
  10 edo
  11 scales aka note lights
  13 (14) Gxen: anchor x 3, (sweeten), equaveSemitones, equaveCents, (largeEDO)
  15 (16) Sxen: tuningTable
  room for future features, can add a 2nd Gnon, Snon, Gxen or Sxen button
  when on the scales display, make col 12 row 0 button light up to match the tonic (*)
  on the 200, move everything but the far left column over 2 cells?
  after moving config buttons around, check microLinnIsDisabledConfigButton() for "1st pick an edo" messages
  On the note lights screen, move both pink walls in 1 column? or move the whole display over 2 cells if on a 200 model?

should there be an option to send both CC1 and CC74 for timbre? done by pressing both pads at once.
  same for poly pressure, channel pressure and CC11 for loudness? press 2 or 3 pads at once
  the choices could be stored in TimbreExpression and LoudnessExpression as e.g. timbreCC1+CC74
  before coding it up, ask in the forum if there's interest, then test with a reaper jsfx that dupllicates CCs

POSSIBILTIES

sweeten 41edo? widen 5/4 by shifting the top note up 2¢ and the bottom note down 2¢
  ratios to widen: 5/4 5/3 5/2 10/3 5/1 20/3 10/1     (maybe 15/8 15/4 15/2?)  (maybe 10/7?)
  ratios to narrow: 6/5 8/5 12/5 16/5 24/5 32/5 48/5  (maybe 9/5 18/5 36/5?)   (maybe 7/5?)
  pitch shifts of 2¢ are unnoticeable, worst case is Cv followed by C^m making C and G shift by 4¢, which is OK
  v(vv#5) chord's 3rd is pulled both ways, so no change, even in an add8 voicing that pulls up harder than down
    Struct Pull {boolean sharp, flat;};
    Pull pull[MAXCOLS*MAXROWS];
    if (pull[cell].sharp && !pull[cell].flat) sharpen(cell);
    if (pull[cell].flat && !pull[cell].sharp) flatten(cell);
  issues: in dim2 chord or v(b5) chord, 7/5 is 2¢ sharper = 5¢ sharp (add 7/5 to the narrowing list?)
  in a v9 chord, 9th is 2¢ sharper = 3¢ sharp, and in an ^9 chord, root is 3¢ flat (add 9/5 to the narrowing list?)
  in a vM7no3 or vM7(4) chord, the root-5th interval is 2¢ flat (widen 15/8?)
  would 11-limit chords become worse?

implement color and scale tables, to speed up cell painting? gets calced in calcTuning()
  byte microLinnColor[NUMSPLITS][MAXCOLS][MAXROWS];              // high 4 bits: the color of each cell, low 4 bits = the CellDisplay (on, off, pulse, etc.)
  byte microLinnScale[NUMSPLITS][MAXCOLS][MAXROWS];              // bits 0-6 are for the 7 scales, bit 7 is for the dots

make system reset be a long press not a short press?

accept sysex82 messages for non-edo tunings? when quantize is on, the pitch starts at and/or drifts to the JI note

run the arpeggiator and the sequencer both at once, so that the arpeggiator uses the sequencer's rhythm & velocity?

POSSIBLE PULL REQUESTS (every non-microtonal feature that doesn't require a new user setting, test thoroughly first)
  sequencer: foot pedal fix, pattern-chaining feature, 4x4 array fix
  make SAME carry over to the other side
  long-press low power affects brightness only
  make doublePerSplit 2 independent vars for volume and transpose
  add 8VE± footswitch function
  line 722 of ls_settings is "midiPreset[0] = 0;", should probably be "midiPreset[s] = 0;"
  lines 1829 and 1830 of ls_midi.ino, "ch" should probably be "ch+1"
  line 587 of ls_fonts.ino, "static Character small_Q = { 4,", 4 should be 5

*********************************************************/

const byte MICROLINN_BASE_VERSION = 128;                   // for the updater app, Device.version = 128 + the current version
//virtual edosteps needed = 7 * maxRowOffset + 24 * maxColOffset + 1 = 368 max, but up it to 512 to give the guitar tuning more leeway
const short MICROLINN_MAX_EDOSTEPS = 512;                  // virtual edosteps run 0..511
const short MICROLINN_MAX_GUITAR_RANGE = 320;              // 512 - 192, 192 = max row length = numCols * maxColOffset = 24 * 8

const byte MICROLINN_MAJ_2ND[MICROLINN_MAX_EDO+1] = {      // defined not as best 9/8 but as best 3/2 minus best 4/3
  0,  0,  0,  0,  1,   1,  1,  1,  1,  1,    // 0-9        // but it's actually just a single edostep for edos 6, 8, 10 & 12
  1,  1,  1,  3,  2,   3,  2,  3,  4,  3,    // 10-19      // and it's 13 and 18 not 13b and 18b
  4,  3,  4,  3,  4,   5,  4,  5,  4,  5,    // 20-29      // used for transposePitch and transposeLights
  6,  5,  6,  5,  6,   5,  6,  7,  6,  7,    // 30-39
  6,  7,  8,  7,  8,   7,  8,  7,  8,  9,    // 40-49
  8,  9,  8,  9, 10,   9                     // 50-55
};

const byte MICROLINN_MAJ_3RD[MICROLINN_MAX_EDO+1] = {      // defined not as best 5/4 but as 2 * (best 3/2 minus best 4/3)
  0,  0,  0,  0,  4,   2,  4,  2,  4,  2,    // 0-9        // used to test if microLinn.guitarTuning is F#BEADGBE
  4,  2,  4,  6,  4,   6,  4,  6,  8,  6,    // 10-19      // if it isn't, the guitar tuning light is light blue
  8,  6,  8,  6,  8,  10,  8, 10,  8, 10,    // 20-29      // 13b = 6666626 and 18b = 8888848 are also allowed
 12, 10, 12, 10, 12,  10, 12, 14, 12, 14,    // 30-39
 12, 14, 16, 14, 16,  14, 16, 14, 16, 18,    // 40-49
 16, 18, 16, 18, 20,  18                     // 50-55
};

const byte MICROLINN_PERF_4TH[MICROLINN_MAX_EDO+1] = {     // defined as best 4/3, see MICROLINN_MAJ_3RD for usage
  0,  0,  0,  0,  5,   2,  2,  3,  3,  4,    // 0-9 
  4,  5,  5,  5,  6,   6,  7,  7,  7,  8,    // 10-19
  8,  9,  9, 10, 10,  10, 11, 11, 12, 12,    // 20-29
 12, 13, 13, 14, 14,  15, 15, 15, 16, 16,    // 30-39
 17, 17, 17, 18, 18,  19, 19, 20, 20, 20,    // 40-49
 21, 21, 22, 22, 22,  23                     // 50-55
};

// MICROLINN_SCALEROWS: for each of the 7 degrees plus the octave, the last note in each row of the note lights display

/********************** this table was generated with the following C program **********************
  for (byte edo = 5; edo <= MICROLINN_MAX_EDO; ++edo) {
    byte M2 = MICROLINN_MAJ_2ND[edo];                     // actually just a single edostep for edos 6, 8, 10 & 12
    if (edo == 10 || edo == 12) M2 = 2;                   // correct 10edo and 12edo
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
      MICROLINN_SCALEROWS[edo][2] -= 1;                   // one less 3rd, makes that last 3rd a 4th, since it's 480¢
      MICROLINN_SCALEROWS[edo][5] -= 1;                   // one less 6th, makes that last 6th a 7th, since it's 960¢
      MICROLINN_SCALEROWS[edo][6] -= 1;                   // one less 7th, makes that last 7th an 8ve, since it's 1200¢
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
  0,  1,  1,  2,  3,  3,  4,  5,  // 5edo **
  0,  1,  2,  2,  3,  4,  5,  6,  // 6edo
  0,  1,  2,  3,  4,  5,  6,  7,  // 7edo
  0,  1,  2,  3,  5,  6,  7,  8,  // 8edo ***
  0,  1,  3,  4,  5,  7,  8,  9,  // 9edo ***
  0,  2,  3,  4,  6,  7,  9, 10,  // 10  **
  0,  2,  4,  5,  6,  8, 10, 11,  // 11  ***
  0,  2,  4,  5,  7,  9, 11, 12,  // 12
  0,  2,  4,  6,  8, 10, 12, 13,  // 13  ***
  0,  2,  4,  6,  8, 10, 12, 14,  // 14  *
  0,  3,  5,  6,  9, 11, 14, 15,  // 15  **
  1,  3,  5,  8, 10, 12, 14, 17,  // 16
  0,  3,  6,  7, 10, 13, 16, 17,  // 17
  0,  3,  6,  8, 11, 14, 17, 18,  // 18  ***
  1,  4,  7,  9, 12, 15, 18, 20,  // 19
  0,  4,  7,  8, 12, 15, 19, 20,  // 20  **
  1,  4,  7, 10, 13, 16, 19, 22,  // 21
  0,  4,  8,  9, 13, 17, 21, 22,  // 22
  1,  4,  8, 11, 14, 18, 21, 24,  // 23  ***
  1,  5,  9, 11, 15, 19, 23, 25,  // 24
  0,  5,  9, 10, 15, 19, 24, 25,  // 25  **
  1,  5,  9, 12, 16, 20, 24, 27,  // 26
  0,  5, 10, 11, 16, 21, 26, 27,  // 27
  1,  5,  9, 13, 17, 21, 25, 29,  // 28  *
  1,  6, 11, 13, 18, 23, 28, 30,  // 29
  0,  6, 11, 12, 18, 23, 29, 30,  // 30  **
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

const byte MICROLINN_DOTS [6 * MICROLINN_MAX_EDO - 24] = { 
// for each edo, this array gives the location in edosteps of each fret marker, 0 means none
// all are single dots except the 5th column is the double dot for the 4th/5th, only double if there are more than 5 dots per octave
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

const byte MICROLINN_SCALES[MICROLINN_ARRAY_SIZE] = {
  // each byte is a bitmask for all 8 scales, except bit 8 is unused because the 8th scale always includes all the notes
  // 1st scale = yo (major), 2nd = gu (minor), 3rd-6th = blank but for the tonic, 7th = yaza rainbow, 8th = full rainbow
  127, 0,64,64, 0, // 5edo
  127,64,64, 0,64,64, // 6edo
  127,64, 0,64,64, 0,64, // 7edo
  127,64,64,64, 0,64,64,64, // 8edo
  127,64, 0,64,64,64,64, 0,64, // 9edo
  127, 0,64,64,64, 0,64,64,64, 0, // 10
  127,64,64,64,64, 0, 0,64,64,64,64, // 11
  127,64,67,66,65,67, 0,67,66,65,66,65, // 12
  127,64,64,64,64,64, 0, 0,64,64,64,64,64, // 13
  127,64,64,64, 0,64,64, 0,64,64, 0,64,64,64, // 14
  127,64,64, 3,66,65,67, 0, 0,67,66,65, 0,66,65, // 15
  127, 0,64,67,66,65, 0,67, 0,67, 0,66,65,66,65, 0, // 16
  127,64, 0,67,66, 0,65,67, 0, 0,67,66, 0,65,66, 0,65, // 17
  127, 0,64,64,64,64,64,64,64, 0,64,64,64,64,64,64,64, 0, // 18
  127, 0,64,67, 0,66,65, 0,67, 0, 0,67, 0,66,65, 0,66,65, 0, // 19
  127, 0,64,67, 0,66,65, 0,67, 0,64, 0,67, 0,66,65, 0,66,65, 0, // 20
  127, 0,64,67,64,66,64,65, 0,67, 0, 0,67, 0,66,64,65,64,66,65, 0, // 21
  127,64,64,64,67,64,66,65,64,67, 0,64, 0,67,64,66,65,64,64,66,65,64, // 22
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 23
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0,64, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 24
  127, 0,64, 0,67, 0,66, 0,65, 0,67, 0,64,64, 0,67, 0,66, 0,65, 0,66, 0,65, 0, // 25
  127, 0,64,64,67,64,64,66,64,65, 0,67, 0,64, 0,67, 0,66,64,65,64,64,66,64,65, 0, // 26
  127,64,64, 0,67,64,64,66, 0,65,64,67, 0,64,64, 0,67,64,66, 0,65,64,64,66, 0,65,64, // 27
  127,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0,64, 0,67, 0,64,66, 0,65,64,64,66, 0,65,64, // 28
  127, 0,64,64,64,67, 0,64,66,65,64, 0,67, 0,64,64, 0,67, 0,64,66,65,64, 0,64,66,65,64, 0, // 29
  127, 0,64,64, 0,67,64,64,66, 0,65,64,67, 0, 0,64, 0, 0,67,64,66, 0,65,64,64,66, 0,65,64, 0, // 30
  127, 0,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0,64,64, 0,67, 0,64,66, 0,65,64,64,66, 0,65,64, 0, // 31
  127, 0,64,64, 0,67,64,64,66, 0,65,64, 0,67, 0, 0,64, 0, 0,67, 0,64,66, 0,65,64,64,66, 0,65,64, 0, // 32
  127, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0,67, 0,64,64, 0,67, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, // 33
  127, 0,64,64, 0,67,64, 0,64,66, 0,65,64, 0,67, 0, 0,64, 0, 0,67, 0,64,66, 0,65,64, 0,64,66, 0,65,64, 0, // 34
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

////////////  runtime vars  ///////////////////////////

short microLinnEdostep[NUMSPLITS][MAXCOLS][MAXROWS];                // each cell's edosteps relative to the anchor cell
signed char microLinnMidiNote[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];   // the midi note number for each untransposed virtual edostep, -1 = unused
short microLinnTuningBend[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];       // the deviation from 12edo as a pitch bend number from -8192 to 8191

float microLinnSemitonesPerPad[NUMSPLITS];                      // # of 12edo semitones in a pitch bend one pad wide, positive even when lefty
short microLinnColOffsetCents[NUMSPLITS];                       // row/col offset cents are set only when user sets an offset directly...
short microLinnRowOffsetCents[NUMSPLITS];                       // ...used to avoid the offset drifting too wide or too narrow when switching edos
short microLinnCustomRowOffsetCents;
short microLinnGlobalRowOffsetCents = 0;                        // 0 means user chose a custom offset, see microLinnAdjustColAndRowOffsets()
short microLinnGuitarTuningCents[MAXROWS];                      // cents between open strings, can be negative, [0] is unused
short microLinnGuitarTuning[MAXROWS];                           // cumulative row offsets, unlike Global.microLinn.guitarTuning, for a faster calcTuning()
byte microLinnHighestGuitarString;                              // used to find lowestEdostep and also to limit the guitar range
byte microLinnLowestGuitarString;                               // ditto
short microLinnLowestEdostep[NUMSPLITS];                        // what not where, lowest edostep when the anchor cell is 0, usually negative
byte microLinnChannelCount[NUMSPLITS];                          // used for rechannelling
byte microLinnNumGroups[NUMSPLITS];                             // 1 if highestEdostep - lowestEdostep < 128, 2 if < 256, 3 if < 384, 4 if < 512

byte microLinnConfigColNum = 0;                                 // active col number in microLinnConfig display, 0 means nothing active
char microLinnAnchorString[6] = "R C  ";                        // row and column of the anchor cell, e.g. "R3C12", top row is row #1
boolean microLinnDoublePerSplitColOffset;                       // true if column offset screen adjusts both splits at once
byte microLinnPrevScale;                                        // for backtracking to the previous scale easily
boolean microLinnCanBacktrack = false;                          // true if newly touching an already selected scale button
boolean microLinnConfigNowScrolling = false;
boolean microLinnMiscOptionsNowScrolling = false;
boolean microLinnUninstallNowScrolling = false;
boolean microLinnOctaveToggled = false;                         // used by the 8VE± footswitch
byte microLinnGuitarTuningRowNum = NUMROWS + 1;                 // active row number 0..7, NUMROWS+1 means no row selected, +1 so that the neighbor isn't lit
byte microLinnMiscOptionsRowNum = 0;                            // 0 = nothing selected, 6 or 5 = CC for cols 1-16 or 17-25, 2 or 3 = tuning table for left/right
byte microLinnUninstall = 2;                                    // 0 = false, 1 = true, 2 = not yet set
byte edo;                                                       // this is the only microLinn var or function that doesn't contain "microLinn" in its name

// these 2 will become Split.microLinn vars
signed char microLinnRowOffset[NUMSPLITS];                      // range is ±25 plus -26 = OFF and +26 = NOVR (no overlap)
byte microLinnShowCustomLEDs[NUMSPLITS];                        // 0 = OFF, 1-3 = the three patterns, 4-6 = patterns under note lights

// these 3 will become Global.microLinn vars
boolean microLinnDrumPadMode = false;
signed char microLinnLocatorCC1 = -1;                           // CC to send with row/column location for each note-on in cols 1-16 or cols 17-25
signed char microLinnLocatorCC2 = -1;                           // ranges from 0 to 119, -1 = OFF


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

/************** lookup functions ************************/

short microLinnTriIndex(byte edo, byte edostep) {      // index into a triangular array
  return (edo * (edo - 1)) / 2 + edostep - 10;         // -10 to skip over edos 1-4
}

byte microLinnDots(byte edo, byte fret) {
  return Device.microLinn.dots[microLinnTriIndex(edo, fret)];
}

byte microLinnRainbowColor(byte edo, byte edostep) {
  return Device.microLinn.rainbows[microLinnTriIndex(edo, edostep)];
}

byte microLinnScaleStep(byte edo, byte edostep) {
  return Device.microLinn.scales[microLinnTriIndex(edo, edostep)];
}

byte microLinnGetFret(byte side, byte col) {
// returns the distance from the "nut" of the dot pattern to the column, octave-reduced
  short fret = (isLeftHandedSplit(LEFT) && isLeftHandedSplit(RIGHT)) ? NUMCOLS - col : col;
  fret -= MICROLINN_MAJ_2ND[edo] * Split[side].transposeLights + Split[side].microLinn.transposeEDOlights;
  byte modBase = (edo == 41 ? 12 : edo);                          // because 41edo kites repeat every 12 frets
  return microLinnMod (fret - 1, modBase);
}

boolean isMicroLinnOn() {
  return Global.microLinn.EDO >= 5;
}

boolean isMicroLinnColOffset(byte side) {
  return Split[side].microLinn.colOffset > 1;
}

boolean isMicroLinnNoOverlap() {                       // says whether to call calcTuning on a split point change
  return microLinnRowOffset[LEFT] == 26 ||    
         microLinnRowOffset[RIGHT] == 26;
}

boolean isMicroLinnDrumPadMode() {
  return microLinnDrumPadMode;
}

// a cell's edostep = the distance in edosteps from the anchor cell, plus transposition
// a cell's virtual edostep = the distance in edosteps from the cell with the lowest pitch, not transposed
// virtual edostep is always >= 0, virtual edostep = edostep - microLinnLowestEdostep[side]
// the lowest edostep is usually in one of the corners, but in guitar tuning it can be in col 1 or col 25
// VirtualTouchInfo, StepEventState have note as a short 0..511 plus -1 for no note
// TouchInfo, VirtualTouchInfo have note as a signed char -1..127, also have microLinnGroup 0..3
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

// called from handleXYZupdate(), sendNewNote(), sendReleasedNote() and handleTouchRelease() in ls_handleTouches.ino
signed char getMicroLinnMidiNote(byte side, byte note, byte group) {
  if (note < 0) return -1;
  return getMicroLinnMidiNote(side, note + (group << 7));
}

// called from StepEventState::sendNoteOn() and StepEventState::sendNoteOff() in ls_sequencer.ino
signed char getMicroLinnMidiNote(byte side, short edostep) {
  if (edostep < 0) return -1;
  if (Split[side].microLinn.tuningTable > 0) {
    // discard higher bits, the DAW should use locator CCs or rechannelling to deduce them
    return microLinnMod(edostep + microLinnTransposition(sensorSplit), 128);      // bug: should this be transposed?
  }
  return microLinnMidiNote[side][edostep];
}

// called from handleXYZupdate(), sendNewNote(), sendReleasedNote() and handleTouchRelease() in ls_handleTouches.ino
signed char getMicroLinnDrumPadMidiNote() {
  if (sensorCol < 2 || sensorCol > 22 || sensorRow < 1 || sensorRow > 6) return -1;
  byte padNum = floor((sensorCol - 2) / 3) + 7 * floor((sensorRow - 1) / 3);
  return Project.sequencer[sensorSplit].seqDrumNotes[padNum];
}

// called from handleXYZupdate(), sendNewNote() and handleTouchRelease() in ls_handleTouches.ino
short getMicroLinnTuningBend(byte side, byte note, byte group) {
  if (note < 0) return 0;
  return microLinnTuningBend[side][note + (group << 7)];
}

// called from StepEventState::sendNoteOn() in ls_sequencer.ino
short getMicroLinnTuningBend(byte side, short edostep) {
  if (edostep < 0) return 0;
  return microLinnTuningBend[side][edostep];
}

// called from scalePitch() in ls_midi.ino
float getMicroLinnSemitonesPerPad(byte side) {
  return microLinnSemitonesPerPad[side];
}

short getMicroLinnRowOffset(byte side) {   // delete later, replace call with Split[side].microLinn.rowOffset
  return microLinnRowOffset[side];
}

// called from determineRowOffsetNote() in ls_handleTouches.ino, duplicates the original determineRowOffsetNote function 
short getMicroLinnRowOffsetNote(byte side, byte row) {
  signed char offset = microLinnRowOffset[side];
  if (offset <= -12) return 18 + (row - 7) * offset;      // ensure upper left cell is 18 = F#-1
  if (offset < 0)    return 65 + row * offset;            // 65 = F3 = 1st string of an all-4ths guitar
  if (offset < 12)   return 30 + row * offset;            // 30 = F#0 = 8th string of a guitar
  if (offset <= 25)  return 18 + row * offset;            // 18 = F#-1 = 8th string of a guitar, down an octave
  offset = microLinnGetSplitWidth(side);                  // row offset must be 26 = no overlap
  if (Global.splitActive && side == RIGHT) {
    return 30 - (Global.splitPoint - 1) + row * offset;   // ensure right split's lower left cell is 30 = F#0
  }
  return 30 + row * offset;
}

// called by sendNewNote() and sendReleasedNote() in ls_handleTouches.ino
// also StepEventState::sendNoteOn() and StepEventState::sendNoteOff() in ls_sequencer.ino
// if there's more than 128 virtual edosteps, send higher midi groups to other channel blocks
// this gives every pad a unique combo of midi note and midi channel
signed char rechannelMicroLinnGroup(byte side, signed char chan, byte group) {
  if (group == 0) return chan;
  if (Split[side].microLinn.tuningTable != 2) return chan;
  if (side == RIGHT) group *= -1;                         // right split adds new blocks below not above
  chan += group * microLinnChannelCount[side];
  if (chan > 16) chan -= 16;                              // wrap around in case the user picked the channels wrong
  if (chan < 1)  chan += 16;
  return chan;
}

byte getMicroLinnShowCustomLEDs(byte side) {  // delete later, replace call with Split[side].microLinn.showCustomLEDs
  return microLinnShowCustomLEDs[side];
}

// called from StepEvent::getFaderMax() in ls_sequencer.ino
byte getMicroLinnSequencerBendRange() {
  if (edo >= 12) return 50;
  return ceil(600.0 / edo);
}

/********** maybe not needed, delete? 
FocusCell microLinnFIndEdostep(byte split, short edostep) {
  byte row = 0;
  boolean success = false;
  while (!success && row < NUMROWS) {
    short col = edostep - microLinnEdostep[split][1][row];
    if (col % Split[split].microLinn.colOffset == 0) {                              // if this row doesn't skip this note
      col = 1.0 * col / Split[split].microLinn.colOffset + 1;
      success = (col > 0 && col < NUMCOLS);
    }
    if (!success) ++row;
  }
  FocusCell c;
  c.col = success ? col : 0; c.row = row;
  return c;
} **********************/

/************** initialization functions ************************/

void initializeMicroLinn() {                             // called in reset(), runs when microLinn firmware is first installed
  Split[LEFT].microLinn.colOffset = 1;                   // or when the user does a global reset
  Split[LEFT].microLinn.transposeEDOsteps = 0;
  Split[LEFT].microLinn.transposeEDOlights = 0;
  Split[LEFT].microLinn.tuningTable = 0;
  Split[LEFT].microLinn.hammerOnWindow = 0;              // 150¢ is a good default
  Split[LEFT].microLinn.hammerOnNewNoteOn = false;
  Split[LEFT].microLinn.pullOffVelocity = 0;
  microLinnRowOffset[LEFT]  = -26;                       // OFF
  microLinnRowOffset[RIGHT] = -26;
  memcpy (&Split[RIGHT].microLinn, &Split[LEFT].microLinn, sizeof(MicroLinnSplit));

  Global.microLinn.EDO = 4;                       // 4 = OFF
  Global.microLinn.octaveStretch = 0;
  Global.microLinn.anchorRow = 4;                 // 4th row from the top
  Global.microLinn.anchorCol = 11;
  Global.microLinn.anchorNote = 60;               // middle-C
  Global.microLinn.anchorCents = 0;  
  Global.microLinn.guitarTuning[0] = 0;           // 12edo F# B E A D G B E
  Global.microLinn.guitarTuning[1] = 5;           // row offset from the string below it
  Global.microLinn.guitarTuning[2] = 5;
  Global.microLinn.guitarTuning[3] = 5;
  Global.microLinn.guitarTuning[4] = 5;
  Global.microLinn.guitarTuning[5] = 5;
  Global.microLinn.guitarTuning[6] = 4;
  Global.microLinn.guitarTuning[7] = 5;
  Global.microLinn.useRainbow = true;
  Global.microLinn.sweeten = false;

  Device.microLinn.MLversion = 0;                 // 0 means OSVersionBuild ends in "A", 1 means "B", etc.
  memcpy (&Device.microLinn.scales, &MICROLINN_SCALES, MICROLINN_ARRAY_SIZE);
  memcpy (&Device.microLinn.rainbows, &MICROLINN_RAINBOWS, MICROLINN_ARRAY_SIZE);
  for (byte EDO = 5; EDO <= MICROLINN_MAX_EDO; ++EDO) microLinnResetDots(EDO);

  for (byte p = 0; p < NUMPRESETS; ++p) {
    memcpy (&config.preset[p].split[LEFT].microLinn,  &Split[LEFT].microLinn,  sizeof(MicroLinnSplit));
    memcpy (&config.preset[p].split[RIGHT].microLinn, &Split[RIGHT].microLinn, sizeof(MicroLinnSplit));
    memcpy (&config.preset[p].global.microLinn, &Global.microLinn, sizeof(MicroLinnGlobal));
  }

  // preset 5 (5th from the top) is 41edo Kite guitar (the presets run 3 2 1 0 5 4)
  config.preset[5].split[LEFT].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
  config.preset[5].split[RIGHT].midiMode = channelPerNote;
  config.preset[5].split[LEFT].bendRangeOption = bendRange24;
  config.preset[5].split[RIGHT].bendRangeOption = bendRange24;
  config.preset[5].split[LEFT].customBendRange = 24;
  config.preset[5].split[RIGHT].customBendRange = 24;
  config.preset[5].split[LEFT].expressionForZ = loudnessChannelPressure;
  config.preset[5].split[RIGHT].expressionForZ = loudnessChannelPressure;
  config.preset[5].split[LEFT].midiChanMain = 1;
  config.preset[5].split[LEFT].midiChanSet[0] = false;
  config.preset[5].split[RIGHT].midiChanMain = 16;
  config.preset[5].split[RIGHT].midiChanSet[15] = false;
  config.preset[5].global.microLinn.EDO = 41;
  config.preset[5].global.microLinn.octaveStretch = 0;
  config.preset[5].global.microLinn.anchorRow = 3;                // 5th row from top, places the two rainbows nicely
  config.preset[5].global.microLinn.anchorCol = 6;
  config.preset[5].global.microLinn.anchorNote = 54;              // ^F#2, Kite guitar standard tuning with D = 0 cents
  config.preset[5].global.microLinn.anchorCents = 39;             // ^M3 = 439c
  config.preset[5].global.microLinn.guitarTuning[0] = 0;          // alternating downmajor and upminor 3rds
  config.preset[5].global.microLinn.guitarTuning[1] = 13;         // row offset from the string below it
  config.preset[5].global.microLinn.guitarTuning[2] = 11;
  config.preset[5].global.microLinn.guitarTuning[3] = 13;
  config.preset[5].global.microLinn.guitarTuning[4] = 11;
  config.preset[5].global.microLinn.guitarTuning[5] = 13;
  config.preset[5].global.microLinn.guitarTuning[6] = 11;
  config.preset[5].global.microLinn.guitarTuning[7] = 13;
  config.preset[5].global.microLinn.sweeten = true;
  config.preset[5].global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
  config.preset[5].global.customRowOffset = 13;                   // 41-equal downmajor 3rds
  config.preset[5].split[LEFT].microLinn.colOffset = 2;
  config.preset[5].split[RIGHT].microLinn.colOffset = 2;
  //config.preset[5].split[LEFT].microLinn.rowOffset = -26;         // OFF
  //config.preset[5].split[RIGHT].microLinn.rowOffset = -26;
  //microLinnRowOffset[LEFT]  = -26;                       
  //microLinnRowOffset[RIGHT] = -26;
  config.preset[5].global.activeNotes = 6;
  config.preset[5].split[LEFT].playedTouchMode = playedBlink;
  config.preset[5].split[RIGHT].playedTouchMode = playedBlink;

  /************** uncomment once there are more presets *******************
  // preset 5 (5th from the top) is 31edo Bosanquet
  config.preset[5].split[LEFT].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
  config.preset[5].split[RIGHT].midiMode = channelPerNote;
  config.preset[5].split[LEFT].bendRangeOption = bendRange24;
  config.preset[5].split[RIGHT].bendRangeOption = bendRange24;
  config.preset[5].split[LEFT].customBendRange = 24;
  config.preset[5].split[RIGHT].customBendRange = 24;
  config.preset[5].split[LEFT].expressionForZ = loudnessChannelPressure;
  config.preset[5].split[RIGHT].expressionForZ = loudnessChannelPressure;
  config.preset[5].split[LEFT].midiChanMain = 1;
  config.preset[5].split[LEFT].midiChanSet[0] = false;
  config.preset[5].split[RIGHT].midiChanMain = 16;
  config.preset[5].split[RIGHT].midiChanSet[15] = false;
  config.preset[5].global.microLinn.EDO = 31;
  config.preset[5].global.microLinn.octaveStretch = 0;
  config.preset[5].global.microLinn.anchorRow = 4;                // 4th row from the top
  config.preset[5].global.microLinn.anchorCol = 11;
  config.preset[5].global.microLinn.anchorNote = 60;              // middle-C
  config.preset[5].global.microLinn.anchorCents = 0;
  config.preset[5].global.microLinn.guitarTuning[0] = 0;          // F# B E A D G B E
  config.preset[5].global.microLinn.guitarTuning[1] = 13;         // row offset from the string below it
  config.preset[5].global.microLinn.guitarTuning[2] = 13;
  config.preset[5].global.microLinn.guitarTuning[3] = 13;
  config.preset[5].global.microLinn.guitarTuning[4] = 13;
  config.preset[5].global.microLinn.guitarTuning[5] = 13;
  config.preset[5].global.microLinn.guitarTuning[6] = 10;
  config.preset[5].global.microLinn.guitarTuning[7] = 13;
  config.preset[5].global.activeNotes = 6;                        // partial rainbow
  config.preset[5].global.rowOffset = 3;                          // 31-equal minor 2nd
  config.preset[5].split[LEFT].microLinn.colOffset = 5;           // 31-equal major 2nd
  config.preset[5].split[RIGHT].microLinn.colOffset = 5;
  //config.preset[5].split[LEFT].microLinn.rowOffset = -26;         // OFF
  //config.preset[5].split[RIGHT].microLinn.rowOffset = -26;
  //microLinnRowOffset[LEFT]  = -26;
  //microLinnRowOffset[RIGHT] = -26;
  config.preset[5].split[LEFT].playedTouchMode = playedBlink;
  config.preset[5].split[RIGHT].playedTouchMode = playedBlink;
  *********************************************/

  setupMicroLinn();
}

void microLinnSetupKitesPersonalPrefs() {       // speed up debugging cycle, delete later once things are more stable
  Split[LEFT].midiMode = Split[RIGHT].midiMode = channelPerNote;
  Split[LEFT].bendRangeOption = Split[RIGHT].bendRangeOption = bendRange24; 
  Split[LEFT].pitchCorrectHold = Split[RIGHT].pitchCorrectHold = pitchCorrectHoldOff;
  Split[LEFT].expressionForZ = Split[RIGHT].expressionForZ = loudnessChannelPressure;
  Split[LEFT].playedTouchMode = Split[RIGHT].playedTouchMode = playedSame;
  Split[RIGHT].colorMain = COLOR_YELLOW;
  Split[RIGHT].colorLowRow = Split[LEFT].colorLowRow = COLOR_ORANGE;
  if (Global.microLinn.EDO == 4) {Global.activeNotes = 9; Global.rowOffset = 4;}      // 12edo fretboard in 3rds
  if (Global.microLinn.EDO == 41) Global.activeNotes = 8;                             // 41edo fretboard
  Global.setSwitchAssignment(3, ASSIGNED_MICROLINN_EDO_UP,   false);  // for debugging, delete later
  Global.setSwitchAssignment(2, ASSIGNED_MICROLINN_EDO_DOWN, false);  // for debugging
  //Device.operatingLowPower = 2;                    // uncomment once dim-but-fast mode is enabled
}

void setupMicroLinn() {                              // runs when the Linnstrument powers up or a preset is loaded
  microLinnSetupKitesPersonalPrefs();
  edo = Global.microLinn.EDO;
  if (edo == 4) edo = 12;                            // because "OFF" is 4edo which is really 12edo
  microLinnPrevScale = Global.activeNotes;           // to disable backtracking
  if (isMicroLinnOn()) lightSettings = LIGHTS_ACTIVE;
  microLinnDoublePerSplitColOffset = Split[LEFT].microLinn.colOffset == Split[RIGHT].microLinn.colOffset;
  microLinnStoreColOffsetCents(LEFT);
  microLinnStoreColOffsetCents(RIGHT);
  microLinnStoreRowOffsetCents(LEFT);
  microLinnStoreRowOffsetCents(RIGHT);
  storeMicroLinnGlobalRowOffsetCents();
  storeMicroLinnCustomRowOffsetCents();
  for (byte row = 1; row < NUMROWS; ++row) {
    microLinnStoreGuitarRowOffsetCents(row);
  }
  microLinnCalcGuitarTuning();
  microLinnSemitonesPerPad[LEFT]  = Split[LEFT].microLinn.colOffset;     // assumes 12edo, if not calcTuning will overwrite this
  microLinnSemitonesPerPad[RIGHT] = Split[RIGHT].microLinn.colOffset;
  calcMicroLinnTuning();
  microLinnUpdateAnchorString();
  updateDisplay();
}

void microLinnResetDots (byte edo) {
  short start1 = microLinnTriIndex (edo, 0);           // index into microLinn.dots
  short start2 = 6 * (edo - 5);                        // index into MICROLINN_DOTS
  memset (&Device.microLinn.dots[start1], 0, edo);

  if (edo == 41) {                                     // kite guitar dots
    for (byte i = 0; i < 41; i += 12) {
      Device.microLinn.dots [start1 + i] = 16;         // single dot
      Device.microLinn.dots [start1 + i + 4] = 40;     // double dot
      if (i == 36) break;
      Device.microLinn.dots [start1 + i + 8] = 84;     // triple dot
    }
    return;
  }

  for (byte i = 0; i < 6; ++i) {
    byte fretNum = MICROLINN_DOTS [start2 + i];
    Device.microLinn.dots [start1 + fretNum] = (i == 4 ? 60 : 24);             // 5th column is for double dots
    if (fretNum > 0) {
      Device.microLinn.dots [start1 + edo - fretNum] = (i == 4 ? 60 : 24);     // fill upper octave symmetrically
    }
  }
  Device.microLinn.dots [start1] = 126;                                        // tonic/octave gets a triple dot
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
  if (LINNMODEL == 128 && Global.microLinn.anchorCol >= 10) {        // "3 12 ", drop R and C to fit 3 digits in
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
  short cents = (1200.0 * edosteps) / edo;                  // unstretched 8ve because we're comparing edos not tunings
                                                            // short not float because we're computing offsets not tunings
  if (cents ==  500) cents =  498;                          // 498¢ so that 12edo -> 30edo makes 480c not 520c
  if (cents ==  700) cents =  702;
  if (cents == -500) cents = -498;
  if (cents == -700) cents = -702;
  return cents;
}

void microLinnStoreColOffsetCents(byte side) {
  microLinnColOffsetCents[side] = microLinnEDOstepsToCents(Split[side].microLinn.colOffset);
}

void microLinnStoreRowOffsetCents(byte side) {
  if (microLinnRowOffset[side] == -26 || microLinnRowOffset[side] == 26) return;         // if OFF or NOVR
  microLinnRowOffsetCents[side] = microLinnEDOstepsToCents(microLinnRowOffset[side]);
}

void storeMicroLinnGlobalRowOffsetCents() {                                // called from handleGlobalSettingNewTouch()
  if (Global.rowOffset == ROWOFFSET_ZERO ||
      Global.rowOffset == ROWOFFSET_GUITAR ||
      Global.rowOffset == ROWOFFSET_NOOVERLAP ||
      Global.rowOffset == ROWOFFSET_OCTAVECUSTOM) {
    return;
  }
  microLinnGlobalRowOffsetCents = microLinnEDOstepsToCents(Global.rowOffset);
}

void clearMicroLinnGlobalRowOffsetCents() {    // called from handleGlobalSettingHold() and handleGlobalSettingRelease() 
  microLinnGlobalRowOffsetCents = 0;           // flag the usual cents as invalid so it can't overwrite the custom cents
}

void storeMicroLinnCustomRowOffsetCents() {                                // called from handleRowOffsetNewTouch()
  if (Global.microLinn.EDO == 4 && Global.customRowOffset == -17) {
    microLinnCustomRowOffsetCents = microLinnEDOstepsToCents(-5);          // -GUI is mostly descending 4ths
  }
  else microLinnCustomRowOffsetCents = microLinnEDOstepsToCents(Global.customRowOffset);
}

void microLinnStoreGuitarRowOffsetCents(byte row) {
  microLinnGuitarTuningCents[row] = microLinnEDOstepsToCents(Global.microLinn.guitarTuning[row]);
}

// true if all intervals are 4ths except that one major 3rd, the exact pitch doesn't matter
boolean isMicroLinnGuitarTuningStandard() {
  byte third = MICROLINN_MAJ_3RD[edo];
  byte fourth = MICROLINN_PERF_4TH[edo];
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

short microLinnCheckRowOffset(short rowOffset, short cents, boolean isGuitarTuning) {
  // ensure coprime row/col offsets so that all notes of the edo are present
  // e.g. 12edo +5 +2 --> 24edo +10 +4 --> 24edo +9 +4
  // ideally if the col offset were bigger than the row offset, and non-guitar tuning, the col offset would get adjusted
  // but it might be hard to be coprime with all 5 kinds of row offsets
  short colOffsetLCM = microLinnLCM (Split[LEFT].microLinn.colOffset, Split[RIGHT].microLinn.colOffset); 
  short zigzagBy = (rowOffset * 1200 > cents * edo) ? -1 : 1;    // really (rowOffset/edo > cents/1200)
  short maxOffset = (isGuitarTuning ? MICROLINN_MAX_GUITAR_RANGE : 25);
  // this while loop exits when rowOffset is a prime number if not sooner
  while (microLinnGCD (rowOffset, colOffsetLCM) > 1 || abs(rowOffset) > maxOffset) {
    rowOffset += zigzagBy;
    zigzagBy += zigzagBy > 0 ? 1 : -1;                           // zigzag through the nearest numbers,
    zigzagBy *= -1;                                              // e.g. 9 --> 8 --> 10 --> 7
  }
  return rowOffset;
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

  // adjust the 2 per-split column offsets if already skipping frets (don't start skip-fretting unasked, too confusing)
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (Split[side].microLinn.colOffset > 1) {
      newOffset = round ((edo * microLinnColOffsetCents[side]) / 1200.0);
      newOffset = constrain (newOffset, 1, 8);
      Split[side].microLinn.colOffset = newOffset;
    }
  }

  // adjust the 2 per-split row offsets
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (microLinnRowOffset[side] >= -25 && microLinnRowOffset[side] <= 25) {        // don't adjust OFF or NOVR
      newOffset = round ((edo * microLinnRowOffsetCents[side]) / 1200.0);
      newOffset = constrain (newOffset, -25, 25);
      if (newOffset == 0) {
        newOffset = microLinnSign(microLinnRowOffset[side]);
      }
      newOffset = microLinnCheckRowOffset(newOffset, microLinnRowOffsetCents[side], false);
      microLinnRowOffset[side] = newOffset;
    }
  }

  // adjust the 7 row offsets in the guitar tuning
  if (wasStandardTuning) {
    // if using cents 12edo 55545 -> 22edo 99979, but should be 99989 to get a double 8ve between outer strings
    Global.microLinn.guitarTuning[1] = MICROLINN_PERF_4TH[edo];
    Global.microLinn.guitarTuning[2] = MICROLINN_PERF_4TH[edo];
    Global.microLinn.guitarTuning[3] = MICROLINN_PERF_4TH[edo];
    Global.microLinn.guitarTuning[4] = MICROLINN_PERF_4TH[edo];
    Global.microLinn.guitarTuning[5] = MICROLINN_PERF_4TH[edo];
    Global.microLinn.guitarTuning[6] = MICROLINN_MAJ_3RD[edo];
    Global.microLinn.guitarTuning[7] = MICROLINN_PERF_4TH[edo];
    microLinnCalcGuitarTuning();
  }
  else {
    // row = 1 because microLinnGuitarTuningCents[0] is unused
    for (byte row = 1; row < NUMROWS; ++row) {
      newOffset = round ((edo * microLinnGuitarTuningCents[row]) / 1200.0);
      if (newOffset == 0) {
        newOffset = microLinnSign(Global.microLinn.guitarTuning[row]);
      }
      newOffset = microLinnCheckRowOffset(newOffset, microLinnGuitarTuningCents[row], true);
      Global.microLinn.guitarTuning[row] = newOffset;
    }
    microLinnCalcGuitarTuning();
    short overflow = microLinnGuitarTuning[microLinnHighestGuitarString]
                   - microLinnGuitarTuning[microLinnLowestGuitarString]
                   - MICROLINN_MAX_GUITAR_RANGE;
    while (overflow > 0) {                                // while not if, to handle weird edge cases
      for (byte row = 1; row < NUMROWS; ++row) {          // shrink all 7 offsets by 1
        Global.microLinn.guitarTuning[row] -= microLinnSign(Global.microLinn.guitarTuning[row]);
      }
      microLinnCalcGuitarTuning();
      overflow = microLinnGuitarTuning[microLinnHighestGuitarString]
               - microLinnGuitarTuning[microLinnLowestGuitarString]
               - MICROLINN_MAX_GUITAR_RANGE;
    }
  }

  // adjust the custom row offset before adjusting the global row offset
  newOffset = round ((edo * microLinnCustomRowOffsetCents) / 1200.0);
  newOffset = constrain (newOffset, -25, 25);
  if (newOffset == 0) {
    newOffset = microLinnSign(Global.customRowOffset);
  }
  newOffset = microLinnCheckRowOffset(newOffset, microLinnCustomRowOffsetCents, false);
  Global.customRowOffset = newOffset;

  // adjust the global row offset, unless it's a special flag
  if (Global.rowOffset == ROWOFFSET_ZERO ||
      Global.rowOffset == ROWOFFSET_GUITAR ||
      Global.rowOffset == ROWOFFSET_NOOVERLAP) {
    return;
  }
  // did the user choose customRowOffset, as opposed to us borrowing it to store a large row offset?
  if (Global.rowOffset == ROWOFFSET_OCTAVECUSTOM && microLinnGlobalRowOffsetCents == 0) {
    return;
  }
  // if the global row offset becomes > 7 (e.g. 12edo +5 --> 22edo +9), it's stored as a custom row offset
  // but if the user later switches to a smaller edo and the row offset becomes <= 7 (e.g. 22edo +9 --> 17edo +7),
  // the row offset is stored as the global row offset, and the custom row offset isn't overwritten
  // the custom row offset will follow microLinnCustomRowOffsetCents and become an octave (or whatever)
  newOffset = round ((edo * microLinnGlobalRowOffsetCents) / 1200.0);
  newOffset = constrain (newOffset, 1, 25);
  newOffset = microLinnCheckRowOffset(newOffset, microLinnGlobalRowOffsetCents, false);
  if (newOffset >= 3 && newOffset <= 7) {
    Global.rowOffset = newOffset;
  } else {
    Global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
    Global.customRowOffset = newOffset;
  }
}

/************** tuning table functions ************************/

// calc the cumulative row offsets by summing the individual row offsets, to speed up calcTuning()
// if Global.microLinn.guitarTuning = [0 5 5 5 5, 5 4 5] then microLinnGuitarTuning = [-20 -15 -10 -5 0 5 9 14]
// called not by calcTuning() but by handleMicroLinnGuitarTuningNewTouch(), so it's done ahead of time while not waiting to play
void microLinnCalcGuitarTuning() {
  byte anchor = Global.microLinn.anchorRow;
  for (byte row = 0; row < MAXROWS; ++row) {
    short sum = 0;
    for (byte i = anchor; i < row; ++i) {             // only one of these two for-loops will run
      sum += Global.microLinn.guitarTuning[i+1];      // add them if going up
    }
    for (byte i = row; i < anchor; ++i) {
      sum -= Global.microLinn.guitarTuning[i+1];      // subtract them if going down
    }
    microLinnGuitarTuning[row] = sum;                 // microLinnGuitarTuning[anchor] always equals 0
  }

  short highestOffset = microLinnGuitarTuning[0];
  short lowestOffset  = microLinnGuitarTuning[0];
  microLinnHighestGuitarString = microLinnLowestGuitarString = 0;
  for (byte row = 1; row < MAXROWS; ++row) {
    if (microLinnGuitarTuning[row] >= highestOffset) {
      highestOffset = microLinnGuitarTuning[row];
      microLinnHighestGuitarString = row;                  // the highest pitched string, not necessarily the highest string
    }
    if (microLinnGuitarTuning[row] < lowestOffset) {
      lowestOffset = microLinnGuitarTuning[row];
      microLinnLowestGuitarString = row;
    }
  }
}

void microLinnCalcLowestEdostepEtc() {
  microLinnCalcLowestEdostepEtc(LEFT);
  microLinnCalcLowestEdostepEtc(RIGHT);
}

// calcs what, not where. also calcs numGroups and channelCount for rechanneling
void microLinnCalcLowestEdostepEtc(byte side) {
  byte lowestRow, lowestCol, highestRow, highestEdostep;
  if (microLinnRowOffset[side] >= 0) {                        // use this split's row offset if positive or NOVR
    lowestRow = 0;
  } else if (microLinnRowOffset[side] >= -25) {               // or if negative but not OFF
    lowestRow = 7;
  } else if (Global.rowOffset == ROWOFFSET_OCTAVECUSTOM) {
    lowestRow = Global.customRowOffset >= 0 ? 0 : 7;
  } else if (Global.rowOffset == ROWOFFSET_GUITAR) {
    lowestRow = microLinnLowestGuitarString;
  } else lowestRow = 0;

  microLinnLowestEdostep[side] = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, lowestRow);
  lowestCol = isLeftHandedSplit(side) ? NUMCOLS - 1 : 1;
  microLinnLowestEdostep[side] -= Split[side].microLinn.colOffset * abs(Global.microLinn.anchorCol - lowestCol);
  microLinnLowestEdostep[side] += microLinnTransposition(side);

  if (Global.rowOffset == ROWOFFSET_GUITAR && microLinnRowOffset[side] == -26) {
    highestRow = microLinnHighestGuitarString;
  } else highestRow = 7 - lowestRow;
  highestEdostep = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, highestRow);
  highestEdostep += Split[side].microLinn.colOffset * abs((NUMCOLS - lowestCol) - Global.microLinn.anchorCol);
  highestEdostep += microLinnTransposition(side);
  microLinnNumGroups[side] = ((highestEdostep - microLinnLowestEdostep[side]) >> 7) + 1;

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

short microLinnSumOfRowOffsets (byte side, byte row1, byte row2) {      // edosteps from row1 to row2
  // use this split's row offset if it's not OFF
  if (microLinnRowOffset[side] == 26) return microLinnGetSplitWidth(side) * (row2 - row1);    // if NOVR
  if (microLinnRowOffset[side] > -26) return microLinnRowOffset[side] * (row2 - row1);        // if not OFF
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
  return (Split[side].transposeOctave / 12.0) * edo
        + Split[side].transposePitch  * MICROLINN_MAJ_2ND[edo]             // semitones are interpreted as whole tones
        - Split[side].transposeLights * MICROLINN_MAJ_2ND[edo]
        + Split[side].microLinn.transposeEDOsteps
        - Split[side].microLinn.transposeEDOlights;
}

void calcMicroLinnTuning() { 
  // calcs microLinnSemitonesPerPad, microLinnEdostep, microLinnMidiNote and microLinnTuningBend
  // called when any column 0 button is released and the user returns to normal playing mode
  // also called when entering microLinnConfig note lights display or leaving anchor cell display
  // also called for octave up/down or EDO+/EDO- via switch1/switch2 control buttons or footswitch presses
  // needed for any change to row offsets, col offsets, transpose, bendrange or anything on microLinnConfig
  // also needed for changes to the split point if in NO_OVERLAP mode

  if (!isMicroLinnOn()) return;
  midiSendControlChange(41, 41, 1, true);    // for debugging, delete later

  byte anchorCol = Global.microLinn.anchorCol;
  byte anchorRow = Global.microLinn.anchorRow;
  // anchorPitch = a standard 12edo midi note, but with 2 decimal places for anchorCents
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;
  // edostepSize = size of 1 edostep in 12edo semitones, e.g. 1\41 is 0.2927 semitones
  float edostepSize = (1200 + Global.microLinn.octaveStretch) / (100.0 * edo);
  short distance, virtualEdostep, roundNote, fineTune; float note;

  microLinnCalcLowestEdostepEtc();
  memset (&microLinnMidiNote, -1, sizeof(microLinnMidiNote));                       // -1 = this edostep is not used
  memset (&microLinnTuningBend, 0, sizeof(microLinnTuningBend));    

  for (byte side = 0; side < NUMSPLITS; ++side) {
    short colOffset = Split[side].microLinn.colOffset;
    microLinnSemitonesPerPad[side] = colOffset * edostepSize;                       // e.g. kite tuning is 2\41 = 0.5854 semitones
    if (isLeftHandedSplit(side)) colOffset *= -1;
    float semitonesPerPad = colOffset * edostepSize;                                // microLinnSemitonesPerPad is used later, semitonesPerPad is used here
    short transpose = microLinnTransposition(side);
    byte bendRange = getBendRange(side);
    for (byte row = 0; row < NUMROWS; ++row) {
      distance = microLinnSumOfRowOffsets(side, anchorRow, row);                    // distance in edosteps between any 2 rows
      distance += transpose - anchorCol * colOffset;                                // distance from the anchor cell, initialized to col 0
      note = anchorPitch + distance * edostepSize;                                  // convert distance from edosteps to a fractional 12edo midi note
      for (byte col = 1; col < NUMCOLS; ++col) {
        distance += colOffset;                                                      // increment rather than recalc each time, for speed
        microLinnEdostep[side][col][row] = distance;                                // the anchor cell is always edostep 0, unless transposed
        virtualEdostep = distance - microLinnLowestEdostep[side];                   // virtual edosteps are always >= 0
        note += semitonesPerPad;                                                    // increment rather than recalc
        if (note >= 0 && note <= 127) {                                             // if not, midiNote will be -1 due to the earlier memsets
          microLinnMidiNote[side][virtualEdostep] = roundNote = round(note);
          fineTune = round((note - roundNote) * 8192.0 / bendRange);                // fine-tuning bend as a zero-centered signed 13-bit integer
          microLinnTuningBend[side][virtualEdostep] = fineTune;
        }
      }
    }
  }
}

/************** midi send/receive functions ************************/

// used for guitar tuning, anchor note, anchor cents and note lights
void microLinnSendPreviewNote (short edostep) {                                                // edosteps from the anchor cell
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;      // midi note with 2 decimal places for cents
  float edostepSize = (1200 + Global.microLinn.octaveStretch) / (100.0 * edo);                 // edo's step size in 12edo semitones
  float note = anchorPitch + edostep * edostepSize;
  if (note <= -0.5 || note >= 127.5) return;
  ensureGuitarTuningPreviewNoteRelease();                                // re-use the standard guitar tuning functions & vars
  guitarTuningPreviewNote = round(note);
  note -= guitarTuningPreviewNote;
  note *= 8192.0 / getBendRange(Global.currentPerSplit);                 // fine-tuning bend as a zero-centered signed 13-bit integer
  short bend = round(note);
  guitarTuningPreviewChannel = takeChannel(Global.currentPerSplit, sensorRow);
  midiSendPitchBend(bend, guitarTuningPreviewChannel);
  midiSendNoteOn(Global.currentPerSplit, guitarTuningPreviewNote, 96, guitarTuningPreviewChannel);
}

// indicates pad's row/column, sent after every played (not arpeggiated, strummed or sequenced) note-on and note-off
// called from sendNewNote() and sendReleasedNote() in ls_handleTouches.ino, thanks to KVR forum member vorp40 for the idea!
void sendMicroLinnLocatorCC(byte channel) {
  // channel differs from sensorCell->channel when in tuning table mode and rechanneling
  if (Split[sensorSplit].microLinn.tuningTable == 1) {
    if (microLinnLocatorCC1 >= 0) {
      midiSendControlChange(microLinnLocatorCC1, sensorCell->microLinnGroup, channel, true); 
    }
    return;
  }
  if (sensorCol <= 16 && microLinnLocatorCC1 >= 0)
    midiSendControlChange(microLinnLocatorCC1, (7 - sensorRow) + 8 * (sensorCol - 1), channel, true);
  if (sensorCol >= 17 && microLinnLocatorCC2 >= 0)
    midiSendControlChange(microLinnLocatorCC2, (7 - sensorRow) + 8 * (sensorCol - 17), channel, true);
}

// nrpn 253 sets the custom row offset, value is either 0...2*max (nrpn format) or -max...max (actual), max = 25

int computeMicroLinnNrpn253() {
  return Global.customRowOffset + (MAXCOLS - 1);
}

void receiveMicroLinnNrpn253(int value) {
  if (inRange(value, 0, 2 * 25)) {
    Global.customRowOffset = value - (MAXCOLS - 1);
  }
}

/************** footswitch functions called in ls_switches.ino ************************/

void changeMicroLinnEDO(int delta) {
  if (!isMicroLinnOn()) return;
  Global.microLinn.EDO += delta;
  if (Global.microLinn.EDO < 5) Global.microLinn.EDO = MICROLINN_MAX_EDO;        // wrap around
  if (Global.microLinn.EDO > MICROLINN_MAX_EDO) Global.microLinn.EDO = 5;
  microLinnAdjustColAndRowOffsets();
  calcMicroLinnTuning();
  updateDisplay();
}

void performMicroLinnOctaveToggle(byte split) {
  microLinnOctaveToggled = !microLinnOctaveToggled;
  switchTransposeOctave(split, microLinnOctaveToggled ? 12 : -12);
}

/************** misc painting functions  ************************/

void paintMicroLinnNormalDisplayCell(byte split, byte col, byte row) {
  // by default clear the cell color
  byte colour = COLOR_OFF;
  CellDisplay cellDisplay = cellOff;
  byte currScale = Global.activeNotes;
  short edostep = microLinnEdostep[split][col][row];
  short virtualEdostep = edostep - microLinnLowestEdostep[split];
  signed char midiNote = microLinnMidiNote[split][virtualEdostep];
  edostep = microLinnMod(edostep, edo);
  edostep -= Split[split].transposePitch * MICROLINN_MAJ_2ND[edo]      // transposing changes the sound but not the note lights
           + Split[split].microLinn.transposeEDOsteps;

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
    // ...or dots...
    else if (currScale == 8) {
      byte fret = microLinnGetFret(split, col);
      if (microLinnDots(edo, fret) & (1 << row)) {
        colour = Split[split].colorMain;
        cellDisplay = cellOn;
      }
    } 
    // ...or rainbows
    else if (currScale == 7 || microLinnScaleStep(edo, edostep) & (1 << currScale)) {
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
    // bug, fails when the tuning is non-octave
    // byte halfEquave = equaveSemitones >> 1;
    // change 55 -> 60 - halfEquave, < 67 -> <= 60 + halfEquave - (equaveSemitones % 2)
    if (blinkMiddleRootNote && edostep == 0 && midiNote >= 55 && midiNote < 67) {       // G to F#
      colour = Split[split].colorAccent;
      cellDisplay = cellFastPulse;
    }
  }

  // if the low row is anything but normal, set it to the appropriate color
  if (row == 0 && Split[split].lowRowMode != lowRowNormal) {
    if ((Split[split].lowRowMode == lowRowCCX && Split[sensorSplit].lowRowCCXBehavior == lowRowCCFader) ||
        (Split[split].lowRowMode == lowRowCCXYZ && Split[sensorSplit].lowRowCCXYZBehavior == lowRowCCFader)) {
      colour = COLOR_BLACK;
      cellDisplay = cellOff;
    }
    else {
      colour = Split[split].colorLowRow;
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
    // custom LEDs shows up underneath the note lights, delete?
    byte pattern = getMicroLinnShowCustomLEDs(split);
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
  byte edostep = microLinnMod(microLinnEdostep[split][col][row], edo);     // octave-reduced
  return microLinnRainbowColor(edo, edostep);
}

void applySameAndMicroLinnBlinkToOtherSplit(boolean calledOnNoteOn, short note) {
  byte otherSide = otherSplit(sensorSplit);
  if (Global.splitActive &&
      (Split[otherSide].playedTouchMode == playedSame ||
       Split[otherSide].playedTouchMode == playedBlink) &&
      !Split[otherSide].ccFaders && !Split[otherSide].strum && !Split[otherSide].sequencer) {
    if (calledOnNoteOn) {
      highlightPossibleNoteCells(otherSide, note);
    }
    else resetPossibleNoteCells(otherSide, note);         // called on noteOff
  }
}

void paintMicroLinnPlusMinus() {                         // for the "8VE±" foot pedal when swiping TAP TEMPO
  if (LINNMODEL == 200) {
    bigfont_draw_string(18,  2, "+", globalColor, true);
    bigfont_draw_string(18, -2, "-", globalColor, false);
  } else {
    smallfont_draw_string(12,  1, "+", globalColor, true);
    smallfont_draw_string(12, -2, "-", globalColor, false);
  }
}

byte getMicroLinnRowOffsetColor() {
  if (Global.customRowOffset != edo) {
    return globalAltColor;
  }
  return globalColor;
}

void paintMicroLinnDebugDump() {     // delete later from here and from ls_displayModes
  if (sensorRow < 4 && sensorCol <= 16) return;
  byte sp = Global.currentPerSplit;
  if (sensorCol == 1) paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][1][0], 0, false);
  if (sensorCol == 2) paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][25][0], 0, false);
  if (sensorCol == 3) paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][1][7], 0, false);
  if (sensorCol == 4) paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][25][7], 0, false);
  if (sensorCol == 5) paintNumericDataDisplay(Split[sp].colorMain, 100 * microLinnSemitonesPerPad[sp], 0, false);
  if (sensorCol == 6) paintNumericDataDisplay(Split[sp].colorMain, microLinnHighestGuitarString, 0, false);
  if (sensorCol == 7) paintNumericDataDisplay(Split[sp].colorMain, microLinnLowestGuitarString, 0, false);
}

/************** misc handling functions ************************/

void handleMicroLinnScaleHold() {                         // long-press one of the 9 scale buttons in Global Settings
  if (!isMicroLinnOn()) return;
  if (sensorRow <= 2) {
    calcMicroLinnTuning();                                // note lights display needs to know the current tuning
    setDisplayMode(displayMicroLinnConfig);
    updateDisplay();
    if (microLinnConfigColNum > 0 && microLinnConfigColNum != 12) {
      setLed(microLinnConfigColNum, 0, Split[LEFT].colorMain, cellOn);        // turn off old button
    }
    microLinnConfigColNum = 12;
    setLed(12, 0, Split[LEFT].colorAccent, cellOn);                           // turn on new button
    microLinnPaintNoteLights();
  }
}

void handleMicroLinnRowOffsetNewTouch() {
  handleNumericDataNewTouchCol(Global.customRowOffset, -25, 25, true);
}

/************** microLinn config functions ************************/

void enterMicroLinnConfig() {
  microLinnConfigNowScrolling = false;
  resetNumericDataChange();
  setDisplayMode(displayMicroLinnConfig);
  updateDisplay();
}

void microLinnPaintConfigButtons() {
  for (byte col = 2; col <= 16; ++col) { 
    if (col == 4 || col == 7 || col == 11 || col == 13 || col == 15) col += 1;               // skip over empty columns
    setLed(col, 0, col == microLinnConfigColNum ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);
  }
}

void microLinnScrollSmall (String text) {
  char tripleText[200];                                      // 200 to fit in the uninstall message, the longest message
  // leading spaces ensure the string first appears on the right edge, not on the left edge
  snprintf(tripleText, sizeof(tripleText), "      %s     %s     %s", text.c_str(), text.c_str(), text.c_str());
  small_scroll_text(tripleText, Split[LEFT].colorMain);
}

void microLinnPaintShowSplitSelection(byte side, boolean doublingFlag) {
  if (side == LEFT || doublingFlag) {
    setLed(15, 7, Split[LEFT].colorMain, cellOn);
  }
  if (side == RIGHT || doublingFlag) {
    setLed(16, 7, Split[RIGHT].colorMain, cellOn);
  }
}

void paintMicroLinnConfig() {
  clearDisplay();
  microLinnPaintConfigButtons();

  if (microLinnConfigNowScrolling) {
    switch (microLinnConfigColNum) {
      case 2: 
        microLinnScrollSmall("LEFT/RIGHT COLUMN OFFSET");
        break;
      case 3: 
        microLinnScrollSmall("LEFT/RIGHT ROW OFFSET");
        break;
      case 5: 
        microLinnScrollSmall("EDO (NOTES PER OCTAVE)");
        break;
      case 6: 
        if (isMicroLinnOn()) {
          microLinnScrollSmall("OCTAVE STRETCH IN CENTS");
        } else {
          microLinnScrollSmall("FIRST SELECT AN EDO");
        }
        break;
      case 8: 
        if (isMicroLinnOn()) {
          microLinnScrollSmall("ANCHOR PAD");
        } else {
          microLinnScrollSmall("FIRST SELECT AN EDO");
        }
        break;
      case 9: 
        if (isMicroLinnOn()) {
          microLinnScrollSmall("ANCHOR NOTE");
        } else {
          microLinnScrollSmall("FIRST SELECT AN EDO");
        }
        break;
      case 10: 
        if (isMicroLinnOn()) {
          microLinnScrollSmall("ANCHOR CENTS");
        } else {
          microLinnScrollSmall("FIRST SELECT AN EDO");
        }
        break;
      case 12: 
        if (isMicroLinnOn()) {
          microLinnScrollSmall("NOTE LIGHTS");
        } else {
          microLinnScrollSmall("FIRST SELECT AN EDO");
        }
        break;
      case 14: 
        microLinnScrollSmall("HAMMER-ON OPTIONS");
        break;
      case 16: 
        microLinnScrollSmall("MISCELLANEOUS OPTIONS");
        break;
    }
    microLinnPaintConfigButtons();
  }

  signed char offset = 0;
  byte side = Global.currentPerSplit;
  switch (microLinnConfigColNum) {
    case 2:
      microLinnPaintShowSplitSelection (side, microLinnDoublePerSplitColOffset);
      if (Split[side].microLinn.colOffset == 1) {
        smallfont_draw_string(2, 1, "OFF", Split[side].colorMain, false);
      } else
        paintNumericDataDisplay(Split[side].colorMain, Split[side].microLinn.colOffset, 0, false);
      break;
    case 3: 
      microLinnPaintShowSplitSelection (side, false);
      if (microLinnRowOffset[side] == -26) {
        smallfont_draw_string(2, 1, "OFF", Split[side].colorMain, false);
      }
      else if (microLinnRowOffset[side] == 26) {
        if (LINNMODEL == 200) {
          smallfont_draw_string(0, 1, "NOVR", Split[side].colorMain, false);
        } else {
          condfont_draw_string(0, 1, "NOVR", Split[side].colorMain, false);
        }
      }
      else
        paintNumericDataDisplay(Split[side].colorMain, microLinnRowOffset[side], -3, false);
      break;
    case 5: 
      if (Global.microLinn.EDO < 5) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, Global.microLinn.EDO, 0, false);
      }
      break;
    case 6: 
      if (isMicroLinnOn()) {
        if (Global.microLinn.octaveStretch <= -100) {
          offset = -5;
        } else if (Global.microLinn.octaveStretch <= -10) {
          offset = -3;
        }
        paintNumericDataDisplay(globalColor, Global.microLinn.octaveStretch, offset, false);
      }
      break;
    case 8:  
      if (isMicroLinnOn()) {
        if (LINNMODEL == 200) {
          smallfont_draw_string(1, 1, microLinnAnchorString, globalColor);
        } else {
          condfont_draw_string(1, 1, microLinnAnchorString, globalColor);
        }
      }
      break;
    case 9: 
      if (isMicroLinnOn()) paintNoteDataDisplay(globalColor, Global.microLinn.anchorNote, LINNMODEL == 200 ? 2 : 1);
      break;
    case 10: 
      if (isMicroLinnOn()) {
        if (Global.microLinn.anchorCents <= -100) {
          offset = -5;
        } else if (Global.microLinn.anchorCents <= -10) {
          offset = -3;
        }
        paintNumericDataDisplay(globalColor, Global.microLinn.anchorCents, offset, false);
      }
      break;
    case 12:
      if (isMicroLinnOn()) microLinnPaintNoteLights();
      break; 
    case 14:
      microLinnPaintHammerOns();
      break;
    case 16:
      microLinnPaintMiscOptions();
      break;
  }
}

boolean microLinnIsConfigButton() {
  return sensorRow == 0 && sensorCol >= 2 && sensorCol <= 16
      && sensorCol != 4 && sensorCol != 7 && sensorCol != 11 && sensorCol != 13 && sensorCol != 15;
}

boolean microLinnIsDisabledConfigButton() {                // a button that doesn't work because we're not in an edo
  return !isMicroLinnOn() && sensorRow == 0 && sensorCol >= 6 && sensorCol <= 12 && sensorCol != 7 && sensorCol != 11;
}

void handleMicroLinnConfigNewTouch() {
  // start tracking the touch duration to enable hold functionality
  sensorCell->lastTouch = millis();

  microLinnConfigNowScrolling = false;
           
  if (microLinnIsConfigButton()) {                                             // mostly handled on hold or release
    if (microLinnConfigColNum > 0 && microLinnConfigColNum != sensorCol) {
      setLed(microLinnConfigColNum, 0, Split[LEFT].colorMain, cellOn);         // turn off old button
    }
    microLinnConfigColNum = sensorCol;
    setLed(microLinnConfigColNum, 0, Split[LEFT].colorAccent, cellOn);         // turn on new button
    updateDisplay();
    return;
  }
  
  if (sensorRow > 0) {                                                         // rows 1-7 are handled right away
    byte oldEDO = Global.microLinn.EDO;
    byte side = Global.currentPerSplit;
    switch (microLinnConfigColNum) {
      case 2:
        microLinnHandleColOffsetNewTouch(side);
        break;
      case 3:
        microLinnHandleShowSplit();
        handleNumericDataNewTouchCol(microLinnRowOffset[side], -26, 26, true);                  // OFF, -25..25, NOVR
        microLinnStoreRowOffsetCents(side);
        break;
      case 5:
        handleNumericDataNewTouchCol(Global.microLinn.EDO, 4, MICROLINN_MAX_EDO, true);                 // 4 means OFF
        if (Global.microLinn.EDO != oldEDO) {
          if (oldEDO == 4) lightSettings = LIGHTS_ACTIVE;
          microLinnAdjustColAndRowOffsets();
        }
        break;
      case 6:
        if (isMicroLinnOn()) handleNumericDataNewTouchCol(Global.microLinn.octaveStretch, -120, 120, true);
        break;
      case 9:
        if (isMicroLinnOn()) {
          handleNumericDataNewTouchCol(Global.microLinn.anchorNote, 0, 127, true);
          microLinnSendPreviewNote(0);
        }
        break;
      case 10:
        if (isMicroLinnOn()) {
          handleNumericDataNewTouchCol(Global.microLinn.anchorCents, -60, 60, true); 
          microLinnSendPreviewNote(0);
        }
        break;
      case 12:
        if (isMicroLinnOn()) microLinnHandleNoteLightsNewTouch();
        break;
      case 14:
        microLinnHandleHammerOnsNewTouch();
        break;
      case 16:
        microLinnHandleMiscOptionsNewTouch();
        break;
    }
  }
}

void handleMicroLinnConfigHold() {
  if (microLinnIsConfigButton() && !microLinnConfigNowScrolling &&            // long-press bottom row (or short-press when no edo is set)
     (isCellPastSensorHoldWait() || microLinnIsDisabledConfigButton())) {     
    microLinnConfigNowScrolling = true;
    paintMicroLinnConfig();                                                   // scroll the name of the button
  } else if (sensorRow > 0 && microLinnConfigColNum == 12) {
    microLinnHandleNoteLightsHold();
  } else if (sensorRow > 0 && microLinnConfigColNum == 16) {
    microLinnHandleMiscOptionsHold();
  }
}

void handleMicroLinnConfigRelease() {
  if (microLinnIsConfigButton() && !isCellPastSensorHoldWait()) {         // short-press bottom row
    if (microLinnConfigColNum == 12 && isMicroLinnOn()) {
      calcMicroLinnTuning();                                          // note lights display needs to know the current tuning
      updateDisplay();
    }
  } else if (microLinnConfigColNum == 12) {
    microLinnHandleNoteLightsRelease();
  } else if (microLinnConfigColNum == 16) {
    microLinnHandleMiscOptionsRelease();
  } else if (sensorRow > 0 && cellsTouched == 0) {
    handleNumericDataReleaseCol(false);
    resetNumericDataChange();
    if (microLinnConfigColNum == 8) {
      calcMicroLinnTuning();                                          // anchor cell chooser needs to know the current tuning
      setDisplayMode(displayMicroLinnAnchorChooser);
      updateDisplay();
    }
    else if (microLinnConfigColNum == 9 || microLinnConfigColNum == 10) {
      ensureGuitarTuningPreviewNoteRelease();
    }
  }
}

boolean microLinnHandleShowSplit() {                                    // adds the ability to affect both splits at once
  if (sensorRow == 7 && (sensorCol == 15 || sensorCol == 16)) {         // modelled after handleOctaveTransposeNewTouch()
    if (displayMode == displayMicroLinnConfig && microLinnConfigColNum == 2)
      microLinnDoublePerSplitColOffset = cell(15, 7).touched == touchedCell && cell(16, 7).touched == touchedCell;
    handleShowSplit();
    updateDisplay();
    return true;
  }
  return false;
}

void microLinnHandleColOffsetNewTouch(byte side) {
  if (microLinnHandleShowSplit()) return;
  handleNumericDataNewTouchCol(Split[side].microLinn.colOffset, 1, MAXROWS, true);
  microLinnStoreColOffsetCents(side);
  microLinnSemitonesPerPad[side] = Split[side].microLinn.colOffset;     // assumes 12edo, if not calcTuning will overwrite this
  if (microLinnDoublePerSplitColOffset) {
    byte otherSide = otherSplit(side);
    Split[otherSide].microLinn.colOffset = Split[side].microLinn.colOffset;
    microLinnStoreColOffsetCents(otherSide);
    microLinnSemitonesPerPad[otherSide] = Split[side].microLinn.colOffset;
  }
}

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

void microLinnPaintNoteLights() {
  clearDisplay();
  microLinnPaintConfigButtons();

  byte col = 12;       
  byte stepspan = 0;                                                            // the degree (2nd, 3rd, etc.) minus 1
  byte currScale = Global.activeNotes;
  byte color;

  if (currScale <= 7) {                                                         // paint the scale 
    for (byte edostep = 0; edostep < edo; ++edostep) {
      while (edostep > MICROLINN_SCALEROWS[edo][stepspan]) {                    // while, not if, to skip over the empty rows of 5edo & 6edo
        stepspan += 1;
        col -= MICROLINN_SCALEROWS[edo][stepspan] - MICROLINN_SCALEROWS[edo][stepspan - 1];
      }
      if (currScale == 7 || microLinnScaleStep(edo, edostep) & (1 << currScale)) {
        if (Global.microLinn.useRainbow) {
          color = microLinnRainbowColor(edo, edostep);
        } else {
          color = (edostep == 0 ? Split[LEFT].colorAccent : Split[LEFT].colorMain);
        }
        setLed(col, 7 - (stepspan % 7), color, cellOn);
      }
      col += 1; 
    }
    col = MICROLINN_SCALEROWS[edo][0];                                                // pink borders
    for (stepspan = 0; stepspan < 7; ++stepspan) {
      setLed(col + 14, 7 - stepspan, COLOR_PINK, cellOn);
    }
    col = MICROLINN_SCALEROWS[edo][1] - MICROLINN_SCALEROWS[edo][0];                  // 2nd - 1sn = major 2nd
    col = max (col, MICROLINN_SCALEROWS[edo][3] - MICROLINN_SCALEROWS[edo][2]);       // 4th - 3rd = minor 2nd
    col -= MICROLINN_SCALEROWS[edo][0];
    if (edo == 8 || edo == 9 || edo == 23) col += 1;
    for (stepspan = 0; stepspan < 7; ++stepspan) {
      setLed(11 - col, 7 - stepspan, COLOR_PINK, cellOn);
    }
  } else if (currScale == 8) {
    paintMicroLinnDotsEditor(false);                                                  // blue dots 
  } 

  for (byte scaleNum = 0; scaleNum < 7; ++scaleNum) {                                        // the 7 scale buttons
    setLed(1, 7 - scaleNum, scaleNum == currScale ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);
  }
  setLed(3, 7, currScale == 7 ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);    // color editor
  setLed(3, 5, currScale == 8 ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);    // dots selector
  setLed(3, 3, Global.microLinn.useRainbow ? COLOR_LIME : Split[LEFT].colorMain, cellOn);    // rainbow enabler
}

void microLinnHandleNoteLightsNewTouch() {
  if ((sensorCol == 1 && sensorRow > 0) ||                                    // scale selectors or
      (sensorCol == 3 && (sensorRow == 7 || sensorRow == 5))) {               // rainbow editor or dots selector
    byte currScale = 7 - sensorRow;                                           // currScale = what user just touched
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
    setLed(sensorCol, sensorRow, Split[LEFT].colorAccent, cellSlowPulse);
    return;
  }

  if (sensorCol == 3 && sensorRow == 3) {                                     // rainbow enabler
    Global.microLinn.useRainbow = !Global.microLinn.useRainbow;
    if (Global.activeNotes > 7) Global.activeNotes = 7;                       // make the user see the change
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
      short ptr = microLinnTriIndex(edo, edostep);
      switch (Device.microLinn.rainbows[ptr]) {                   // cycle through the colors in rainbow order
        case 8:  Device.microLinn.rainbows[ptr] = 1;  break;      // white to red
        case 1:  Device.microLinn.rainbows[ptr] = 9;  break;      // red to orange
        case 9:  Device.microLinn.rainbows[ptr] = 2;  break;      // orange
        case 2:  Device.microLinn.rainbows[ptr] = 10; break;      // yellow (not used)
        case 10: Device.microLinn.rainbows[ptr] = 3;  break;      // lime (used as yellow)
        case 3:  Device.microLinn.rainbows[ptr] = 4;  break;      // green
        case 4:  Device.microLinn.rainbows[ptr] = 5;  break;      // cyan
        case 5:  Device.microLinn.rainbows[ptr] = 6;  break;      // blue
        case 6:  Device.microLinn.rainbows[ptr] = 11; break;      // magenta
        case 11: Device.microLinn.rainbows[ptr] = 8;  break;      // pink
      }
    } else if (Global.activeNotes < 7) {
      Device.microLinn.scales[microLinnTriIndex(edo, edostep)] ^= (1 << Global.activeNotes);      // xor to toggle the bit
      if (Device.microLinn.scales[microLinnTriIndex(edo, edostep)] & (1 << Global.activeNotes)) {
        if (edostep > MICROLINN_SCALEROWS[edo][6]) {
          edostep -= edo;                                        // adjust notes in the first row below the tonic
        }
        microLinnSendPreviewNote(edostep);
      }
    }
    updateDisplay(); 
  }
}

void microLinnHandleNoteLightsHold() {
  if (!isCellPastConfirmHoldWait()) return;                                       // long-press 800 ms

  byte start = microLinnTriIndex(edo, 0);
  byte currScale = Global.activeNotes;

  if (currScale == 7 - sensorRow && sensorCol == 1 && sensorRow > 0) {            // scale selector button
    for (byte edostep = 0; edostep < edo; ++edostep) {                            // reset the scale
      bitWrite (Device.microLinn.scales[start + edostep], currScale, 
               bitRead(MICROLINN_SCALES[start + edostep], currScale));
    }
    updateDisplay();
    return;
  }

  if (currScale == 7 && sensorCol == 3 && sensorRow == 7) {                                     // color editor button
    memcpy (&Device.microLinn.rainbows[start], &MICROLINN_RAINBOWS[start], edo);                // reset the rainbow
    updateDisplay();
    return;
  }

  if (currScale == 8 && sensorCol == 3 && sensorRow == 5) {                                     // dots selector button
    microLinnResetDots (edo);
    updateDisplay();
  }
}

void microLinnHandleNoteLightsRelease() {
  if (!isCellPastConfirmHoldWait()) {                                           // short-press

    byte currScale = Global.activeNotes;
    if ((sensorCol == 1 && sensorRow >= 1 && currScale == 7 - sensorRow) ||     // scale selector
        (sensorCol == 3 && sensorRow == 7 && currScale == 7) ||                 // rainbow editor
        (sensorCol == 3 && sensorRow == 5 && currScale == 8)) {                 // dots selector
      if (microLinnCanBacktrack) {
        Global.activeNotes = microLinnPrevScale;                                // backtrack
        microLinnPrevScale = currScale;
        updateDisplay(); 
      } else {
        setLed(sensorCol, sensorRow, Split[LEFT].colorAccent, cellOn);          // stop pulsing
      }
    }

    if (sensorCol > 3 && sensorRow > 0 && Global.activeNotes == 8) {            // enter dots editor
      setDisplayMode(displayMicroLinnDotsEditor);
      updateDisplay();
      return;
    }
  }
  if (cellsTouched == 0) {
    ensureGuitarTuningPreviewNoteRelease();
  }
}

void paintMicroLinnDotsEditor(boolean fullScreen) {             // fullscreen is true only when actually editing
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

void handleMicroLinnDotsEditorNewTouch() {
  byte fret = microLinnGetFret(Global.currentPerSplit, sensorCol);
  Device.microLinn.dots[microLinnTriIndex(edo, fret)] ^= (1 << sensorRow);          // xor to toggle the bit
  updateDisplay();
}

void microLinnPaintHammerOns() {
}

void microLinnHandleHammerOnsNewTouch() {
}

void microLinnPaintMiscOptionsButtons() {
  for (byte row = 1; row < MAXROWS; ++row) {
    setLed(1, row, microLinnMiscOptionsRowNum == row ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);
  }
}

void microLinnPaintMiscOptions() {
  microLinnPaintMiscOptionsButtons();
  if (microLinnMiscOptionsNowScrolling) {
    switch (microLinnMiscOptionsRowNum) {
      case 7: microLinnScrollSmall("DRUM PAD MODE"); break;
      case 6: microLinnScrollSmall("LOCATOR CC FOR COLUMNS 1-16"); break;
      case 5: microLinnScrollSmall("LOCATOR CC FOR COLUMNS 17-25"); break;
      case 4: microLinnScrollSmall("TUNING TABLE MODE FOR LEFT SPLIT"); break;
      case 3: microLinnScrollSmall("TUNING TABLE MODE FOR RIGHT SPLIT"); break;
      case 2: microLinnScrollSmall("SHOW CUSTOM LIGHT PATTERN IN LEFT SPLIT"); break;
      case 1: microLinnScrollSmall("SHOW CUSTOM LIGHT PATTERN IN RIGHT SPLIT"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintMiscOptionsButtons();
    return;
  }

  switch (microLinnMiscOptionsRowNum) {
    case 7:
      smallfont_draw_string(3, 1, microLinnDrumPadMode ? "ON" : "OFF", globalColor, false);
      break;
    case 6:
      if (microLinnLocatorCC1 == -1) {
        smallfont_draw_string(3, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, microLinnLocatorCC1, 3, false);
      }
      break;
    case 5:
      if (microLinnLocatorCC2 == -1) {
        smallfont_draw_string(3, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, microLinnLocatorCC2, 3, false);
      }
      break;
    case 4:
      microLinnPaintTuningTableOption(LEFT);
      break;
    case 3:
      microLinnPaintTuningTableOption(RIGHT);
      break;
    case 2:
      if (microLinnShowCustomLEDs[LEFT] == 0) {    
        smallfont_draw_string(3, 1, "OFF", Split[LEFT].colorMain, false);
      } else {
        paintNumericDataDisplay(Split[LEFT].colorMain, microLinnShowCustomLEDs[LEFT], 3, false);
      }
      break;
    case 1:
      if (microLinnShowCustomLEDs[RIGHT] == 0) {    
        smallfont_draw_string(3, 1, "OFF", Split[RIGHT].colorMain, false);
      } else {
        paintNumericDataDisplay(Split[RIGHT].colorMain, microLinnShowCustomLEDs[RIGHT], 3, false);
      }
      break;
  }
}

void microLinnPaintTuningTableOption(byte side) {
  switch (Split[side].microLinn.tuningTable) {
    case 0: smallfont_draw_string(3, 1, "OFF", Split[side].colorMain, false); break;
    case 1: smallfont_draw_string(3, 1, "ON",  Split[side].colorMain, false); break;
    case 2:
      byte col = 16;
      if (LINNMODEL == 200) {
        smallfont_draw_string(3, 1, "RCH", Split[side].colorMain, false);
        col = 19;
      } else {
        condfont_draw_string(3, 1, "RCH", Split[side].colorMain, false);
      }
      for (byte group = 0; group < microLinnNumGroups[side]; ++group) {
        setLed(col, group + 2, Split[side].colorMain, cellOn);
      }
      break;
  }
}

void microLinnHandleMiscOptionsNewTouch() {
  microLinnMiscOptionsNowScrolling = false;
  if (sensorCol == 1) {
    if (sensorRow > 0) 
      microLinnMiscOptionsRowNum = sensorRow;
      updateDisplay(); 
    return;
  }
  switch (microLinnMiscOptionsRowNum) {
    case 7:
      handleNumericDataNewTouchCol(microLinnDrumPadMode);
      if (microLinnDrumPadMode) {
        for (byte ch = 0; ch < 16; ++ch) {    // clear all previous pitch bends so all drum hits sound the same
          midiSendPitchBend(0, ch);
        }
      }  
      break;
    case 6:
      handleNumericDataNewTouchCol(microLinnLocatorCC1, -1, 119, true);           // -1 means OFF
      break;
    case 5:
      handleNumericDataNewTouchCol(microLinnLocatorCC2, -1, 119, true);
      break;
    case 4:
      handleNumericDataNewTouchCol(Split[LEFT].microLinn.tuningTable, 0, 2, true);
      if (Split[LEFT].microLinn.tuningTable == 2) microLinnSetUpRechannelling(LEFT);
      break;
    case 3:
      handleNumericDataNewTouchCol(Split[RIGHT].microLinn.tuningTable, 0, 2, true);
      if (Split[RIGHT].microLinn.tuningTable == 2) microLinnSetUpRechannelling(RIGHT);
      break;
    case 2:
      handleNumericDataNewTouchCol(microLinnShowCustomLEDs[LEFT], 0, 6, true);
      break;
    case 1:
      handleNumericDataNewTouchCol(microLinnShowCustomLEDs[RIGHT], 0, 6, true);
      break;
  }
  updateDisplay(); 
}

void microLinnHandleMiscOptionsHold() {
  if (sensorCol == 1 && sensorRow > 0 &&                                       // long-press col 1
      isCellPastSensorHoldWait() && !microLinnMiscOptionsNowScrolling) {     
    microLinnMiscOptionsNowScrolling = true;
    microLinnPaintMiscOptions();                                               // scroll the name of the button
  }
}

void microLinnHandleMiscOptionsRelease() {                                     // short-press col 1
  if (sensorCol == 1 && sensorRow > 0) {
    updateDisplay(); 
  }
}

void microLinnSetUpRechannelling(byte side) {
  microLinnCalcLowestEdostepEtc(side);
  if (microLinnNumGroups[side] == 1) return;
  Split[side].midiMode = 1;
  Split[side].midiChanMain = 1;
  Split[side].midiChanMainEnabled = microLinnNumGroups[side] < 4;
  memset(&Split[side].midiChanSet, 0, 16);
  if (side == LEFT) {
    if (microLinnNumGroups[side] == 2) memset(&Split[side].midiChanSet[1], true, 7);    // 2-8
    if (microLinnNumGroups[side] == 3) memset(&Split[side].midiChanSet[1], true, 5);    // 2-6
    if (microLinnNumGroups[side] == 4) memset(&Split[side].midiChanSet[0], true, 4);    // 1-4
  } else {
    if (microLinnNumGroups[side] == 2) memset(&Split[side].midiChanSet[8], true, 7);    // 9-15
    if (microLinnNumGroups[side] == 3) memset(&Split[side].midiChanSet[10], true, 5);   // 11-15
    if (microLinnNumGroups[side] == 4) memset(&Split[side].midiChanSet[12], true, 4);   // 13-16
  }
}

/************** guitar tuning functions ************************/

void paintMicroLinnGuitarTuning() {
  byte anchor = Global.microLinn.anchorRow;
  byte color;
  boolean accented;
  clearDisplay();
  for (byte r = 0; r < NUMROWS; ++r) {
    accented  = (r == microLinnGuitarTuningRowNum);
    accented |= (r == microLinnGuitarTuningRowNum - 1 && r >= anchor);
    accented |= (r == microLinnGuitarTuningRowNum + 1 && r <= anchor);
    if (r == anchor) {
      color = (accented ? COLOR_ORANGE : COLOR_RED);
    } else { 
      color = (accented ? COLOR_CYAN : COLOR_GREEN);
    }
    setLed(1, r, color, cellOn);
  }
  byte r = microLinnGuitarTuningRowNum;
  if (r == anchor || r >= NUMROWS) return;     // r == NUMROWS+1 means it hasn't been set yet
  if (r < anchor) r += 1;
  paintNumericDataDisplay(globalColor, Global.microLinn.guitarTuning[r], 0, false);
}

void handleMicroLinnGuitarTuningNewTouch() {
  if (sensorCol == 1) {
    microLinnGuitarTuningRowNum = sensorRow;
    updateDisplay();
  }
  else if (microLinnGuitarTuningRowNum != Global.microLinn.anchorRow) {
    /******************** old way, limits each row offset to ±MAXROWS, delete later
    short stringAbove = microLinnGuitarTuningRowNum + 1;
    short stringBelow = microLinnGuitarTuningRowNum - 1;
    if (stringAbove == MAXROWS) stringAbove = stringBelow;
    if (stringBelow < 0) stringBelow = stringAbove;
    stringAbove = Global.microLinn.guitarTuning[stringAbove];         // convert into edosteps from the anchor string
    stringBelow = Global.microLinn.guitarTuning[stringBelow];
    short ceiling, floor;
    ceiling = min (stringAbove, stringBelow) + MAXROWS;
    floor   = max (stringAbove, stringBelow) - MAXROWS;
    ******************************/
    byte r = microLinnGuitarTuningRowNum;
    if (r < Global.microLinn.anchorRow) r += 1;
    short currTotalRange = microLinnGuitarTuning[microLinnHighestGuitarString]
                         - microLinnGuitarTuning[microLinnLowestGuitarString];
    short wiggleRoom = MICROLINN_MAX_GUITAR_RANGE - currTotalRange;
    short ceiling = Global.microLinn.guitarTuning[r] + wiggleRoom;                 // bug: what about negative offsets?
    short floor   = Global.microLinn.guitarTuning[r] - wiggleRoom;
ceiling = 3 * edo; floor = -ceiling;  // delete later
    handleNumericDataNewTouchCol(Global.microLinn.guitarTuning[r], floor, ceiling, true);
    microLinnStoreGuitarRowOffsetCents(r);
    microLinnCalcGuitarTuning();
  }

  if (microLinnGuitarTuningRowNum == Global.microLinn.anchorRow && sensorCol > 1) return;
  unsigned char lefty = isLeftHandedSplit(Global.currentPerSplit) ? -1 : 1;
  short edostep = microLinnGuitarTuning[microLinnGuitarTuningRowNum]
                - lefty * Split[Global.currentPerSplit].microLinn.colOffset * (Global.microLinn.anchorCol - 1);
  microLinnSendPreviewNote(edostep);
}

/************** octave transpose functions ************************/

void  paintMicroLinnEdostepTranspose(byte side) {
  // paint the 2 new rows on the transpose display
  if (!isMicroLinnOn()) return;
  if (MICROLINN_MAJ_2ND[edo] == 1) return;

  // Paint the edostep transpose values
  if (!doublePerSplit || Split[LEFT].microLinn.transposeEDOsteps == Split[RIGHT].microLinn.transposeEDOsteps) {
    paintTranspose(Split[Global.currentPerSplit].colorMain, SPLIT_ROW, Split[side].microLinn.transposeEDOsteps);
  }
  else if (doublePerSplit) {
    if (abs(Split[LEFT].microLinn.transposeEDOsteps) > abs(Split[RIGHT].microLinn.transposeEDOsteps)) {
      paintTranspose(Split[LEFT].colorMain, SPLIT_ROW, Split[LEFT].microLinn.transposeEDOsteps);
      paintTranspose(Split[RIGHT].colorMain, SPLIT_ROW, Split[RIGHT].microLinn.transposeEDOsteps);
    }
    else {
      paintTranspose(Split[RIGHT].colorMain, SPLIT_ROW, Split[RIGHT].microLinn.transposeEDOsteps);
      paintTranspose(Split[LEFT].colorMain, SPLIT_ROW, Split[LEFT].microLinn.transposeEDOsteps);
    }
  }

  // Paint the edostep-light transpose values
  if (!doublePerSplit || Split[LEFT].microLinn.transposeEDOlights == Split[RIGHT].microLinn.transposeEDOlights) {
    paintTranspose(Split[Global.currentPerSplit].colorMain, GLOBAL_SETTINGS_ROW, Split[side].microLinn.transposeEDOlights);
  }
  else if (doublePerSplit) {
    if (abs(Split[LEFT].microLinn.transposeEDOlights) > abs(Split[RIGHT].microLinn.transposeEDOlights)) {
      paintTranspose(Split[LEFT].colorMain, GLOBAL_SETTINGS_ROW, Split[LEFT].microLinn.transposeEDOlights);
      paintTranspose(Split[RIGHT].colorMain, GLOBAL_SETTINGS_ROW, Split[RIGHT].microLinn.transposeEDOlights);
    }
    else {
      paintTranspose(Split[RIGHT].colorMain, GLOBAL_SETTINGS_ROW, Split[RIGHT].microLinn.transposeEDOlights);
      paintTranspose(Split[LEFT].colorMain, GLOBAL_SETTINGS_ROW, Split[LEFT].microLinn.transposeEDOlights);
    }
  }
}

void paintMicroLinnOctaveTransposeButton() {
  if (!isMicroLinnOn()) return;
  // light the octave/transpose switch if the pitch is transposed by edosteps
  if ((Split[LEFT].microLinn.transposeEDOsteps < 0 && Split[RIGHT].microLinn.transposeEDOsteps < 0) ||
      (Split[LEFT].microLinn.transposeEDOsteps < 0 && Split[RIGHT].microLinn.transposeEDOsteps == 0) ||
      (Split[LEFT].microLinn.transposeEDOsteps == 0 && Split[RIGHT].microLinn.transposeEDOsteps < 0)) {
    setLed(0, OCTAVE_ROW, COLOR_RED, cellOn);
  }
  else if ((Split[LEFT].microLinn.transposeEDOsteps > 0 && Split[RIGHT].microLinn.transposeEDOsteps > 0) ||
           (Split[LEFT].microLinn.transposeEDOsteps > 0 && Split[RIGHT].microLinn.transposeEDOsteps == 0) ||
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

void handleMicroLinnOctaveTransposeNewTouchSplit(byte side) {
  // handle touches to the 2 new rows on the transpose display
  if (!isMicroLinnOn()) return;
  if (MICROLINN_MAJ_2ND[edo] == 1) return;
  if (sensorRow == SPLIT_ROW &&
      sensorCol > 0 && sensorCol < 16) {
    Split[side].microLinn.transposeEDOsteps = sensorCol - 8;
  }
  else if (sensorRow == GLOBAL_SETTINGS_ROW &&
           sensorCol > 0 && sensorCol < 16) {
    Split[side].microLinn.transposeEDOlights = sensorCol - 8;
  }
}

/************** uninstall functions ************************/

void enterMicroLinnUninstall() {
  setDisplayMode(displayMicroLinnUninstall);
  microLinnUninstallNowScrolling = true;
  paintMicroLinnUninstall();
}

void microLinnPaintUninstallButtons() {
  setLed(3, 0, COLOR_GREEN, cellOn);
  setLed(4, 0, microLinnUninstall == 1 ? COLOR_CYAN : COLOR_GREEN, cellOn);
  setLed(5, 0, COLOR_GREEN, cellOn);
  setLed(8, 0, COLOR_RED, cellOn);
  setLed(9, 0, microLinnUninstall == 0 ? COLOR_ORANGE : COLOR_RED, cellOn);
  setLed(10, 0, COLOR_RED, cellOn);
  setLed(13, 0, COLOR_BLUE, cellOn);
  setLed(14, 0, COLOR_BLUE, cellOn);
  setLed(15, 0, COLOR_BLUE, cellOn);
}

void paintMicroLinnUninstall() {
  clearDisplay();
  microLinnPaintUninstallButtons();
  if (microLinnUninstallNowScrolling) {
    microLinnScrollSmall("UNINSTALL MICROLINN? GREEN = YES, RED = NO, BLUE = EXIT");
    microLinnPaintUninstallButtons();
  }
}

void handleMicroLinnUninstallNewTouch() {
  microLinnUninstallNowScrolling = false;
  if (sensorRow > 0) return;
  if (sensorCol >= 3 && sensorCol <= 5) {          // green button
    microLinnUninstall = 1;
    switchSerialMode(true);
    storeSettings();
    updateDisplay(); 
  }
  else if (sensorCol >= 8 && sensorCol <= 10) {    // red
    microLinnUninstall = 0;
    switchSerialMode(true);
    storeSettings();
    updateDisplay(); 
  }
  else if (sensorCol >= 13 && sensorCol <= 15) {   // blue
    setDisplayMode(displayGlobal);
    updateDisplay(); 
  }
}

/**************  OBSOLETE CODE, DELETE LATER  ************************

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
  signed char offset = microLinnRowOffset[side]; 
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

const byte NUM_FORKS = 3;
const byte MAX_LONGPRESS_MESSAGES = 3;     // after the 3rd long-press of OS version since power-up,
byte numTimesForkMenu = 0;                 // stop showing the "LONG-PRESS FOR FORK NAMES" message
byte forkMenuColNum = 0;                   // active column number in fork menu, 0 means nothing active
boolean forkMenuNowScrolling = false;

void paintForkMenu() {
  clearDisplay();
  paintForkMenuButtons();
  // leading spaces ensure the string first appears on the right edge, not on the left edge
  switch (forkMenuColNum) {    
    case 0:
      if (numTimesForkMenu <= MAX_LONGPRESS_MESSAGES) {
        small_scroll_text_row1("       LONG-PRESS FOR FORK NAMES", Split[LEFT].colorMain);
      }
      break;
    case 1: 
      small_scroll_text_row1("      MICROLINN", Split[LEFT].colorMain);
      break;
    case 2: 
      small_scroll_text_row1("      BRIGHTNESS", Split[LEFT].colorMain);
      break;
    case 3:
      small_scroll_text_row1("      YOUR FORK HERE", Split[LEFT].colorMain);
      break;  
  }
  paintForkMenuButtons();
}

void paintForkMenuButtons() {
  for (byte col = 1; col <= NUM_FORKS; ++col) {
    setLed(col, 0, col == forkMenuColNum ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);
  }
}

void enterForkMenu() {
  forkMenuColNum = 0;
  numTimesForkMenu += 1;
  resetNumericDataChange();
  setDisplayMode(displayForkMenu);
  paintForkMenu();                            // use paintForkMenu, updateDisplay() doesn't work here
}

void handleForkMenuNewTouch() {
  // start tracking the touch duration to be able to enable hold functionality
  sensorCell->lastTouch = millis();
  forkMenuNowScrolling = false;
  if (sensorRow == 0 && sensorCol <= NUM_FORKS) { 
    if (forkMenuColNum > 0 && forkMenuColNum != sensorCol) {
      setLed(forkMenuColNum, 0, Split[LEFT].colorMain, cellOn);   // turn off old button
    }
    forkMenuColNum = sensorCol;
    setLed(sensorCol, 0, Split[LEFT].colorAccent, cellOn);        // turn on new button
  }
}

void handleForkMenuHold() {
  if (sensorRow == 0 && sensorCol <= NUM_FORKS
      && isCellPastSensorHoldWait() && !forkMenuNowScrolling) {          // long-press bottom row
    forkMenuNowScrolling = true;
    paintForkMenu();                                                     // scroll the name of the fork
  }
}

void handleForkMenuRelease() {
  if (sensorRow == 0 && sensorCol <= NUM_FORKS && !isCellPastSensorHoldWait()) {    // short-press bottom row
    forkMenuColNum = 0;
    switch (sensorCol) {
      case 1:
        microLinnConfigNowScrolling = false;
        resetNumericDataChange();
        setDisplayMode(displayMicroLinnConfig);
        updateDisplay();
        break;
      case 2:
        resetNumericDataChange();
        setDisplayMode(displayBrightness);  
        updateDisplay();
        break;
      case 3:
        forkMenuColNum = 3;
        break;
    }
  }
}


void paintBrightnessScreen() {
  clearDisplay();
  setLed(2, 6, Split[LEFT].colorAccent, cellOn);                       // zero marker, like on the transpose screen
  for (byte col = 14; col > 2; --col ) {
    setLed(col, 6, col - 2 > brightness ? COLOR_BLACK : Split[LEFT].colorMain, cellOn);       // black erases
  }
}

void handleBrightnessNewTouch() {
  if (sensorRow >= 5 && sensorCol <= 16) {
    brightness = min (max (sensorCol - 2, 1), 12);
    updateDisplay(); 
  }
}


// PolyPressure controls LED colors, faster than CCs 20-22, thanks to KVR forum member dr_loop for the code!
// https://www.kvraudio.com/forum/viewtopic.php?t=519090
void microLinnReceivePolyPressure(byte midiData1, byte midiData2, byte midiChannel) {
  if (displayMode != displayNormal) return;

  byte row = 7 - (midiData1 % 8);
  byte col = midiData1 / 8 + 1;
  if (midiChannel == 1) col += 16;    	// for LS-200 use MIDI-Chan 2 for columns 17 to 25
  if (col >= NUMCOLS) return;
  
  byte layer = LED_LAYER_CUSTOM1;
  if (userFirmwareActive) {
    layer = LED_LAYER_CUSTOM2;
  }
  if (midiData2 > COLOR_PINK) midiData2 = COLOR_OFF;
  setLed(col, row, midiData2, cellOn, layer);
  checkRefreshLedColumn(micros());
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



void microLinnPaintRainbows41() {
  byte startCol[NUMSPLITS] = {1, 1};                                // where each split starts
  byte splitWidth[NUMSPLITS] = {NUMCOLS - 1, NUMCOLS - 1};          // how many columns each split spans
  short rainbowStart;
  if (Global.splitActive) {
    splitWidth[LEFT] = Global.splitPoint - 1;
    startCol[RIGHT] = Global.splitPoint;
    splitWidth[RIGHT] = NUMCOLS - Global.splitPoint;
  } else if (Global.currentPerSplit == LEFT) {
    splitWidth[RIGHT] = 0;
  } else {
    splitWidth[LEFT] = 0;
  }
  for (byte side = 0; side < NUMSPLITS; ++side) {
    for (byte row = 0; row < NUMROWS; ++row) {
      rainbowStart = microLinnEdostep[side][startCol[side]][row];        // rainbowStart is a pointer into RAINBOWS41
      if (isLeftHandedSplit(side)) {
        rainbowStart *= -1;
      }
      if (rainbowStart % 2 != 0) {
        rainbowStart += 41;                                                      // force it to be even
      }
      rainbowStart = microLinnMod(rainbowStart / 2, 41);                         // convert from edosteps to columns
      memcpy(&Device.customLeds[1][row * MAXCOLS + startCol[side]],              // [1] means 1st custom light pattern
             &MICROLINN_RAINBOWS41[isLeftHandedSplit(side)][rainbowStart], splitWidth[side]);
    }
  }
}




// not called, leave the 3 light patterns alone and just use paintMicroLinnNormalDisplayCell()
void microLinnPaintFullRainbow() {                   // fill 2nd custom light pattern using anchor cell, transpose, etc.
  for (byte side = 0; side < NUMSPLITS; ++side) {
    byte lowCol, highCol;
    getSplitBoundaries(side, lowCol, highCol);
    for (byte col = lowCol; col <= highCol; ++col) {
      for (byte row = 0; row < NUMROWS; ++row) {
        byte edostep = microLinnEdostep[side][col][row];
        Device.customLeds[1][row * MAXCOLS + col] = microLinn.rainbows[microLinn.EDO][edostep];
      }
    }
  }
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


const byte CUSTOM_LEDS_PATTERN_DOTS [LED_LAYER_SIZE] = {              // two green kites for Kite guitar
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,
   0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,
   0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0, 25,
   0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

const byte CUSTOM_LEDS_PATTERN_DOTS_LEFTY [LED_LAYER_SIZE] = {  
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,
   0, 25,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,
   0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

    if (isFullyLefty) {
      memcpy(&Device.customLeds[2][0], &CUSTOM_LEDS_PATTERN_DOTS_LEFTY[0], LED_LAYER_SIZE);
    } else {
      memcpy(&Device.customLeds[2][0], &CUSTOM_LEDS_PATTERN_DOTS[0], LED_LAYER_SIZE);
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



void handleOctaveTransposeNewTouchSplitSkipFretting(byte side) {
  // alternate version of handleOctaveTransposeNewTouchSplit
  // send CCs reporting the transposes to LinnstrumentMicrotonal app, it will do the transposing
  // use the CC that the right foot switch is assigned to via long-pressing the CC65 option
  // repurpose semitones to be whole tones and lights to be arrows/edosteps, so that ±7 of each covers 41edo
  // midi for octave transposing via footswitch reports each increment/decrement, plus a 0 CC for pedal release
  // but midi from here directly reports the current amount of transpose, no zero CCs
  // the CCvalue is 93-99 for ±3 octaves, 57-71 for ±7 tones, or 25-39 for ±7 arrows
  // see also paintOctaveTransposeDisplay function in ls_displayModes.ino

  signed char newTransposeOctave = skipFrettingData[side].transposeOctave; 
  signed char newTransposeTone   = skipFrettingData[side].transposeTone;
  signed char newTransposeArrow  = skipFrettingData[side].transposeArrow;

  if (sensorCol > 0 && sensorCol < 16) {
         if (sensorRow == OCTAVE_ROW)   {newTransposeOctave = sensorCol - 8;}
    else if (sensorRow == SWITCH_1_ROW) {newTransposeTone   = sensorCol - 8;}
    else if (sensorRow == SWITCH_2_ROW) {newTransposeArrow  = sensorCol - 8;}
  }
  newTransposeOctave = min (max (newTransposeOctave, -3), 3);

  byte chan = (side == LEFT ? 1 : 16);                                        // midi channel
  byte CCnum = Global.ccForSwitchCC65[SWITCH_FOOT_R];
    
  if (newTransposeOctave != skipFrettingData[side].transposeOctave) {
    skipFrettingData[side].transposeOctave = newTransposeOctave;                           
    midiSendControlChange (CCnum, 96 + newTransposeOctave, chan, true);       // range is 93-99                   
  }

  if (newTransposeTone != skipFrettingData[side].transposeTone) { 
    skipFrettingData[side].transposeTone = newTransposeTone;                           
    midiSendControlChange (CCnum, 64 + newTransposeTone, chan, true);         // range is 57-71                   
  }

  if (newTransposeArrow != skipFrettingData[side].transposeArrow) { 
    skipFrettingData[side].transposeArrow = newTransposeArrow;                           
    midiSendControlChange (CCnum, 32 + newTransposeArrow, chan, true);        // range is 25-39             
  }
}

from ls_settings.ino
void handleOctaveTransposeNewTouchSplit(byte side) {

  if (isSkipFretting(side) && Global.rowOffset > 7) {               // rowOffset > 7 to exclude 12edo Wicki-Hayden users,
    handleOctaveTransposeNewTouchSplitSkipFretting (side);          // who will want to transpose normally
    return;
  }
**********************************************************************************************/




/**********************************************************************************************

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
  microLinnStoreRowOffsetCents();
  microLinnStoreColOffsetCents(LEFT);
  microLinnStoreColOffsetCents(RIGHT);
  microLinnPrevEDO = microLinn.EDO;
  calcMicroLinnTuning();
  updateDisplay();
  microLinnUpdateAnchorString();
}




byte forkMenuProtectForkData(byte audienceMessageToEdit) {      // keep the user from editing fork data stored at the end of the message
  switch (audienceMessageToEdit) {
    case MICROLINN_MSG: 
      return MICROLINN_MSG_LEN;
    default: 
      return 30;
  }
}

 // fill in all 30 spaces of the message
      int strl = strlen(Device.audienceMessages[audienceMessageToEdit]);
      byte maxLen = forkMenuProtectForkData (audienceMessageToEdit);      // see ls_forkMenu.ino
      if (strl < maxLen) {
        for (byte ch = strl; ch < maxLen; ++ch) {
          Device.audienceMessages[audienceMessageToEdit][ch] = ' ';       // extend to full length with trailing spaces
        }
      }
      Device.audienceMessages[audienceMessageToEdit][maxLen] = '\0';      // null-terminated




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




const byte MICROLINN_DOTS2[MICROLINN_ARRAY_SIZE] = { 
// goes to 28 columns so that the full octave can be deduced by symmetry for edos up to 55
// 0 = unmarked, 16 = single dot, 40 = double (4th/5th if > 5 dots per octave), 84 = triple (octave)
  84, 0,16,16, 0, // 5edo ---- edos 5-9 mark P4 and P5
  84, 0,16, 0,16, 0, // 6edo
  84, 0, 0,16,16, 0, 0, // 7edo
  84, 0, 0,16, 0,16, 0, 0, // 8edo
  84, 0, 0,16, 0, 0,16, 0, 0, // 9edo (9b)
  84, 0,16, 0,16, 0,16, 0,16, 0, // 10edo --- edos 10-31 mark (M2) m3 P4 P5 M6 (m7)
  84, 0,16, 0,16, 0, 0,16, 0,16, 0, // 11edo (11b)
  84, 0, 0,16, 0,16, 0,16, 0,16, 0, 0, // 12edo
  84, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, // 13edo
  84, 0,16, 0,16, 0,40, 0,40, 0,16, 0,16, 0, // 14edo
  84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0, // 15edo
  84, 0,16, 0, 0,16, 0,40, 0,40, 0,16, 0, 0,16, 0, // 16edo
  84, 0, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0, 0, // 17edo
  84, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, // 18edo
  84, 0, 0,16, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0,16, 0, 0, // 19edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, // 20edo
  84, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0, // 21edo
  84, 0, 0, 0,16,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0, 0, // 22edo
  84, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0, 0, 0,16, 0, 0, // 23edo
  84, 0, 0, 0,16, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0,16, 0, 0, 0, // 24edo
  84, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0, // 25edo
  84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0, 0,16, 0, 0, 0, // 26edo
  84, 0, 0, 0, 0,16,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16,16, 0, 0, 0, 0, // 27edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 28edo
  84, 0, 0, 0, 0,16, 0,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16, 0,16, 0, 0, 0, 0,  // 29edo
  84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,  // 30edo (exception -- 15edo dots doubled)
  84, 0, 0, 0, 0,16, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16, 0, 0,16, 0, 0, 0, 0,  // 31edo
  84, 0, 0,16, 0,16, 0, 0,16, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0,16, 0, 0,16, 0,16, 0, 0,  // 32edo --- edos 32-55 approximate 12edo
  84, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,40, 0, 0, 0, 0,40, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,  // 33edo
  84, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,  // 34edo
  84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,  // 35edo
  84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,  // 36edo
  84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,  // 37edo
  84, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,  // 38edo
  84, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,  // 39edo
  84, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,  // 40edo
  16, 0, 0, 0,40, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,40, // 41edo (exception -- kite guitar dots)
  84, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,  // 42edo
  84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,  // 43edo
  84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,  // 44edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 45edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 46edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 47edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 48edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 49edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 50edo
  84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 51edo
  84, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,  // 52edo
  84, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,  // 53edo
  84, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,  // 54edo
  84, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0,16,16,0, 0, 0,40, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0    // 55edo
};

const byte MICROLINN_DOTS[MICROLINN_MAX_EDO+1][MAXCOLS+3] = { 
// goes to 28 columns so that the full octave can be deduced by symmetry for edos up to 55
// 0 = unmarked, 16 = single dot, 40 = double (4th/5th if > 5 dots per octave), 84 = triple (octave)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,84, 0,16,16, 0,84, 0,16,16, 0,84, 0,16,16, 0,84, 0,16,16, 0,84, 0,16,16, 0,84, 0,16,  //  5edo ---- edos 5-9 mark P4 and P5
  0,84, 0,16, 0,16, 0,84, 0,16, 0,16, 0,84, 0,16, 0,16, 0,84, 0,16, 0,16, 0,84, 0,16, 0,  //  6edo
  0,84, 0, 0,16,16, 0, 0,84, 0, 0,16,16, 0, 0,84, 0, 0,16,16, 0, 0,84, 0, 0,16,16, 0, 0,  //  7edo
  0,84, 0, 0,16, 0,16, 0, 0,84, 0, 0,16, 0,16, 0, 0,84, 0, 0,16, 0,16, 0, 0,84, 0, 0,16,  //  8edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,84, 0, 0,16, 0, 0,16, 0, 0,84, 0, 0,16, 0, 0,16, 0, 0,84,  //  9edo (9b)
  0,84, 0,16, 0,16, 0,16, 0,16, 0,84, 0,16, 0,16, 0,16, 0,16, 0,84, 0,16, 0,16, 0,16, 0,  // 10edo --- edos 10-31 mark (M2) m3 P4 P5 M6 (m7)
  0,84, 0,16, 0,16, 0, 0,16, 0,16, 0,84, 0,16, 0,16, 0, 0,16, 0,16, 0,84, 0,16, 0,16, 0,  // 11edo (11b)
  0,84, 0, 0,16, 0,16, 0,16, 0,16, 0, 0,84, 0, 0,16, 0,16, 0,16, 0,16, 0, 0,84, 0, 0,16,  // 12edo
  0,84, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0,84, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0,84, 0,  // 13edo
  0,84, 0,16, 0,16, 0,40, 0,40, 0,16, 0,16, 0,84, 0,16, 0,16, 0,40, 0,40, 0,16, 0,16, 0,  // 14edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16,  // 15edo
  0,84, 0,16, 0, 0,16, 0,40, 0,40, 0,16, 0, 0,16, 0,84, 0,16, 0, 0,16, 0,40, 0,40, 0,16,  // 16edo
  0,84, 0, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0, 0,84, 0, 0, 0,16, 0, 0,16, 0, 0,16,  // 17edo
  0,84, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0,84, 0, 0,16, 0, 0, 0,16, 0, 0,  // 18edo
  0,84, 0, 0,16, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0,16, 0, 0,84, 0, 0,16, 0,16, 0, 0,40,  // 19edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,  // 20edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,84, 0, 0,16, 0, 0,16,  // 21edo
  0,84, 0, 0, 0,16,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0, 0,84, 0, 0, 0,16,16,  // 22edo
  0,84, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0,40, 0, 0,16, 0, 0, 0,16, 0, 0,84, 0, 0,16, 0,  // 23edo
  0,84, 0, 0, 0,16, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0,16, 0, 0, 0,84, 0, 0, 0,  // 24edo
  0,84, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0, 0,84, 0, 0,  // 25edo
  0,84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0, 0,16, 0, 0, 0,84, 0,  // 26edo
  0,84, 0, 0, 0, 0,16,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16,16, 0, 0, 0, 0,84,  // 27edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,  // 28edo
  0,84, 0, 0, 0, 0,16, 0,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16, 0,16, 0, 0, 0,  // 29edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0, 0,16,  // 30edo (exception -- 15edo dots doubled)
  0,84, 0, 0, 0, 0,16, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0, 0,40, 0, 0, 0, 0,16, 0, 0,16, 0,  // 31edo
  0,84, 0, 0,16, 0,16, 0, 0,16, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0,16, 0, 0,16,  // 32edo --- edos 32-55 approximate 12edo
  0,84, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,40, 0, 0, 0, 0,40, 0, 0,16, 0, 0,16, 0,16,  // 33edo
  0,84, 0, 0,16, 0, 0,16, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16, 0,  // 34edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0, 0, 0,40, 0, 0,16, 0, 0,16, 0,  // 35edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,16,  // 36edo
  0,84, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0,16, 0, 0,  // 37edo
  0,84, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,  // 38edo
  0,84, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0,16, 0,  // 39edo
  0,84, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0,16, 0, 0,40, 0, 0,16, 0,  // 40edo
  0,16, 0, 0, 0,40, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,84, 0, 0, 0,16, 0, 0, 0,  // 41edo (exception -- kite guitar dots)
  0,84, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0, 0,  // 42edo
  0,84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40, 0, 0,  // 43edo
  0,84, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40, 0,  // 44edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40, 0,  // 45edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,40,  // 46edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0,16,16, 0, 0,40,  // 47edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0, 0,  // 48edo
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16, 0, 0,  // 49edo (...0,40)
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0, 0,  // 50edo (...0,40)
  0,84, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16, 0,  // 51edo (...0, 0,40)
  0,84, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16, 0,  // 52edo (...0, 0,40)
  0,84, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,40, 0, 0, 0,16,16,  // 53edo (...0, 0, 0,40)
  0,84, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0,16,  // 54edo (...0, 0, 0,40)
  0,84, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,16, 0, 0, 0,16, 0, 0, 0, 0,40, 0, 0, 0,16   // 55edo (...16,0, 0, 0,40)
};

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

**********************************************************************************************/