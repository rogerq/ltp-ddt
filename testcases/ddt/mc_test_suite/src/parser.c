#include "common.h"
void usage()
{
	printf("USAGE\n");
}

int main(int argc, char *argv[])
{
	int ret = SUCCESS;
	int help = FALSE;
	int tvp514 = TRUE;
	struct mc_capture_testparams testoptions;
	testoptions.tvp514x = 1;
	testoptions.inp_width = 720;
	testoptions.inp_height = 480;
	int c, device, index;
	char shortoptions[] = "i:w:h:";
	int func=0;

	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
		case 'w':
			testoptions.inp_width = atoi(optarg);
			printf("Width Submitted %d\n",testoptions.inp_width);
			break;
		case 'h':
			testoptions.inp_height = atoi(optarg);
			printf("Height Submitted %d\n",testoptions.inp_height);
			break;
		case 'i':
			func = atoi(optarg);
			printf("func:%d\n", func);
			switch (func){
			case 0:
				ret = mc_tvp514_test(&testoptions);
				break;
			case 1:
				ret = mc_tvp7002_test(&testoptions);
				break;
			case 2:
				ret = prv();
				break;
			case 3:
				ret = mc_tvp514_prv_rzr(&testoptions);
				break;
			default:
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