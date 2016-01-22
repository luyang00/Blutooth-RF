#include "SyncGen.h"
#define CHANNEL_BAND 125
int seed;
int next;
int first_gen=1;
void srand(int s)
{
    seed = s;
	first_gen=1;
}

int rand()
{
	if(first_gen)
	{
		first_gen=0;
		next = seed * 29+37;
		return seed%124;
	}
	else
	{
		next = next * 29+37;

		return next%124;
	}
}
int abs(int value)
{
	if(value>0)
		return value;
	else 
		return (0-value);
}

void hoop_sequence_gen(int seed,u8 * hoop_sequence,int length)
{
	int i=0;

	srand(seed);
	for(i=0;i<length;i++)
	{
		*(hoop_sequence+i)=abs(rand());
	}
}
void sync_sequence_gen(int s,u8 * sync_sequence,int length)
{
	int i,j;
	int a,b,c;
	float denominator;
	denominator=1.0/length;
	srand(s+120);
	for(i=0;i<length;i++)
	{
		a=denominator * i;
		b=denominator * (i+1)*CHANNEL_BAND;
		c=denominator * (i+2)*CHANNEL_BAND;
		while(!(sync_sequence[i]>denominator*i*CHANNEL_BAND && sync_sequence[i] < denominator*(i+1)*CHANNEL_BAND))
		{
			sync_sequence[i]=abs(rand());
		}
	}
}