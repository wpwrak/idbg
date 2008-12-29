#define	EP1_SIZE	4


void __usb_send(const char *label, uint8_t *buf, int len,
    void (*cb)(void *user), void *user);
void __usb_recv(const char *label, uint8_t *buf, int len,
    void (*cb)(void *user), void *user);


#define usb_send(ep, buf, len, callback, user) \
	__usb_send("usb_send: " #ep, buf, len, callback, user)
#define usb_recv(ep, buf, len, callback, user) \
	__usb_recv("usb_recv: " #ep, buf, len, callback, user)
#define usb_left(...) 0
