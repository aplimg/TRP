/*  TRPPRINT.C
	Copyright (C)1989, 1990 Applied Imagination, Inc.
    Written by Steven R. Stuart - Jan 1990
*/
#pragma nestcmnt


#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <malloc.h>
#include <bios.h>
#include <dos.h>
#include <process.h>
#include "trpdefs.h"

#pragma trace-
BOOL pt_form( void )
/* Print a form on printer */
{
extern struct TESTFORM engine;
extern int model;
extern char buffer[];
extern char *label_ptr[], *symbol_ptr[], *units_ptr[];

    int i;
/*
    nomenclature( -1 ); /* Take out control chars */

    for( i=0; i<10; i++ ) {
        sprintf( buffer,
            "%-3s % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g % 5.2g\n",
            symbol_ptr[i],
            engine.cell[0][i],
            engine.cell[1][i],
            engine.cell[2][i],
            engine.cell[3][i],
            engine.cell[4][i],
            engine.cell[5][i],
            engine.cell[6][i],
            engine.cell[7][i],
            engine.cell[8][i],
            engine.cell[9][i]
            );
        send_print_line( buffer, strlen( buffer ) );
        } /* for */

    sendbyte( 10 ); /* line feed */
    sendbyte( 10 );

    for( i=0; i<10; i++ ) {
        sprintf( buffer, "   %-3s : %-6s : %s\n",
            symbol_ptr[i],
            units_ptr[i],
            label_ptr[i]
            );
        send_print_line( buffer, strlen( buffer ) );
        }

    sendbyte( 12 );     /* form feed */
    nomenclature( model ); /* Put any control chars back */
*/

} /* pt_form */


#pragma trace-
BOOL pt_calcdata( void )
/* Print TPE program output file on printer */
{
extern char infile_name[];

    int i, c;
    BOOL test;
    FILE *f_in;

    if( (f_in = fopen( infile_name, "r" )) != NULL ) {
            while( (c = fgetc( f_in )) != EOF ) {
                if( getkey() == ESC ) break;    /* break if ESC pressed */
                if( c == 10 ) {
                    sendbyte( 0x0D );
                    sendbyte( 0x0A );
                }
                if( (c!=12) && (c!=10) ) {    /* filter formfeed & linefeed */
                    test = sendbyte( c );      /* send char to ptr     */
                    if( ! test ) break;             /* break if printer err */
                } /* if */
            } /* while */
            fclose( f_in );
            sendbyte( 12 );                         /* form feed */
            return TRUE;
    } /* if */
    else {
        say_error( "Can't find reduced data file" );
        return FALSE;
    }


} /* pt_calcdata */


#pragma trace-
BOOL sendbyte( unsigned int byte )
/*
  +----------------------------------------------------------------------+
  | Print the string print_stn[] on report printer.                      |
  |  Return NULL if printer not ready.                                   |
  +----------------------------------------------------------------------+
*/
{
extern int printer_port;
extern BOOL use_bios;

    int i;

    if( getkey() == ESC ) return FALSE;      /* break if user abort */
    if( use_bios ) {
        i = putc( byte, stdprn );
        if( i == EOF ) return FALSE;
        else return TRUE;
        }

    else {
        for( i=0; i<5; i++ ) {  /* Allow 5 retries for printer error */
            if( SELECTED( biosprint( STAT_PRT, NULL, printer_port )) ) {
                biosprint( SEND_PRT, byte, printer_port );
                return TRUE;
                } /* if */
            else { /* B.O. */
                sound( 900, 9 );
                sound( 300, 18 );
                } /* else */
            } /* for */
        return FALSE;
        }

} /* sendbyte */

#pragma trace-
BOOL epsprint( void )
{
    extern int v_res, h_res;
    int horiz, vert, dot, byte;

    if( ! sendbyte( ESC ) ) return FALSE;    /* Set 8/72" line spacing */
    sendbyte( 65 );   /* 'A' */
    sendbyte( 8 );    /* '^H' */
    for( horiz=h_res-1; horiz>=7; horiz-=8 ) {      /* Span horz bytes */

        /* Set up printer graphics */
        sendbyte( ESC );                            /* setup 480 dot/line */
        sendbyte( 'K' );
        if( v_res == 200 ) {
            sendbyte( 144 );
            sendbyte( 1 );
        }
        else {
            sendbyte( v_res % 256 );
            sendbyte( v_res / 256 );
        }

        for( vert=0; vert<=v_res; vert++ ) {        /* Get 8 bits of char */
            if( getkey() == ESC ) return FALSE;     /* break if user abort */
            /* Assemble graphic byte */
            dot = readdot( vert, horiz );
            byte = (dot == 0) ? 0 : 128;
            dot = readdot( vert, horiz-1 );
            byte += (dot == 0) ? 0 : 64;
            dot = readdot( vert, horiz-2 );
            byte += (dot == 0) ? 0 : 32;
            dot = readdot( vert, horiz-3 );
            byte += (dot == 0) ? 0 : 16;
            dot = readdot( vert, horiz-4 );
            byte += (dot == 0) ? 0 : 8;
            dot = readdot( vert, horiz-5 );
            byte += (dot == 0) ? 0 : 4;
            dot = readdot( vert, horiz-6 );
            byte += (dot == 0) ? 0 : 2;
            dot = readdot( vert, horiz-7 );
            byte += (dot == 0) ? 0 : 1;

            /* Send assembled graphic byte to printer */
            if( ! sendbyte( byte ) ) return FALSE;

            /* For half hight picture */
            if( v_res == 200 ) sendbyte( byte ); /* Double high this resolution */

            } /* end for(vert) */

        sendbyte( 13 ); /* carriage return */
        sendbyte( 10 ); /* line-feed */
        } /* end for(horz) */

    sendbyte( ESC );  /* Reset  to 1/6" line spacing */
    sendbyte( 50 );   /* '2' */
    sendbyte( 12 );   /* form-feed */
    return TRUE;


} /* epsprint */


#pragma trace-
BOOL okiprint( void )
/*
    Routine prints the graph on an Oki printer.
*/
{
    extern int v_res, h_res;
    int horiz, vert, dot, byte;
    BOOL scaled = TRUE;

    if( ! sendbyte( 3 ) ) return FALSE; /* Graphic command */
    for( horiz=h_res-1; horiz>=7; horiz-=8 ) {  /* Span horz bytes */
        for( vert=0; vert<=v_res; vert++ ) {    /* Get 8 bits of char */
            if( getkey() == ESC ) return FALSE; /* break if ESC pressed */

            /* Assemble graphic byte */
            dot = readdot( vert, horiz );
            byte = (dot == 0) ? 0 : 1;
            dot = readdot( vert, horiz-1 );
            byte += (dot == 0) ? 0 : 2;
            dot = readdot( vert, horiz-2 );
            byte += (dot == 0) ? 0 : 4;
            dot = readdot( vert, horiz-3 );
            byte += (dot == 0) ? 0 : 8;
            dot = readdot( vert, horiz-4 );
            byte += (dot == 0) ? 0 : 16;
            dot = readdot( vert, horiz-5 );
            byte += (dot == 0) ? 0 : 32;
            dot = readdot( vert, horiz-6 );
            byte += (dot == 0) ? 0 : 64;
            dot = readdot( vert, horiz-7 );
            byte += (dot == 0) ? 0 : 128;

            /* Send assembled graphic byte to printer */
            if( ! sendbyte( byte ) ) return FALSE;
            if( byte == 3 ) sendbyte( 3 ); /* Oki want it this way */
  /*
if( scaled && (vert % 7)==0 ) vert++; /* Skip a point if scaling **/
*/

            if( v_res == 200 ) { /* Double high this resolution */
                if( byte == 3 ) sendbyte( 3 ); /* Oki want it this way */
                sendbyte( byte ); /* Send assembled graphic byte to printer */
                }
            } /* end for(vert) */
        sendbyte( 3 );              /* Graphic command */
        sendbyte( 14 );             /* Graphic car-ret */
        } /* end for(horz) */
    sendbyte( 3 );              /* Graphic command */
    sendbyte( 2 );              /* Reset graphic mode */
    sendbyte( 12 );             /* formfeed */
    return TRUE;

} /* okiprint */

#pragma trace-
BOOL send_print_line( char stn[], int nr_chars )
{
    int i;

    for( i=0; i<nr_chars; i++ ) {
        if( ! sendbyte( stn[i] ) ) return FALSE;
        }
    return TRUE;

} /* send_print_line */

#pragma trace-
BOOL lazprint( void )
{
extern int v_res, h_res;
extern char buffer[];

    int horiz, vert, dot, byte, sweep;
    unsigned int hi, lo;

    /* Set resolution to 75 dpi */
    if( ! send_print_line( "\x1B*t75R", 6 ) ) return FALSE;
    send_print_line( "\x1B&a5C", 6 ); /* Horz cursor to col 5 (1/2") */
    send_print_line( "\x1B*r1A", 5 );  /* Start raster graphics    */

    /* Set up line length */
    switch( v_res ) {
        case 200:           sweep = 25; break; /* 200 / 8 = 25     */
        case 480:           sweep = 60; break; /* 480 / 8 = 60     */
        case 348: case 350: sweep = 44; break; /* 350 / 8 = 43.nn  */
        }

    sprintf( buffer, "\x1B*b%iW", (sweep==25) ? 50 : sweep );

    for( horiz=h_res-1; horiz>=0; horiz-- ) { /* Span horz bytes */

        /* Tell ptr how many bytes to expect */
        send_print_line( buffer, 6 );

        for( vert=0; vert<=sweep*8-7; vert+=8 ) { /* Get 8 bits of char */

            if( getkey() == ESC ) return FALSE; /* break if ESC pressed */

            /* Assemble graphic byte */
            dot = readdot( vert, horiz );
            byte = (dot == 0) ? 0 : 128;
            dot = readdot( vert+1, horiz );
            byte += (dot == 0) ? 0 : 64;
            dot = readdot( vert+2, horiz );
            byte += (dot == 0) ? 0 : 32;
            dot = readdot( vert+3, horiz );
            byte += (dot == 0) ? 0 : 16;
            dot = readdot( vert+4, horiz );
            byte += (dot == 0) ? 0 : 8;
            dot = readdot( vert+5, horiz );
            byte += (dot == 0) ? 0 : 4;
            dot = readdot( vert+6, horiz );
            byte += (dot == 0) ? 0 : 2;
            dot = readdot( vert+7, horiz );
            byte += (dot == 0) ? 0 : 1;

            if( v_res == 200 ) {
                /* Double high this resolution */
                hi = byte >> 4;     /* Shift hi nybble to lo nybble */
                lo = byte & 0x0F;   /* Mask hi nybble */
                if( ! sendbyte( dbl( hi ) ) ) return FALSE;
                sendbyte( dbl( lo ) );
                }
            else if( ! sendbyte( byte ) ) return FALSE;
            } /* for vert */
        } /* for horiz */

    send_print_line( "\x1B*rB", 4 ); /* End raster transfer */
    sendbyte( 12 ); /* Form feed */
    send_print_line( "\x1BE", 2 );   /* Reset printer */
    return( TRUE );

} /* lazprint */


#pragma trace-
unsigned dbl( unsigned nybble )
/*
    Doubles the bits of the lo half of nybble.
    i.e.  0x0A becomes 0xCC  ==  0000 1010 -> 1100 1100
          0x06 becomes 0x3C  ==  0000 0110 -> 0011 1100
*/
{
    int i;
    unsigned byte=0;

    for( i=0; i<4; i++ ) {
        byte   <<= 2;
        byte   += ((nybble & 0x08) == 0) ? 0 : 3;
        nybble <<= 1;
        }
    return byte;

} /* dbl */


