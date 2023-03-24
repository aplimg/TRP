/*  File: TRPWNDO.C
    Steven R. Stuart     5 Apr 89
*/

#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include "trpdefs.h"
#pragma trace-

#define MA         0xb000 /* start of video memory for Monochrome Adapter */
#define CGA        0xb800 /* start of video memory for Color Graphics Adapter */

/*
void vpoke( int segment, int offset, int value );
int vpeek( int segment, int offset );
*/

void scroll( struct WINDOW window, int lines, int direction )
/*
  +----------------------------------------------------------------------+
  |   Scroll 'window' 'direction' [ UP | DOWN ] 'lines'                  |
  |   Zero (0) lines clears window                                       |
  +----------------------------------------------------------------------+
*/
{
  union REGS regs;

    regs.h.ah = direction;
    regs.h.al = lines;
    regs.h.ch = window.topleftrow;
    regs.h.cl = window.topleftcol;
    regs.h.dh = window.botrightrow;
    regs.h.dl = window.botrightcol;
    regs.h.bh = window.attr;
    int86( 0x10, &regs, &regs );
    if( direction == DOWN ) poscurs( window.topleftrow, window.topleftcol );
    else poscurs( window.botrightrow, window.topleftcol );

} /* end func scroll */

#pragma trace-
int make_window( struct WINDOW window, BOOL save )
/*
  +----------------------------------------------------------------------+
  |   Put struct window on screen.. if 'save' == TRUE, save              |
  |        the section of screen where window will go                    |
  |   Return pointer to mem_bufr section. (NULL if no save)              |
  +----------------------------------------------------------------------+
*/
{
  extern char buffer[];

  static int *pointer, *mem_bufr;
  int i, j, segment, address, offset, currow;
  int tlrow, tlcol, brrow, brcol;
  union REGS regs;

/* These make computations less complicated */
  tlrow = window.topleftrow - 1;
  tlcol = window.topleftcol - 1;
  brrow = window.botrightrow + 1;
  brcol = window.botrightcol + 1;

/* set video segment for Monochrome or Color Graphics Adaptor */
  if ((biosequip() & 0x30) == 0x30) segment = MA;
  else segment = CGA;

/* address points to first byte of video ram to save */
  address = tlrow*160 + tlcol*2;

    if( save ) { /* Save underlying screen */
        /* mem_bufr points to the memory block that holds the saved screen data */
        mem_bufr = calloc( (brrow-tlrow+1) * (brcol-tlcol+1), 2 );
        if( mem_bufr == NULL ) { /* Insufficient memory */
            sprintf( buffer, "401  Calloc err- coreleft: %u bytes.", coreleft() );
            say_error( buffer );
            return NULL;
            }

        /* Fill mem_bufr with current screen memory data */
        pointer = mem_bufr;
        for( i=tlrow; i<=brrow; i++ ) {
            offset = address;
            for( j=tlcol; j<=brcol; j++ ) {
                *pointer++ = vpeek( segment, offset );
                offset += 2;
                } /* for j */
            address += 160;
            } /* for i */
        } /* if save == TRUE */
    else mem_bufr = NULL; /* Just send this if screen not saved */

/* now display the window */
  regs.h.ah = UP;
  regs.h.al = 0;
  regs.h.ch = tlrow;
  regs.h.cl = tlcol;
  regs.h.dh = brrow;
  regs.h.dl = brcol;
  regs.h.bh = window.attr;
  int86( 0x10, &regs, &regs );

/* Outline the window */
  address = tlrow*160 + tlcol*2;
  poscurs( tlrow, tlcol );
  writechs( H_LINE2, window.attr, brcol-tlcol+1 );
  writech( TLC12 );
  poscurs( brrow, tlcol );
  writechs( H_LINE2, window.attr, brcol-tlcol+1 );
  writech( BLC12 );
  poscurs( tlrow, brcol );
  writech( TRC12 );
  poscurs( brrow, brcol );
  writech( BRC12 );
  for( i=tlrow+1; i<brrow; i++ ) {
    poscurs( i, tlcol );
    writech( V_LINE1 );
    poscurs( i, brcol );
    writech( V_LINE1 );
    }

/* Place window identification at top and place cursor at top-left */
  poscurs( tlrow, tlcol+3 );
  printa( window.ident, window.attr );
  poscurs( window.topleftrow, window.topleftcol );

/* Return pointer to saved screen data under window */
  return mem_bufr;

} /* end func make_window */

#pragma trace-
int take_window( struct WINDOW window, int *mem_bufr )
/*
  +----------------------------------------------------------------------+
  |    Restore the screen that was covered by window                     |
  |    and saved at mem_bufr address                                     |
  +----------------------------------------------------------------------+
*/
{
  int *pointer;
  int i, j, segment, address, offset, currow;
  int tlrow, tlcol, brrow, brcol;

/* These make computations less complicated */
  tlrow = window.topleftrow - 1;
  tlcol = window.topleftcol - 1;
  brrow = window.botrightrow + 1;
  brcol = window.botrightcol + 1;

/* set video segment for Monochrome or Color Graphics Adaptor */
  if ((biosequip() & 0x30) == 0x30) segment = MA;
  else segment = CGA;

/* Find address of top left corner of window */
  address = tlrow*160 + tlcol*2;

/* Fill video with data at mem_bufr */
  if( mem_bufr == NULL ) return NULL;
  pointer = mem_bufr;
  for( i=tlrow; i<brrow+1; i++ ) {
    offset = address;
    for( j=tlcol; j<brcol+1; j++ ) {
      vpoke( segment, offset, *pointer++ );
      offset += 2;
      }
    address += 160;
    }

/* free mem_bufr memory */
  free( mem_bufr );
  return NULL;

} /* end func take_window */

/* eof: TRPWNDO.C */

