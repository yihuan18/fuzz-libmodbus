/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>

//yihuan add
#include "modbus.c"
#include "modbus-tcp.c"

int main(void)
{
    int s = -1;
    modbus_t *ctx;

    //yihuan add defination start
    modbus_t *ctx1;
    int req_length;
    //yihuan add defination end

    modbus_mapping_t *mb_mapping;

    FILE *fp = fopen("/home/yihuan/modbus_log", "a+");
    if (fp==0) { printf("can't open file\n"); return 0;}

    ctx = modbus_new_tcp("127.0.0.1", 1502);
    /* modbus_set_debug(ctx, TRUE); */

    mb_mapping = modbus_mapping_new(500, 500, 500, 500);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        fseek(fp, 0, SEEK_END);
        char sz_add[] = "Failed to allocate the mapping\n";
        fwrite(sz_add, strlen(sz_add), 1, fp);
        modbus_free(ctx);
        return -1;
    }

    s = modbus_tcp_listen(ctx, 1);
    
    
    //yihuan add client start
    ctx1 = modbus_new_tcp("127.0.0.1", 1502);
    modbus_set_debug(ctx1, TRUE);

    if (modbus_connect(ctx1) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx1);
        fseek(fp, 0, SEEK_END);
        char sz_add[] = "Connection failed\n";
        fwrite(sz_add, strlen(sz_add), 1, fp);
        return -1;
    }
    //yihuan add client end

    modbus_tcp_accept(ctx, &s);

    //yihuan add read data start
    uint8_t buf[1500];
    memset(buf, 0, 1500);
    req_length = read(0, buf, 1500);
    fseek(fp, 0, SEEK_END);
    fwrite(buf, sizeof(buf), 1, fp);
    send_msg(ctx1, buf, req_length);
    //yihuan add read data start

    //for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;

        rc = modbus_receive(ctx, query);
        printf("got\n");
        fseek(fp, 0, SEEK_END);
        char sz_add[] = "got\n";
        fwrite(sz_add, strlen(sz_add), 1, fp);
        if (rc > 0) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
            fwrite("after reply", strlen("after reply"), 1, fp);
        } else if (rc == -1) {
            /* Connection closed by the client or error */
            //break;
        }
   // }

    //printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (s != -1) {
        close(s);
    }
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);
    fclose(fp);
    return 0;
}
