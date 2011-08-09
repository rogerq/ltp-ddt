#include "common.h"
void usage()
{
	printf("mc_capture -i 0/1\n");
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int help = 1;
	int tvp514 = 0;
	int c, device, index;
	char shortoptions[] = "i:w:h:";
	int func=0;

	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
		case 'i':
			func = atoi(optarg);
			printf("func:%d\n", func);
			switch (func){
			case 0:
				ret = mc_tvp514();
				break;
			case 1:
				usage();
				exit(1);
			}
			break;
		default:
			usage();
			exit(1);
		}
	}
	return ret;
}	
