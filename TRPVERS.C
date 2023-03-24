/*  TRPVERS.C
    Written by Steven R. Stuart  Jan 1990
	Copyright (C)1990 Applied Imagination, Inc.
*/

#include <conio.h>
#include <stdio.h>
#include "trpdefs.h"
#include "trpvers.h"

extern int black_b, blue_b, green_b, cyan_b, red_b, magenta_b, brown_b, white_b;
extern int black_f, blue_f, green_f, cyan_f, red_f, magenta_f, brown_f, white_f;
extern int gray_f, ltblue_f, ltgreen_f, ltcyan_f, ltred_f;
extern int ltmagenta_f, yellow_f, brtwhite_f;

#pragma trace-
void version()
/* Simply state version and serial nr of program */
{
extern int vidmode,printmode, printer_port;
extern BOOL use_bios;
extern char buffer[];

    struct WINDOW vers = {  8,  5, 17, 35, "µ AI Æ", red_b + blue_f };

    make_window( vers, FALSE );
    poscurs( vers.topleftrow+1, vers.topleftcol+2 );
    sprintf( buffer, "Version : %s", VERS );
#ifdef MODULE_A
    strcat( buffer, "a" );
#endif
#ifdef MODULE_B
    strcat( buffer, "b" );
#endif
#ifdef MODULE_C
    strcat( buffer, "c" );
#endif
#ifdef DEMO
    strcat( buffer, "-DEMO" );
#endif
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+2, vers.topleftcol+2 );
    sprintf( buffer, "Serial  : %s", SER );
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+3, vers.topleftcol+2 );
    sprintf( buffer, "Video   : " );
    switch( vidmode ) {
        case 1:  strcat( buffer, "CGA" );
                 break;
        case 2:  strcat( buffer, "EGA" );
                 break;
        case 3:  strcat( buffer, "VGA" );
                 break;
        case 4:  strcat( buffer, "Hercules" );
                 break;
        default: strcat( buffer, "Auto" );
    }
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+4, vers.topleftcol+2 );
    sprintf( buffer, "Printer : " );
    switch( printmode ) {
        case 1:  strcat( buffer, "Okidata" );
                 break;
        case 2:  strcat( buffer, "LaserJet" );
                 break;
        default: strcat( buffer, "FX-80" );
    }
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+5, vers.topleftcol+2 );
    sprintf( buffer, "Port    : " );
    switch( printer_port ) {
        case 0:  strcat( buffer, "LPT1" );
                 break;
        case 1:  strcat( buffer, "LPT2" );
                 break;
        default: strcat( buffer, "Unk" );
    }
    if( use_bios ) strcat( buffer, " bios" );
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+6, vers.topleftcol+2 );
    sprintf( buffer, "License : %s", LIC1 );
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+7, vers.topleftcol+12 );
    sprintf( buffer, "%s", LIC2 );
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+8, vers.topleftcol+12 );
    sprintf( buffer, "%s", LIC3 );
    printa( buffer, vers.attr );

    poscurs( vers.topleftrow+9, vers.topleftcol+12 );
    sprintf( buffer, "%s", LIC4 );
    printa( buffer, vers.attr );

    getch();
    make_window( vers, FALSE );
    poscurs( vers.topleftrow+2, vers.topleftcol+2 );
	printa( "Applied Imagination, Inc.", vers.attr  );
    poscurs( vers.topleftrow+3, vers.topleftcol+2 );
    printa( "128 West Wooster", vers.attr   );
    poscurs( vers.topleftrow+4, vers.topleftcol+2 );
    printa( "Bowling Green, Ohio 43402", vers.attr   );
    poscurs( vers.topleftrow+5, vers.topleftcol+2 );
    printa( "United States of America", vers.attr   );
    poscurs( vers.topleftrow+7, vers.topleftcol+2 );
    printa( "Tel: (419) 352-8373", vers.attr   );
    poscurs( vers.topleftrow+8, vers.topleftcol+2 );
    printa( "Fax: (419) 353-0285", vers.attr   );

} /* version */

/* eof: TRPVERS */

