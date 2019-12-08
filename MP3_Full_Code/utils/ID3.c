// Process ID3v2.4 header information
//
//  For detailed information, refer to these pages:
//     http://id3.org/id3v2.3.0
//     http://id3.org/id3v2.4.0-structure

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "ff.h"
#include "ID3.h"

// Convert an unsigned integer from network byte order (big endian) to 
// host byte order (little endian).
static uint32_t ntohl( uint32_t val ) {
   uint8_t *p = (uint8_t *)&val;
   register uint8_t tmp;

   tmp = p[3];
   p[3] = p[0];
   p[0] = tmp;
   tmp = p[1];
   p[1] = p[2];
   p[2] = tmp;
   return val;
}

// Extract a string from ID3 header.
static void readString( FIL *fp, uint8_t *ptr, uint16_t size ) {
  UINT br;
  uint8_t kind;
  uint16_t *buffer = malloc( size );
  uint16_t *ptr16 = buffer;

  // read kind from stream
  f_read( fp, &kind, 1, &br );
  --size;

  // for ID3v2.4, text type of 3 is ASCII-like
  switch( kind ) {
  case 0:  // ISO-8859-1, NULL terminated.
  case 3:  // UTF-8 encoded Unicode,  NULL terminated.
    // Just read the string and exit.
    if( size < MAXTAGSIZE )
      f_read( fp, ptr, size, &br );
    else {
      f_read( fp, buffer, size, &br );
      buffer[MAXTAGSIZE-1] = 0;
      strcpy( (void *)ptr, (void *)buffer );
    }
    ptr[size] = 0;

    break;
  case 1: // UTF-16 encoded Unicode with BOM, NULL terminated.
    // The first 16-bits are the Byte Order Marker (BOM) U+FEFF.  So we want
    // to figure out which byte is the FF and store that.
    f_read( fp, buffer, 2, &br ); // BOM

    // If BOM is correct, great.  Otherwise read one extra byte to skip 
    // to the LSB.
    if( *buffer == 0xfeff ) {
      size -= 2;
    } else {
      f_read( fp, ptr, 1, &br );
      size -= 3;
    }
    // Read the entire Unicode string, then convert to 8-bit.
    f_read( fp, buffer, size, &br );
    size >>= 1;
    if( size > MAXTAGSIZE-1 )
        size = MAXTAGSIZE-1;
    while( size-- )
      *(ptr++) = (uint8_t)*(ptr16++);
    *ptr = 0;
    break;

  default:  // UTF-16BE encoded Unicode without BOM, NULL terminated.
    // Cortex-M4 is are little-endian, so read a byte to adjust.
    f_read( fp, ptr, 1, &br );
    size -= 1;

    // Read the entire Unicode string, then convert to 8-bit.
    f_read( fp, buffer, size, &br );
    size >>= 1;
    if( size > MAXTAGSIZE-1 ) size = MAXTAGSIZE-1;
    while( size-- )
      *(ptr++) = (uint8_t)*(ptr16++);
    *ptr = 0;
    break;
  }
  free( buffer );
}

// Sizes are 28-bit numbers, made up 4 7-bit pieces.   
#define SIZE(b3,b2,b1,b0) ((((b3&0x7f)*128+(b2&0x7f))*128+(b1&0x7f))*128+(b0&0x7f))

uint32_t getID3Tags( FIL *fp, struct id3tags * fields ) {
  uint8_t header[10];
  uint32_t total, headersize;
  uint32_t size;
  uint8_t version;
  UINT br;

  // Initialize all the fields to null strings.
  fields->artist[0] = fields->title[0] = fields->album[0] = 0;

  // Try to detect ID3v2.4 header.  If we can't detect one, rewind to the
  // start of the file, NULL each field, and return.
  f_read( fp, header, 10, &br );

  version = header[3];
  if ( strncmp( (void *)header, "ID3", 3 ) || version < 3 || version > 4 ) {
    f_lseek( fp, 0 );
    return 0;
  }

  // Get the total size of the ID3 header.
  headersize = total = SIZE( header[6], header[7], header[8], header[9] );

  // Now, read through everything.
  while( total > 0 ) {
    // Read data for the next tag
    f_read( fp, header, 10, &br );

    // if tag is NULL, skip it
    if( !memcmp( header, "\0\0\0", 4 ) )
      break;

    // Get size of the field, depending on the version. 
    if( version == 3 )
      size = ntohl(*(uint32_t *)(&header[4]));
    else
      size = SIZE( header[4], header[5], header[6], header[7] );

    // Handle song titles
    if ( !strncmp( (const char *)header, "TIT1", 4 ) ||
         !strncmp( (const char *)header, "TIT2", 4 ) ) {
      readString( fp, fields->title , size );
    // Handle artists
    } else if ( !strncmp( (const char *)header, "TPE1", 4 ) ) {
      readString( fp, fields->artist , size );
    // Handle albums
    } else if ( !strncmp( (const char *)header, "TALB", 4 ) ) {
      readString( fp, fields->album , size );
    // Skip all other fields
    } else {
      f_lseek( fp, f_tell( fp ) + size );
    }

    // Adjust total bytes processed by header and tag sizes
    total -= 10 + size;
  }

  // Before returning, be sure to position file pointer at start of
  // actual MP3 data.

  f_lseek( fp, headersize );
  return headersize;
}

