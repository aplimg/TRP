/*  CONVERT.C

    Steven R. Stuart January 29, 1990

    v1.0    1-29-90
    v1.1    5-21-90 Changed ftoa5() to work with -.nnn instead of -0.nn
*/

#pragma trace-
#pragma NESTCMNT

#include <stdio.h>
#include <direct.h>
#include <dos.h>
#include <bios.h>
#include <string.h>
#include <malloc.h>
#include "trpdefs.h"

#define MAX_LINES   100

void main( int argc, char *argv[] )
{
    clrscrn();
    printf( "Turbine Data Reduction Processor file conversion. v1.1\n" );
	printf( "Copyright (C)1990 Applied Imagination, Inc.\n" );
    printf( "All Rights Reserved.\n\n" );

    if( argc == 3 ) convert( argv[1], argv[2] );
    else usage();

} /* main */


void convert( char mdl[], char f_name[] )
{
int model, line_nr, status = TRUE;
int i, month, day, year, rpm, fac;
long sn;
char *tokn, *part;
char *lines[MAX_LINES];
char buffer[122], buffer2[120];
int xi1, xi2;
float xf1, xf2, xf3, xf4;
FILE *f_in;

    if( strcmpi( "-3", mdl ) == 0 )          model = MDL_TPE3;
    else if( strcmpi( "-5", mdl ) == 0 )     model = MDL_TPE5;
    else if( strcmpi( "-6", mdl ) == 0 )     model = MDL_TPE6;
    else if( strcmpi( "-8", mdl ) == 0 )     model = MDL_TPE8;
    else if( strcmpi( "-10UA", mdl ) == 0 )  model = MDL_TPE10UA;
    else if( strcmpi( "-10", mdl ) == 0 )    model = MDL_TPE10;
    else if( strcmpi( "-11", mdl ) == 0 )    model = MDL_TPE11;
    else {
        usage();
        return;
    }

    f_in = fopen( f_name, "r" );
    if( f_in == NULL ) {
        printf( "Cannot open file %s\n", f_name );
        return;
        }

    while( ! strstr( buffer, "99999" ) ) { /* end of file */
        buffer[0] = '\0';
        line_nr = 0;
        while( (! strstr( buffer, "99999" )) &&
               (! strstr( buffer, "88888" )) ) {
            status = fgets( buffer, 120, f_in );
            if( status == NULL ) {
                printf( "Premature EOF error in file %s\n", f_name );
                return;
            }
            switch( line_nr+1 ) {
                case 1: /* check date & length */
                    part = buffer; /* Point to buffer first time around */
                    for( i=0; i<3; i++ ) { /* Get date parts */
                        tokn = strtok( part, "/- " );
                        if( tokn == NULLCHARPTR ) break;
                        part = NULLCHARPTR;
                        switch( i ) { /* Form start date */
                            case 0: /* Month */
                                month = atoi( tokn );
                                break;
                            case 1: /* Day */
                                day = atoi( tokn );
                                break;
                            case 2: /* Year */
                                year = atoi( tokn );
                                break;
                        } /* switch */
                    } /* for */
                    sprintf( buffer2, "%02i-%02i-%02i %s",
                                      month, day, year, tokn+3 );
                    strcpy( buffer, buffer2 );
                    buffer[74] = '\0';
                    sift( buffer, '\n' );
                    while( strlen( buffer ) < 73 ) strcat( buffer, " " );
                    strcat( buffer, "\n" );
                    break;
                case 2: /* get sn */
                    sift( buffer, '.' );
                    sift( buffer, ',' );
                    sscanf( buffer, "%li %i %i", &sn, &rpm, &fac ); /* get serial nr */
                    break;
                case 3:
                    sift( buffer, ',' );
                    if( fac == 1 ) { /* On wing facility? */
                        sscanf( buffer, "%f %f %f %f", &xf1, &xf2, &xf3, &xf4 );
                        ftoa5( xf1, buffer2 );
                        xi1 = atoi( buffer2 );
                        ftoa5( xf2, buffer2 );
                        xi2 = atoi( buffer2 );
                        sprintf( buffer, "%i %i %f %f\n", xi1, xi2, xf3, xf4 );
                    }
                    break;
                default:
                    sift( buffer, ',' );
                    break;
            } /* switch */
            lines[line_nr] = malloc( strlen( buffer ) + 1 ); /* allocate storage */
            strcpy( lines[line_nr], buffer ); /* put in storage */
            if( ++line_nr == MAX_LINES ) {
                printf( "Line count error in file %s\n", f_name );
                return;
            }
        } /* while */
        if( ! savefile( model, sn, lines, line_nr-1 ) ) {
            printf( "Could not make new file\n" );
            return;
        } /* while */
    } /* while */

    return;

} /* convert */


void sift( char siftline[], char siftchar )
{
int i;

    for( i=0; i<strlen(siftline); i++ ) {
        if( siftline[i] == siftchar ) siftline[i] = ' ';
    }

} /* sift */



BOOL savefile( int model, long serial, char *lines[], int nr_lines )
{

int  i, status;
char h, l;
char ext[5] = " ";
char loc_buff[14] = "SN";
char tst_name[14];
struct ffblk filedata;
FILE *f_out;

    /* First, append serial # to "SN" */
    ltoa( serial, loc_buff+2, 10 );
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
                f_out = fopen( tst_name, "w" );
                if( f_out == NULL ) {
                    printf( "Could not create file\n" );
                    exit( 1 );
                }
                printf( "Creating file %s\n", tst_name );
                for( i=0; i<nr_lines; i++ ) {
                    status = fputs( lines[i], f_out );
                    if( status != 0 ) {
                        printf( "Error writing to file" );
                        exit( 1 );
                    }
                }
                fputs( "99999 9*0\n", f_out ); /* End of data flag */
                fclose( f_out );
                return;
            } /* if findfirst */
        } /* for l */
    } /* for h */
    printf( "Too many files for SN%l\n", serial );
} /* savefile */


void ftoa5( float f, char *t ) /* Float to 5 char ascii */
{
        /* Sets up "t" with the proper 5 char numberic string */

    int i;
    char buffer[81];

    if( f == 0 ) {
        strcpy( t, "    0");
        return;
        }
    ftoa( f, buffer, 0, 5, 3 ); /* Convert to ascii */
    while( buffer[0] == ' ' )   /* Remove leading spaces */
        for( i=0; (i<80 && buffer[i]!=NULL); i++ )
            buffer[i] = buffer[i+1];
    if( f < 0F && f > -1F ) {   /* if -1 < f < 0 make it "-.n" */
        strncpy( t, buffer+1, 5 );      /* skip "-0" get next 5 signif digits */
        t[0] = '-';                             /* reinsert "-" */
        }
    else strncpy( t, buffer, 5 ); /* Get 5 most significant digits */
    t[5] = NULL; /* Force end of string */
    while( strlen( t ) < 5 ) {  /* Insert leading space(s) if not 5 chars */
        for( i=4; i!=0; i-- ) t[i] = t[i-1];
        t[0] = ' ';
        }
    return;

} /* ftoa5 */


void usage( void )
{   printf( "\n\nusage: convert -model fname\n\n" );
    printf( "         model [-3]    TPE331-3\n" );
    printf( "         model [-5]    TPE331-5\n" );
    printf( "         model [-6]    TPE331-6\n" );
    printf( "         model [-8]    TPE331-8\n" );
    printf( "         model [-10]   TPE331-10\n" );
    printf( "         model [-10UA] TPE331-10UA\n" );
    printf( "         model [-11]   TPE331-11\n" );
    printf( "         fname is name of existing file to be converted\n\n" );
} /* usage */

/* eof: CONVERT.C */
