#include "shi.h"

//main function to parse the log file
void set_regular_expressions (to_regex *_)
{
	g_object_set_data (G_OBJECT (MainWindow), "position", _);
	set_regex_window (MainWindow);
}
void add_entry_box_regex (GtkWidget *container)
{
	char type = *(char*)g_object_get_data (G_OBJECT (container), "char");
	int position = *(int*)g_object_get_data (G_OBJECT (container), "position");
	std::cout << "add ad\t" << type << " " << position << std::endl;
	std::cout << position << std::endl;
	GtkWidget *sidebox, *closebutton, *Entry;
	sidebox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	closebutton = gtk_button_new_from_icon_name ("gtk-close", GTK_ICON_SIZE_SMALL_TOOLBAR);
	Entry = gtk_entry_new_with_buffer (log_files.at(position).add_regex (type));

	gtk_box_pack_start (GTK_BOX (sidebox), closebutton, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (sidebox), Entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (container), sidebox, TRUE, TRUE, 0);
	g_object_set_data (G_OBJECT (Entry), "char", g_object_get_data (G_OBJECT (container), "char"));
	g_object_set_data (G_OBJECT (Entry), "position", g_object_get_data (G_OBJECT (container), "position"));
	g_signal_connect_swapped (closebutton, "clicked", G_CALLBACK (remove_expression), Entry);
	gtk_widget_show_all (container);
}
void remove_expression (GtkEntry *Entry)
{
	char type = *(char*)g_object_get_data (G_OBJECT (Entry), "char");
	int position = *(int*)g_object_get_data (G_OBJECT (Entry), "position");
	std::cout << "remove at\t" << type << " " << position << std::endl;
	std::cout << position << std::endl;
	log_files.at (position).remove_ex (type, gtk_entry_get_buffer (Entry));
	gtk_widget_destroy (gtk_widget_get_parent (GTK_WIDGET (Entry)));
}

void initialize_log_file_stats (GtkAdjustment *adjustment)
{
//if there are no log files, exit the function
	if (log_files.size () == 0)
		return;
	std::vector <boost::regex>	year,
					month,
					day,
					hour,
					minute,
					second,
					device,
					event,
					state;
	boost::regex *tmp;
	boost::smatch	YearP,
			MonthP,
			DayP,
			HourP,
			MinuteP,
			SecondP,
			DeviceP,
			EventP,
			StateP;
	std::string contents, line;
	std::stringstream buffer;
	int contentsLength;
	GtkTextIter start, end;
	GtkTextBuffer *TextBuffer;
	DATA *check;
	EVENT *E;
	bool blankline = false;
	long long int old_start, CheckPos;
	for (int i = 0; i < log_files.size (); i ++)
	{
//set gtk text buffer from contents of the logfile and pass to string
		TextBuffer = log_files.at (i).get_text_file ();
		gtk_text_buffer_get_start_iter (TextBuffer, &start);
		gtk_text_buffer_get_end_iter (TextBuffer, &end);
		contents = gtk_text_buffer_get_text (TextBuffer, &start, &end, TRUE);
//assign contents of log file to stream buffer
		buffer.str (contents);
//generate regex filters from the regex expressions set in the regex settings
//window
		for (int k = 0; k < log_files.at (i).Year_Regex.size (); k ++)
		{
//call regex constructor with string from regex settings window
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Year_Regex.at (k))));
//add to vector
			year.push_back (*tmp);
//clear pointer (duh)
			delete tmp;
//repeat this eight more times for each regex field
		}
		for (int k = 0; k < log_files.at (i).Month_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Month_Regex.at (k))));
			month.push_back (*tmp);
			delete tmp;
		}
		for (int k= 0; k < log_files.at (i).Day_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Day_Regex.at (k))));
			day.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).Hour_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Hour_Regex.at (k))));
			hour.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).Minute_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Minute_Regex.at (k))));
			minute.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).Second_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Second_Regex.at (k))));
			second.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).Device_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Device_Regex.at (k))));
			device.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).Event_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).Event_Regex.at (k))));
			event.push_back (*tmp);
			delete tmp;
		}
		for (int k = 0; k < log_files.at (i).State_Regex.size (); k ++)
		{
			tmp = new boost::regex (gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (log_files.at (i).State_Regex.at (k))));
			state.push_back (*tmp);
			delete tmp;
		}
		while (!buffer.eof ())
		{
//clear data and event classes
			check = new DATA;
			E = new EVENT;
//set start and primary end position of time point in DATA class and getline
			if (!blankline)
			{
				old_start = buffer.tellg ();
				check->start = buffer.tellg ();
				std::getline (buffer, line);
				check->end = buffer.tellg ();
			}
			else
			{
				check->start = old_start;
				std::getline (buffer, line);
				check->end = buffer.tellg ();
			}
//search line for match with each regex entered into the field
			int k = 0;
			for (int k = 0; k < year.size (); k ++)
//if one regex creates a match, stop searching with the rest
				if (boost::regex_search (line, YearP, year.at (k)))
					break;
//repeat this process for the other eight regex fields
			for (int k = 0; k < month.size (); k ++)
				if (boost::regex_search (line, MonthP, month.at (k)))
					break;
			for (int k = 0; k < day.size (); k ++)
				if (boost::regex_search (line, DayP, day.at (k)))
					break;
			for (int k = 0; k < hour.size (); k ++)
				if (boost::regex_search (line, HourP, hour.at (k)))
					break;
			for (int k = 0; k < minute.size (); k ++)
				if (boost::regex_search (line, MinuteP, minute.at (k)))
					break;
			for (int k = 0; k < second.size (); k ++)
				if (boost::regex_search (line, SecondP, second.at (k)))
					break;
			for (int k = 0; k < device.size (); k ++)
				if (boost::regex_search (line, DeviceP, device.at (k)))
					break;
			for (int k = 0; k < event.size (); k ++)
				if (boost::regex_search (line, EventP, event.at (k)))
					break;
			for (int k = 0; k < state.size (); k ++)
				if (boost::regex_search (line, StateP, state.at (k)))
					break;
//if there was no time noted in the line, continue to the next line
			if (	YearP[0] == "" or
				MonthP[0] == "" or
				DayP[0] == "" or
				HourP[0] == "" or
				MinuteP[0] == "" or
				SecondP[0] == "")
				continue;
			else
			{
//add time notation to DATA
				check->year = YearP[0];
				check->month = MonthP[0];
				check->day = DayP[0];
				check->hour = HourP[0];
				check->minute = MinuteP[0];
				check->second = SecondP[0];
			}
//if there were matches for devices, events, or states, add those to EVENT
			if (!DeviceP.empty ())
				E->device_name = DeviceP[0];
			if (!EventP.empty ())
				E->event_name = EventP[0];
			if (!StateP.empty ())
				E->state = StateP[0];
//add EVENT to DATA
			check->events.push_back (*E);
			if (log_files.at (i).data.size () == 0)
			{
				log_files.at (i).data.push_back (*check);
			}
			else
			{
				CheckPos = log_files.at (i).is_same_data (*check);
				if (CheckPos != -1)
				{
					//std::cout << "position\t" << CheckPos << std::endl
					//	<< "data size\t" << log_files.at (i).data.size () << std::endl;
					log_files.at (i).merge_data (CheckPos, *check);
				}
				else
				{
					log_files.at (i).data.push_back (*check);
				}
			}
			delete check;
			delete E;
		}
	}
	for (int r = 0; r < log_files.size (); r ++)
	{
		std::cout << log_files.at (r).data.at (0).start << std::endl
				<< log_files.at (r).data.at (0).end << std::endl;
		log_files.at (r).highlight_time_point (0);
	}
	int highest = 0;
	for (int i = 0; i < log_files.size (); i ++)
	{
		std::cout << "data size\t" << log_files.at (i).data.size () << std::endl;
		if (log_files.at (i).data.size () > highest)
			highest = log_files.at (i).data.size ();
	}
	gtk_adjustment_set_upper (GTK_ADJUSTMENT (adjustment), highest);
	gtk_adjustment_set_upper (current_time, highest);
	generate_device_list (adjustment);
	generate_device_map (adjustment);
}

void error_window (char *error_string)
{
	/*
	GtkApplication *warning = gtk_application_new ("app.shi.error", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (warning, "activate", G_CALLBACK (error_window_dialogue), warning);
	g_application_run (G_APPLICATION (warning), 0, NULL);
	g_object_unref (warning);
	*/

	GtkWidget	*window,
			*box,
			*error_label,
			*ok_button;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	error_label = gtk_label_new (error_string);
	ok_button = gtk_button_new_with_label ("Ok");

	gtk_container_add (GTK_CONTAINER (window), box);
	gtk_container_add (GTK_CONTAINER (box), error_label);
	gtk_container_add (GTK_CONTAINER (box), ok_button);

	g_signal_connect_swapped (ok_button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
	gtk_widget_show_all (window);
}

void error_window_dialogue (GtkApplication *app, char *error_warning)
{
	GtkWidget	*window,
			*error_label,
			*ok_button;

	window = gtk_application_window_new (app);
	error_label = gtk_label_new (error_warning);

}

void load_from_json (GtkWidget *refresh)
{
	GtkWidget	*file_chooser,
			*window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	int i;
	char *filename;
	int *log_files_pos = (int*) g_object_get_data (G_OBJECT (refresh), "position");
	file_chooser = gtk_file_chooser_dialog_new (	"Open a Regex Set",
							GTK_WINDOW (window),
							GTK_FILE_CHOOSER_ACTION_OPEN,
							("_Cancel"),
							GTK_RESPONSE_CANCEL,
							("_Open"),
							GTK_RESPONSE_ACCEPT,
							NULL);
	i = gtk_dialog_run (GTK_DIALOG (file_chooser));
	if (i == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *a = GTK_FILE_CHOOSER (file_chooser);
		filename = gtk_file_chooser_get_filename (a);
	}
	gtk_widget_destroy (file_chooser);
	if (filename != nullptr)
	{
		log_files.at (*log_files_pos).Year_Regex.clear ();
		log_files.at (*log_files_pos).Month_Regex.clear ();
		log_files.at (*log_files_pos).Day_Regex.clear ();
		log_files.at (*log_files_pos).Hour_Regex.clear ();
		log_files.at (*log_files_pos).Minute_Regex.clear ();
		log_files.at (*log_files_pos).Second_Regex.clear ();
		log_files.at (*log_files_pos).Device_Regex.clear ();
		log_files.at (*log_files_pos).Event_Regex.clear ();
		log_files.at (*log_files_pos).State_Regex.clear ();
		std::ifstream b;
		b.open (filename);
		if (b.is_open ())
		{
			std::string a;
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('y'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('M'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('d'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('h'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('m'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('s'),
					a.c_str (),
					-1);;
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('D'),
					a.c_str (),
					-1);;
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('e'),
					a.c_str (),
					-1);
			}
			while (true and b.is_open ())
			{
				std::getline (b, a);
				if (a.empty ())
					break;
				gtk_entry_buffer_set_text (
					log_files.at (*log_files_pos).add_regex ('S'),
					a.c_str (),
					-1);
			}
		}
	}
	gtk_widget_show_all (refresh);
}
void save_to_json (int *log_files_pos)
{
	GtkWidget	*file_chooser,
			*window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	int i;
	char *filename;
	file_chooser = gtk_file_chooser_dialog_new (	"Save a Regex Set",
							GTK_WINDOW (window),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							("_Cancel"),
							GTK_RESPONSE_CANCEL,
							("_Save"),
							GTK_RESPONSE_ACCEPT,
							NULL);
	i = gtk_dialog_run (GTK_DIALOG (file_chooser));
	if (i == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *a = GTK_FILE_CHOOSER (file_chooser);
		filename = gtk_file_chooser_get_filename (a);
	}
	gtk_widget_destroy (file_chooser);
	if (filename != nullptr)
	{
		std::ofstream a;
		a.open (filename);
		for (int i = 0; i < log_files.at (*log_files_pos).Year_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Year_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Month_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Month_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Day_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Day_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Hour_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Hour_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Minute_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Minute_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Second_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Second_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Device_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Device_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).Event_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.Event_Regex.at (i)) << std::endl;
		}
		a << std::endl;
		for (int i = 0; i < log_files.at (*log_files_pos).State_Regex.size (); i ++)
		{
			a << gtk_entry_buffer_get_text (
				log_files.at (*log_files_pos)
				.State_Regex.at (i)) << std::endl;
		}
		a << std::endl;
	}

}

void save_project ()
{
	GtkWidget	*file_chooser,
			*window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	int i;
	char *filename;
	file_chooser = gtk_file_chooser_dialog_new (	"Open a Regex Set",
							GTK_WINDOW (window),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							("_Cancel"),
							GTK_RESPONSE_CANCEL,
							("_Save"),
							GTK_RESPONSE_ACCEPT,
							NULL);
	i = gtk_dialog_run (GTK_DIALOG (file_chooser));
	if (i == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *a = GTK_FILE_CHOOSER (file_chooser);
		filename = gtk_file_chooser_get_filename (a);
	}
	gtk_widget_destroy (file_chooser);
	if (filename != nullptr)
	{
		std::ofstream a;
		a.open (filename);
		for (int j = 0; j < log_files.size (); j ++)
		{
//bigass loop to print all the different regular expressions
			a << log_files.at (j).filename << std::endl;
			a << log_files.at (j).Year_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Year_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Year_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).Month_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Month_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Month_Regex.at (i)) << std::endl;;
			}
			a << log_files.at (j).Day_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Day_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Day_Regex.at (i)) << std::endl;;
			}
			a << log_files.at (j).Hour_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Hour_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Hour_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).Minute_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Minute_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Minute_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).Second_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Second_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Second_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).Device_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Device_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Device_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).Event_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).Event_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.Event_Regex.at (i)) << std::endl;
			}
			a << log_files.at (j).State_Regex.size () << std::endl;
			for (int i = 0; i < log_files.at (j).State_Regex.size (); i ++)
			{
				a << gtk_entry_buffer_get_text (
					log_files.at (j)
					.State_Regex.at (i)) << std::endl;
			}
//that hurt my hands
		}
	}
}
void open_project (GtkWidget *tabs)
{
//make variables for file dialog and reading in from save file
	GtkWidget 	*window = gtk_window_new (GTK_WINDOW_TOPLEVEL),
			*file_chooser = gtk_file_chooser_dialog_new (	"Open a Project",
									GTK_WINDOW (window),
									GTK_FILE_CHOOSER_ACTION_OPEN,
									("_Cancel"),
									GTK_RESPONSE_CANCEL,
									("_Open"),
									GTK_RESPONSE_ACCEPT,
									NULL);

	int i;
	char *OpenFile, *LogFile, CharReadIn;
	std::ifstream b;
	std::string HoldsLine;
//class pointer to make new objects as needed
	LOG_FILE_DATA *NewClass;
//run file dialog
	i = gtk_dialog_run (GTK_DIALOG (file_chooser));
	if (i == GTK_RESPONSE_ACCEPT)
	{
//get save file path from file dialog
		OpenFile = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));
	}
	gtk_widget_destroy (file_chooser);
//check file path actually was gotten
	if (OpenFile != nullptr)
	{
//empty current log files and read in from project file
		log_files.clear ();
		b.open (OpenFile);
		if (b.is_open ())
		{
			while (!b.eof ())
			{
				NewClass = new LOG_FILE_DATA;
//read in first log file path (or empty line for a bad project file)
				std::getline (b, HoldsLine);
				if (HoldsLine.empty ())
					break;
//set the file path for the new class object
				NewClass->filename = HoldsLine;
				std::cout << HoldsLine << std::endl;
				std::string::size_type SizeType;
				int NumberOfReg;

				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('y'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('M'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('d'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('h'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('m'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('s'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('D'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('e'), HoldsLine.c_str (), -1);
				}
				std::getline (b, HoldsLine);
				NumberOfReg = std::stoi (HoldsLine, &SizeType);
				for (int i = 0; i < NumberOfReg; i ++)
				{
					std::getline (b, HoldsLine);
					gtk_entry_buffer_set_text (NewClass->add_regex ('S'), HoldsLine.c_str (), -1);
				}

				log_files.push_back (*NewClass);
			}
		}
	}
//clear all current notebook pages
	for (int z = 0; z < gtk_notebook_get_n_pages (GTK_NOTEBOOK (tabs)); z ++)
		gtk_notebook_remove_page (GTK_NOTEBOOK (tabs), z);
		for (int x = 0; x < log_files.size (); x ++)
		{
			GtkWidget	*scroll = gtk_scrolled_window_new (NULL, NULL),
					*text,
					*box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0),
					*set_regex = gtk_button_new_with_label ("Set Regular Expressions"),
					*close = gtk_button_new_from_icon_name ("gtk-close", GTK_ICON_SIZE_SMALL_TOOLBAR);
			std::string LogFileName = log_files.at (x).filename;
			std::cout << LogFileName << std::endl;
			int page;
			std::string str, sub;
			str = LogFileName;
//get end of string to make filename for tab title
			sub = str.substr (str.rfind ("/", std::string::npos) + 1, std::string::npos);
			std::ifstream in;
			std::string contents;
			std::stringstream buffer;
			in.open (log_files.at (x).filename);
			if (in.is_open ())
			{
				buffer << in.rdbuf ();
				contents = buffer.str ();
				gtk_text_buffer_set_text (log_files.at (x).get_text_file (), contents.c_str (), -1);
			}
			gtk_box_pack_start (GTK_BOX (box), set_regex, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box), scroll, TRUE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (box), close, FALSE, TRUE, 0);
			gtk_notebook_append_page (GTK_NOTEBOOK (tabs), box, NULL);
			gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (tabs), box, sub.c_str());

			text = gtk_text_view_new_with_buffer (GTK_TEXT_BUFFER (log_files.at (x).get_text_file()));
			page = gtk_notebook_get_n_pages (GTK_NOTEBOOK (tabs));

			gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), false);
			gtk_text_view_set_editable (GTK_TEXT_VIEW (text), false);
			gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (text), false);
			gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_NONE);
			gtk_text_view_set_monospace (GTK_TEXT_VIEW (text), GTK_WRAP_NONE);
			gtk_text_view_set_justification (GTK_TEXT_VIEW (text), GTK_JUSTIFY_LEFT);
			gtk_scrollable_set_vscroll_policy (GTK_SCROLLABLE (text), GTK_SCROLL_NATURAL);
			gtk_scrollable_set_hscroll_policy (GTK_SCROLLABLE (text), GTK_SCROLL_NATURAL);
			gtk_container_add (GTK_CONTAINER (scroll), text);
			gtk_widget_show_all (tabs);

			struct _ {GtkWidget *y; int z;};
			_*to_remove = new _;
			to_remove->y = tabs; to_remove->z = page - 1;
			g_signal_connect_swapped (close, "clicked", G_CALLBACK (remove_page), to_remove);

			to_regex *i = new to_regex;
			i->pos = page -1;
			g_signal_connect_swapped (set_regex, "clicked", G_CALLBACK (set_regular_expressions), i);
		}
}
//when you move the scrubber, this gets fired off to reflect the changes in
//the log files' active time point
void scrubber_change_time (GtkAdjustment *adjustment)
{
	if (log_files.size () < 1)
		return;
	gtk_adjustment_set_value (current_time, gtk_adjustment_get_value (adjustment));
	for (int i = 0; i < log_files.size (); i ++)
	{
		log_files.at (i).set_current_data (gtk_adjustment_get_value (current_time));
	}
	return;
}
//moves the current time forward
void skip_forward (GtkAdjustment *adjustment)
{
	if (log_files.size () < 1)
		return;
	gtk_adjustment_set_value (current_time, gtk_adjustment_get_value (current_time) + 1);
	gtk_adjustment_set_value (adjustment, gtk_adjustment_get_value(current_time));
	for (int i = 0; i < log_files.size (); i ++)
	{
		log_files.at (i).set_current_data (gtk_adjustment_get_value (current_time));
	}
}
//moves time backward. really!
void skip_backward (GtkAdjustment *adjustment)
{
	if (log_files.size () < 1)
		return;
	gtk_adjustment_set_value (current_time, gtk_adjustment_get_value (current_time) - 1);
	gtk_adjustment_set_value (adjustment, gtk_adjustment_get_value(current_time));
	for (int i = 0; i < log_files.size (); i ++)
	{
		log_files.at (i).set_current_data (gtk_adjustment_get_value (current_time));
	}
}
//it plays...
void play (GtkAdjustment *adjustment)
{
	if (log_files.size () < 1)
		return;
	if (Playing == true)
		return;
	//gdk_threads_add_idle (G_SOURCE_FUNC(play_loop), adjustment);
	Playing = true;
	std::thread (play_loop, adjustment).detach ();
	return;
}
//called by play because it needs to be on a separate thread for some
//godforsaken reason
void play_loop (GtkAdjustment *adjustment)
{
	while (Playing)
	{
		if (gtk_adjustment_get_value (adjustment) >= gtk_adjustment_get_upper (adjustment))
			break;
		gtk_adjustment_set_value (current_time, gtk_adjustment_get_value (current_time) + 1);
		gtk_adjustment_set_value (adjustment, gtk_adjustment_get_value(current_time));
		gtk_widget_show_all (gtk_widget_get_parent (GTK_WIDGET (adjustment)));
		std::this_thread::sleep_for (std::chrono::milliseconds (200));
	}
	return;
}
//guess what this function does?
void stop ()
{
	Playing = false;
}

//it generates...
//a device list!
void generate_device_list (GtkAdjustment *adjustment)
{
	GtkWidget 	*DevList = (GtkWidget*)g_object_get_data (G_OBJECT (adjustment), "dev_list"),
			*DevListScroll = (GtkWidget*)g_object_get_data (G_OBJECT (adjustment), "dev_list_scroll");
//get rid of all the previous crap displayed in the box
	gtk_widget_destroy (DevList);
//after emptying the box, put it back
	DevList = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (DevListScroll), DevList);
	std::vector <GtkWidget*> list_of_widgets;
	GtkWidget 	*labels,
			*boxes;
//loop through all the log files and do sum shit
	for (int a = 0; a < log_files.size (); a ++)
	{

	}
}

//it makes a map yay!
void generate_device_map (GtkAdjustment *adjustment)
{
	return;
}
void draw_on_device_map (GtkWidget *widget, cairo_t *cr, gpointer data)
{

}
