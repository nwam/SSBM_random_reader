# SSBM_RANDOM_READER

##Hypothesis
The RNG which is in charge of random character selection in Super Smash Bros. Melee is not that random. It tends to select the same character(s) in close vicinity of being selected, or even in a row. 

## Aquiring Data
This project requires a stablized video of SSBM random character selection.
The test data in the `data/test*` folder(s) was and can be easily aquired using 
  * a Gamecube emulator
  * a SSBM ROM
  * a script which automates the random selection process which can be found at `src/ssbm_random.ahk`
  * a screenrecorder (recommended) or video camera

Note: data should be converted from video to image sequence.  
From the test data, a sample of each character being selected must be placed in `data/character_samples` and a sample of no character selected must be placed at `data/blank/blank.png`. Since there already is sample data present, this is not necessarily needed.

## Output
The program outputs the selected characters in the order of selection. The results are stored into a CSV file.

## Results
Results of a test are available in `data/`.
TODO: analyze results of large tests to determine if the RNG in SSBM is as bad as hypothesized

## The Engine
Currently, the selected character is determined by a simple 1-pixel linear energy system. This is sufficient for the data being handled since it was directly recorded using a screen recorder. 

## Usage
To build an executable, in `src/` run `cmake .` followed by `make`.
To run, use `./rr <data_folder> <output_file>`
