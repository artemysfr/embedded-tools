#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>

#include <modbus/modbus.h>

#define DEFAULT_UART_PATH	"/dev/ttymxc3"
#define UART_SPEED		9600
#define SLAVE_ADDRESS		0x01
#define TEMP_ADDRESS		0x01
#define HUM_ADDRESS		0x02

#define CHECK_AND_HANDLE(ret_val, ret_error) {if (ret_val < 0) {perror(ret_error); return -1; }}

int give_up = 0;
void sig_handler(int sig_num)
{
	give_up = 1;
}

int main(int argc, char **argv)
{
	int ret;
	modbus_t *ctx;
	uint8_t result[2];
	char *uart_path;

	if (argc > 1) {
		uart_path = argv[1];
	} else {
		uart_path = DEFAULT_UART_PATH;
	}
	printf("Using serial port %s at %d\n", uart_path, UART_SPEED);

	ctx = modbus_new_rtu(uart_path, UART_SPEED, 'N', 8, 1);
	if (ctx == NULL) {
		perror("unable to create modbus context");
		return -1;
	}

	/* 1 sec timeout on modbus cmds */
	modbus_set_response_timeout(ctx, 1, 0);

	ret = modbus_set_slave(ctx, SLAVE_ADDRESS);
	CHECK_AND_HANDLE(ret, "modbus_set_slave");

#ifdef DEBUG
	ret = modbus_set_debug(ctx, 1);
	CHECK_AND_HANDLE(ret, "modbus_set_debug");
	printf("debug mode ON");
#endif // DEBUG

	ret = modbus_connect(ctx);
	CHECK_AND_HANDLE(ret, "modbus_connect");

	ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
	CHECK_AND_HANDLE(ret, "modbus_rtu_set_serial_mode");

	signal(SIGINT, sig_handler);

	int temp, hum;
	while (!give_up) {
	        ret = modbus_read_input_registers(ctx, TEMP_ADDRESS, 1, (uint16_t*)result);
		CHECK_AND_HANDLE(ret, "modbus_read_input_registers");
		temp = (result[1] << 8) | result[0]; /* will only work on little endian systems */
	        printf("Temperature = %d C - ", temp/10);

	        ret = modbus_read_input_registers(ctx, HUM_ADDRESS, 1, (uint16_t*)result);
		CHECK_AND_HANDLE(ret, "modbus_read_input_registers");
		hum = (result[1] << 8) | result[0]; /* will only work on little endian systems */
	        printf("Humidity = %d %\n", hum/10);

		sleep(2);
	}

	printf("bye bye...\n");
	modbus_close(ctx);
	modbus_free(ctx);
}
