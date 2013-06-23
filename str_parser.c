// module: str_parser.c
// reads two text files - inputs and patterns and looks for the patterns in the input file, using different run modes:
//	1 - exact match
//	2 - substring match
// 	3 - limited edit distance match - uses Levenshtein Distance algorithm
// author: Alex Fok
// modules:
// compilation: gcc  str_parser.c -o str_parser
// usage: str_parser input.txt patterns.txt 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lookup1(char** inputs, int in_lines, char** patterns, int pat_lines);
void lookup2(char** inputs, int in_lines, char** patterns, int pat_lines);
void lookup3(char** inputs, int in_lines, char** patterns, int pat_lines);
char** read_file(char *filename, int *num_lines);
void usage();

int LevenshteinDistance(char* s, int m, char* t, int n);
int minimum(int i, int j, int k);


main(int argc, char **argv)
{
	char **inputs, **patterns, *input_file, *patterns_file;
	int in_lines, pat_lines;

	if(argc != 4){
		usage();
		exit(-1);
	}
	
	input_file=argv[1];
        patterns_file=argv[2];
	char run_mode=argv[3][0];
	
	fprintf(stdout,"*******Inputs are:********");
	inputs = read_file(input_file, &in_lines);
        fprintf(stdout,"*******Patterns are:********");
	patterns = read_file(patterns_file, &pat_lines);

        fprintf(stdout,"*******Results:********\n\n");

	// Look for patterns in inputs
	switch (run_mode)
	{
	case '1':
		lookup1(inputs, in_lines, patterns, pat_lines);
		break;
        case '2':
		lookup2(inputs, in_lines, patterns, pat_lines);
                break;
        case '3':
		lookup3(inputs, in_lines, patterns, pat_lines);
                break;
        default:
		lookup1(inputs, in_lines, patterns, pat_lines);
                break;
	}

	free (inputs);
	free (patterns);
}

// Exact pattern matching lookup
void lookup1(char** inputs, int in_lines, char** patterns, int pat_lines)
{
	int i,j;
	for (i=0;i<pat_lines;i++)
	{
		for (j=0;j<in_lines;j++)
		{
			if (!strcmp(inputs[j], patterns[i]))
			{
				fprintf(stdout,"%s\n", inputs[j]);
			}
		}
	}

}

// Lookup based on prefix matching
void lookup2(char** inputs, int in_lines, char** patterns, int pat_lines)
{
        int i,j;
        for (i=0;i<pat_lines;i++)
        {
                for (j=0;j<in_lines;j++)
                {
                        if (strstr(inputs[j], patterns[i]))
                        {
                                fprintf(stdout,"%s\n", inputs[j]);
                        }
                }
        }

}

// Lookup based on edit distance
void lookup3(char** inputs, int in_lines, char** patterns, int pat_lines)
{
	int d;
        int i,j;

        for (i=0;i<pat_lines;i++)
        {
                for (j=0;j<in_lines;j++)
                {
        	d = LevenshteinDistance(inputs[j], strlen(inputs[j]), patterns[i], strlen(patterns[i]));
//	        fprintf(stdout,"LevenshteinDistance: %d\n", d);
			if (d<=1)
                        {
                                fprintf(stdout,"%s\n", inputs[j]);
                        }
                }
        }

}

// Calculates Levenshtein Distance between two strings
int LevenshteinDistance(char* s, int m, char* t, int n)
{
  // for all i and j, d[i,j] will hold the Levenshtein distance between
  // the first i characters of s and the first j characters of t;
  // note that d has (m+1)x(n+1) values
  int **d=NULL;
  int i, j;

  // Allocate and initiate two dimensional array
  d = (int **)malloc((m+1)*sizeof(int*));
  for (i=0;i<=m;i++)
  {
    d[i]=(int *)malloc((n+1)*sizeof(int*));
    memset(d[i],0,(n+1));
  }

  for (i=0; i<=m;i++)
    d[i][0] = i; // the distance of any first string to an empty second string
  for (j=0; j<=n;j++)
    d[0][j] = j; // the distance of any second string to an empty first string

//fprintf(stdout,"Zeroed first lines");


  for (j=1;j<=n;j++)
  {
    for (i=1;i<=m;i++)
    {
      if (s[i] == t[j])  
        d[i][j] = d[i-1][j-1];       // no operation required
      else {
        d[i][j] = minimum
                   (
                     d[i-1][j] + 1,  // a deletion
                     d[i][j-1] + 1,  // an insertion
                     d[i-1][j-1] + 1 // a substitution
                   );
      }
    }
  }

/* Matrix print
  fprintf(stdout,"\n***Matrix*** s=%s, t=%s, %dX%d \n", s, t, m, n);
  for (i=0;i<=m;i++)
  {
    for (j=0;j<=n;j++)
    {
	fprintf(stdout,"%d ", d[i][j]);
    }
   fprintf(stdout,"\n");
  }
*/

  return d[m][n];
}

// Returns minimum of 3 integers
int minimum(int i, int j, int k)
{
	if (i < j)
	{
		return (i > k) ? k : i;
	}
	else
        {
                return (j > k) ? k : j;
        }
}

// Read a file to strings array
char** read_file(char *filename, int *num_lines)
{
	int lines_count=0;
	char **lines=NULL;
	*num_lines = 0;

	// Read the whole file
        FILE *f=fopen(filename,"rb");
	if (!f)
	{
		perror("Invalid filename");
		exit(-1);
	}
	fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
        char *data=malloc(len+1);fread(data,1,len,f);fclose(f);
	// count lines
	int i;
	for (i=0;i<len;i++)
	{
		if (data[i]=='\n')
			lines_count++;
	}
	
	lines=malloc(lines_count*sizeof(char*));
	fprintf(stdout,"lines: %d\n", lines_count);

	int j = 0;
	lines[0]=&data[0];
        for (i=0;i<len;i++)
        {
                if (data[i]=='\n')
		 {
			data[i]=0;
                        lines[++j]=&data[i+1];
		 }
        }

	// fool proof
        if (j != lines_count)
        {
                perror("ooPs - learn count lines, dude");
                exit(-1);
        }

	for (i=0;i<lines_count;i++)
		fprintf(stdout,"line%d: %s\n", i, lines[i]);

        *num_lines = lines_count;

	return lines;
}

// Prints program usage
void usage()
{
  fprintf(stderr, "USAGE: str_parser inputs_file patterns_file run_mode (1 2 3) \n\\
e.g.: str_parser input.txt patterns.txt 1\n");
}

