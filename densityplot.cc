//
// densityplot.cc
//
// C++ Program to read a Gnuplot data file and write a 
//     postscipt file to display it as a density plot.
//
//  $Id: densityplot.cc,v 1.1.1.1 1994/12/06 05:26:59 jak Exp $
//
//  Author: John Kassebaum
//
// $Log: densityplot.cc,v $
// Revision 1.1.1.1  1994/12/06 05:26:59  jak
// Initial import of DensityPlot code. -jak
//
//
//

static char rcsid_densityplot_cc[] = "$Id: densityplot.cc,v 1.1.1.1 1994/12/06 05:26:59 jak Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BLOCKSIZE    1024
#define Null(A)             ((A *) 0)
#define New(A)              ((A *) malloc( sizeof(A) ) )
#define NewBlock(A,N)       ((A *) malloc( sizeof(A) * (N)) )
#define BiggerBlock(A,B,N)  ((A *) realloc( (void *)(B), sizeof(A) * (N)))

void colorize( int val , int max )
{
    if( val < max / 3 ){
	    printf("%02X%02X%02X",0,0,val);
	} else if( val > 2 * max / 3 ){
	    printf("%02X%02X%02X", val, val, val);
	} else {
	    printf("%02X%02X%02X", val, 0, 0);
	}
}

//
// Usage Info -------------------------------------------------------------
//
#define USAGE    "Usage: %s [args] < input data_file > output data_file\n"
#define DESCR_0    "Where args are one or more of:\n"
#define DESCR_1    " -h            prints this help message and exits.\n"
#define DESCR_2    " -s            prints statistics\n"
#define DESCR_3    " -o <type>     output type (dynamic, magnitude, or log (log-magnitude)) (default magnitude)\n"
#define DESCR_4   "\n input data_file  -  A gnuplot file of floating point numbers.\n"
#define DESCR_5   "\n output_file      -  A file containing a postscript image.\n"
//
// ------------------------------------------------------------------------
//

int main( int argc, char **argv )
{
    int c,i,j,k;
    int stats, flag;
    long int data_count;
    long int space_alloc;
    double min_value, max_value, scale, move;
    double tempf;
    float *data;
    long   rows, cols;
    long   row,col;
    enum output_type {dynamic, magnitude, logmagnitude } output;

    stats = 0;
	output = magnitude;
	
  //
  // Parse Command Line
  //
    for (c=1; c< argc; c++) {
        if ( !strcmp( argv[ c ],"-h") || !strcmp( argv[ c ],"-help")){
            fprintf(stderr,USAGE,argv[0]);
            fprintf(stderr,DESCR_0);
            fprintf(stderr,DESCR_1);
            fprintf(stderr,DESCR_2);
            fprintf(stderr,DESCR_3);
            fprintf(stderr,DESCR_4);
            fprintf(stderr,DESCR_5);
            exit(0);
        } else if (!strcmp( argv[ c ],"-o")){
		    c++;
            if ( !strcmp( argv[ c ],"dynamic") ){
                output = dynamic;
            } else if ( !strcmp( argv[ c ],"magnitude") ) {
                output = magnitude;
            } else if ( !strcmp( argv[ c ],"log") ) {
                output = logmagnitude;
			}
        } else if (!strcmp( argv[ c ],"-s")){
            stats = 1;
        }
    }

    data_count = 0;
    space_alloc = 0;
    data = (float *)0;
    
    if( data = NewBlock( float, BLOCKSIZE ) ){
        space_alloc = BLOCKSIZE;
    } else {
        perror(argv[0]);
        abort();
    };
        
	{
	  //
	  // binary input data
	  //
	    int fd, bytes_read, block_size;
		fd = 0;    // note: stdin is always fd = 0
		
		block_size = BLOCKSIZE;
	    while ((bytes_read = read( fd, (char *) &data[ data_count ], (block_size*sizeof(float)) ))
		                   > 0 )
		{
		    data_count += ( bytes_read / sizeof(float) );
			if( (block_size = (space_alloc - data_count)) < 1) {
				if( data = BiggerBlock( float, data, (space_alloc + BLOCKSIZE) ) ){
					space_alloc += BLOCKSIZE;
					block_size = space_alloc - data_count;
				} else {
					perror(argv[0]);
					abort();
				}
			}
		}
		
	    if( bytes_read == -1 ) {
			perror(argv[0]);
			abort();
		}
		
        cols = (unsigned long) floor( data[0] ) + 1;
        rows = (data_count) /  (cols);
      //
      // find min and max values
      //
		flag = 0;
		for(row=1; row <= rows; row++){   // skip first row and first column
            for(col=1; col < cols; col++ ){
				if (!flag) {
				    if( output == logmagnitude ){
					    min_value = max_value = log( data[ row * cols + col ] );
				    } else if(( output == magnitude ) || ( output == dynamic )){
					    min_value = max_value =( data[ row * cols + col ] );
					}
					flag = 1;
				} else {
					if( data[ row * cols + col ] < min_value ){
				        if( output == logmagnitude ){
						    min_value = log( data[row * cols + col ] );
						} else if(( output == magnitude ) || ( output == dynamic )){
						    min_value = ( data[row * cols + col ] );
						}
					}
					if( data[row * cols + col ] > max_value ){
				        if( output == logmagnitude ){
						    max_value = log( data[row * cols + col ] );
						} else if(( output == magnitude ) || ( output == dynamic )){
						    max_value = ( data[row * cols + col ] );
						}
					}
				}
            }
		}

        scale = ((double)(0x0ff)) / (max_value - min_value);
        move  = (0.0 - min_value);
 	}
    
    if (stats) {
        fprintf(stderr, "Space Allocated for %d floats.\n", space_alloc);
        fprintf(stderr, "Space Used for %d floats.\n", data_count);
        fprintf(stderr, "Number of Rows = %d.\n", rows);
        fprintf(stderr, "Number of Cols = %d.\n", cols);
        fprintf(stderr, "Minimum value = %lg.\n", min_value);
        fprintf(stderr, "Maximum value = %lg.\n", max_value);
        fprintf(stderr, "Will Scale data by %lg.\n", scale);
        fprintf(stderr, "Will translate data by %lg.\n", move);
    }
        
    printf("%s!PS-Adobe-2.0\n","%");
	printf("%sTitle:Density Image Plot of 3D binary Gnuplot format data file.\n","%%");
	printf("%sBoundingBox: 0 0 2100 500\n","%%");
	printf("%sEndComments\n","%%");
	printf("/DataString %d string def\n", (cols)*2);
    printf("0 0 moveto 0 500 lineto 2100 500 lineto 2100 0 lineto closepath 0 setgray fill\n");
    printf("72 72 translate\n");
    printf("-90 rotate\n");
    printf("-400 2000 scale\n");
	printf("%d %d %d [ %d 0 0 %d 0 0 ]\n",cols, rows, 8, cols, rows);
	printf("{\n");
	printf("    currentfile DataString readhexstring pop\n");
 	printf("} false 3 colorimage \n");
    for( row=1; row <= rows; row++){   // skip first row and first column
	
	    if( output == dynamic ){
			//
			// Dynamic Scaling by Row
			//
			
			flag = 0;
			for(col=1; col < cols; col++ ){
				if (!flag) {
					min_value = max_value =( data[ row * cols +col ] );
					flag = 1;
				} else {
					if( data[ row * cols + col ] < min_value )
						min_value = ( data[row * cols + col ] );
					if( data[row * cols + col ] > max_value )
						max_value = ( data[row * cols + col ] );
				}
			}
			scale = ((double)(0x0ff)) / (max_value - min_value);
			move  = (0.0 - min_value);
	    }
	
        for( col=1 ; col <= cols ; col++){
		    int val;
			if( output == logmagnitude ){
			    val = (int) rint( (( log(data[ row*cols + col ]) + move) * scale));
			} else if(( output == magnitude ) || ( output == dynamic )){
			    val = (int) rint( ((    (data[ row*cols + col ]) + move) * scale));
			}
			if (val > 0x0ff) val = 0x0ff;
			if (val < 0x000) val = 0x0000;
			colorize( val, 0x0ff );
        }
		
		printf("\n");
    }

    free( data );
}

