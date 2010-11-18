#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <dstat.h>

#define LOCAL_CONF "/.dstat.conf"



static const int MESSAGE_LENGTH = 128;
static const int MAX_CONFIG_SIZE = 2048;
static const int MAX_HOME_LENGTH = _MAXPATH - sizeof( LOCAL_CONF );



enum config_param { NO_PARAM, PARAM_DATE,
			       PARAM_TIME, COUNT_PARAMS };
enum config_arg { NO_ARG, ARG_HIDE, ARG_DDMM, ARG_MMDD,
			   ARG_DDMMYYYY, ARG_MMDDYYYY,
			   ARG_HHMM, ARG_HHMMSS,
			   COUNT_ARGS };

			   

struct param_desc
{
	const char * title;
	const enum config_arg * valid_args;
	int count_args;
};



struct param_arg
{
	config_param param;
	config_arg arg;
};


struct config
{
	struct param_arg order[ COUNT_PARAMS ];
	int nof_params;
}



static const char * ARGS[ COUNT_ARGS ] = {
	"", "hide",
	"dd.mm", "mm.dd", "dd.mm.yyyy", "mm.dd.yyyy",
	"hh:mm", "hh:mm:ss"
};

static const config_arg NO_ARGS[] = { NO_ARG };

static const config_arg DATE_ARGS[] = {
	ARG_HIDE, ARG_DDMM, ARG_MMDD, ARG_DDMMYYYY, ARG_MMDDYYYY
};

static const config_arg TIME_ARGS[] = {
	ARG_HIDE, ARG_HHMM, ARG_HHMMSS
};

#define PARAM( title, arr ) { title, arr, sizeof( arr ) / sizeof( config_type ) }
static const param_desc PARAMS[ COUNT_PARAMS ] = {
	PARAM( "", NO_ARGS ),
	PARAM( "date", DATE_ARGS ),
	PARAM( "time", TIME_ARGS )
};
#undef PARAM


static const struct param_arg DEF_CONFIG[] = {
	{ PARAM_TIME, TIME_HHMM }
};



static int fail( const char * message );
static void report( const char * message );
static void report_string( const char * message, const char * param );
static bool parse_config( const char * path, struct config * config );
static int format( char * message, const struct config * config );
static int format_date( char * cursor, int limit, enum config_arg arg );
static int format_time( char * cursor, int limit, enum config_arg arg );



int main( int argc, char * argv[] )
{
	Display * display;
	int screen;
	Window window;
	char status[ MESSAGE_LENGTH + 1 ];
	time_t secs;
	struct tm * now;
	struct config config;
	char home_conf[ _MAXPATH + 1 ];
	
	strncpy( home_conf, getenv( "HOME" ), MAX_HOME_LENGTH );
	home_conf[ MAX_HOME_LENGTH ] = '\0';
	strcat( home_conf, LOCAL_CONF );

	if( ! parse_config( "/etc/dstat.conf", & config ) &&
	    ! parse_config( home_conf, & config ) )
	{
		config.nof_params = sizeof( DEF_CONFIG ) /sizeof( struct param_arg );
		memcpy( config.order, DEF_CONFIG, sizeof( DEF_CONFIG ) );
	}

	for(;;)
	{

		display = XOpenDisplay( NULL );
		if( display == 0 )
			return fail( "can`t open display" );
	
		screen = XDefaultScreen( display );
		window = XRootWindow( display, screen );
		
		if( format( status, & config ) != 0 )
			return -1;

		XStoreName( display, window, status );

		XCloseDisplay( display );

		usleep( 500000 );
	}

	return 0;
}



static int fail( const char * message )
{
	assert( message != NULL );
	
	report( message );
	
	return -1;
}



static void report( const char * message )
{
	assert( message != NULL );
	
	fprintf( stderr, "dstat error : %s\n", message );
}



static void report_string( const char * message, const char * param )
{
	char result[ MESSAGE_LENGTH + 1 ];
	
	assert( message != NULL );
	assert( param != NULL );
	
	snprintf( result, MESSAGE_LENGTH, message, param );
	result[ MESSAGE_LENGTH ] = '\0';
	
	report( result );
}



static bool parse_config( const char * path, struct config * config )
{
	int file = -1;
	long file_size;
	char * source = NULL;
	ssize_t result;
	static const char * delimiters = " \r\n\t=";
	char * param;
	char * arg;
	const struct param_desc * param_desc;
	enum config_param config_param;
	struct param_arg * param_arg;
	int index_arg;
	
	assert( path != NULL );
	assert( config != NULL );
	
	config->nof_params = 0;

	file = open( path, O_RDONLY );
	if( file == -1 )
		return 0;
	
	file_size = filelength( file );
	if( file == -1 )
		goto error_sizing;
	if( file_size > MAX_CONFIG_SIZE )
		goto error_too_big;
	
	source = malloc( file_size + 1 );
	if( source == NULL )
		goto error_allocating;
	
	result = read( file, source, file_size );
	if( result != file_size )
		goto error_reading;
	
	source[ file_size ] = '\0';
	close( file );
	file = -1;
	
	param = strtok( source, delimiters );
	while( param != NULL )
	{
		arg = strtok( NULL, delimiters );
		if( arg == NULL )
			goto error_no_arg;
		
		config_param = NO_PARAM;
		while( config_param < COUNT_PARAMS )
		{
			if( strcmp( param, PARAMS[ config_param ].title ) == 0 )
				break;
			
			++config_param;
		}
		
		if( config_param == COUNT_PARAMS )
			goto error_param;
		
		param_desc = & PARAMS[ config_param ];
		
		index_arg = 0;
		while( index_arg < param_desc->count_args )
		{
			if( strcmp( arg, ARGS[ param_desc->valid_args[ index_arg ] ] ) == 0 )
				break;
			
			++index_arg;
		}
		
		if( index_arg == param_desc->count_args )
			goto error_arg;

		param_arg = & config->order[ config->nof_params ];
		param_arg->param = config_param;
		param_arg->arg = param_desc->valid_args[ index_arg ];
		
		++config->nof_params;
		
		param = strtok( NULL, delimiters );
	}
	
	free( source );
	source = NULL;
	
	return true;

error_sizing:
	report( "can`t determine config size" ); goto cleanup;
	
error_too_big:
	report( "config is too big" ); goto cleanup;
	
error_allocating:
	report( "not enough memory to load config" ); goto cleanup;
	
error_reading:
	report( "can`t read config" ); goto cleanup;
	
error_no_arg:
	report_string( "parameter `%s` has no argument",  param ); goto cleanup;
	
error_param:
	report_string( "unknown parameter `%s`", param ); goto cleanup;
	
error_arg:
	report_string( "unknown argument for parameter `%s`", param ); goto cleanup;
	
cleanup:
	if( file != -1 )
		close( file );
	if( source != NULL )
		free( source );
	return false;
}


static int format( char * message, const struct config * config )
{
	char * cursor;
	int limit = MESSAGE_LENGTH;
	int formatted;
	struct param_arg * param_arg;
	
	assert( message != NULL );
	assert( config != NULL );
	
	message[ 0 ] = '\0';
	cursor = message;
	
	for( int i = 0; i < config->nof_params; ++i )
	{
		param_arg = & config->order[ i ];
		switch( param_arg->param )
		{
			case PARAM_DATE:
				formatted = format_date( cursor, limit, param_arg->arg );
				break;
			case PARAM_TIME:
				formatted = format_time( cursor, limit, param_arg->arg );
				break;
			default:
				return fail( "parameters parsed incorrectly" );
		}
		
		if( formatted < 0 )
			return fail( "can`t format parameter" );
		limit -= formatted;
		cursor += formatted;
	}
	
	return 0;
}




static int format_date( char * cursor, int limit, enum config_arg arg )
{
	time_t raw_time;
	struct tm * now;
	int d, m, y;
	
	time( & raw_time );
	now = localtime( & raw_time );
	
	d = now->tm_mday;
	m = now->tm_mon + 1;
	y = now->tm_year + 1900;
	
	switch( arg )
	{
		case ARG_MMDD:
			return snprintf( cursor, limit, "%02d:%02d", m, d );
		case ARG_DDMM:
			return snprintf( cursor, limit, "%02d:%02d", d, m );
		case ARG_MMDDYYYY:
			return snprintf( cursor, limit, "%02d:%02d:%04d", m, d, y );
		case ARG_DDMMYYYY:
			return snprintf( cursor, limit, "%02d:%02d:%04d", d, m, y );
		default:
			return fail( "`date` parsed incorrectly" );
	}
}



static int format_time( char * cursor, int limit, enum config_arg arg )
{
	time_t raw_time;
	struct tm * now;
	int h, m, s;
	
	time( & raw_time );
	now = localtime( & raw_time );
	
	h = now->tm_hour;
	m = now->tm_min;
	s = now->tm_sec;
	
	switch( arg )
	{
		case ARG_HHMM:
			return snprintf( cursor, limit, "%d:%02d", h, m );
		case ARG_HHMMSS:
			return snprintf( cursor, limit, "%d:%02d:%02d", h, m, s );
		default:
			return fail( "`time` parsed incorrectly" );
	}
}
