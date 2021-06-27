//
//
#include "peripheral.h"

void periRegister(int peri_id, uint8_t *reg_file)
{
	reg_file = (uint8_t *)mmap(PERI_ADDR[peri_id], sizeof(uint8_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

void periLogout(int peri_id)
{
	munmap(PERI_ADDR[peri_id], sizeof(uint8_t));
}

void periInit(volatile uint8_t *cmd_reg, unsigned char counter)
{
	*(cmd_reg + 1) = counter;
	*cmd_reg = VDEV_INIT;
	while (!(*cmd_reg & VDEV_READY))
		;
}

void periWrite(volatile uint8_t *cmd_reg, size_t offset, void *src, size_t len)
{
	while (!(*cmd_reg & VDEV_READY))
		;
	memcpy((void *)(cmd_reg + offset), src, len);
}

void periRead(volatile uint8_t *cmd_reg, size_t offset, void *dest, size_t len)
{
	while (!(*cmd_reg & VDEV_READY))
		;
	memcpy(dest, (void *)(cmd_reg + offset), len);
}

bool periIsFinished(volatile uint8_t *cmd_reg)
{
	return (*cmd_reg & VDEV_FINISH);
}
