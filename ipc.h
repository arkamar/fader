struct ipc {
	int fd;
};

enum fader_type {
	FADER_NOP,
	FADER_SET,
	FADER_SET_PERC,
	FADER_INC,
	FADER_INC_PERC,
	FADER_DEC,
	FADER_DEC_PERC,
};

struct msg {
	int8_t type;
	int8_t reserved;
	int16_t value;
};

int ipc_bind(struct ipc *);
int ipc_connect(struct ipc *);
int ipc_close(struct ipc *);

int ipc_msg_send(struct ipc *, const struct msg *);
int ipc_msg_recv(struct ipc *, struct msg *);
