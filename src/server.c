/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies Ltd.  All rights reserved.
 * Copyright (c) 2009 HNR Consulting.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * $Id$
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"
#include "perftest_parameters.h"
#include "perftest_resources.h"
#include "perftest_communication.h"

extern int write_bw_server(struct S_THREAD_ARGS* args);

int main(int argc, char *argv[])
{
	int	ret_parser;
	struct ibv_device		*ib_dev = NULL;
	struct pingpong_context		ctx;
	struct perftest_parameters	user_param;
	struct perftest_comm		user_comm;

	pthread_t thread;
	int thread_count = 0;

	/* init default values to user's parameters */
	memset(&user_param,0,sizeof(struct perftest_parameters));
	memset(&user_comm,0,sizeof(struct perftest_comm));
	memset(&ctx,0,sizeof(struct pingpong_context));

	user_param.verb    = WRITE;
	user_param.tst     = BW;
	strncpy(user_param.version, VERSION, sizeof(user_param.version));
	/* Configure the parameters values according to user arguments or default values. */
	ret_parser = parser(&user_param,argv,argc);
	if (ret_parser) {
		if (ret_parser != VERSION_EXIT && ret_parser != HELP_EXIT)
			fprintf(stderr," Parser function exited with Error\n");
		goto return_error;
	}

	if((user_param.connection_type == DC || user_param.use_xrc) && user_param.duplex) {
		user_param.num_of_qps *= 2;
	}

	/* Finding the IB device selected (or default if none is selected). */
	ib_dev = ctx_find_dev(&user_param.ib_devname);
	if (!ib_dev) {
		fprintf(stderr," Unable to find the Infiniband/RoCE device\n");
		goto return_error;
	}

	/* Getting the relevant context from the device */
	ctx.context = ctx_open_device(ib_dev, &user_param);
	if (!ctx.context) {
		fprintf(stderr, " Couldn't get context for the device\n");
		goto free_devname;
	}

	/* Verify user parameters that require the device context,
	 * the function will print the relevent error info. */
	if (verify_params_with_device_context(ctx.context, &user_param))
	{
		fprintf(stderr, " Couldn't get context for the device\n");
		goto free_devname;
	}

	/* See if link type is valid and supported. */
	if (check_link(ctx.context,&user_param)) {
		fprintf(stderr, " Couldn't get context for the device\n");
		goto free_devname;
	}
	if (user_param.output == FULL_VERBOSITY && user_param.machine == SERVER) {
		printf("\n************************************\n");
		printf("* Waiting for client to connect... *\n");
		printf("************************************\n");
	}

	while(1) {
    	struct perftest_comm *p_user_comm;
		struct perftest_parameters *p_user_param;
		struct pingpong_context *p_ctx;
		int rc;
		static int is_server_started = 0;

 		MAIN_ALLOC(p_ctx, struct pingpong_context, 1, free_nothing);
		memcpy((void*)p_ctx,(void*)&ctx, sizeof(struct pingpong_context));
		MAIN_ALLOC(p_user_comm, struct perftest_comm, 1, free_context);
		MAIN_ALLOC(p_user_param, struct perftest_parameters, 1, free_user_comm);
    	memcpy((void*)p_user_param,(void*)&user_param, sizeof(struct perftest_parameters));
		
		struct S_THREAD_ARGS args = {p_user_comm, p_user_param, p_ctx};

		if (create_comm_struct(p_user_comm,p_user_param)) {
			fprintf(stderr," Unable to create RDMA_CM resources\n");
			goto free_user_param;
		}

		if (!is_server_started)
		{
			if (setup_server(p_user_comm))
			{
				fprintf(stderr," Unable to setup_server\n");
				goto free_user_param;
			}
		}

		if ((rc = accept_connection(p_user_comm)))
		{
			fprintf(stderr," Unable to accept_connection\n");
			goto free_user_param;
		}
		if (pthread_create(&thread, NULL, (FuncPtr)write_bw_server, (void*)&args)) {
			fprintf(stderr, "Error creating server thread\n");
			goto free_user_param;
		}
		fprintf(stdout, "Created thread %d.", ++thread_count);
		pthread_detach(thread);

		continue;


free_user_param:
		free(p_user_param);
free_user_comm:
    	free(p_user_comm);
free_context:
		free(p_ctx);
free_nothing:
        if (rc == EINTR) break;
    }

	free(user_param.ib_devname);
    return SUCCESS;

free_devname:
	free(user_param.ib_devname);
return_error:
	//coverity[leaked_storage]
	return FAILURE;	
}