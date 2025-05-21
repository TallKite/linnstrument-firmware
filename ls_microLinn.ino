/**************************************** MICROLINN ****************************************

This code augments the official code and does not replace any of it. It only runs if 
  something in the microLinnConfig display is turned ON
  any sequencer patterns are chained
  a switch or footswitch is PLAY, PREV, NEXT or MUTE and the sequencer isn't visible
  the played mode is BLNK
  SAME or BLNK is enabled on both splits
  a footswitch is set to 8VE± or QNT±
  etc.
The code that actually handles playing in real time is unchanged, so there should be no increases in latency

When the user releases a column 0 button, there is about a 1/8 second delay before one can play
When microLinn is on, there's a slight extra delay while caculating tuning tables
But getNoteNumber() will use these tables, which might reduce latency while playing

a function that starts with "microLinn" is only called in this .ino file, e.g. microLinnTriIndex()
as opposed to a function that contains "microLinn" but doesn't start with it, e.g. isMicroLinnOn()

The maximum row offset is increased from 16 to 25, so that one row begins where another one ends
The maximum column offset is 8 so that notes from the other 7 rows can fill in the gaps

The microLinn software version number is the usual 6-digit version number plus our 3-digit number
This is the number that the Global Settings OS VERS button shows, e.g. 234.072.001
The microLinn data structure version number is Device.version (16 as of late 2024) plus an offset of 56,
plus Device.microLinn.MLversion = 0 (soon to be 1), making 72.0, soon to be 72.1
This is the number ls_extstorage.ino uses to migrate user settings from one OS version to the next


======================= TO DO ==============================

Ask Jim: Split[].microLinn.colOffset and Global.microLinn.useRainbow are both 0 after updating to microLinn
when to call storeSettings()?

MUST-DO #1

delete code for col 16 and/or 17 shortcuts (search for ifndef)
IMPORTANT: Later on when you update the OS to a different version, you'll be asked if you want to uninstall microLinn. 
Say no (tap the red button in the middle, turns orange) if you're updating to a newer version of microLinn. This avoids 
needlessly deleting your microtonal user settings. Say yes (tap the blue button on the left, turns light blue) otherwise, 
e.g. if you're going back to an official (non-microtonal) version of the firmware. This does delete your microtonal user 
settings, which is necessary in order to avoid deleting all your other user settings plus your calibration data. 
Once you've decided, tap the green button on the right to continue.

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

import types 9-11: warning excess data
import type 14, 15: various data failures

31edo (+3 +5) R3C11 = C2, last column is silent (probably just from no calibration)

if row & col offsets aren't coprime, paint a red dot on the global display
and paint the actual offset figure in red
byte getMicroLinnOffsetColor(rowOffset, colOffset) returnss globalColor or red

fix bug with anchor row = 0 and guitar tuning, strings preview note are off by one

make rainbow enabler button disappear when fretboard is displayed

ask on the forum about the unplugging bug for NRPN send/receive

when 72.1 comes out, add new fields to importGlobal() and importSplits(), update import sizes

test if changing splits always triggers a calctuning call, if so, just calc the displayed split(s)

test all 5 of herman miller's 41edo lumatone layouts

test 41edo note-on, note-on, note-off with a mono synth, does tuning bend fail? what about in chanPerNote mode?

make anchor cents sound only on release, too obnoxious otherwise

test new switch function 8VE± with transposing, also make it and toggle_quantize not latch on a long press

test equaves: setting edo to 36 and equave to 600c makes 72edo! 26edo and 240c = 130edo

fix custom LED bugs: completelyRefreshLeds, startBufferedLeds, finishBufferedLeds, see paintSequencerDisplay()
  see NRPN 247
  see https://www.kvraudio.com/forum/viewtopic.php?t=519090, checkRefreshLedColumn(micros())

fix all differing per-split values such as column offset to match as soon as double split is enabled
    when you hold R split and tap L split, the L value changes to the R value
    display the old L value in fast-blinking green for 0.25 seconds, then the new L value in non-blinking green
    OR  when you hold R split and tap L split, the R value changes to the L value
    display the blinking yellow R value for 0.5 seconds, then the non-blinking yellow new L value
  But swiping past the opposite value shouldn't link

test PREV/NEXT footswitches with sequencer

test octave up/down footswitches while playing, does the calcTuning call make it glitchy?
  if so, copy how the original code does transposing

debug same/blink applying to the other side for note-offs, search for resetPossibleNoteCells

organize config into Snon = per-split non-xen, Gnon = global non-xen, Sxen = per-split xen, Gxen = global xen
  __ Col Row __ Snon __ Gnon __ __ edo __ scales __ Gxen __ Sxen
   2      colOffsets
   3      rowOffsets
   5 (6)  Snon: (collapseToScale), (hammerOnWindow), (hammerOnMode), (pullOffMode), showCustomLEDs,
   7 (8)  Gnon: drumPad, locatorCC1, locatorCC2, Bulk Importing ON/OFF
  10      edo
  12      scales aka note lights
  14 (15) Gxen: anchor x 3, equaveSemitones, equaveCents, (sweeten), (largeEDO)
  16      Sxen: tuningTable

  room for future features, can add a 2nd Gnon, Snon, or Gxen button
  when on the scales display, col 12 row 0 button lights up to match the tonic
  on the 200, move everything but the far left column over 2 cells?
  after moving config buttons around, check microLinnIsDisabledConfigButton() for "1st pick an edo" messages
  On the note lights screen, move both pink walls in 1 column? or move the whole display over 2 cells if on a 200 model?


POSSIBLE NEW FEATURES

warning: RAM is limited, can cause Arduino IDE triple compile error

see https://github.com/teknico/linnstrument-firmware/releases/tag/t01.001

drum pad mode, besides ON and OFF, add MRMB/MRB for marimba mode
  this adds an 8th column and gets the midi notes from the current scale, transposable
  if microLinn is off, the scale starts at lo-C = 48
  if on, it starts at the anchor note (needs to include tuning bends)
  if custom light pattern or if microLinn fretboard, no scale, no sound

hammerons/pulloffs screen, select for each split,
  window of so many cents, 0 = off
  option to send or not send a new noteon when hammering or pulling off? (guitar = yes, wind instrument = no)
  pulloffs: old note's velocity = the new note's velocity or the original old velocity? (pull-off vs lift-off)
  byte pullOffVelocity; 0 = 2nd note's noteOff velocity, 1 = 1st noteOn veloc, 2 = 2nd noteOn veloc, 3 = average them
  see handleStrummedRowChange(), handleStrummedOpenRow()
HAMMER-ONS AND PULL-OFFS: Set the hammer-on window to say 150¢. When you play a note within 150¢ of another note on the same row, 
it will hammer-on and mute the other note, as if you were strumming in the other split. When you release it, the old note will be 
unmuted (pull-off). Access it through the microLinn menu, see below.

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

Collapse to scale:
  if microLinn is off, select one of the first 9 scales
  OR select an edo and select one of the first 7 scales
  (if display is custom LEDs or microlinn full rainbow or fretboard, and no microLinnPrevScale, collapse scale is stuck in OFF)
  change collapse scale from OFF to set the width of a single-pad pitch bend in edosteps 1..L, L = largest scale step
  the last option is AVG = the average scale step = 1\N, smaller than L, e.g. if heptatonic AVG = 1\7 = 171¢
  all the unlit notes in each row go away and the horizontal range increases
  the scale's tonic is automatically included in the scale, so the anchor cell is never removed, amd no empty scales
  the anchor cell stays the same, and the anchor column is made up of the nearest scale notes
  if this is not what the user wants, they can set the guitar tuning to make the anchor column be all scale notes
  if col offset is ON, the scale is first collapsed, then notes are skipped

  the row offset becomes inconsistent edosteps but a consistent interval, stepspan = round (numNotes * rowOffset / edo)
  but if unrounded = N.5, row offsets alternate N and N+1, e.g. if 12edo heptatonic, +6\12 makes alternating 4ths and 5ths
  if you select the guitar tuning before or after collapsing, stepspans vary, follows the formula, but N.5 is rounded down
  if you select No Overlap, stepspan = width of the split
  
  you can still bend normally within a pad for vibrato, but sliding to a note often means overshooting or undershooting
  if in CDEFGABC' and 1 edostep, to slide from D to E, end on F. from C to G, end on C', from C to C', end on A' 
  if in CDEFGABC' and 2 edosteps, to slide from D to E, slide normally, but to slide from E to F, end midway between E and F
  if in any scale and AVG mode, you can slide to an octave normally, and slide to any note normally if Quantize Hold is on
  if you change scales while in scale mode, the L and AVG numbers are recalculated and applied to microLinnBendPerPad
  if you change to one of the custom LED patterns or to microlinn full rainbow or fretboard, the last scale selected is used
  
  Large EDO subset of up to 55 notes:
    for an N-note scale, select N notes of S-edo then select large EDO = L-edo, runs 56..311 (or N+1..311?)
    it's a good idea to pad a very uneven scale with a few extra notes, makes bends work better
    short Global.microLinn.largeEDO = 56..maxLargeEDO plus 55 = OFF plus maxLargeEDO+1 = JI, plus ANY
    56..72, 87, 94, 99 	118 	130 144	152 159 171, 183, 205,	217 	224 	270 	311, 1200, ANY  94 144 205
    byte Global.microLinn.largeEdoScale[39] = 1 scale, 39 x 8 = 312 max edo
      the S-edo scale gets mapped to L-edo by cents
      a new button appears on the scales screen, tap it to enter fine-tuning mode
      tap any note and get a big blue number, edosteps in L-edo or tenths of a cents in JI, swipe it to adjust ±127
      tap the col 12 scales button to get back to the scales screen to tap another note
      rank-2 scales?
      OLD WAY
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
    to transpose in 311edo, 311edo maj 2nd = 53\311, round(53/15) = 4, so transpose by edosteps is really 4 edosteps = 14¢

investigate lumatone/bosanquet-style color schemes
add new color between lime and green for neutral intervals?

add new played mode rainbow (RNBW or RNB) that reveals the rainbow color even on the fretboard or custom LED pattern
  or just have it kick in when cell color = off?

make doublePerSplit work better for volume
  doublePerSplit becomes doubleSplitVol and doubleSplitTranspose
  volume screen shows two long bars not one, paint in rows 6 and 2, handle in rows 5-7 and 0-3
  get rid of doublePerSplit display on preset screen, get rid of doublePerSplit reset in per-split screen

write code for 6 new memories/presets, 6 new buttons next to the 6 standard ones (column 15 or column 22)?
  for the Linn 128, move the program change number one pad to the left to make room for the new buttons

when microLinn is installed, move the 16 sequencer projects around so that the row of the 4x4 array doesn't change?
  the uninstall code moves them back?
SEQUENCER PROJECTS LOAD/SAVE SCREEN: When exporting/importing projects to/from your computer, the updater app refers to 
projects by number. These project numbers now run top to bottom, with the top row being 1-4 not 13-16. This is more 
intuitive because it matches how people read, for example how you are reading this very paragraph right now.

incorporate the channel pressure fix (will be part of future official updates anyway)
  see https://www.kvraudio.com/forum/viewtopic.php?t=591770

cleanup: search for "delete", "uncomment" and "bug"

TO DECIDE

drop maxEDOsteps down to 384? max numGroups becomes 3. decide after inplementing collapse to scale

implement namespace? search for [^a-z]microLinn[a-z], change to [^a-z]ML[a-z]
   fix sizeof(ML) by hand

POSSIBILTIES

Add the ability to set the split point to col 1? enables single-tap mode to switch splits
  settings.ino handleSplitPointNewTouch() comment out first line
  handleControlButtonNewTouch()
    case SPLIT_ROW:  (doubleTap || Global.splitPoint == 1)
    maybe set Global.splitActive == false;

should there be an option to send both CC1 and CC74 for timbre? done by pressing both pads at once.
  same for poly pressure, channel pressure and CC11 for loudness? press 2 or 3 pads at once
  the choices could be stored in TimbreExpression and LoudnessExpression as e.g. timbreCC1+CC74
  before coding it up, ask in the forum if there's interest, then test with a reaper jsfx that dupllicates CCs

make system reset be a long press not a short press?

accept sysex82 messages for non-edo tunings? when quantize is on, the pitch starts at and/or drifts to the JI note

run the arpeggiator and the sequencer both at once, so that the arpeggiator uses the sequencer's rhythm & velocity?

POSSIBLE MEMORY USAGE AND SPEED OPTIMIZATIONS

  DeviceSettingsV12 = 2204 bytes
  MicroLinnDevice = 4591 bytes
  DeviceSettings  = 7549 bytes, so pre-microLinn it was 2958
  PresetSettings = 400 bytes (Global plus both splits)
  thus the 6 presets will be 2400 bytes
  total is 10349 bytes
  microLinn sizeof(config) = 16564 bytes

reduce Global.mainNotes and Global.accentNotes from [12] to [9] and change from int to short? 
  96 bytes becomes 36 bytes, saves 420 bytes with 6 presets, 840 bytes if 12 presets

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

implement color and scale tables, to speed up cell painting? gets calced in calcTuning()
  byte microLinnColor[NUMSPLITS][MAXCOLS][MAXROWS];              // high 4 bits: the color of each cell, low 4 bits = the CellDisplay (on, off, pulse, etc.)
  byte microLinnScale[NUMSPLITS][MAXCOLS][MAXROWS];              // bits 0-6 are for the 7 scales, bit 7 is for the fretboard


POSSIBLE PULL REQUESTS (every non-microtonal feature that doesn't require a new user setting, test thoroughly first)
  sequencer: foot pedal fix, pattern-chaining feature, 4x4 array fix
  make SAME carry over to the other side
  DONE (long-press low power affects brightness only)
  make doublePerSplit 2 independent vars for volume and transpose
  add 8VE± footswitch function
  line 722 of ls_settings is "midiPreset[0] = 0;", should probably be "midiPreset[s] = 0;"
  lines 1829 and 1830 of ls_midi.ino, "ch" should probably be "ch+1"
  line 587 of ls_fonts.ino, "static Character small_Q = { 4,", 4 should be 5
  line 900 of ls_midi.ino, "if (inRange(value, 0, 14)) {", 14 should be 15
  line 127 of midi.txt, limits should be 0-15, also in the list playedCell is missing


note to self: user can enter/exit microLinn only by going to microLinnConfig and changing the edo from 4 
or by loading a preset (directly or via NRPN), the EDO+ and EDO- switches/pedals don't let you enter or exit

note to self: to use multi-channel output with my non-MPE s90es synth, deselect the main channel

********************************************************************************************************************/


// virtual edosteps needed = 7 * maxRowOffset + 24 * maxColOffset + 1 = 368 max, but up it to 512 to give the guitar tuning more leeway
const short MICROLINN_MAX_EDOSTEPS = 512;                  // virtual edosteps run 0..511
const short MICROLINN_MAX_GUITAR_RANGE = 320;              // 512 - 192, 192 = max row length = numCols * maxColOffset = 24 * 8

const byte MICROLINN_MIN_2ND[MICROLINN_MAX_EDO+1] = {      // used for Bosanquet layout
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
  1,  1,  1,  1,  2,   3,  2,  3,  2,  3,    // 10-19      // and it's 13b and 18b, not 13 and 18
  4,  3,  4,  3,  4,   5,  4,  5,  4,  5,    // 20-29      // used for transposePitch and transposeLights
  6,  5,  6,  5,  6,   5,  6,  7,  6,  7,    // 30-39
  6,  7,  8,  7,  8,   7,  8,  7,  8,  9,    // 40-49
  8,  9,  8,  9, 10,   9                     // 50-55
};

const byte MICROLINN_MIN_3RD[MICROLINN_MAX_EDO+1] = {      // defined as best 6/5, used for Harmonic Table layouts
  0,  0,  0,  0,  3,   1,  1,  1,  2,  2,    // 0-9        // but often 6/5 and 5/4 aren't coprime, so use 8/7 7/6
  2,  2,  3,  3,  3,   4,  4,  3,  5,  5,    // 10-19      // and for certain smaller edos, use P4/2 not P5/2
  5,  5,  6,  6,  5,   7,  7,  7,  7,  8,    // 20-29
  7,  7,  9,  9,  9,   9, 10,  9,  9, 10,    // 30-39
 10, 11, 11, 11, 11,  11, 11, 11, 11, 13,    // 40-49
 13, 13, 13, 14, 15,  15                     // 50-55
};

const byte MICROLINN_MAJ_3RD[MICROLINN_MAX_EDO+1] = {      // defined as best 5/4, used for Harmonic Table layouts
  0,  0,  0,  0,  4,   2,  2,  3,  3,  3,    // 0-9  
  3,  3,  4,  4,  5,   5,  5,  7,  6,  6,    // 10-19
  7,  7,  7,  7,  9,   8,  8,  8,  9,  9,    // 20-29
 11, 11, 10, 10, 11,  11, 11, 13, 13, 13,    // 30-39
 13, 13, 14, 14, 15,  15, 16, 16, 16, 16,    // 40-49
 16, 17, 17, 17, 17,  17                     // 50-55
};

const byte MICROLINN_PERF_4TH[MICROLINN_MAX_EDO+1] = {     // defined as best 4/3
  0,  0,  0,  0,  5,   2,  2,  3,  3,  4,    // 0-9        // used to test if microLinn.guitarTuning is F#BEADGBE
  4,  5,  5,  5,  6,   6,  7,  7,  7,  8,    // 10-19      // if it isn't, the guitar tuning light is light blue
  8,  9,  9, 10, 10,  10, 11, 11, 12, 12,    // 20-29      // 13b = 6666626 and 18b = 8888848 are also allowed
 12, 13, 13, 14, 14,  15, 15, 15, 16, 16,    // 30-39      // also used for Wicki-Hayden layouts
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
// lime (10) is used for yellow because the official yellow (2) is too orangish
// 8     = white         = 12-edo-ish           = 3-limit
// 10/3  = yellow/green  = submajor/superminor  = 5-over/5-under
// 5/1   = blue/red      = subminor/supermajor  = 7-over/7-under
// 6     = magenta       = neutral              = 11-over or 11-under or 13-over or 13-under
// 11    = pink          = 600c, a half-8ve     = 12-edo-ish but not quite 3-limit, "off-white"
// 4     = cyan          = outside notes aka interordinals (e.g. 19edo and 24edo)
// 9     = orange        = used in 55edo for superneutral and subneutral, to avoid 3 magentas in a row
//         cyan/orange   = used in 41edo for 7/5 and 10/7

// Bosanquet / Wicki-Hayden colors
//                    for 5n and 7n edos
//  white = natural   pink = tonic
//  green = b         down
// yellow = #         up
//   cyan = bb        dud
// orange = ##        dup
//   blue = bbb       trud/trup
//   pink = x#
//    red = 
// purple = 
// yellow/orange = 

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
//8, 10,8, 10,8, 10,3, 10,8, 10,8, 10,8, 10,8, 10,3,10, // 18b
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
  127, 0,65,65, 0, // 5edo
  127, 0,64, 1,64, 0, // 6edo
  127,64,64, 1,65,64, 0, // 7edo
  127, 0,64, 0,64,64, 0,64, // 8edo
  127,64,64, 0,65,65,64,64, 0, // 9edo
  127, 0,64, 0,65, 0,65, 0,64, 0, // 10
  127,64,64, 0, 0,65,65,64,64, 0, 0, // 11
  127, 0,71,10,69,75, 0,79, 2,69,10,65, // 12
  127, 0,64,65, 0,64,65,64,65, 0,64,65,64, // 13
  127, 0,65, 0,67, 0,66, 0,67, 0,65, 0,66, 0, // 14
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

const byte MICROLINN_FRETBOARDS [6 * (MICROLINN_MAX_EDO - 4)] = { 
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

////////////  runtime vars  ///////////////////////////

short microLinnEdostep[NUMSPLITS][MAXCOLS][MAXROWS];                // each cell's edosteps relative to the anchor cell
signed char microLinnMidiNote[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];   // the midi note number for each untransposed virtual edostep, -1 = unused
short microLinnTuningBend[NUMSPLITS][MICROLINN_MAX_EDOSTEPS];       // the deviation from 12edo as a pitch bend number from -8192 to 8191

float microLinnSemitonesPerPad[NUMSPLITS];                      // number of 12edo semitones in a pitch bend one pad wide, positive even when lefty
short microLinnColOffsetCents[NUMSPLITS];                       // row/col offset cents are set only when user sets an offset directly...
short microLinnRowOffsetCents[NUMSPLITS];                       // ...used to avoid the offset drifting too wide or too narrow when switching edos
short microLinnGlobalRowOffsetCents;
short microLinnGuitarTuningCents[MAXROWS];                      // cents between open strings, can be negative, [0] is unused
short microLinnGuitarTuning[MAXROWS];                           // cumulative row offsets, unlike Global.microLinn.guitarTuning, for a faster calcTuning()
short microLinnHighestGuitarNote;                               // the highest/lowest note in the anchor column, as edosteps from the anchor cell...
short microLinnLowestGuitarNote;                                // ...used to find lowestEdostep and numGroups, and to limit guitar range when changing edos
short microLinnLowestEdostep[NUMSPLITS];                        // what not where, lowest edostep when the anchor cell is 0, usually negative
byte microLinnChannelCount[NUMSPLITS];                          // used for rechannelling
byte microLinnNumGroups[NUMSPLITS];                             // 1 if highestEdostep - lowestEdostep < 128, 2 if < 256, 3 if < 384, 4 if < 512
short microLinnSweetener[NUMSPLITS];                            // 2¢ (or whatever) as a zero-centered midi pitch bend value
char microLinnAnchorString[6] = "R C  ";                        // row and column of the anchor cell, e.g. "R3C12", top row is row #1
boolean microLinnDoublePerSplitColOffset;                       // true if column offset screen adjusts both splits at once
byte microLinnPrevScale;                                        // for backtracking to the previous scale easily
boolean microLinnCanBacktrack = false;                          // true if newly touching an already selected scale button

byte microLinnConfigColNum = 0;                                 // active col number in microLinnConfig display, 0 means nothing active
byte microLinnPerSplitOptionsRowNum = 0;                        // active col number, 0 means nothing active
boolean microLinnConfigNowScrolling = false;
boolean microLinnMiscOptionsNowScrolling = false;
boolean microLinnPerSplitOptionsNowScrolling = false;
boolean microLinnOctaveToggled = false;                         // used by the 8VE± footswitch
byte microLinnGuitarTuningRowNum = NUMROWS + 1;                 // active row number 0..7, NUMROWS+1 means no row selected, +1 so that the neighbor isn't lit
byte microLinnMiscOptionsRowNum = 0;                            // 0 = nothing selected, 6 or 5 = CC for cols 1-16 or 17-25, 2 or 3 = tuning table for left/right
boolean microLinnUninstall = false;
byte edo;                                                       // this is the only microLinn var or function that doesn't contain "microLinn" in its name

byte microLinnHammerOnEdosteps[NUMSPLITS];                      // convert Split[side].microLinn.hammerOnWindow from cents to edosteps
byte microLinnCollapsedScale[MICROLINN_MAX_EDO];                // e.g. 12edo major scale --> [0 2 4 5 7 9 11 0 0 0 0 0]
byte microLinnNumNotes;                                         // number of notes in the collapsed scale, 1..55
byte microLinnLargestStep;                                      // largest step in the collapsed scale, 1..55

// these 4 will become Split.microLinn vars
signed char microLinnRowOffset[NUMSPLITS] = {-26, -26};         // range is ±25 plus -26 = OFF and +26 = NOVR (no overlap)
byte microLinnShowCustomLEDs[NUMSPLITS] = {0, 0};               // 0 = OFF, 1-3 = the three patterns, 4-6 = patterns under note lights
byte microLinnCollapseBendPerPad = 0;                           // width of a single-pad pitch bend in edosteps, 0 = OFF, 1..L (L = largest scale step), L+1 = AVG = 1\N-edo
byte microLinnLayout[NUMSPLITS] = {0, 0};

// these 3 will become Global.microLinn vars
boolean microLinnDrumPadMode = false;
signed char microLinnLocatorCC1 = -1;                           // CC to send with row/column location for each note-on in cols 1-16 or cols 17-25
signed char microLinnLocatorCC2 = -1;                           // ranges from 0 to 119, -1 = OFF

struct MicroLinnPull {boolean sharp, flat;};                    // used to sweeten 41edo
MicroLinnPull microLinnPull[MICROLINN_MAX_EDOSTEPS];

byte microLinnImportType = 0;                                   // 0 means no import is currently happening
short microLinnImportSize = 0;
short microLinnImportCounter = 0;
boolean microLinnImportXen = true;                              // don't import xen data for most presets if rawEDO == 4
boolean microLinnImportingON = true;   // CHANGE TO FALSE LATER
unsigned long microLinnLastMomentMidiPP;                        // for timeout calculations

unsigned long MLcalcTuningTimer;  // delete later
int MLdebug[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
boolean is(byte array[], short n) {                         // read from the nth boolean of a packed array of booleans
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

/************** lookup functions ************************/

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

boolean microLinnIsScaleStep(byte edo, short edostep) {
  return microLinnIsScaleStep(edo, edostep, Global.activeNotes);
}

boolean microLinnIsScaleStep(byte edo, short edostep, byte currScale) {
  if (currScale == 7) return true;    // full rainbow
  if (currScale  > 7) return false;   // custom LED pattern
  return Device.microLinn.scales[microLinnTriIndex(edo, edostep)] & (1 << currScale);
}

byte microLinnGetFret(byte side, byte col) {
// returns the distance from the "nut" of the dot pattern to the column, octave-reduced
  short fret = (isLeftHandedSplit(LEFT) && isLeftHandedSplit(RIGHT)) ? NUMCOLS - col : col;
  fret -= MICROLINN_MAJ_2ND_TR[edo] * Split[side].transposeLights + Split[side].microLinn.transposeEDOlights;
  byte modBase = (edo == 41 ? 12 : edo);                          // because 41edo kites repeat every 12 frets
  return microLinnMod (fret - 1, modBase);
}

boolean isMicroLinnOn() {
  return Global.microLinn.EDO >= 5;
}

boolean isMicroLinnImportingOn() {
  return microLinnImportingON;
}

boolean isMicroLinnColOffset(byte side) {
  return Split[side].microLinn.colOffset != 1;
}

boolean isMicroLinnNoOverlap() {                       // says whether to call calcTuning on a split point change
  return microLinnRowOffset[LEFT] == 26 ||    
         microLinnRowOffset[RIGHT] == 26;
}

boolean isMicroLinnDrumPadMode() {
  return microLinnDrumPadMode;
}

boolean isMicroLinnUninstallTrue() {
  return microLinnUninstall;
}

void toggleMicroLinnUninstall() {
  microLinnUninstall = !microLinnUninstall;
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
signed char getMicroLinnMidiNote(byte side, signed char note, byte group) {
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
short getMicroLinnTuningBend(byte side, signed char note, byte group) {
  if (note < 0) return 0;
  return microLinnTuningBend[side][note + (group << 7)];
}
short getMicroLinnTuningBend2(byte side, signed char note, byte group) {
  // first draft of implementing sweetening
  if (note < 0) return 0;
  short edostep = note + (group << 7);
  short bend = microLinnTuningBend[side][edostep];
  if (edo == 41) {
    if (microLinnPull[edostep].sharp && !microLinnPull[edostep].flat) bend += microLinnSweetener[side];
    if (microLinnPull[edostep].flat && !microLinnPull[edostep].sharp) bend -= microLinnSweetener[side];
  }
  return bend;
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
  if (offset <= -12) return 18 + (row - 7) * offset;      // ensure upper left cell is 18 = F#-1 (middle-C is C3)
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
  return ceil(600.0 / edo);                               // larger edos have a smaller bend range
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
  Device.microLinn.MLversion = 0;
  memcpy (&Device.microLinn.scales, &MICROLINN_SCALES, MICROLINN_ARRAY_SIZE);
  memcpy (&Device.microLinn.rainbows, &MICROLINN_RAINBOWS, MICROLINN_ARRAY_SIZE);
  for (byte EDO = 5; EDO <= MICROLINN_MAX_EDO; ++EDO) microLinnResetFretboard(EDO);

  //Global.microLinn.drumPadMode = false;
  //Global.microLinn.locatorCC1 = -1;
  //Global.microLinn.locatorCC2 = -1;
  Global.microLinn.EDO = 4;                       // 4 = OFF
  Global.microLinn.useRainbow = true;
  //memset(&Global.microLinn.rainbow,   0, MICROLINN_MAX_EDO);
  //memset(&Global.microLinn.fretboard, 0, MICROLINN_MAX_EDO);
  Global.microLinn.guitarTuning[0] = 0;           // 12edo F# B E A D G B E
  Global.microLinn.guitarTuning[1] = 5;           // row offset from the string below it
  Global.microLinn.guitarTuning[2] = 5;
  Global.microLinn.guitarTuning[3] = 5;
  Global.microLinn.guitarTuning[4] = 5;
  Global.microLinn.guitarTuning[5] = 5;
  Global.microLinn.guitarTuning[6] = 4;
  Global.microLinn.guitarTuning[7] = 5;
  Global.microLinn.anchorRow = 4;                 // 4th row from the top
  Global.microLinn.anchorCol = 11;
  Global.microLinn.anchorNote = 60;               // middle-C
  Global.microLinn.anchorCents = 0;
  //Global.microLinn.equaveSemitones = 12;
  Global.microLinn.octaveStretch = 0;
  Global.microLinn.sweeten = 20;                    // 20 = 2¢
  //Global.microLinn.largeEDO = 55;
  //memset(largeEdoScale, 0, sizeof(largeEdoScale));

  for (byte side = 0; side < NUMSPLITS; ++side) {
    Split[side].microLinn.colOffset = 1;
    //Split[side].microLinn.rowOffset = -26;                 // -26 = OFF
    //Split[side].microLinn.collapseBendPerPad = 0;
    Split[side].microLinn.hammerOnWindow = 0;              // 150¢ is a good default
    Split[side].microLinn.hammerOnNewNoteOn = true;
    Split[side].microLinn.pullOffVelocity = 0;
    //Split[side].microLinn.showCustomLEDs = 0;
    Split[side].microLinn.transposeEDOsteps = 0;
    Split[side].microLinn.transposeEDOlights = 0;
    Split[side].microLinn.tuningTable = 0;
    microLinnRowOffset[side] = -26;
  }

  for (byte p = 0; p < NUMPRESETS; ++p) {
    memcpy (&config.preset[p].global.microLinn, &Global.microLinn, sizeof(MicroLinnGlobal));
    memcpy (&config.preset[p].split[LEFT].microLinn,  &Split[LEFT].microLinn,  sizeof(MicroLinnSplit));
    memcpy (&config.preset[p].split[RIGHT].microLinn, &Split[RIGHT].microLinn, sizeof(MicroLinnSplit));
  }

  // preset 5 (5th from the top) is 41edo Kite guitar (the presets run 3 2 1 0 5 4)
  for (byte side = 0; side < NUMSPLITS; ++side) {
    config.preset[5].split[side].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
    config.preset[5].split[side].bendRangeOption = bendRange24;
    config.preset[5].split[side].customBendRange = 24;
    config.preset[5].split[side].expressionForZ = loudnessChannelPressure;
  }
  config.preset[5].split[LEFT].midiChanMain = 1;
  config.preset[5].split[LEFT].midiChanSet[0] = false;
  config.preset[5].split[RIGHT].midiChanMain = 16;
  config.preset[5].split[RIGHT].midiChanSet[15] = false;
  config.preset[5].global.microLinn.EDO = 41;
  //short i = microLinnTriIndex(41, 0); 
  //memcpy(&Global.microLinn.rainbow,   &Device.microLinn.rainbows[i],   41); 
  //memcpy(&Global.microLinn.fretboard, &Device.microLinn.fretboards[i], 41); 
  config.preset[5].global.microLinn.guitarTuning[1] = 13;         // alternating downmajor and upminor 3rds
  config.preset[5].global.microLinn.guitarTuning[2] = 11;         // row offset from the string below it
  config.preset[5].global.microLinn.guitarTuning[3] = 13;
  config.preset[5].global.microLinn.guitarTuning[4] = 11;
  config.preset[5].global.microLinn.guitarTuning[5] = 13;
  config.preset[5].global.microLinn.guitarTuning[6] = 11;
  config.preset[5].global.microLinn.guitarTuning[7] = 13;
  config.preset[5].global.microLinn.anchorRow = 3;                // 5th row from top, places the two rainbows nicely
  config.preset[5].global.microLinn.anchorCol = 6;
  config.preset[5].global.microLinn.anchorNote = 54;              // ^F#2, Kite guitar standard tuning with D = 0 cents
  config.preset[5].global.microLinn.anchorCents = 39;             // ^M3 = 439c
  config.preset[5].global.microLinn.sweeten = 20;
  config.preset[5].global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
  config.preset[5].global.customRowOffset = 13;                   // 41-equal downmajor 3rds
  config.preset[5].split[LEFT].microLinn.colOffset = 2;
  config.preset[5].split[RIGHT].microLinn.colOffset = 2;
  config.preset[5].global.activeNotes = 6;
  config.preset[5].split[LEFT].playedTouchMode = playedBlink;
  config.preset[5].split[RIGHT].playedTouchMode = playedBlink;

  /************** uncomment once there are more presets *******************
  // preset 5 (5th from the top) is 31edo Bosanquet
  for (byte side = 0; side < NUMSPLITS; ++side) {
    config.preset[5].split[side].midiMode = channelPerNote;        // overwrite the 1-channel defaults with MPE settings
    config.preset[5].split[side].bendRangeOption = bendRange24;
    config.preset[5].split[side].customBendRange = 24;
    config.preset[5].split[side].expressionForZ = loudnessChannelPressure;
  }
  config.preset[5].split[LEFT].midiChanMain = 1;
  config.preset[5].split[LEFT].midiChanSet[0] = false;
  config.preset[5].split[RIGHT].midiChanMain = 16;
  config.preset[5].split[RIGHT].midiChanSet[15] = false;
  config.preset[5].global.microLinn.EDO = 31;
  //short i = microLinnTriIndex(31, 0); 
  //memcpy(&Global.microLinn.rainbow,   &Device.microLinn.rainbows[i], 31); 
  //memcpy(&Global.microLinn.fretboard, &Device.microLinn.fretboards[i], 31);
  config.preset[5].global.microLinn.guitarTuning[1] = 13;         // F# B E A D G B E
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
  config.preset[5].split[LEFT].playedTouchMode = playedBlink;
  config.preset[5].split[RIGHT].playedTouchMode = playedBlink;
  *********************************************/

  microLinnSetupKitesPersonalPrefs();
  setupMicroLinn();
}

void storeMicroLinnRainbowAndFretboard() {         // called from storeSettingsToPreset()
  byte rawEDO = Global.microLinn.EDO;
  if (rawEDO == 4) return;
  //short i = microLinnTriIndex(rawEDO, 0);  
  //memcpy(&Global.microLinn.rainbow, &Device.microLinn.rainbows[i], rawEDO);
  //memcpy(&Global.microLinn.fretboard, &Device.microLinn.fretboards[i], rawEDO); 
}

void loadMicroLinnRainbowAndFretboard() {          // called from loadSettingsFromPreset()
  byte rawEDO = Global.microLinn.EDO;
  if (rawEDO == 4) return;
  //short i = microLinnTriIndex(rawEDO, 0);
  //memcpy(&Device.microLinn.rainbows[i], &Global.microLinn.rainbow, rawEDO);
  //memcpy(&Device.microLinn.fretboards[i], &Global.microLinn.fretboard, rawEDO); 
}

void microLinnSetupKitesPersonalPrefs() {       // speed up debugging cycle, delete later once things are more stable
  Split[LEFT].midiMode = Split[RIGHT].midiMode = channelPerNote;
  Split[LEFT].bendRangeOption = Split[RIGHT].bendRangeOption = bendRange24; 
  Split[LEFT].pitchCorrectHold = Split[RIGHT].pitchCorrectHold = pitchCorrectHoldOff;
  Split[LEFT].expressionForZ = Split[RIGHT].expressionForZ = loudnessChannelPressure;
  Split[LEFT].playedTouchMode = Split[RIGHT].playedTouchMode = playedSame;
  Split[RIGHT].colorMain = COLOR_YELLOW;
  Split[RIGHT].colorLowRow = Split[LEFT].colorLowRow = COLOR_ORANGE;
  Global.activeNotes = 7;              // full rainbow
  if (Global.microLinn.EDO == 4) {
    Global.rowOffset = 4;}             // 12edo fretboard in 3rds
  if (Global.microLinn.EDO == 41) {
    Global.activeNotes = 8; customLedPatternActive = true;}                            // 41edo fretboard
  Global.setSwitchAssignment(3, ASSIGNED_MICROLINN_EDO_UP,   false);  // for debugging, delete later
  Global.setSwitchAssignment(2, ASSIGNED_MICROLINN_EDO_DOWN, false);  // for debugging
  Device.operatingLowPower = 2;
}

void setupMicroLinn() {                              // runs when the Linnstrument powers up or a preset is loaded
  edo = Global.microLinn.EDO;
  if (edo == 4) edo = 12;                            // because "OFF" is 4edo which is really 12edo
  microLinnPrevScale = Global.activeNotes;           // to disable backtracking
  if (isMicroLinnOn()) lightSettings = LIGHTS_ACTIVE;
  microLinnDoublePerSplitColOffset = Split[LEFT].microLinn.colOffset == Split[RIGHT].microLinn.colOffset;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    microLinnCalcHammerOnEdosteps(side);
    microLinnStoreColOffsetCents(side);
    microLinnStoreRowOffsetCents(side);
  }
  storeMicroLinnGlobalRowOffsetCents();
  for (byte row = 1; row < NUMROWS; ++row) {
    microLinnStoreGuitarRowOffsetCents(row);
  }
  microLinnCalcGuitarTuning();
  microLinnSemitonesPerPad[LEFT]  = Split[LEFT].microLinn.colOffset;     // assumes 12edo, if not calcTuning will overwrite this
  microLinnSemitonesPerPad[RIGHT] = Split[RIGHT].microLinn.colOffset;
  microLinnImportType = 0;                                   // 0 means no import is currently happening
  microLinnUpdateAnchorString();
  calcMicroLinnTuning();
  updateDisplay();
}

void microLinnCalcHammerOnEdosteps(byte side) {
  microLinnHammerOnEdosteps[side] = floor((edo * Split[side].microLinn.hammerOnWindow) / 120.0);       // hammerOnWindow is dimes not cents
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

// called from handleGlobalSettingNewTouch(), handleGlobalSettingHold(), 
// handleGlobalSettingRelease() and handleRowOffsetNewTouch()
void storeMicroLinnGlobalRowOffsetCents() {      
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

void microLinnStoreGuitarRowOffsetCents(byte row) {
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

short microLinnCheckRowOffset(short rowOffset, short cents, boolean isGuitarTuning) {
  // ensure coprime row/col offsets so that all notes of the edo are present
  // e.g. 12edo +5 +2 --> 24edo +10 +4 --> 24edo +9 +4
  // ideally if the col offset were bigger than the row offset, and non-guitar tuning, the col offset would get adjusted
  // but it might be hard to be coprime with all 5 kinds of row offsets
  short colOffsetLCM = microLinnLCM (Split[LEFT].microLinn.colOffset, Split[RIGHT].microLinn.colOffset); 
  if (colOffsetLCM == 0) return rowOffset;                       // if both col offsets are 0, do nothing
  short zigzagBy = (rowOffset * 1200 > cents * edo) ? -1 : 1;    // this is really (rowOffset/edo > cents/1200)
  short maxOffset = (isGuitarTuning ? MICROLINN_MAX_GUITAR_RANGE : 25);
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
    if (microLinnLayout[side] > 0) continue;
    newOffset = round ((edo * microLinnColOffsetCents[side]) / 1200.0);
    newOffset = constrain (newOffset, 1, 8);
    Split[side].microLinn.colOffset = newOffset;
  }

  // adjust the 2 per-split row offsets, unless they are OFF or NOVR
  for (byte side = 0; side < NUMSPLITS; ++side) {
    if (microLinnRowOffset[side] == -26 || microLinnRowOffset[side] == 26) continue;
    if (microLinnLayout[side] > 0) continue;
    newOffset = round ((edo * microLinnRowOffsetCents[side]) / 1200.0);
    newOffset = constrain (newOffset, -25, 25);
    if (newOffset == 0) {
      newOffset = microLinnSign(microLinnRowOffset[side]);
    }
    newOffset = microLinnCheckRowOffset(newOffset, microLinnRowOffsetCents[side], false);
    microLinnRowOffset[side] = newOffset;
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
    // row = 1 because microLinnGuitarTuningCents[0] is unused
    for (byte row = 1; row < NUMROWS; ++row) {
      newOffset = round ((edo * microLinnGuitarTuningCents[row]) / 1200.0);
      if (newOffset == 0) {
        newOffset = microLinnSign(Global.microLinn.guitarTuning[row]);
      }
      newOffset = microLinnCheckRowOffset(newOffset, microLinnGuitarTuningCents[row], true);
      Global.microLinn.guitarTuning[row] = newOffset;
    }
    microLinnCheckGuitarTuning();
  }

  // adjust the global row offset
  // if the global row offset becomes > 7 (e.g. 12edo +5 --> 22edo +9), it's stored as a custom row offset
  // but if the user later switches to a smaller edo and the row offset becomes <= 7 (e.g. 22edo +9 --> 17edo +7),
  // the row offset is stored as the global row offset, and the custom row offset becomes an octave
  newOffset = round ((edo * microLinnGlobalRowOffsetCents) / 1200.0);
  newOffset = constrain (newOffset, -25, 25);
  if (newOffset == 0) {
    newOffset = microLinnSign(microLinnGlobalRowOffsetCents);
  }
  newOffset = microLinnCheckRowOffset(newOffset, microLinnGlobalRowOffsetCents, false);
  if (Global.rowOffset == ROWOFFSET_ZERO ||
      Global.rowOffset == ROWOFFSET_GUITAR ||
      Global.rowOffset == ROWOFFSET_NOOVERLAP) {
    Global.customRowOffset = newOffset;
  } else if (newOffset >= 3 && newOffset <= 7) {
    Global.rowOffset = newOffset;
    Global.customRowOffset = (edo < 12 ? edo : edo - MICROLINN_PERF_4TH[edo]);
  } else {
    Global.rowOffset = ROWOFFSET_OCTAVECUSTOM;
    Global.customRowOffset = newOffset;
  }

  // apply the default layout, if any
  for (byte side = 0; side < NUMSPLITS; ++side) {
    signed char row, col;
    switch (microLinnLayout[side]) {
      case 1: case 2: {row = MICROLINN_MIN_2ND[edo];  col = MICROLINN_MAJ_2ND[edo]; break;}  // Bosanquet
      case 3: case 4: {row = MICROLINN_PERF_4TH[edo]; col = MICROLINN_MAJ_2ND[edo]; break;}  // Wicki-Hayden
      case 5: case 6: {row = MICROLINN_MIN_3RD[edo];  col = MICROLINN_MAJ_3RD[edo]; break;}  // Harmonic Table
      default: continue;
    }
    if (microLinnLayout[side] == 2) {row = abs(row - col);}         // translate hexagonal to square the other way,
    else if (microLinnLayout[side] % 2 == 0) {row += col;}          // by shifting the next row sideways one square
    microLinnRowOffset[side] = row;
    Split[side].microLinn.colOffset = col;
  };  
}

/************** sweetening functions ************************/

// not yet called anywhere, will be called on note-ons
// ratios to widen: 5/4 5/3 5/2 10/3 5/1 20/3 10/1     (maybe 15/8 15/4 15/2?)  (maybe 10/7?)
// ratios to narrow: 6/5 8/5 12/5 16/5 24/5 32/5 48/5  (maybe 9/5 18/5 36/5?)   (maybe 7/5?)
void sweetenMicroLinn41(short newNote) {
  if (edo != 41) return;
  microLinnPull[newNote].sharp = false;
  microLinnPull[newNote].flat = false;
  for (byte side = 0; side < NUMSPLITS; ++side) {
    for (byte ch = 0; ch < 16; ++ch) {
      if (noteTouchMapping[side].musicalTouchCount[ch] != 1) continue;
      byte col = focusCell[side][ch].col;
      byte row = focusCell[side][ch].row;
      short edostep = getMicroLinnVirtualEdostep(side, col, row);
      switch (microLinnMod(edostep - newNote, 41)) {
        case 13:    // 13\41 = vM3 = 5/4
        case 30:    // 30\41 = vM6 = 5/3
        {
          microLinnPull[edostep].sharp = true;
          microLinnPull[newNote].flat = true;
          break;
        }
        case 11:     // 11\41 = ^m3 = 6/5
        case 28:     // 28\41 = ^m6 = 8/5
        {
          microLinnPull[edostep].flat = true;
          microLinnPull[newNote].sharp = true;
          break;
        }
      }
    }
  }
}

/************** calcTuning functions ************************/

// calc the cumulative row offsets by summing the individual row offsets, to speed up calcTuning()
// if Global.microLinn.guitarTuning = [0 7 5 5 5' 5 4 5] then microLinnGuitarTuning = [-22 -15 -10 -5 0 5 9 14]
// [0 7 5 5 5 5 4 5] -> [0 7 12 17 22 27 31 36]
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
  byte lowestRow, lowestCol, highestRow, highestEdostep;
  if (microLinnRowOffset[side] >= 0) {                        // use this split's row offset if positive or NOVR
    lowestRow = 0;
  } else if (microLinnRowOffset[side] >= -25) {               // or if negative but not OFF
    lowestRow = 7;
  } else if (Global.rowOffset == ROWOFFSET_OCTAVECUSTOM) {
    lowestRow = Global.customRowOffset >= 0 ? 0 : 7;
  } else lowestRow = 0;

  if (Global.rowOffset == ROWOFFSET_GUITAR && microLinnRowOffset[side] == -26) {
    microLinnLowestEdostep[side] = microLinnLowestGuitarNote;
  } else {
    microLinnLowestEdostep[side] = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, lowestRow);
  }
  lowestCol = isLeftHandedSplit(side) ? NUMCOLS - 1 : 1;
  microLinnLowestEdostep[side] -= Split[side].microLinn.colOffset * abs(Global.microLinn.anchorCol - lowestCol);
  microLinnLowestEdostep[side] += microLinnTransposition(side);

  // calc microLinnNumGroups for rechannelling
  if (Split[side].microLinn.tuningTable < 2) return;
  if (Global.rowOffset == ROWOFFSET_GUITAR && microLinnRowOffset[side] == -26) {
    highestEdostep = microLinnHighestGuitarNote;
  } else {
    highestRow = 7 - lowestRow;
    highestEdostep = microLinnSumOfRowOffsets(side, Global.microLinn.anchorRow, highestRow);
  }
  highestEdostep += Split[side].microLinn.colOffset * abs((NUMCOLS - lowestCol) - Global.microLinn.anchorCol);
  highestEdostep += microLinnTransposition(side);
  microLinnNumGroups[side] = ((highestEdostep - microLinnLowestEdostep[side]) >> 7) + 1;

  // calc microLinnChannelCount for rechannelling
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
        + Split[side].transposePitch  * MICROLINN_MAJ_2ND_TR[edo]          // semitones are interpreted as whole tones
        - Split[side].transposeLights * MICROLINN_MAJ_2ND_TR[edo]
        + Split[side].microLinn.transposeEDOsteps
        - Split[side].microLinn.transposeEDOlights;
}

void calcMicroLinnTuning() { 
  // calcs microLinnSemitonesPerPad, microLinnEdostep, microLinnMidiNote and microLinnTuningBend
  // called when any column 0 button is released and the user returns to normal playing mode
  // also called when entering microLinnConfig note lights display or leaving anchor cell display
  // also called for octave up/down or EDO+/EDO-/EDO± via switch1/switch2 control buttons or footswitch presses
  // needed for any change to row offsets, col offsets, transpose, bendrange or anything on microLinnConfig
  // also needed for changes to the split point if in NO_OVERLAP mode

  if (!isMicroLinnOn()) return;
midiSendControlChange(41, 41, 1, true);    // for debugging, delete later
MLcalcTuningTimer = micros();
  microLinnCalcLowestEdostepEtc(LEFT);
  microLinnCalcLowestEdostepEtc(RIGHT);
  if (microLinnCollapseBendPerPad > 0) {
    microLinnCollapsedTuning();
    return;
  }

  byte anchorCol = Global.microLinn.anchorCol;
  byte anchorRow = Global.microLinn.anchorRow;
  // anchorPitch = a standard 12edo midi note, but with 2 decimal places for anchorCents
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;
  // edostepSize = size of 1 edostep in 12edo semitones, e.g. 1\41 is 0.2927 semitones
  float edostepSize = (1200 + Global.microLinn.octaveStretch) / (100.0 * edo);
  short distance, virtualEdostep, roundNote, fineTune; float note;

  memset (&microLinnMidiNote, -1, sizeof(microLinnMidiNote));                       // -1 = this edostep is not used
  memset (&microLinnTuningBend, 0, sizeof(microLinnTuningBend));    

  for (byte side = 0; side < NUMSPLITS; ++side) {
    short colOffset = Split[side].microLinn.colOffset;
    microLinnSemitonesPerPad[side] = colOffset * edostepSize;                       // e.g. kite tuning is 2\41 = 0.5854 semitones
    if (isLeftHandedSplit(side)) colOffset *= -1;
    float semitonesPerPad = colOffset * edostepSize;                                // microLinnSemitonesPerPad is used later, semitonesPerPad is used here
    short transpose = microLinnTransposition(side);
    float bendRange = 8192.0F / getBendRange(side);                                 // invert to avoid dividing 400 times
    microLinnSweetener[side] = round(Global.microLinn.sweeten * bendRange / 1000);  // 20 = 2¢ = 20 thousandths of a semitone
    for (byte row = 0; row < NUMROWS; ++row) {
      distance = microLinnSumOfRowOffsets(side, anchorRow, row);                    // distance in edosteps between any 2 rows
      distance += transpose - anchorCol * colOffset;                                // distance from the anchor cell, initialized to col 0
      note = anchorPitch + distance * edostepSize;                                  // convert distance from edosteps to a fractional 12edo midi note
      for (byte col = 1; col < NUMCOLS; ++col) {
        distance += colOffset;                                                      // increment rather than recalc each time, for speed
        microLinnEdostep[side][col][row] = distance;                                // the anchor cell is always edostep 0, unless transposed
        note += semitonesPerPad;                                                    // increment rather than recalc
        if (note >= 0 && note <= 127) {                                             // if not, midiNote will be -1 due to the earlier memsets
          virtualEdostep = distance - microLinnLowestEdostep[side];                 // virtual edosteps are always >= 0
          microLinnMidiNote[side][virtualEdostep] = roundNote = round(note);
          fineTune = round((note - roundNote) * bendRange);                         // fine-tuning bend as a zero-centered signed 13-bit integer
          microLinnTuningBend[side][virtualEdostep] = fineTune;
        }
      }
    }
  }
MLcalcTuningTimer = calcTimeDelta(MLcalcTuningTimer, micros()) / 1000;
}

void microLinnCollapsedTuning() { 

  microLinnSetupCollapsedScale();

  byte anchorCol = Global.microLinn.anchorCol;
  // anchorPitch = a standard 12edo midi note, but with 2 decimal places for anchorCents
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;
  // edostepSize = size of 1 edostep in 12edo semitones, e.g. 1\41 is 0.2927 semitones
  float edostepSize = (1200 + Global.microLinn.octaveStretch) / (100.0 * edo);
  short distance, virtualEdostep, roundNote, fineTune; float note;

  memset (&microLinnMidiNote, -1, sizeof(microLinnMidiNote));                       // -1 = this edostep is not used
  memset (&microLinnTuningBend, 0, sizeof(microLinnTuningBend));    
  float bendPerPad = microLinnCollapseBendPerPad;
  if (microLinnCollapseBendPerPad == microLinnLargestStep + 1) {
    bendPerPad = edo / microLinnNumNotes;
  }

  for (byte side = 0; side < NUMSPLITS; ++side) {
    short colOffset = Split[side].microLinn.colOffset;
    microLinnSemitonesPerPad[side] = colOffset * edostepSize * bendPerPad;
    if (isLeftHandedSplit(side)) colOffset *= -1;
    float semitonesPerPad = colOffset * edostepSize * bendPerPad;                   // microLinnSemitonesPerPad is used later, semitonesPerPad is used here
    short transpose = microLinnTransposition(side);
    float bendRange = 8192.0F / getBendRange(side);                                 // invert to avoid dividing 400 times
    microLinnSweetener[side] = round(Global.microLinn.sweeten * bendRange / 1000);  // 20 = 2¢ = 20 thousandths of a semitone
    for (byte row = 0; row < NUMROWS; ++row) {
      distance = microLinnEdostep[side][anchorCol][row];
      distance += transpose - anchorCol * colOffset;                                // distance from the anchor cell, initialized to col 0
      note = anchorPitch + distance * edostepSize;                                  // convert distance from edosteps to a fractional 12edo midi note
      for (byte col = 1; col < NUMCOLS; ++col) {
        distance += colOffset;                                                      // increment rather than recalc each time, for speed
        microLinnEdostep[side][col][row] = distance;                                // the anchor cell is always edostep 0, unless transposed
        note += semitonesPerPad;                                                    // increment rather than recalc
        if (note >= 0 && note <= 127) {                                             // if not, midiNote will be -1 due to the earlier memsets
          virtualEdostep = distance - microLinnLowestEdostep[side];                 // virtual edosteps are always >= 0
          microLinnMidiNote[side][virtualEdostep] = roundNote = round(note);
          fineTune = round((note - roundNote) * bendRange);                         // fine-tuning bend as a zero-centered signed 13-bit integer
          microLinnTuningBend[side][virtualEdostep] = fineTune;
        }
      }
    }
  }
}

/************** collapsing functions ************************/

void microLinnSetupCollapsedScale() {
  // calc microLinnCollapsedScale = (0 2 4 5 7 9 11 12 0 0 0 0) for a 12edo major scale
  byte currScale = Global.activeNotes;
  if (currScale >= 7) currScale = microLinnPrevScale;
  if (currScale >= 7) currScale = 0;  
  memset (&microLinnCollapsedScale, 0, sizeof(microLinnCollapsedScale));
  microLinnNumNotes = 1;                                               // tonic is always part of the scale       
  microLinnLargestStep = 1;
  for (byte edostep = 1; edostep <= edo; ++edostep) {                  // <= edo to ensure octave is always in the scale
    if (microLinnIsScaleStep(edo, edostep, currScale)) {
      microLinnCollapsedScale[microLinnNumNotes] = edostep;
      byte currStep = edostep - microLinnCollapsedScale[microLinnNumNotes - 1];
      if (microLinnLargestStep < currStep) microLinnLargestStep = currStep;
      microLinnNumNotes += 1;
    }
  }
  microLinnNumNotes -= 1;                                              // don't count the octave

  // calc microLinnEdostep[side][0][row] cumulatively from the anchor row
  // (-10 -7 -3 0 4 7 11 14) for 7 notes and an offset of 6\12 
  //  (-9 -6 -3 0 3 6 9 12)  for 7 notes and an offset of 5\12 
  byte anchorColDistance = microLinnScaleStepsToEdoSteps(-Global.microLinn.anchorCol); 
  byte anchorRow = Global.microLinn.anchorRow;
  for (byte side = 0; side < NUMSPLITS; ++side) {

    for (byte row = 0; row < MAXROWS; ++row) {
      short rowOffset = microLinnSumOfRowOffsets(side, anchorRow, row);
      byte edostep = microLinnMod(rowOffset, edo);
      if (!microLinnIsScaleStep(edo, edostep, currScale)) {
        byte i = 0;
        while (microLinnCollapsedScale[i] < edostep) {i += 1;}
        byte overshoot = microLinnCollapsedScale[i] - edostep;
        byte undershoot = edostep - microLinnCollapsedScale[i-1];
        if (overshoot <= undershoot) {
          rowOffset += overshoot;
        } else {
          rowOffset -= undershoot;
        }
      }
      microLinnEdostep[side][0][row] = rowOffset + anchorColDistance;
    }
  }

  /****  delete later  // calc microLinnEdostep for the anchor column from cumulative row offsets
  for (byte side = 0; side < NUMSPLITS; ++side) {
    for (byte row = 0; row < MAXROWS; ++row) {
      short rowOffset = microLinnSumOfRowOffsets(side, anchorRow, row);
      microLinnEdostep[side][anchorCol][row] = round(1.0 * microLinnNumNotes * rowOffset / edo);
    }
  }****/
}

short microLinnScaleStepsToEdoSteps(short scaleSteps) {
  // convert 11 scalesteps to 7 + 4 = octave + 5th = 12 + 7 = 19 edosteps
  byte n = microLinnMod(scaleSteps, microLinnNumNotes);
  return edo * (scaleSteps - n) / microLinnNumNotes + microLinnCollapsedScale[n];
}

/************** midi send/receive functions ************************/

// used for guitar tuning, anchor note, anchor cents and note lights
void microLinnSendPreviewNote (short edostep) {                                                // edosteps from the anchor cell
  float anchorPitch = Global.microLinn.anchorNote + Global.microLinn.anchorCents / 100.0;      // midi note with 2 decimal places for cents
  float edostepSize = (1200 + Global.microLinn.octaveStretch) / (100.0 * edo);                 // edo's step size in 12edo semitones
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

// indicates pad's row/column, sent before every played (not arpeggiated, strummed or sequenced) note-on
// called from sendNewNote() in ls_handleTouches.ino, thanks to KVR forum member vorp40 for the idea!
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
  //case 1001: value = Split[side].microLinn.rowOffset + 26; break;
  //case 1002: value = Split[side].microLinn.collapseBendPerPad; break;
    case 1003: value = Split[side].microLinn.hammerOnWindow; break;
    case 1004: value = (Split[side].microLinn.hammerOnNewNoteOn ? 1 : 0); break;
    case 1005: value = Split[side].microLinn.pullOffVelocity; break;
  //case 1006: value = Split[side].microLinn.showCustomLEDs; break;
    case 1050: value = Split[side].microLinn.transposeEDOsteps + 7; break;
    case 1051: value = Split[side].microLinn.transposeEDOlights + 7; break;
  //case 1052: value = Split[side].microLinn.layout[side]; break;
    case 1053: value = Split[side].microLinn.tuningTable; break;
  //case 1200: value = (Global.microLinn.drumPadMode ? 1 : 0); break;
  //case 1201: value = Global.microLinn.locatorCC1 + 1; break;
  //case 1202: value = Global.microLinn.locatorCC2 + 1; break;
    case 1250: value = Global.microLinn.EDO; break;
    case 1251: value = (Global.microLinn.useRainbow ? 1 : 0); break;
    case 1252: value = Global.microLinn.guitarTuning[1] + n; break;
    case 1253: value = Global.microLinn.guitarTuning[2] + n; break;
    case 1254: value = Global.microLinn.guitarTuning[3] + n; break;
    case 1255: value = Global.microLinn.guitarTuning[4] + n; break;
    case 1256: value = Global.microLinn.guitarTuning[5] + n; break;
    case 1257: value = Global.microLinn.guitarTuning[6] + n; break;
    case 1258: value = Global.microLinn.guitarTuning[7] + n; break;
    case 1259: value = Global.microLinn.anchorCol; break;
    case 1260: value = Global.microLinn.anchorRow; break;
    case 1261: value = Global.microLinn.anchorNote; break;
    case 1262: value = Global.microLinn.anchorCents + 60; break;
  //case 1263: value = Global.microLinn.equaveSemitones; break;
  //case 1264: value = Global.microLinn.equaveCents + 60; break;
    case 1265: value = Global.microLinn.sweeten; break;
  //case 1266: value = Global.microLinn.largeEDO; break;
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
  switch (parameter) {
    case 1000: if (inRange(value, 0, 8))   Split[side].microLinn.colOffset = value; break;
  //case 1001: if (inRange(value, 0, 52))  Split[side].microLinn.rowOffset = value - 26; break;
  //case 1002: if (inRange(value, 0, 55))  Split[side].microLinn.collapseBendPerPad = value; break;
    case 1003: if (inRange(value, 0, 240)) Split[side].microLinn.hammerOnWindow = value; break;
    case 1004: if (inRange(value, 0, 1))   Split[side].microLinn.hammerOnNewNoteOn = (value == 1); break;
    case 1005: if (inRange(value, 0, 3))   Split[side].microLinn.pullOffVelocity = value; break;
  //case 1006: if (inRange(value, 0, 6))   Split[side].microLinn.showCustomLEDs = value; break;
    case 1050: if (inRange(value, 0, 14))  Split[side].microLinn.transposeEDOsteps = value - 7; break;
    case 1051: if (inRange(value, 0, 14))  Split[side].microLinn.transposeEDOlights = value - 7; break;
  //case 1052:  if (inRange(value, 0,6))   Split[side].microLinn.layout = value; break;
    case 1053: if (inRange(value, 0, 2))   Split[side].microLinn.tuningTable = value; break;
  //case 1200: if (inRange(value, 0, 1))   Global.microLinn.drumPadMode = (value == 1); break;
  //case 1201: if (inRange(value, 0, 120)) Global.microLinn.locatorCC1 = value - 1; break;
  //case 1202: if (inRange(value, 0, 120)) Global.microLinn.locatorCC2 = value - 1; break;
    case 1250: if (inRange(value, 4, 55))  Global.microLinn.EDO = value; break;
    case 1251: if (inRange(value, 0, 1))   Global.microLinn.useRainbow = (value == 1); break;
    case 1252: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[1] = value - n; break;
    case 1253: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[2] = value - n; break;
    case 1254: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[3] = value - n; break;
    case 1255: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[4] = value - n; break;
    case 1256: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[5] = value - n; break;
    case 1257: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[6] = value - n; break;
    case 1258: if (inRange(value, 0, 2*n)) Global.microLinn.guitarTuning[7] = value - n; break;
    case 1259: if (inRange(value, 1, 25))  Global.microLinn.anchorCol = value; break;
    case 1260: if (inRange(value, 0, 8))   Global.microLinn.anchorRow = value; break;
    case 1261: if (inRange(value, 0, 127)) Global.microLinn.anchorNote = value; break;
    case 1262: if (inRange(value, 0, 121)) Global.microLinn.anchorCents = value - 60; break;
  //case 1263: if (inRange(value, 1, 48))  Global.microLinn.equaveSemitones = value; break;
  //case 1264: if (inRange(value, 0, 121)) Global.microLinn.equaveCents = value - 60; break;
    case 1265: if (inRange(value, 0, 60))  Global.microLinn.sweeten = value; break;
  //case 1266: if (inRange(value, 55, 312)) Global.microLinn.largeEDO = value; break;
    case 1300: setupMicroLinn();
  }
  updateDisplay();
}

// called when nrpn 299 with a value > 2048 is received, uses polypressure not sysex because not all DAWs support sysexes,
// also there's not enough RAM to hold the larger imports in memory before overwriting user settings,
// so we must overwrite as soon as we receive each byte, so the sysex's CRC would be useless anyway
void exportMicroLinnData(int exportType) {
  exportType -= 2048;
  if (exportType < 1 || exportType > 16)                      {microLinnScrollSmall("INVALID EXPORT TYPE"); return;}
  if (exportType == 1 && Global.activeNotes < 9)              {microLinnScrollSmall("FIRST SELECT A LIGHT PATTERN"); return;}
  if (exportType == 3 && audienceMessageToEdit == -1)         {microLinnScrollSmall("FIRST EDIT AN AUDIENCE MESSAGE"); return;}
  if (exportType >= 5 && exportType <= 8 && !isMicroLinnOn()) {microLinnScrollSmall("FIRST SELECT AN EDO"); return;}
  if (exportType == 13 && Device.lastLoadedPreset == -1)      {microLinnScrollSmall("FIRST LOAD A MEMORY"); return;}

  byte midiData = (exportType >= 5 && exportType <= 8) ? edo : 0;      // the edo says where to load import types 5-8
  midiSendNRPN(300, (exportType << 7) + midiData, 1);
  microLinnSendPolyPressure(Device.version, Device.microLinn.MLversion, 9);         // channel is 1-indexed
  
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
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.scales[n+i],
                                  Device.microLinn.scales[n+i+1]);
      }
      break;
    case 6:
      for (byte i = 0; i < edo; i += 2) {
        microLinnSendPolyPressure(Device.microLinn.rainbows[n+i],
                                  Device.microLinn.rainbows[n+i+1]);
      }
      break;
    case 7:
      for (byte i = 0; i < edo;i += 2) {
        microLinnSendPolyPressure(Device.microLinn.fretboards[n+i],
                                  Device.microLinn.fretboards[n+i+1]);
      }
      break;
    case 8:   // all edo data for the current edo only
      microLinnSendPolyPressure(Global.microLinn.EDO, 
                                Global.microLinn.useRainbow ? 1 : 0);
      for (byte i = 1; i < MAXROWS; i += 1) {                                     // [0] is unused
        microLinnSendPolyPressure(Global.microLinn.guitarTuning[i] + MICROLINN_MAX_GUITAR_RANGE);
      }
      microLinnSendPolyPressure(12, //Global.microLinn.equaveSemitones, 
                                Global.microLinn.octaveStretch + 60);  //Global.microLinn.equaveCents + 60);
      microLinnSendPolyPressure(Global.rowOffset, 
                                Global.customRowOffset + 25);
      for (byte side = 0; side < NUMSPLITS; ++side) {
        microLinnSendPolyPressure(Split[side].microLinn.colOffset,
                                  microLinnRowOffset[side] + 26);  //Split[side].microLinn.rowOffset) + 26;
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
      array = (byte *) &config.device.minUSBMIDIInterval;                // skip everything before minUSBMIDIInterval
      n = (byte *) &config.project - array;                              // stop short of the current sequencer project
      for (unsigned short i = 0; i < n; i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
      }
      break;
    case 16:   // send calibration data
      array = (byte *) &Device.calRows;
      n = sizeof(Device.calRows) + sizeof(Device.calCols) + sizeof(Device.calCrc) + 4;   
      for (short i = 0; i < n; i += 2) {
        microLinnSendPolyPressure(array[i], array[i+1]);
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

  if (!microLinnImportingON)                           {microLinnScrollSmall("FIRST TURN ON IMPORTING"); return;}
  if (importType < 1 || importType > 15)               {microLinnScrollSmall("IMPORT FAILURE INVALID IMPORT TYPE " + String(importType)); return;}
  if (importType == 1 && Global.activeNotes < 9)       {microLinnScrollSmall("FIRST SELECT A LIGHT PATTERN"); return;}
  if (importType == 3 && audienceMessageToEdit == -1)  {microLinnScrollSmall("FIRST EDIT AN AUDIENCE MESSAGE"); return;}
  if (importType >= 5 && importType <= 8) {
    if (EDO < 5 || EDO > MICROLINN_MAX_EDO)            {microLinnScrollSmall("IMPORT FAILURE INVALID EDO " + String(EDO)); return;}
    edo = Global.microLinn.EDO = EDO;                  // switch to the edo if it's valid
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
      microLinnImportSize = 16*30;
      break;
    case 5: case 6: case 7:  // scales/rainbow/fretboard for 1 edo
      microLinnImportSize = edo; 
      break;
    case 8:   // All edo data for the current edo only
      microLinnImportSize = 24 + 3 * edo + 3 * (edo % 2);             // 3 arrays are padded to double-bytes
      break;
    case 9: case 10: case 11:  // scales or rainbows or fretboards for all edos
      microLinnImportSize = MICROLINN_ARRAY_SIZE;
      break;
    case 12:  // All edo data for all edos
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
      arrayPtr = (byte *) &config.device.minUSBMIDIInterval;          // start with minUSBMIDIInterval
      microLinnImportSize = (byte *) &config.project - arrayPtr;      // stop short of the current sequencer project
      break;
    case 16:  // calibration data
      Device.calibrated = false;
      microLinnImportSize = sizeof(Device.calRows) + sizeof(Device.calCols) + sizeof(Device.calCrc) + 4;
      break;
  }
}

// called when a poly pressure message is received, assumed to be part of a bulk import (channel is 0-indexed)
void receiveMicroLinnPolyPressure(byte data1, byte data2, byte channel) {
  if (microLinnImportType == 0) return;
  microLinnLastMomentMidiPP = micros();     // for an eventual time-out function

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
      if (microLinnImportType == 16) Device.calibrated = true;
      microLinnScrollSmall("IMPORT SUCCESS");
    }
    microLinnImportType = 0;
    setupMicroLinn();
    storeSettings();
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
    microLinnImportType = 0;
    microLinnScrollSmall("IMPORT WARNING EXCESS DATA " + String(i) + " " +  String(i/2+8));
    setupMicroLinn();
    storeSettings();
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
      if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[n+i]   = data1;
      if (i+1 >= microLinnImportSize) break;
      if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[n+i+1] = data2;
      break;

    case 6:   // 1 rainbow
      if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[n+i]   = data1;
      if (i+1 >= microLinnImportSize) break;
      if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[n+i+1] = data2;
      break;

    case 7:   // 1 fretboard
      Device.microLinn.fretboards[n+i] = data1;
      if (i+1 >= microLinnImportSize) break;
      Device.microLinn.fretboards[n+i+1] = data2;
      break;

    case 8:   // all edo data for 1 edo only
      m = edo + (edo % 2);                                  // round an odd edo up to an even number
      if (i == 0) {
        if (data1 != Global.microLinn.EDO) microLinnCancelImport();
        if (microLinnInRange(data2, 0, 1)) Global.microLinn.useRainbow = (data2 == 1);
      } else if (i <= 14) {
        n = data1 | (data2 << 8);
        if (n > 2 * MICROLINN_MAX_GUITAR_RANGE) microLinnCancelImport();
        Global.microLinn.guitarTuning[i >> 1] = n - MICROLINN_MAX_GUITAR_RANGE;
        if (i == 14) microLinnCheckGuitarTuning();
      } else if (i == 16) {
        //if (microLinnInRange(data1, 1, 48))  Global.microLinn.equaveSemitones = data1;
        if (microLinnInRange(data2, 0, 120)) Global.microLinn.octaveStretch = data2 - 60;
        //if (microLinnInRange(data2, 0, 120)) Global.microLinn.equaveCents = data2 - 60;
      } else if (i == 18) {
        if (data1 == ROWOFFSET_NOOVERLAP || data1 == 3 || data1 == 4 || data1 == 5 || data1 == 6 ||
            data1 == 7 || data1 == ROWOFFSET_OCTAVECUSTOM || data1 == ROWOFFSET_GUITAR || data1 == ROWOFFSET_ZERO) {
          Global.rowOffset = data1;
        } else microLinnCancelImport();
        if (microLinnInRange(data2, 0, 50)) Global.customRowOffset = data2 - 25;
      } else if (i == 20) {
        if (microLinnInRange(data1, 0, 8))  Split[LEFT].microLinn.colOffset = data1;
        if (microLinnInRange(data2, 0, 52)) microLinnRowOffset[LEFT] = data2 - 26;
        //if (microLinnInRange(data2, 0, 52)) Split[LEFT].microLinn.rowOffset = data2 - 26;
      } else if (i == 22) {
        if (microLinnInRange(data1, 0, 8))  Split[RIGHT].microLinn.colOffset = data1;
        if (microLinnInRange(data2, 0, 52)) microLinnRowOffset[RIGHT] = data2 - 26;
        //if (microLinnInRange(data2, 0, 52)) Split[RIGHT].microLinn.rowOffset = data2 - 26;
      } else if (i < 24 + m) {
        if (microLinnInRange(data1, 0, 127)) Device.microLinn.scales[n + i - 24] = data1;
        if (i - 23 >= edo) break;
        if (microLinnInRange(data2, 0, 127)) Device.microLinn.scales[n + i - 23] = data2;
      } else if (i < 24 + 2 * m) {
        if (microLinnInRange(data1, 0, COLOR_PINK)) Device.microLinn.rainbows[n + i - 24 - m] = data1;
        if (i - 23 - m >= edo) break;
        if (microLinnInRange(data2, 0, COLOR_PINK)) Device.microLinn.rainbows[n + i - 23 - m] = data2;
      } else if (i < 24 + 3 * m)  {
        Device.microLinn.fretboards[n + i - 24 - 2 * m] = data1;
        if (i - 23 - 2 * m >= edo) break;
        Device.microLinn.fretboards[n + i - 23 - 2 * m] = data2;
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

    case 16:   // calibration data
      array = (byte *) &Device.calRows;
      array[i] = data1; 
      if (i+1 < microLinnImportSize) array[i+1] = data2;
      break;
  }
  microLinnImportCounter += 2;
}

void microLinnCancelImport() {
  microLinnImportType = 0;
  short i = microLinnImportCounter;                // i = the counter, 0-indexed
  short j = microLinnImportCounter / 2 + 8;        // j = which midi message, 1-indexed, include the headers
  microLinnScrollSmall("IMPORT FAILURE " + String(i) + " " +  String(j));
  storeSettings();
  updateDisplay();
}

boolean microLinnInRange (short data, short lo, short hi) {
  if (microLinnImportType == 0) return false;      // avoid loading 2nd polypressure byte if the 1st byte is invalid
  boolean result = data >= lo && data <= hi;
  if (!result) microLinnCancelImport();
  return result;
}

void microLinnImportDevice(byte data1, byte data2, unsigned short i) {               // don't import calibration data
  if (i >= 5736) return;                                                             // version 72.0 size is 5736 bytes
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
    // ignore data1 = microLinn.MLversion and data2 = padding
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

void microLinnImportGlobal(byte data1, byte data2, unsigned short i, byte presetNum) {     // version 72.0 size is 192 bytes
  if (i < 0 || i >= sizeof(GlobalSettings)) return;
  GlobalSettings *g = (presetNum == 0 ? &Global : &config.preset[presetNum - 1].global);
  if (i >= 168 && !microLinnImportXen) return;
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
    if (data1 == ROWOFFSET_NOOVERLAP || data1 == 3 || data1 == 4 || data1 == 5 || data1 == 6 ||
        data1 == 7 || data1 == ROWOFFSET_OCTAVECUSTOM || data1 == ROWOFFSET_GUITAR || data1 == ROWOFFSET_ZERO) {
      g->rowOffset = data1;
    } else microLinnCancelImport();
    signed char n = (signed char)data2;
    if (microLinnInRange(abs(n), 0, 25))  g->customRowOffset = n;         // assume microLinn is on, fix later if it isn't
  } else if (i <= 108) {
    if (microLinnInRange(data1,  0, 127)) g->guitarTuning[i-102] = data1;
    if (microLinnInRange(data2,  0, 127)) g->guitarTuning[i-101] = data2;
  } else if (i == 110) {
    if (microLinnInRange(data1,  0, 3))   g->velocitySensitivity = (VelocitySensitivity)data1;   // ignore data2 = padding
  } else if (i == 112) {
    if (microLinnInRange(number, 0, 127)) g->minForVelocity = data1;
  } else if (i == 114) {
    if (microLinnInRange(number, 0, 127)) g->maxForVelocity = data1;
  } else if (i == 116) {
    if (microLinnInRange(number, 0, 127)) g->valueForFixedVelocity = data1; 
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
    if (microLinnInRange(number, 0, 127)) g->ccForSwitchCC65[(i-130)/2] = data1;
  } else if (i <= 148) {
    if (microLinnInRange(number, 0, 127)) g->ccForSwitchSustain[(i-140)/2] = data1;
  } else if (i <= 158) {
    if (microLinnInRange(number, ASSIGNED_TAP_TEMPO, MAX_ASSIGNED)) g->customSwitchAssignment[(i-150)/2] = data1;
  } else if (i == 160) {
    if (microLinnInRange(data1,  0, 1)) g->midiIO = data1;
    if (microLinnInRange(data2,  0, 4)) g->arpDirection = (ArpeggiatorDirection)data2;
  } else if (i == 162) {
    if (microLinnInRange(data1,  0, 8)) g->arpTempo = (ArpeggiatorStepTempo)data1;
    if (microLinnInRange(data2,  0, 2)) g->arpOctave = data2;
  } else if (i == 164) {
    if (microLinnInRange(data1,  0, 1)) g->sustainBehavior = (SustainBehavior)data1;
    if (microLinnInRange(data2,  0, 1)) g->splitActive = (data2 == 1);
  // import drum pad, locatorCC here
  } else if (i == 166) {
    if (data1 == 4) {
      if (g->customRowOffset < -17 || g->customRowOffset > 16) {       // limits are tighter when microLinn is off
        microLinnImportCounter -= 66;                                  // point to the problem, the customRowOffset byte
        microLinnCancelImport();
      }
      microLinnImportXen = (presetNum == 0 || presetNum == 5);         // the presets run top to bottom 3 2 1 0 5 4
      if (!microLinnImportXen) return;
    }
    if (microLinnInRange(data1, 4, 55)) g->microLinn.EDO = data1;
    if (microLinnInRange(data2, 0, 1))  g->microLinn.useRainbow = (data2 == 1);
  } else if (i <= 182) {
    short n = data1 | (data2 << 8);
    if (abs(n) > MICROLINN_MAX_GUITAR_RANGE) microLinnCancelImport();
    g->microLinn.guitarTuning[(i-168)/2] = n;
    if (i == 182) microLinnCheckGuitarTuning();
  } else if (i == 184) {
    if (microLinnInRange(data1, 1, 25))  g->microLinn.anchorCol = data1;
    if (microLinnInRange(data2, 0, 7))   g->microLinn.anchorRow = data2;
  } else if (i == 186) {
    if (microLinnInRange(data1, 0, 127)) g->microLinn.anchorNote = data1;
    signed char n = (signed char)data2;
    if (microLinnInRange(abs(n), 0, 60)) g->microLinn.anchorCents = n;
  } else if (i == 188) {
    signed char n = (signed char)data1;
    if (microLinnInRange(abs(n), 0, 60)) g->microLinn.octaveStretch = n;
    if (microLinnInRange(data2, 0, 60))  g->microLinn.sweeten = data2;
  } else if (i == 190) {
    // ignore bytes 190 and 191 = padding
  }
}

void microLinnImportSplits(byte data1, byte data2, unsigned short i,  byte presetNum) {       // version 72.0 size is 102 bytes per side
  if (i < 0 || i >= 2 * sizeof(SplitSettings)) return;
  byte side = (i < sizeof(SplitSettings) ? LEFT : RIGHT);
  if (side == RIGHT) i -= sizeof(SplitSettings);
  SplitSettings *spl = (presetNum == 0 ? &Split[side] : &config.preset[presetNum - 1].split[side]);
  if (i > 98 && !microLinnImportXen) return;
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
    if (microLinnInRange(number, 0, 129)) spl->customCCForY = data1;
  } else if (i == 32) {
    if (microLinnInRange(number, 0, 127)) spl->minForY = data1;
  } else if (i == 34) {
    if (microLinnInRange(number, 0, 127)) spl->maxForY = data1;
  } else if (i == 36) {
    if (microLinnInRange(data1,  0, 1))   spl->relativeY = (data1 == 1);        // ignore data2 = padding 
  } else if (i == 38) {
    if (microLinnInRange(number, 0, 127)) spl->initialRelativeY = data1;
  } else if (i == 40) {
    if (microLinnInRange(data1,  0, 2))   spl->expressionForZ = (LoudnessExpression)data1;    // ignore data2 = padding 
  } else if (i == 42) {
    if (microLinnInRange(number, 0, 127)) spl->customCCForZ = data1;
  } else if (i == 44) {
    if (microLinnInRange(number, 0, 127)) spl->minForZ = data1;
  } else if (i == 46) {
    if (microLinnInRange(number, 0, 127)) spl->maxForZ = data1;
  } else if (i == 48) {
    if (microLinnInRange(data1,  0, 1))   spl->ccForZ14Bit = (data1 == 1);      // ignore data2 = padding
  } else if (i <= 64) {
    if (microLinnInRange(number, 0, 128)) spl->ccForFader[(i-50)/2] = data1;
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
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRow = data1;
  } else if (i == 78) {
    if (microLinnInRange(data1,  0, 1))   spl->lowRowCCXYZBehavior = data1;      // ignore data2 = padding
  } else if (i == 80) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowX = data1;
  } else if (i == 82) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowY = data1; 
  } else if (i == 84) {
    if (microLinnInRange(number, 0, 128)) spl->ccForLowRowZ = data1;
  } else if (i == 86) {
    signed char n = (signed char)data1;
    if (n % 12 != 0) microLinnCancelImport();
    if (microLinnInRange(abs(n), 0, 60)) spl->transposeOctave = n;
    n = (signed char)data2;
    if (microLinnInRange(abs(n), 0, 7))  spl->transposePitch = n;
  } else if (i == 88) {
    signed char n = (signed char)data1;
    if (microLinnInRange(abs(n), 0, 7))  spl->transposeLights = n;
    if (microLinnInRange(data2,  0, 1))  spl->ccFaders = (data2 == 1);
  } else if (i == 90) {
    if (microLinnInRange(data1, 0, 1))   spl->arpeggiator = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))   spl->strum = (data2 == 1);
  } else if (i == 92) {
    if (microLinnInRange(data1, 0, 1))   spl->mpe = (data1 == 1);
    if (microLinnInRange(data2, 0, 1))   spl->sequencer = (data2 == 1);
  } else if (i == 94) {
    if (microLinnInRange(data1, 0, 2))   spl->sequencerView = (SequencerView)data1;
    if (!microLinnImportXen && data2 == 1) return;
    if (microLinnInRange(data2, 0, 8))   spl->microLinn.colOffset = data2;
  } else if (i == 96) {
    if (microLinnInRange(data1, 0, 240)) spl->microLinn.hammerOnWindow = data1;
    if (microLinnInRange(data2, 0, 1))   spl->microLinn.hammerOnNewNoteOn = (data2 == 1);
  } else if (i == 98) {
    if (microLinnInRange(data1, 0, 3))   spl->microLinn.pullOffVelocity = data1;
    if (!microLinnImportXen) return;
    signed char n = (signed char)data2;
    if (microLinnInRange(abs(n), 0, 14)) spl->microLinn.transposeEDOsteps = n;
  } else if (i == 100) {
    signed char n = (signed char)data1;
    if (microLinnInRange(abs(n), 0, 14)) spl->microLinn.transposeEDOlights = n;
    if (microLinnInRange(data2,  0, 2))  spl->microLinn.tuningTable = data2;
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
    if ((Split[split].lowRowMode == lowRowCCX   && Split[sensorSplit].lowRowCCXBehavior   == lowRowCCFader) ||
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
  return microLinnRainbowColor(edo, microLinnEdostep[split][col][row]);
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

byte getMicroLinnRowOffsetColor(short rowOffset) {           // for most Global displays
  if (microLinnGCD(rowOffset, Split[LEFT].microLinn.colOffset) > 1 ||
      microLinnGCD(rowOffset, Split[RIGHT].microLinn.colOffset) > 1) 
    return COLOR_RED;
  return globalColor;
}

byte getMicroLinnRowOffsetColorNoOverlap() {                 // for Global NO OVERLAP button
  if (microLinnGCD(microLinnGetSplitWidth(LEFT),  Split[LEFT].microLinn.colOffset)  > 1) return COLOR_RED;
  if (microLinnGCD(microLinnGetSplitWidth(RIGHT), Split[RIGHT].microLinn.colOffset) > 1) return COLOR_RED;
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar1() {                   // for Global GUITAR button
  if (isMicroLinnOn()) {
    for (byte row = 1; row < NUMROWS; ++row) {
      if (microLinnGCD(Global.microLinn.guitarTuning[row], Split[LEFT].microLinn.colOffset)  > 1) return COLOR_RED;
      if (microLinnGCD(Global.microLinn.guitarTuning[row], Split[RIGHT].microLinn.colOffset) > 1) return COLOR_RED;
    }
  } else {
    for (byte row = 1; row < NUMROWS; ++row) {
      signed char stringGap = Global.guitarTuning[row] - Global.guitarTuning[row - 1];
      if (microLinnGCD(stringGap, Split[LEFT].microLinn.colOffset)  > 1) return COLOR_RED;
      if (microLinnGCD(stringGap, Split[RIGHT].microLinn.colOffset) > 1) return COLOR_RED;
    }
  }
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar2(byte row, boolean sideButton) {             // for non-microtonal guitar tuning display
  if (sideButton && row == guitarTuningRowNum) return Split[Global.currentPerSplit].colorAccent;  // side buttons are in col 1
  if (row > 0) {
    signed char stringGap = Global.guitarTuning[row] - Global.guitarTuning[row - 1];
    if (getMicroLinnRowOffsetColor(stringGap) == COLOR_RED) return COLOR_RED;
  }
  if (row < NUMROWS - 1) {
    signed char stringGap = Global.guitarTuning[row] - Global.guitarTuning[row + 1];
    if (getMicroLinnRowOffsetColor(stringGap) == COLOR_RED) return COLOR_RED;
  }
  if (sideButton) return COLOR_GREEN;
  return globalColor;
}

byte getMicroLinnRowOffsetColorGuitar3(byte row, boolean sideButton) {         // for microtonal guitar tuning display
  if (sideButton && row < Global.microLinn.anchorRow) row += 1;
  if (row > 0) {                                                               // side buttons are in col 1
    if (microLinnGCD(Global.microLinn.guitarTuning[row], Split[LEFT].microLinn.colOffset)  > 1) return COLOR_RED;
    if (microLinnGCD(Global.microLinn.guitarTuning[row], Split[RIGHT].microLinn.colOffset) > 1) return COLOR_RED;
  }
  if (sideButton) return COLOR_GREEN;
  return globalColor;
}

byte getMicroLinnRowOffsetColor() {                     // for PerSplit row display
  byte side = Global.currentPerSplit;
  if (microLinnGCD(microLinnRowOffset[side], Split[side].microLinn.colOffset) > 1) 
    return (Split[side].colorMain == COLOR_RED ? COLOR_MAGENTA : COLOR_RED);
  return Split[side].colorMain;
}

byte getMicroLinnColOffsetColor() {                     // for PerSplit col display
  byte side = Global.currentPerSplit;
  if (Split[side].microLinn.colOffset == 1) return Split[side].colorMain;

  byte badColor = (Split[side].colorMain == COLOR_RED ? COLOR_MAGENTA : COLOR_RED);
  signed char rowOffset = microLinnRowOffset[side];
  if (rowOffset == 26) {rowOffset = microLinnGetSplitWidth(side);}
  else if (rowOffset == -26) {
    rowOffset = Global.rowOffset;
    if (rowOffset == ROWOFFSET_ZERO) {rowOffset = 0;}
    else if (rowOffset == ROWOFFSET_NOOVERLAP) {rowOffset = microLinnGetSplitWidth(side);}
    else if (rowOffset == ROWOFFSET_OCTAVECUSTOM && Global.customRowOffset == -17 && !isMicroLinnOn()) {
      switch (Split[side].microLinn.colOffset) {case 2: case 4: case 5: case 6: case 8: return badColor;}
      return Split[side].colorMain;
    }
    else if (rowOffset == ROWOFFSET_OCTAVECUSTOM) {
      rowOffset = Global.customRowOffset;
    }
    else if (rowOffset == ROWOFFSET_GUITAR) {
      if (isMicroLinnOn()) {
        for (byte row = 1; row < NUMROWS; ++row) {
          if (microLinnGCD(Global.microLinn.guitarTuning[row], Split[side].microLinn.colOffset) > 1) return badColor;
        }
        return Split[side].colorMain;
      } else {
        for (byte row = 1; row < NUMROWS; ++row) {
          signed char stringGap = Global.guitarTuning[row] - Global.guitarTuning[row - 1];
          if (microLinnGCD(stringGap, Split[side].microLinn.colOffset) > 1) return badColor;
        }
        return Split[side].colorMain;
      }
    }
  }

  if (microLinnGCD(rowOffset, Split[side].microLinn.colOffset) > 1)
    return badColor;
  return Split[side].colorMain;
}

/*void debugFreeRam() {
  static unsigned long lastFrame = 0;
  unsigned long now = micros();
  if (Device.serialMode && sensorCol == 1 && sensorRow == 0 && calcTimeDelta(now, lastFrame) >= 500000) {
    lastFrame = now;

    char* heapend = sbrk(0);
    register char* stack_ptr asm ("sp");
    struct mallinfo mi = mallinfo();
    Serial.print("RAM dynamic:");
    Serial.print(mi.uordblks);
    Serial.print(" static:");
    Serial.print(&_end - ramstart);
    Serial.print(" free:");
    Serial.println(stack_ptr - heapend + mi.fordblks);
  }
}*/

#include <malloc.h>

extern char _end;
extern "C" char* sbrk(int i);
char* ramstart = (char*)0x20070000;
char* ramend = (char*)0x20088000;

void paintMicroLinnDebugDump() {     // delete later from here and from ls_displayModes
  if (sensorRow < 4 && sensorCol <= 16) return;

  char* heapend = sbrk(0);
  register char* stack_ptr asm ("sp");
  struct mallinfo mi = mallinfo();

  byte* arrayPtr1 = (byte *) &config.device.minUSBMIDIInterval;
  byte* arrayPtr2 = (byte *) &config.project;
  int allSettingsSize = arrayPtr2 - arrayPtr1;
  arrayPtr2 = (byte *) &Global;
  int deviceSize = (byte *) &Global - arrayPtr1;
  int MLversion  = (byte *) &Device.microLinn.MLversion - arrayPtr1;
  int scales     = (byte *) &Device.microLinn.scales - arrayPtr1;
  int rainbows   = (byte *) &Device.microLinn.rainbows - arrayPtr1;
  int fretboards = (byte *) &Device.microLinn.fretboards - arrayPtr1;

  byte sp = Global.currentPerSplit;
  if (sensorRow == 6) {
  if (sensorCol == 1)  paintNumericDataDisplay(Split[sp].colorMain, mi.uordblks, 0, false);
  if (sensorCol == 2)  paintNumericDataDisplay(Split[sp].colorMain, &_end - ramstart, 0, false);
  if (sensorCol == 3)  paintNumericDataDisplay(Split[sp].colorMain, stack_ptr - heapend + mi.fordblks, 0, false);
  if (sensorCol == 4)  paintNumericDataDisplay(Split[sp].colorMain, microLinnEdostep[sp][25][7], 0, false);
  if (sensorCol == 8)  paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 1, 0, false);
  if (sensorCol == 9)  paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 10, 0, false);
  if (sensorCol == 10) paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 100, 0, false);
  if (sensorCol == 11) paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 1000, 0, false);
  if (sensorCol == 12) paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 10000, 0, false);
  if (sensorCol == 13) paintNumericDataDisplay(Split[sp].colorMain, MLcalcTuningTimer / 100000, 0, false);
  if (sensorCol == 16) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportType, 0, false);
  if (sensorCol == 17) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportSize, 0, false);
  if (sensorCol == 18) paintNumericDataDisplay(Split[sp].colorMain, microLinnImportCounter, 0, false);
  if (sensorCol == 20) paintNumericDataDisplay(Split[sp].colorMain, allSettingsSize, 0, false);
  if (sensorCol == 21) paintNumericDataDisplay(Split[sp].colorMain, deviceSize, 0, false);
  if (sensorCol == 22) paintNumericDataDisplay(Split[sp].colorMain, MLversion, 0, false);
  if (sensorCol == 23) paintNumericDataDisplay(Split[sp].colorMain, scales, 0, false);
  if (sensorCol == 24) paintNumericDataDisplay(Split[sp].colorMain, rainbows, 0, false);
  if (sensorCol == 25) paintNumericDataDisplay(Split[sp].colorMain, fretboards, 0, false);
  } else {
    byte side = sensorCol % 2;
    if (sensorCol <= 2) paintNumericDataDisplay(Split[1-side].colorMain, Split[1-side].microLinn.colOffset, 0, false);
    if (sensorCol == 3) paintNumericDataDisplay(Split[sp].colorMain, Global.microLinn.useRainbow, 0, false);
    if (sensorCol >= 4 && sensorCol <= 15) 
                        paintNumericDataDisplay(Split[side].colorMain, config.preset[(sensorCol-4)/2].split[side].microLinn.colOffset, 0, false);
    if (sensorCol >= 16 && sensorCol <= 21) 
                        paintNumericDataDisplay(Split[sp].colorMain, config.preset[sensorCol-16].global.microLinn.useRainbow, 0, false);
  }
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

/************** microLinn config functions ************************/

byte microLinnGetColor (boolean selected) {
  return selected ? Split[LEFT].colorAccent : Split[LEFT].colorMain;
}

void enterMicroLinnConfig() {
  microLinnConfigNowScrolling = false;
  resetNumericDataChange();
  setDisplayMode(displayMicroLinnConfig);
  updateDisplay();
}

void microLinnPaintConfigButtons() {
  for (byte col = 2; col <= 16; ++col) { 
    //switch (col) {case 4: case 6: case 8: case 9: case 11: case 13: case 15: continue;}        // skip over empty columns
    if (col == 4 || col == 7 || col == 11 || col == 13 || col == 15) col += 1;               // skip over empty columns
    setLed(col, 0, microLinnGetColor(col == microLinnConfigColNum), cellOn);
  }
}

void microLinnScrollSmall (String text) {
//  if (displayMode == displayMicroLinnConfig &&
//      microLinnConfigColNum > 10 &&
//      !isMicroLinnOn()) {
//    text = "FIRST SELECT AN EDO";
//  }
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

  /*
      case 2:  microLinnScrollSmall("LEFT/RIGHT COLUMN OFFSET"); break;
      case 3:  microLinnScrollSmall("LEFT/RIGHT ROW OFFSET"); break;
      case 5:  microLinnScrollSmall("LEFT/RIGHT OPTIONS"); break;
      case 7:  microLinnScrollSmall("GLOBAL OPTIONS"); break;
      case 10: microLinnScrollSmall("EDO (NOTES PER OCTAVE)"); break;
      case 12: microLinnScrollSmall("NOTE LIGHTS"); break;
      case 14: microLinnScrollSmall("LEFT/RIGHT MICROTONAL OPTIONS"); break;
      case 16: microLinnScrollSmall("GLOBAL MICROTONAL OPTIONS"); break;  
  */

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
        paintNumericDataDisplay(getMicroLinnColOffsetColor(), Split[side].microLinn.colOffset, 0, false);
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
        paintNumericDataDisplay(getMicroLinnRowOffsetColor(), microLinnRowOffset[side], -4, false);
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
        if (Global.microLinn.octaveStretch <= -10) offset = -3;
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
        if (isMicroLinnOn()) handleNumericDataNewTouchCol(Global.microLinn.octaveStretch, -60, 60, true);
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
        microLinnHandleHammerOnsNewTouch(side);
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

void microLinnPaintPerSplitOptionsButtons() {
  for (byte row = 3; row <= 6; ++row) {
    setLed(1, row, microLinnGetColor(microLinnPerSplitOptionsRowNum == row), cellOn);
  }
}

void paintMicroLinnPerSplitOptions() {
  microLinnPaintPerSplitOptionsButtons();
  if (microLinnPerSplitOptionsNowScrolling) {
    switch (microLinnPerSplitOptionsRowNum) {
      case 6: microLinnScrollSmall("COLLAPSE TO SCALE"); break;
      case 5: microLinnScrollSmall("SHOW CUSTOM LIGHT PATTERN"); break;
      case 4: microLinnScrollSmall("HAMMER-ON WINDOW"); break;
      case 3: microLinnScrollSmall("PULL-OFF MODE"); break;
    }
    microLinnPaintConfigButtons();
    microLinnPaintPerSplitOptionsButtons();
    return;
  }

  byte side = LEFT;
  switch (microLinnPerSplitOptionsRowNum) {
    case 6:
      if (microLinnCollapseBendPerPad == 0) {
        smallfont_draw_string(3, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, microLinnCollapseBendPerPad, 3, false);
      }
      break;
    case 5:
      if (microLinnShowCustomLEDs[side] == 0) {    
        smallfont_draw_string(3, 1, "OFF", Split[side].colorMain, false);
      } else {
        paintNumericDataDisplay(Split[side].colorMain, microLinnShowCustomLEDs[side], 3, false);
      }
      break;
    case 4:
      if (Split[side].microLinn.hammerOnWindow == 0) {
        smallfont_draw_string(3, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, 10 * Split[side].microLinn.hammerOnWindow, 3, false);
      }
      break;
    case 3:
      if (Split[side].microLinn.hammerOnWindow == 0) {
        smallfont_draw_string(3, 1, "N/A", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, Split[side].microLinn.pullOffVelocity, 3, false);
      }
      break;
  }
}

void paintMicroLinnConfigNEW() {
  clearDisplay();
  microLinnPaintConfigButtons();

  if (microLinnConfigNowScrolling) {
    switch (microLinnConfigColNum) {
      case 2:  microLinnScrollSmall("LEFT/RIGHT COLUMN OFFSET"); break;
      case 3:  microLinnScrollSmall("LEFT/RIGHT ROW OFFSET"); break;
      case 5:  microLinnScrollSmall("LEFT/RIGHT OPTIONS"); break;
      case 7:  microLinnScrollSmall("GLOBAL OPTIONS"); break;
      case 10: microLinnScrollSmall("EDO (NOTES PER OCTAVE)"); break;
      case 12: microLinnScrollSmall("NOTE LIGHTS"); break;
      case 14: microLinnScrollSmall("LEFT/RIGHT MICROTONAL OPTIONS"); break;
      case 16: microLinnScrollSmall("GLOBAL MICROTONAL OPTIONS"); break;
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
      //microLinnPaintPerSplitOptions();
      break;
    case 7:
      //microLinnPaintGlobalOptions();
      break;
    case 10: 
      if (Global.microLinn.EDO < 5) {
        smallfont_draw_string(2, 1, "OFF", globalColor, false);
      } else {
        paintNumericDataDisplay(globalColor, Global.microLinn.EDO, 0, false);
      }
      break;
    case 12:
      if (isMicroLinnOn()) microLinnPaintNoteLights();
      break; 
    case 14:
      //if (isMicroLinnOn()) microLinnPaintPerSplitXenOptions();
      break;
    case 16:
      //if (isMicroLinnOn()) microLinnPaintGlobalXenOptions();
      break;
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
      if (microLinnIsScaleStep(edo, edostep)) {
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
    for (stepspan = 0; stepspan < 7; ++stepspan) {
      setLed(11 - col, 7 - stepspan, COLOR_PINK, cellOn);
    }
  } else if (currScale == 8) {
    paintMicroLinnFretboardEditor(false);                                                    // blue dots
  } 

  for (byte scaleNum = 0; scaleNum < 7; ++scaleNum) {                                        // the 7 scale buttons
    setLed(1, 7 - scaleNum, scaleNum == currScale ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);
  }
  setLed(3, 7, currScale == 7 ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);    // color editor
  setLed(3, 5, currScale == 8 ? Split[LEFT].colorAccent : Split[LEFT].colorMain, cellOn);    // fretboard selector
  setLed(3, 3, Global.microLinn.useRainbow ? COLOR_LIME : Split[LEFT].colorMain, cellOn);    // rainbow enabler
}

void microLinnHandleNoteLightsNewTouch() {
  if ((sensorCol == 1 && sensorRow > 0) ||                                    // scale selectors or
      (sensorCol == 3 && (sensorRow == 7 || sensorRow == 5))) {               // rainbow editor or fretboard selector
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
      short i = microLinnTriIndex(edo, edostep);
      switch (Device.microLinn.rainbows[i]) {                   // cycle through the colors in rainbow order
        case 8:  Device.microLinn.rainbows[i] = 1;  break;      // white to red
        case 1:  Device.microLinn.rainbows[i] = 9;  break;      // red to orange
        case 9:  Device.microLinn.rainbows[i] = 2;  break;      // orange
        case 2:  Device.microLinn.rainbows[i] = 10; break;      // yellow (not used)
        case 10: Device.microLinn.rainbows[i] = 3;  break;      // lime (used as yellow)
        case 3:  Device.microLinn.rainbows[i] = 4;  break;      // green
        case 4:  Device.microLinn.rainbows[i] = 5;  break;      // cyan
        case 5:  Device.microLinn.rainbows[i] = 6;  break;      // blue
        case 6:  Device.microLinn.rainbows[i] = 11; break;      // magenta
        case 11: Device.microLinn.rainbows[i] = 8;  break;      // pink
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

  if (currScale == 7 && sensorCol == 3 && sensorRow == 7) {                                 // color editor button
    memcpy (&Device.microLinn.rainbows[start], &MICROLINN_RAINBOWS[start], edo);            // reset the rainbow
    updateDisplay();
    return;
  }

  if (currScale == 8 && sensorCol == 3 && sensorRow == 5) {                                 // fretboard selector button
    microLinnResetFretboard(edo);
    updateDisplay();
  }
}

void microLinnHandleNoteLightsRelease() {
  if (!isCellPastConfirmHoldWait()) {                                           // short-press

    byte currScale = Global.activeNotes;
    if ((sensorCol == 1 && sensorRow >= 1 && currScale == 7 - sensorRow) ||     // scale selector
        (sensorCol == 3 && sensorRow == 7 && currScale == 7) ||                 // rainbow editor
        (sensorCol == 3 && sensorRow == 5 && currScale == 8)) {                 // fretboard selector
      if (microLinnCanBacktrack) {
        Global.activeNotes = microLinnPrevScale;                                // backtrack
        microLinnPrevScale = currScale;
        updateDisplay(); 
      } else {
        setLed(sensorCol, sensorRow, Split[LEFT].colorAccent, cellOn);          // stop pulsing
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

void handleMicroLinnFretboardEditorNewTouch() {
  byte fret = microLinnGetFret(Global.currentPerSplit, sensorCol);
  Device.microLinn.fretboards[microLinnTriIndex(edo, fret)] ^= (1 << sensorRow);          // xor to toggle the bit
  updateDisplay();
}

void microLinnPaintHammerOns() {
  // write a trailing zero
}

void microLinnHandleHammerOnsNewTouch(byte side) {
  microLinnCalcHammerOnEdosteps(side);
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
      byte col = 19;
      if (LINNMODEL == 200) {
        smallfont_draw_string(3, 1, "RCH", Split[side].colorMain, false);
      } else {
        condfont_draw_string(3, 1, "RCH", Split[side].colorMain, false);
        col = 16;
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
  clearDisplay();
  for (byte r = 0; r < NUMROWS; ++r) {
    if (r == microLinnGuitarTuningRowNum) {color = COLOR_CYAN;}
    else if (r == microLinnGuitarTuningRowNum - 1 && r >= anchor) {color = COLOR_BLUE;}
    else if (r == microLinnGuitarTuningRowNum + 1 && r <= anchor) {color = COLOR_BLUE;}
    else {color = getMicroLinnRowOffsetColorGuitar3(r, true);}
    setLed(1, r, color, cellOn);
    if (r == anchor) setLed(2, r, color, cellOn);
  }
  byte r = microLinnGuitarTuningRowNum;
  if (r == anchor || r >= NUMROWS) return;     // r == NUMROWS+1 means it hasn't been set yet
  if (r < anchor) r += 1;
  color = getMicroLinnRowOffsetColorGuitar3(r, false);
  paintNumericDataDisplay(color, Global.microLinn.guitarTuning[r], 0, false);
}

void handleMicroLinnGuitarTuningNewTouch() {
  if (sensorCol == 2 && sensorRow != Global.microLinn.anchorRow) return;
  if (sensorCol <= 2) {
    microLinnGuitarTuningRowNum = sensorRow;
    updateDisplay();
  }
  else if (microLinnGuitarTuningRowNum != Global.microLinn.anchorRow) {
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
    microLinnStoreGuitarRowOffsetCents(row);
    microLinnCalcGuitarTuning();
  }

  if (microLinnGuitarTuningRowNum == Global.microLinn.anchorRow && sensorCol > 2) return;
  signed char lefty = isLeftHandedSplit(Global.currentPerSplit) ? -1 : 1;
  short edostep = microLinnGuitarTuning[microLinnGuitarTuningRowNum]
                - lefty * Split[Global.currentPerSplit].microLinn.colOffset * (Global.microLinn.anchorCol - 1);
  microLinnSendPreviewNote(edostep);
}

/************** octave transpose functions ************************/

void  paintMicroLinnEdostepTranspose(byte side) {
  // paint the 2 new rows on the transpose display
  if (!isMicroLinnOn()) return;
  if (MICROLINN_MAJ_2ND_TR[edo] == 1) return;

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
  if (MICROLINN_MAJ_2ND_TR[edo] == 1) return;
  if (sensorRow == SPLIT_ROW &&
      sensorCol > 0 && sensorCol < 16) {
    Split[side].microLinn.transposeEDOsteps = sensorCol - 8;
  }
  else if (sensorRow == GLOBAL_SETTINGS_ROW &&
           sensorCol > 0 && sensorCol < 16) {
    Split[side].microLinn.transposeEDOlights = sensorCol - 8;
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
