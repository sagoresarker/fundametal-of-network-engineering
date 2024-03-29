#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

int tun_alloc(char *dev, int flags) {
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strncpy(dev, ifr.ifr_name, IFNAMSIZ - 1);

    return fd;
}

int create_tuntap_interface(char *dev, int flags) {
    int tunfd;
    char *interface_type;

    if (flags & IFF_TUN) {
        interface_type = "TUN";
    } else if (flags & IFF_TAP) {
        interface_type = "TAP";
    } else {
        fprintf(stderr, "Invalid interface type\n");
        return -1;
    }

    printf("Creating %s interface: %s\n", interface_type, dev);

    tunfd = tun_alloc(dev, flags);

    if (tunfd < 0) {
        fprintf(stderr, "Failed to create %s interface\n", interface_type);
    } else {
        printf("%s interface created successfully. File descriptor: %d\n", interface_type, tunfd);
    }

    return tunfd;
}

int main() {
    // Check if the program is run as root
    if (geteuid() != 0) {
        fprintf(stderr, "This program must be run as root. Please try again.\n");
        return 1;
    }

    char tun_name[IFNAMSIZ];
    char tap_name[IFNAMSIZ];

    // Create a TUN interface
    strncpy(tun_name, "tun0", IFNAMSIZ - 1);
    int tun_fd = create_tuntap_interface(tun_name, IFF_TUN);

    // Create a TAP interface
    strncpy(tap_name, "tap0", IFNAMSIZ - 1);
    int tap_fd = create_tuntap_interface(tap_name, IFF_TAP);

    // You can check the interfaces using the ip command
    system("ip link show");

    // Close file descriptors when done
    close(tun_fd);
    close(tap_fd);

    return 0;
}
