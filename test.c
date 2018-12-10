#include "test.h"
#include <stdlib.h>
#include <malloc.h>

struct IBRes ib_res;

int setup_ib ()
{
    int	ret		         = 0;
    struct ibv_device **dev_list = NULL;    

    //ret = ibv_fork_init();
    //check(ret == 0, "Failed to ibv_fork_init.")

    /* get IB device list */
    int num = 0;
    dev_list = ibv_get_device_list(&num);
    check(dev_list != NULL, "Failed to get ib device list.");
    int i;
    for(i = 0;i < num;i++){
        printf("%d\t%s\n", i, ibv_get_device_name(dev_list[i]));
    }

    /* create IB context */
    ib_res.ctx = ibv_open_device(dev_list[ib_res.device_index]);
    check(ib_res.ctx != NULL, "Failed to open ib device.");

    /* allocate protection domain */
    ib_res.pd = ibv_alloc_pd(ib_res.ctx);
    check(ib_res.pd != NULL, "Failed to allocate protection domain.");

    /* query IB port attribute */
    ret = ibv_query_port(ib_res.ctx, IB_PORT, &ib_res.port_attr);
    check(ret == 0, "Failed to query IB port information.");
    
    /* register mr */
    ib_res.ib_buf      = (char *) memalign (4096, ib_res.ib_buf_size);
    check (ib_res.ib_buf != NULL, "Failed to allocate ib_buf");

    ib_res.mr = ibv_reg_mr (ib_res.pd, (void *)ib_res.ib_buf,
			    ib_res.ib_reg_buf_size,
			    IBV_ACCESS_LOCAL_WRITE |
			    IBV_ACCESS_REMOTE_READ |
			    IBV_ACCESS_REMOTE_WRITE);
    check (ib_res.mr != NULL, "Failed to register mr");

    return 0;
    
    /* query IB device attr */
    ret = ibv_query_device(ib_res.ctx, &ib_res.dev_attr);
    check(ret==0, "Failed to query device");
    
    /* create cq */
    ib_res.cq = ibv_create_cq (ib_res.ctx, ib_res.dev_attr.max_cqe, 
			       NULL, NULL, 0);
    check (ib_res.cq != NULL, "Failed to create cq");
    
    /* create qp */
    struct ibv_qp_init_attr qp_init_attr = {
        .send_cq = ib_res.cq,
        .recv_cq = ib_res.cq,
        .cap = {
            .max_send_wr = ib_res.dev_attr.max_qp_wr,
            .max_recv_wr = ib_res.dev_attr.max_qp_wr,
            .max_send_sge = 1,
            .max_recv_sge = 1,
        },
        .qp_type = IBV_QPT_RC,
    };

    ib_res.qp = ibv_create_qp (ib_res.pd, &qp_init_attr);
    check (ib_res.qp != NULL, "Failed to create qp");

    ibv_free_device_list (dev_list);
    return 0;

 error:
    if (dev_list != NULL) {
	ibv_free_device_list (dev_list);
    }
    return -1;
}

void fin_ib ()
{
    if (ib_res.qp != NULL) {
	ibv_destroy_qp (ib_res.qp);
    }
 
    if (ib_res.cq != NULL) {
	ibv_destroy_cq (ib_res.cq);
    }

    if (ib_res.mr != NULL) {
	ibv_dereg_mr (ib_res.mr);
    }

    if (ib_res.pd != NULL) {
        ibv_dealloc_pd (ib_res.pd);
    }

    if (ib_res.ctx != NULL) {
        ibv_close_device (ib_res.ctx);
    }

    if (ib_res.ib_buf != NULL) {
	free (ib_res.ib_buf);
    }
}

int main(int argc, const char *argv[]){
    if(argc != 4){
        return 0;
    }

    memset (&ib_res, 0, sizeof(struct IBRes));
    ib_res.device_index = atoi(argv[1]);
    ib_res.ib_buf_size = atoi(argv[2]);
    ib_res.ib_reg_buf_size = atoi(argv[3]);

    printf("%lu %lu\n", ib_res.ib_buf_size, ib_res.ib_reg_buf_size);
    
    setup_ib();
    fin_ib();

    return 0;
}
