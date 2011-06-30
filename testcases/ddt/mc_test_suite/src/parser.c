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
	char shortoptions[] = "i:h";
	int choice=0;

	testoptions.inp_width=720;testoptions.inp_height=576;
	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
		case 'i':
			choice = atoi(optarg);
			printf("choice:%d\n", choice);
			switch (choice){
			case 0:
				ret = mc_tvp514_test(&testoptions);
				break;
			case 1:
				ret = mc_tvp7002_test(&testoptions);
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