#ifndef TEST_H_
#define TEST_H_

#include <assert.h>
#include <errno.h>
#include <infiniband/verbs.h>

#define IB_PORT 1

#define check(s, m) if(!(s)) {printf("%s\t%d\n", (m), errno); goto error;}

struct IBRes {
    struct ibv_context		*ctx;
    struct ibv_pd		*pd;
    struct ibv_mr		*mr;
    struct ibv_cq		*cq;
    struct ibv_qp		*qp;
    struct ibv_port_attr	 port_attr;
    struct ibv_device_attr	 dev_attr;

    char   *ib_buf;
    size_t  ib_buf_size;
    size_t  ib_reg_buf_size;
};

extern struct IBRes ib_res;

int  setup_ib ();
void fin_ib ();

#endif /*setup_ib.h*/
