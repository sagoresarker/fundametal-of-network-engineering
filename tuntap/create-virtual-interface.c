#include<stdio.h>
#include<linux/if.h>
#include<linux/if_tun.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include <string.h>

#include <unistd.h>

int tun_alloc(char *dev, int flags){
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if((fd = open(clonedev, O_RDWR)) < 0){
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;

    if (*dev){
        strncpy(ifr.ifr_name, dev, IFNAMSIZ-1);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0){
        close(fd);
        return err;
    }

    strncpy(dev, ifr.ifr_name, IFNAMSIZ - 1);

    return fd;
}

int main(){
    printf("Creating a virtual interface\n");
    return 0;
}