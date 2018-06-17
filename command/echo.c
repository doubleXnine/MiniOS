#include "type.h"				
#include "const.h"				
#include "protect.h"						
#include "string.h"				
#include "proc.h"				
#include "global.h"
#include "proto.h"

int disp_pos;

int main(int argc, char* argv[])
{
	int i=0x5000;
	/*for(i=1; i<argc; i++)
	{
		if(i==1)disp_str(argv[i]);
		else{
			disp_str(" ");
			disp_str(argv[i]);
		}
	}
	disp_str("\n");*/
	while(1)
	{
		disp_str("E");
		disp_int(i++);
	}
	return 0;
}

