/*  RSETUP.C
    Turbine Data Reduction program setup
    Written by Steven R. Stuart
    Dec. 11, 1989

	Copyright (C)1989 Applied Imagination, Inc.
*/


#include <stdio.h>
#include <bios.h>
#include <dos.h>
#include <conio.h>


#define BOOL        int
#define FALSE       0
#define TRUE        !FALSE
#define SWITCHCHAR  '/'
#define BATFILE     "REDUCE.BAT"

void main( void )
{
char user_select;
char video, port, printer;
BOOL ok, invalid, color_t, color_g;
FILE *fp;

clrscrn();
printf( "Turbine Data Reduction Processor program setup\n" );
printf( "Copyright (C)1990 Applied Imagination, Inc." );
sleep( 3 );

do { /* Do this whole procedure till ok and !invalid */
    ok = invalid = color_t = color_g = FALSE;
    clrscrn();
    printf( "\n\n\nSelect the number of the graphic adapter that you\n" );
    printf( "will be using.\n\n" );
    printf( "1 - CGA  -Color graphics adapter.    (640 x 200)\n" );
    printf( "2 - EGA  -Enhanced graphics adapter. (640 x 350)\n" );
    printf( "3 - VGA  -Video graphics array.      (640 x 480)\n" );
    printf( "4 - Hercules graphic adapter.        (720 x 348)\n\n" );
    printf( "0 - Autoselect. This choice will attempt to load the\n" );
    printf( "                highest resolution that your system may have.\n" );
    printf( "                However, it is not guaranteed to work with\n" );
    printf( "                all graphic cards. Try this one if you're\n" );
    printf( "                not sure of your graphic adapter type.\n" );
    printf( "\n  Your adapter? " );
    do {
        user_select = getch();
        if( user_select < '0' || user_select > '4' ) sound( 200, 10 );
        }
    while( user_select < '0' || user_select > '4' );
    sound( 2000, 3 );
    video = user_select;
    if( video >= '1' && video <= '3' ) {
        printf( "\n\n  Use color? (y/[n]) " );
        user_select = getche();
        if( user_select == 'y' || user_select == 'Y' ) color_t = TRUE;
        sound( 2000, 3 );

/*  Un-comment when color graphic is implemented..
        if( color_t ) {
            printf( "\n  Color graph also? (y/[n]) " );
            user_select = getche();
            if( user_select == 'y' || user_select == 'Y' ) color_g = TRUE;
            sound( 2000, 3 );
            }
*/
        }

    clrscrn();
    printf( "\n\n\nSelect the number of printer protocol that you\n" );
    printf( "will be using.\n\n" );
    printf( "0 - Epson FX-80\n" );
    printf( "1 - Okidata 182/183/192/193\n" );
    printf( "2 - HP LaserJet+\n" );
    printf( "\n  Your printer? " );
    do {
        user_select = getch();
        if( user_select < '0' || user_select > '2' ) sound( 200, 10 );
        }
    while( user_select < '0' || user_select > '2' );
    sound( 2000, 3 );
    printer = user_select;

    clrscrn();
    printf( "\n\n\nSelect the number of the port that your\n" );
    printf( "printer is attached to.\n\n" );
    printf( "0 - BIOS print (must use for serial printer)\n" );
    printf( "1 - LPT1 direct\n" );
    printf( "2 - LPT2 direct\n" );
    printf( "\n         Use 'BIOS print' if you use the MODE command to\n" );
    printf(   "         redirect your print. 'LPTx direct' handles printer\n" );
    printf(   "         errors more gracefully.\n" );
    printf( "\n  Which port is your printer attached to?" );
    do {
        user_select = getch();
        if( user_select < '0' || user_select > '2' ) sound( 200, 10 );
        }
    while( user_select < '0' || user_select > '2' );
    sound( 2000, 3 );
    port = user_select;

    clrscrn();
    printf( "\n\n\nYou have selected: \n\n   Video adapter   : " );
    switch( video ) {
        case '0': printf( "Auto-select." );
            break;
        case '1': printf( "CGA" );
            break;
        case '2': printf( "EGA" );
            break;
        case '3': printf( "VGA" );
            break;
        case '4': printf( "Hercules" );
            break;
        default: printf( "invalid choice." );
            invalid = TRUE;
            break;
        }
    if( color_t ) printf( " in color" );
    printf( "\n   Printer protocol: " );
    switch( printer ) {
        case '0' : printf( "Epson FX-80" );
            break;
        case '1' : printf( "Okidata 182/183/192/193" );
            break;
        case '2' : printf( "HP LaserJet+" );
            break;
        default: printf( "invalid choice." );
            invalid = TRUE;
            break;
            }
    printf( "\n   Printer port    : " );
    switch( port ) {
        case '0': printf( "through BIOS" );
            break;
        case '1': printf( "LPT1" );
            break;
        case '2': printf( "LPT2" );
            break;
        default: printf( "invalid choice." );
            invalid = TRUE;
            break;
            }

    if( invalid ) {
        printf( "\n\nYou have made an invalid choice..\n" );
        printf( "Press any key to restart setup." );
        getch();
        } /* if */
    else { /* Selection ok.. acknowledge */
        printf( "\n\nAre you satisfied with your choices? (y/[n])" );
        user_select = getche();
        if( toupper( user_select ) == 'Y' ) {
            fp = fopen( BATFILE, "w" );
            if( fp == NULL ) {
                perror( "could not open file, RSETUP aborted." );
                exit( 1 );
                }
            fprintf( fp, "echo off\n" );
            fprintf( fp, "RDCE /V%cP%cL%c", video, printer, port );
            if( color_t ) fprintf( fp, "C" );
            if( color_g ) fprintf( fp, "0" );
            ok = TRUE;
            clrscrn();
            printf( "\n\nIf you change your configuration,\n" );
            printf( "run RSETUP again.\n" );
            printf( "\n\nTo operate Turbine Data Reduction Processor, type REDUCE \x11\xc4\xd9\n" );
            exit( 0 );
            } /* if */
        } /* else */
    } /* do */
while( !ok ); /* Get video configuration */

} /* main */



