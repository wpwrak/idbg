struct ep_descr;

#define usb_send(ep, buf, len, callback, user) \
	__usb_send(buf, len)
