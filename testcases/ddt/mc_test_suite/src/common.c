#include "common.h"

void mc_capture_help(struct mc_capture_testparams *options)
{
	printf("Usage: mc_capture [OPTIONS]\n");
	printf("Options:\n");
	printf("\t-w\tWidth to be to be set\n");
	printf("\t-h\tHeight to be to be set\n");
	exit(0);
}

void media_device_close(int media_fd)
{
	/* Close the Media device */
	close(media_fd);
}

int media_device_open(struct media_dev *media)
{
	/* Open the Media device */
	media->media_fd = open((const char *) MEDIA_DEVICE, O_RDWR);
	if (media->media_fd  <= 0) {
		printf("Cannot open = %s device\n", MEDIA_DEVICE);
		return -1;
	}
	printf("\n%s: Opened Media Device\n", MEDIA_NAME);
	return 0;
}

int enumerate_all_entities(struct media_dev *media)
{
	int ret, index;

	printf("Enumerating media entities\n");
	index = 0;
	do {
		memset(&media->entity[index], 0, sizeof(struct media_entity_desc));
		media->entity[index].id = index | MEDIA_ENT_ID_FLAG_NEXT;
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_ENTITIES, &media->entity[index]);
		if (ret < 0) {
			break;
		} else {
			if (!strcmp(media->entity[index].name, E_VIDEO_CCDC_OUT_NAME))
				media->video =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, E_TVP514X_NAME)){
				media->tvp5146 =  media->entity[index].id;
				printf("E_TVP514X_NAME\n");
			}
			else if (!strcmp(media->entity[index].name, E_TVP7002_NAME))
				media->tvp7002 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, E_MT9T111_NAME))
				media->mt9t111 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, E_CCDC_NAME)){
				media->ccdc =  media->entity[index].id;
				printf("E_CCDC_NAME\n");
			}
			printf("[%d]:%s\n", media->entity[index].id, media->entity[index].name);
		}
		index++;
	} while (ret == 0);

	if ((ret < 0) && (index <= 0)) {
		printf("Failed to enumerate entities OR no entity registered - %d\n",
				ret);
		return ret;
	}
	media->num_entities = index;
	printf("Total number of entities: %d\n", media->num_entities);

	return 0;
}

int enumerate_all_entities_rsz(struct media_dev *media)
{
	int ret, index;

	index = 0;
	do {
		memset(&media->entity[index], 0, sizeof(struct media_entity_desc));
		media->entity[index].id = index | MEDIA_ENT_ID_FLAG_NEXT;
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_ENTITIES, &media->entity[index]);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else {
			if (!strcmp(media->entity[index].name, E_VIDEO_RSZ_OUT_NAME)) {
				media->video =  media->entity[index].id;
			}
			else if (!strcmp(media->entity[index].name, E_TVP514X_NAME)) {
				media->tvp5146 =  media->entity[index].id;
			}
			else if (!strcmp(media->entity[index].name, E_CCDC_NAME)) {
				media->ccdc =  media->entity[index].id;
			}
			else if (!strcmp(media->entity[index].name, E_PRV_NAME)) {
				media->prv =  media->entity[index].id;
			}
			else if (!strcmp(media->entity[index].name, E_RSZ_NAME)) {
				media->rsz =  media->entity[index].id;
			}
			printf("[%x]:%s\n", media->entity[index].id, media->entity[index].name);
		}
		index++;
	}while (ret == 0);

	if ((ret < 0) && (index <= 0)) {
		printf("Failed to enumerate entities OR no entity registered - %d\n",ret);
		return ret;
	}
	media->num_entities = index;
	printf("Total number of entities: %d\n", media->num_entities);
	return ret;
}

int enumerate_all_entities_prv(struct media_dev *media)
{
	int ret, index = 0;
	printf("ENTERED ENUMERATION FOR PREVIEWER\n");
	do {
		memset(&entity[index], 0, sizeof(entity));
		media->entity[index].id = index | MEDIA_ENT_ID_FLAG_NEXT;

		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_ENTITIES, &media->entity[index]);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else {
			if (!strcmp(media->entity[index].name, E_VIDEO_PRV_OUT_NAME)) {
				media->video =  entity[index].id;
				printf("E_VIDEO_PRV_OUT_NAME\n");
			}
			else if (!strcmp(media->entity[index].name, E_TVP514X_NAME)) {
				media->tvp5146 =  entity[index].id;
				printf("E_TVP514X_NAME\n");
			}
			else if (!strcmp(media->entity[index].name, E_CCDC_NAME)) {
				media->ccdc =  entity[index].id;
				printf("E_CCDC_NAME\n");
			}
			else if (!strcmp(media->entity[index].name, E_PRV_NAME)) {
				media->prv =  entity[index].id;
				printf("E_PRV_NAME\n");
			}
			printf("[%d]:%s\n", media->entity[index].id, media->entity[index].name);
		}
		index++;
	}while (ret == 0);

	if ((ret < 0) && (index <= 0)) {
		printf("Failed to enumerate entities OR no entity registered - %d\n",
				ret);
		return ret;
	}
	media->num_entities = index;
	printf("Total number of entities: %d\n", media->num_entities);

	return 0;
}

int reset_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i;

	/* Open the Media device */
	ret  = media_device_open(media);
	if (ret < 0)
		return ret;

	printf("Resetting all links...\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			for(i = 0; i < media->entity[index].links; i++) {
				link.source.entity = links.links->source.entity;
				link.source.index = links.links->source.index;
				link.source.flags = MEDIA_PAD_FL_OUTPUT;
				link.sink.entity = links.links->sink.entity;
				link.sink.index = links.links->sink.index;
				link.sink.flags = MEDIA_PAD_FL_INPUT;
				link.flags = (link.flags & ~MEDIA_LNK_FL_ENABLED) ;				
				ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
				if(ret)
					break;
				links.links++;
			}
		}
	}
	media_device_close(media->media_fd);
	return 0;
}

int setup_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i, input;

	printf("Enumerating links/pads for entities\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			if(media->entity[index].pads)
				printf("pads for entity %d=", media->entity[index].id);
			for(i = 0 ; i < media->entity[index].pads; i++) {
				printf("(%d %s) ", links.pads->index,
						(links.pads->flags & MEDIA_PAD_FL_INPUT) ?
						"INPUT" : "OUTPUT");
				links.pads++;
			}
			printf("\n");
			for(i = 0; i < media->entity[index].links; i++) {
				printf("[%d:%d]===>[%d:%d]",
						links.links->source.entity,
						links.links->source.index,
						links.links->sink.entity,
						links.links->sink.index);
				if(links.links->flags & MEDIA_LNK_FL_ENABLED)
					printf("\tACTIVE\n");
				else
					printf("\tINACTIVE \n");
				links.links++;
			}
			printf("\n");
		}
	}
	printf("INPUT SOURCE %d \n",media->hd);
	if (media->hd == 1)
		input = media->tvp7002;
	else if (media->hd == 0)
		input = media->tvp5146;

	printf("Enabling link [tvp5146]===>[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = input;
	link.source.index = 0;	/* Only 1 pad */
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->ccdc;
	link.sink.index = 0; /* Sink pad of CCDC, 0 */
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
		return ret;
	} else {
		printf("[tvp514x]===>[ccdc]\tenabled\n");
	}
	/* Enable 'ccdc===>memory' link */
	printf("Enabling link [ccdc]===>[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->ccdc;
	link.source.index = 1; /* Source pad of CCDC: 1 */
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->video;
	link.sink.index = 0;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret)
		printf("failed to enable link between ccdc and video node\n");
	else
		printf("[ccdc]===>[video_node]\tenabled\n");

	return ret;
}

int setup_media_links_rsz(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i;

	printf("5.enumerating links/pads for entities\n");
	for(index = 0; index < media->num_entities; index++) {

		links.entity = media->entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);

		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else{
			/* display pads info first */
			if(media->entity[index].pads)
				printf("pads for entity %x=", media->entity[index].id);

			for(i = 0;i< media->entity[index].pads; i++)
			{
				printf("(%x, %s) ", links.pads->index,(links.pads->flags & MEDIA_PAD_FL_INPUT)?"INPUT":"OUTPUT");
				links.pads++;
			}

			printf("\n");

			/* display links now */
			for(i = 0;i< media->entity[index].links; i++)
			{
				printf("[%x:%x]-------------->[%x:%x]",links.links->source.entity,
				       links.links->source.index,links.links->sink.entity,links.links->sink.index);
				       if(links.links->flags & MEDIA_LNK_FL_ENABLED)
						printf("\tACTIVE\n");
				       else
						printf("\tINACTIVE \n");

				links.links++;
			}

			printf("\n");
		}
	}

	printf("**********************************************\n");

	/* 6. enable 'tvp514x-->ccdc' link */
	
	printf("6. ENABLEing link [tvp514x]----------->[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->tvp5146;
	link.source.index = P_TVP514X;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->ccdc;
	link.sink.index = P_CCDC_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
		//goto cleanup;
	} else
		printf("[tvp514x]----------->[ccdc]\tENABLED\n");

	/* 7.enable 'ccdc-->prv' link */
	printf("7. ENABLEing link [CCDC]----------->[PRV]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->ccdc;
	link.source.index = P_CCDC_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->prv;
	link.sink.index = P_PRV_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between ccdc and previewer\n");
		//goto cleanup;
	} else
		printf("[ccdc]----------->[prv]\tENABLED\n");

	/* 8.enable 'prv-->rsz' link */
	printf("7. ENABLEing link [prv]----------->[rsz]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->prv;
	link.source.index = P_PRV_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->rsz;
	link.sink.index = P_RSZ_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between prv and rsz\n");
		//goto cleanup;
	} else
		printf("[prv]----------->[rsz]\tENABLED\n");

	/* 9. enable 'rsz->memory' link */
	printf("8. ENABLEing link [rsz]----------->[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->rsz;
	link.source.index = P_RSZ_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->video;
	link.sink.index = P_VIDEO;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between rsz and video node\n");
		//goto cleanup;
	} else
		printf("[rsz]----------->[video_node]\t ENABLED\n");

	printf("**********************************************\n");
	return ret;
}

int setup_media_links_prv(struct media_dev *media)
{
	int index, ret, i;
	struct media_link_desc link;
	struct media_links_enum links;

	printf("5.enumerating links/pads for entities\n");
	for(index = 0; index < media->num_entities; index++) {

		links.entity = entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);

		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else{
			/* display pads info first */
			if(media->entity[index].pads)
				printf("pads for entity %x=", media->entity[index].id);

			for(i = 0;i< media->entity[index].pads; i++)
			{
				printf("(%x, %s) ", links.pads->index,(links.pads->flags & MEDIA_PAD_FL_INPUT)?"INPUT":"OUTPUT");
				links.pads++;
			}

			printf("\n");

			/* display links now */
			for(i = 0;i< media->entity[index].links; i++)
			{
				printf("[%x:%x]-------------->[%x:%x]",links.links->source.entity,
				       links.links->source.index,links.links->sink.entity,links.links->sink.index);
				       if(links.links->flags & MEDIA_LNK_FL_ENABLED)
						printf("\tACTIVE\n");
				       else
						printf("\tINACTIVE \n");

				links.links++;
			}

			printf("\n");
		}
	}

	printf("**********************************************\n");

	for(index = 0; index < media->num_entities; index++) {

		links.entity = media->entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);

		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else{

			for(i = 0;i< media->entity[index].links; i++)
			{
				       if(links.links->flags & MEDIA_LNK_FL_ENABLED) {
					        /* de-enable the link */
					        memset(&link, 0, sizeof(link));

						link.flags |=  ~MEDIA_LNK_FL_ENABLED;
						link.source.entity = links.links->source.entity;
						link.source.index = links.links->source.index;
						link.source.flags = MEDIA_PAD_FL_OUTPUT;

						link.sink.entity = links.links->sink.entity;
						link.sink.index = links.links->sink.index;
						link.sink.flags = MEDIA_PAD_FL_INPUT;

						ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
						if(ret) {
							printf("failed to de-enable link \n");
						}
				       }

				links.links++;
			}
		}
	}
	printf("**********************************************\n");
	/* 6. enable 'tvp514x-->ccdc' link */

	printf("6. ENABLEing link [tvp514x]----------->[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->tvp5146;
	link.source.index = P_TVP514X;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->ccdc;
	link.sink.index = P_CCDC_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
	} else
		printf("[tvp514x]----------->[ccdc]\tENABLED\n");

	/* 7.enable 'ccdc-->prv' link */
	printf("7. ENABLEing link [CCDC]----------->[PRV]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->ccdc;
	link.source.index = P_CCDC_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->prv;
	link.sink.index = P_PRV_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between ccdc and previewer\n");
	} else
		printf("[ccdc]----------->[prv]\tENABLED\n");

	/* 8. enable 'prv->memory' link */
	printf("8. ENABLEing link [prv]----------->[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = media->prv;
	link.source.index = P_PRV_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = media->video;
	link.sink.index = P_VIDEO;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between prv and video node\n");
	} else
		printf("[prv]----------->[video_node]\t ENABLED\n");

	return ret;
}

