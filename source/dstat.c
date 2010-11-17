#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <dstat.h>



#define STATUS_LENGTH 128



static int fail( const char * message );



int main( int argc, char * argv[] )
{
	Display * display;
	int screen;
	Window window;
	char status[ STATUS_LENGTH ];
	time_t secs;
	struct tm * now;
	FILE * config;


	config = fopen( "/etc/dstat.conf", "rb" );
	if( config != NULL )
	{
		fclose( config );
	}


	for(;;)
	{

		display = XOpenDisplay( NULL );
		if( display == 0 )
			return fail( "can`t open display" );
	
		screen = XDefaultScreen( display );
		window = XRootWindow( display, screen );

	       	time( & secs );
		now = localtime( & secs );

		snprintf( status, STATUS_LENGTH,
			"%d:%02d", now->tm_hour, now->tm_min );

		XStoreName( display, window, status );

		XCloseDisplay( display );

		sleep( 1 );
	}

	return 0;
}



static int fail( const char * message )
{
	fprintf( stderr, "dstat error : %s\n", message );
	return 1;
}

