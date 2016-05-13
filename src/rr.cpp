/* Random Reader
 * this class reads, counts, and outputs 
 * the random selection chosen in Super Smash Bros. Melee
 *
 * inputs:
 *  directory which holds an image sequence of a SSBM random selection
 *  output file for CSV file
*/

#include <stdio.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <climits>
#include <opencv2/opencv.hpp>

//ratios to find the region of interest
const double x_start_ratio = 63.0/640.0;
const double x_end_ratio   = 154.0/640.0;
const double y_start_ratio = 415.0/524.0;
const double y_end_ratio   = 432.0/524.0;

#define IMG_COLS 640
#define IMG_ROWS 524

#define NUM_CHARACTERS 25
#define CHARACTER_SAMPLES_DIR "../data/character_samples/"
#define NO_CHARACTER_SAMPLE "../data/blank/blank.png"

#define THRESHOLD 5000

using namespace cv;

typedef struct {
    Mat img;
    std::string name;
} Character;

int main(int argc, char** argv )
{

    /////////////////INIT///////////////////

    //check for number of args
    if ( argc != 3 )
    {
        printf("usage: rr <Image_Seq_Dir> <Output_File>\n");
        return -1;
    }
    
    //convert args to strings
    std::string img_seq_dir(argv[1]);
    std::string character_samples_dir(CHARACTER_SAMPLES_DIR);

    //declare variables which are reused
    DIR *dir;
    struct dirent *entry;




    //get the positions for the region of interest
    //TODO: make generic with img.cols and img.rows
    int x_start = IMG_COLS*x_start_ratio;
    int x_end   = IMG_COLS*x_end_ratio;
    int y_start = IMG_ROWS*y_start_ratio;
    int y_end   = IMG_ROWS*y_end_ratio;
    int x_size  = x_end - x_start;
    int y_size  = y_end - y_start;
    Rect roi_rect( x_start, y_start, x_size, y_size);




    ///////////////LOAD SAMPLES/////////////////

    //create a Mat array with a sample roi of each character
    Character characters[NUM_CHARACTERS];

    //open the character samples dir
    if ((dir = opendir(CHARACTER_SAMPLES_DIR)) == NULL) {
        printf("error: could not open directory");
        return -1;
    }

    //loop through the files in the character samples dir
    for (int i=0; i<NUM_CHARACTERS; i++) {
        //read next file in dir
        if ((entry = readdir(dir)) == NULL) {
            printf("not sufficient character samples");
            return -1;
        }

        //exclude "." and ".."
        std::string d_name(entry->d_name);
        if (d_name.compare(".") == 0 || d_name.compare("..") == 0) {
            i--;
            continue;
        }

        //store the sample roi
        characters[i].img = (imread(character_samples_dir + "/" + d_name, 0))(roi_rect);

        //store the character's name
        characters[i].name = d_name.erase(d_name.find_last_of("."), string::npos);
    }

    //create a Mat with no character selected
    Mat no_character = (imread(NO_CHARACTER_SAMPLE, 0))(roi_rect);




    //////////////////LOAD AND ANALYZE DATA////////////////

    //open the data dir
    if ((dir = opendir(argv[1])) == NULL) {
        printf("error: could not open directory");
        return -1;
    }

    //init some variables and open the output file
    bool character_detected = false;
    int num_detected = 0;
    std::ofstream output_file(argv[2]);

    //loop through each file in the data dir
    while ((entry = readdir(dir)) != NULL) {
        //exclude "." and ".."
        std::string d_name(entry->d_name);
        if (d_name.compare(".") == 0 || d_name.compare("..") == 0) continue;

        //open the image (in greyscale)
        Mat image;
        image = imread(img_seq_dir + "/" + d_name, 0);

        //extract the ROI (character's name)
        Mat roi = image(roi_rect);

        //compare to sample with no character selected
        int no_char_energy = 0;
        for(int i=0; i<x_size; i++) for(int j=0; j<y_size; j++){
            no_char_energy += abs( roi.at<uchar>(j,i) - no_character.at<uchar>(j,i));
        }

        //continue if the current frame has no character selected
        if(no_char_energy < THRESHOLD) {
            character_detected = false;
            continue;
        }

        //continue if a character is still selected from the previous frame
        if(character_detected) continue;

        //compare the roi to all the samples 
        int energy[NUM_CHARACTERS];
        for (int n=0; n<NUM_CHARACTERS; n++){
            energy[n] = 0;
            for(int i=0; i<x_size; i++) for(int j=0; j<y_size; j++){
                energy[n] +=  abs( roi.at<uchar>(j,i) - characters[n].img.at<uchar>(j,i));
            }
/*          //character found if energy is less than threshold
            if(energy[n] < THRESHOLD){
                std::cout << characters[n].name << std::endl;
                character_detected = true; 
            } 
*/
        }

        //find the sample which produced the minimum energy
        std::string detected_character;
        int min_energy = INT_MAX;
        int min_energy_index;
        for(int n=0; n<NUM_CHARACTERS; n++){
            if (energy[n] < min_energy) {
                min_energy       = energy[n];
                min_energy_index = n;
            }
        }

        //write the character name to output
        output_file << characters[min_energy_index].name << ", ";
        std::cout << characters[min_energy_index].name << std::endl;
        character_detected = true;
        num_detected++;

            
/*      //DEBUG: display the image and break 
        namedWindow("Display Image", WINDOW_AUTOSIZE ); 
        imshow("Display Image", no_character); 
        waitKey(0); 
        break; 
*/
    } 

    std::cout << "Total characters detected: " << num_detected << std::endl;
    output_file.close();
}
