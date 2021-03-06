#ifndef SHI
#define SHI

#include <gtk/gtk.h>
#include <gtk/gtk.h>

#include <boost/regex.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <chrono>
#include <regex>
#include <thread>
#include "classes.h"

//global shit
//base application pointer
extern GtkApplication *MainWindow;
//vector of the data structure so there can be one structure for every file loaded
extern std::vector <LOG_FILE_DATA> log_files;
//keeps track of actual current time between log files
//there's going to be a lot of problems with this between multiple log files
//but whatever
//extern long int current_time;
extern GtkAdjustment *current_time;
//if (Playing) it's playing duuhh
extern bool Playing;

#define default_text "There are no log files currently loaded."
//GUI functions
//"front end" stuff. These are all the primary functions that control
//SHIs behavior. They call all the secondary functions (in the backend
//that perform the underlying processes

//produces main window
void mainwindowactivate (GtkApplication *app);
//function to run the drawing area
void drawing_area (GtkWidget *area);
//open file dialogue for project files
void open_project (GtkWidget *tabs);
//open file dialogue for log files
struct open_file_params {GtkWidget *tabs;};
void open_file (open_file_params *_);
//save file dialogue for project files
void save_project ();
//add a new log file to the view
void add_text_view (char *filename, GtkWidget *tabs);
//updates the text view with new log files
void update_text_view (char *filename, GtkWidget *notepad);
//reads the events from the next smallest time segment in the log file
void read_time_segment ();
//wrapper on the remove page function for gtk notebooks
void remove_page (void *page);

//Backend Functions
//none of these functions are used individually or called by
//themselves. these are all used to provide the information to display
//with the GUI functions

//parses the log files and populates the device classes
//it also calls the gtk parsing dialogue
struct to_regex {int pos;};
void set_regular_expressions (to_regex *_);
//creates the parsing dialogue window
struct to_regex_window {GtkApplication *a; int b;};
void set_regex_window (GtkApplication *app);
//removes an entry box from the parse log file window
struct to_remove_entry {char a; GtkWidget *b; int pos;};
void remove_expression (GtkEntry *Entry);
//add entry boxes for syntax types:
struct to_add_entry {char a; GtkWidget *b; int pos;};
void add_entry_box_regex (GtkWidget *container);
//saves regular expression set to a JSON file
void save_to_json (int *log_files_pos);
//loads regular expression set from a JSON file
void load_from_json (GtkWidget *refresh);

//functions to highlight text buffers:
//creates a giant list of all events correlated to the time across all log files
void initialize_log_file_stats (GtkAdjustment *adjustment);
//sorts all the times in chronological order
void sort_times();
//moves iterator to next time and adjusts UI accordingly
void move_time_forward ();
//moves iterator to previous time and adjusts UI accordingly
void move_time_backward ();

//moves the time forward when the play button moves
void play (GtkAdjustment *adjustment);
void play_loop (GtkAdjustment *adjustment);
//stops playback
void stop ();
//skip button Functions
void skip_forward (GtkAdjustment *adjustment);
void skip_backward (GtkAdjustment *adjustment);
//connects the current time to the scrubber
void scrubber_change_time (GtkAdjustment *adjustment);

//creates device list window
void generate_device_list (GtkAdjustment *adjustment);
//creates device map window
void generate_device_map (GtkAdjustment *adjustment);
void draw_on_device_map (GtkWidget *widget, cairo_t *cr, gpointer data);

//creates error window
void error_window_dialogue (char *error_warning);
void error_window (char *error_string);

#endif
