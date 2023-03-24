/*  TRPFILE.C
*/

#pragma NESTCMNT

#include <stdio.h>
#include <direct.h>
#include <dos.h>
#include <bios.h>
#include <string.h>
#include <malloc.h>
#include "trpdefs.h"
#include "trpvers.h"

extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;

#define MAX_LINES   150

#pragma trace-
void calc_out( void )
{
extern char infile_name[], errfile_name[];
extern char buffer[];

int ctr=0, i, length, top=0, bottom=18, start_char=2;
char c;
char far *line[MAX_LINES];             /* 150 lines = 12k of far heap */
struct WINDOW view = { 5, 1, 23, 78, "µ OUTPUT Æ", cyan_b + black_f };
FILE *f_out;

    /* First, fill array with file lines */

    f_out = fopen( infile_name, "r" );          /* open file */
    if( f_out == NULL ) f_out = fopen( errfile_name, "r" );
    if( f_out == NULL ) {
        say_error( "Can't open output file." );
        return;
        }
    else {
        cursoff();
        while( ((fgets( buffer, 300, f_out )) != NULL) && /* cont til EOF */
                (ctr < MAX_LINES) ) {

            buffer[101] = '\0';          /* Don't let line len exceed this */
            length = strlen( buffer );

            /* Sift out control chars */
            for( i=0; i<length; i++ ) {
                if( (buffer[i] == ENTER) ||
                    (buffer[i] == 0x0A)  ||
                    (buffer[i] == 0x0C)
                  ) buffer[i] = ' ';
                } /* for */
            while( strlen( buffer ) < 100 ) strcat( buffer, " " );
            length = strlen( buffer );

            /* Allocate storage for line */
            line[ctr] = farmalloc( length+1 );
            /* Store line */
            if( line[ctr] != NULL ) farstrcpy( line[ctr], buffer );
            ctr++;

            } /* while */

        fclose( f_out );
        } /* else */

    /* Ok, now array full */
    make_window( view, FALSE );
    for( i=0; i<19; i++ ) {
        poscurs( view.topleftrow+i, 1 );
        sprintf( buffer, "%.78Fs", line[i]+2 );
        printa( buffer, view.attr );
        } /* for */

    do {
        drain();
        c = getch();
        switch( c ) {
            case ESC: break;
            case FUNCTION: /* Function key pressed */
                c = getch();
                switch( c ) {
                    case UPAROW:
                        if( top == 0 ) sound( 200, 5 );
                        else {
                            top--; bottom--;
                            scroll( view, 1, DOWN );
                            sprintf( buffer, "%.78Fs", line[top]+start_char );
                            printa( buffer, view.attr );
                            }
                        break;
                    case DNAROW:
                        if( bottom == ctr-1 ) sound( 200, 5 );
                        else {
                            top++; bottom++;
                            scroll( view, 1, UP );
                            sprintf( buffer, "%.78Fs", line[bottom]+start_char );
                            printa( buffer, view.attr );
                            }
                        break;
                    case LTAROW:
                        if( start_char == 0 ) sound( 200, 5 );
                        else {
                            start_char--;
                            for( i=0; i<19; i++ ) {
                                poscurs( view.topleftrow+i, 1 );
                                sprintf( buffer, "%.78Fs", line[i+top]+start_char );
                                printa( buffer, view.attr );
                                } /* for */
                        } /* else */
                        break;
                    case RTAROW:
                        if( start_char == 20 ) sound( 200, 5 );
                        else {
                            start_char++;
                            for( i=0; i<19; i++ ) {
                                poscurs( view.topleftrow+i, 1 );
                                sprintf( buffer, "%.78Fs", line[i+top]+start_char );
                                printa( buffer, view.attr );
                                } /* for */
                            } /* else */
                        break;
                    default:
                        view_help();
                        break;
                    } /* switch function */
                break;
            default:
                view_help();
                break;
            } /* switch userchar */
        } /* do */
    while( c != ESC );

    /* Release lines in heap */
    for( i=0; i<ctr; i++ ) farfree( line[i] );
    curson();

} /* calc_out */


#pragma trace-
void view_help( void )
{
extern char buffer[];

    int i;
    int *under_help;
    struct WINDOW help = { 10, 10, 15, 34, "µ HELP Æ", red_b + white_f };
    char *help_l[] = { "\x18   - scroll up",
                       "\x19   - scroll down",
                       "\x1B   - scroll left",
                       "\x1A   - scroll right",
                       "ESC - return to main menu",
                        "      ..press any key..",
                     };

    sound( 200, 5 );
    under_help = make_window( help, TRUE );
    for( i=0; i<6; i++ ) {
        poscurs( help.topleftrow+i, help.topleftcol );
        sprintf( buffer, "%s", help_l[i] );
        printa( buffer, help.attr );
        }
    if( getch() == FUNCTION ) getch();;
    take_window( help, under_help );

} /* view_help */


#pragma trace-
BOOL delete( char f_spec[] )
{ /* Delete file f_spec[] */

    remove( f_spec );

} /* delete */

#ifndef DEMO
#pragma trace-
BOOL savefile( void )
{
extern int model;
extern struct TESTFORM engine;
extern struct ffblk filedata;

int  success;
char h, l;
char ext[5] = " ";
char loc_buff[14] = "SN";
char tst_name[14];

    /* First, append serial # to "SN" */
    ltoa( engine.serial, loc_buff+2, 10 );
    strcat( loc_buff, "." );              /* Insert the dot */
    ext[0] = model + 48;                  /* Starts at TPE3 = 0 */
    strcat( loc_buff, ext );              /* append model numeral */

    ext[2] = '\0';
    for( h='A'; h<='Z'; h++ ) {
        ext[0] = h;
        for( l='A'; l<='Z'; l++ ) {
            ext[1] = l;
            strcpy( tst_name, loc_buff );         /* make a copy for test */
            strcat( tst_name, ext );              /* add extent test */
            if( findfirst( tst_name, &filedata, FA_NORMAL ) != 0 ) {
                /* Found free name */
                success = put_in_file( tst_name );
                if( success == TRUE ) return TRUE;
                else return FALSE;
                } /* if findfirst */
            } /* for l */
        } /* for h */

} /* savefile */

#else
/* DEMO = T */
BOOL savefile( void )
{
    int *under_msg;
    struct WINDOW msg = { 10, 20, 10, 54, "µ DEMO Æ", black_b + ltred_f };

    cursoff();
    under_msg = make_window( msg, TRUE );
    printa( "Sorry.. the demo won't save files.", msg.attr );
    sleep( 3 );
    take_window( msg, under_msg );
    curson();

    return TRUE;

} /* demo savefile */
#endif

#pragma trace-
BOOL getfiles( void )
{
extern char buffer[];

    int sel;
    char loc_buff[14] = "SN";
    struct WINDOW getfile = { 10, 5, 11, 35, "µ LOAD Æ", brown_b + yellow_f };

    make_window( getfile, FALSE );
    printa( "Input serial number of engine", getfile.attr );
    poscurs( getfile.botrightrow, getfile.topleftcol );
    printa( "or ENTER ÄÙ for all: ", getfile.attr );
    sel = getline( buffer, 6 );
    if( sel == -1 ) return FALSE;
    if( (sel == 0) && (buffer[0] != '\0') ) {
        strcat( loc_buff, buffer );             /* Tack on serial */
        strcat( loc_buff, ".*" );               /* Tack on ambiguous ext */
        sel = selfile( loc_buff );
        }
    else sel = selfile( "SN*.*" );
    if( sel == FALSE ) {
        printa( "No file loaded.  Press any key for main menu.", green_b + blue_f );
        getch();
        return FALSE;
        }
    else return TRUE;

} /* getfiles */



#pragma trace-
BOOL selfile( char f_spec[] )
{
    extern struct ffblk filedata;
    extern BOOL formmade;
    extern char buffer[];
    extern curfile[];
    extern char filenames[5][14];

    int choice=0, findstat, i=0;
    BOOL found = FALSE;
    struct WINDOW oldfile = { 6, 5, 21, 74, "µ LOAD Æ", green_b + blue_f };

    make_window( oldfile, FALSE );
    if( findfirst( f_spec, &filedata, FA_NORMAL ) == 0 ) {
        found = TRUE;
        strcpy( filenames[i], filedata.ff_name );  /* save filenames */
        listfiles( i, &filedata );
        i++;
        while( (findstat = findnext( &filedata )) == 0 ) {
            found = TRUE;
            strcpy( filenames[i], filedata.ff_name );  /* save filenames */
            poscurs( oldfile.topleftrow+i*3, oldfile.topleftcol );
            listfiles( i, &filedata );
            i++;
            if( i == 5 ) {
                found = FALSE;
                poscurs( oldfile.botrightrow, oldfile.topleftcol );
                choice = get_fchoice( i );
                if( choice != 0 ) break; /* User selected */
                i = 0;
                scroll( oldfile, 0, UP );
                }
            } /* while */

        /* Didn't fill file block.. any left? */
        if( (choice == 0) && found ) {
            /* If so, ask user about these */
            poscurs( oldfile.botrightrow, oldfile.topleftcol );
            choice = get_fchoice( i );
            scroll( oldfile, 0, DOWN );
            }
        if( choice > 0 ) { /* File selected so get it */
            scroll( oldfile, 0, DOWN );
            sprintf( buffer, "Loading file: %s", &filenames[--choice] );
            printa( buffer, oldfile.attr );
            formmade = file2edit( &filenames[choice] ); /* Set formmade if success */
            if( formmade ) strcpy( curfile, &filenames[choice] ); /* Set current filename */
            poscurs( oldfile.topleftrow+1, oldfile.topleftcol );
            return formmade; /* TRUE if good read */
            }
        scroll( oldfile, 0, DOWN );
        return FALSE; /* User ESCaped */

        } /* if findfirst() */
    else {
        printa( "No files found.", oldfile.attr );
        poscurs( oldfile.topleftrow+1, oldfile.topleftcol );
        return FALSE;
        }

} /* getfiles */

#pragma trace-
void listfiles( int ref_nr, struct ffblk *filedata )
{
    extern char buffer[];

    int row, col, i;
    char loc_buff[10], store[101];
    FILE *f_in;

    row = cursrow();
    col = curscol();
    cursoff();
    for( i=0; (i<8) &&
              (filedata->ff_name[i]!='.') &&
              (filedata->ff_name[i]!='\0')
              ; i++ ) loc_buff[i] = filedata->ff_name[i];
    loc_buff[i] = '\0';

    sprintf( buffer, "%01i", ref_nr+1 );   /* reference nr */
    printa( buffer, green_b + brtwhite_f );
    sprintf( buffer, "  %-8s %02d-%02d-%02d %02d:%02d",
        loc_buff,                                   /* Serial nr    */
        (filedata->ff_fdate >> 5) &0x0f,            /* month        */
        (filedata->ff_fdate & 0x1f),                /* day          */
        ((filedata->ff_fdate >> 9) & 0x7f) + 80,    /* year         */
        (filedata->ff_ftime >> 11) & 0x1f,          /* hour         */
        (filedata->ff_ftime >> 5) & 0x3f            /* minute       */
          );
    printa( buffer, green_b + blue_f );
    row++;
    poscurs( row, col );
    f_in = fopen( filedata->ff_name, "r" );         /* Get comment from file */
    fgets( store, 100, f_in );
    sprintf( buffer, "%12s%.58s", "", store+9 );        /* print comment */
    printa( buffer, green_b + blue_f );
    fclose( f_in );
    row += 2;
    poscurs( row, col );
    curson();

} /* listfiles */


#pragma trace-
int get_fchoice( int nr_choices )
{
extern char buffer[];

    char c;

    printa( "Press selection number ", green_b + brown_f );
    printa( "[1", green_b + brtwhite_f );

    if( nr_choices == 1 ) {
        printa( "]", green_b + brtwhite_f );
        printa( ", or ", green_b + brown_f );
        printa( "ENTER", green_b + brtwhite_f );
        }
    else if( nr_choices == 5 ) {
        printa( "-5]", green_b + brtwhite_f );
        printa( ",  ", green_b + brown_f );
        printa( "ENTER", green_b + brtwhite_f );
        printa( " for more, ", green_b + brown_f );
        printa( "ESC", green_b + brtwhite_f );
        }
    else {
        sprintf( buffer, "-%i]", nr_choices );
        printa( buffer, green_b + brtwhite_f );
        printa( ", or ", green_b + brown_f );
        printa( "ENTER", green_b + brtwhite_f );
        }
    printa( " to return: ", green_b + brown_f );
    curson();
    while( TRUE ) {
        c = getch();
        if( c == FUNCTION ) { /* Dump any function key */
            getch();
            }
        if( c == ESC ) return -1; /* Escape pressed */
        if( isdigit( c ) ) {
            c -= 48;        /* Convert to number */
            if( (c > 0) && (c <= nr_choices) ) return c;
            }
        if( c == ENTER ) return 0;
        else sound( 200, 5 );
        }

} /* get_fchoice */
