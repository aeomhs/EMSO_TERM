#include <stdio.h>
#include <unistd.h>

#include "../../KSF_NET/ksf_net_lib.h"
#include "../lib/controllers.h"

int main(){
	int sock, c_sock, ret;
	
	struct request *req;

	req = (struct request*)malloc(sizeof(struct request));
	sock = server_open(R3_STG_PORT);
	printf("R3.storage_controller init\n");
	
	while(1){
		printf("wait request from R2.data_controller\n");		
		c_sock = wait_request(sock, req);
		
		/* cmd is save('s') */
		if(req->cmd == 's'){
			ret = save_data(req->type, req->data);
			if(ret < 0){
				printf("Failed save_data function!!\n");
				ret = response(c_sock, 'f', 0, "Failed not save_data func");
			}
			else{
				printf("Successed saving data!!\n");
				ret = response(c_sock, 's', 0, "Successed saving data");
			}
		}
		else{
    		/* not support request cmd */
			printf("not support request command\n");
			ret = response(c_sock, 'f', 0, "not support request");
		}
		/* response failed */
		if(ret < 0){
		    printf("failed send response\n");
		}
		sleep(1);
	}
	free(req);
	server_close(sock);
	
	return 0;
}
