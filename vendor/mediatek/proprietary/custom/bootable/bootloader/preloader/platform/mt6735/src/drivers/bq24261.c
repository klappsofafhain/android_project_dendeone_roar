#include "platform.h"
#include "i2c.h"
#include "pmic.h"
#include "bq24261.h"

/**********************************************************
 *
 *	[I2C Slave Setting]
 *
 *********************************************************/
#define BQ24261_SLAVE_ADDR_WRITE   0xD6
#define BQ24261_SLAVE_ADDR_READ    0xD7

/**********************************************************
 *
 *[Global Variable]
 *
 *********************************************************/
#define bq24261_REG_NUM 7
unsigned char bq24261_reg[bq24261_REG_NUM] = {0};


#define BQ24261_I2C_ID	I2C0
static struct mt_i2c_t bq24261_i2c;

/**********************************************************
 *
 *	[I2C Function For Read/Write bq24261]
 *
 *********************************************************/

static struct mt_i2c_t bq24261_i2c;

kal_uint32 bq24261_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0]= addr;
	write_data[1] = value;

	bq24261_i2c.id = BQ24261_I2C_ID;
	bq24261_i2c.addr = (BQ24261_SLAVE_ADDR_WRITE >> 1);
	bq24261_i2c.mode = ST_MODE;
	bq24261_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&bq24261_i2c, write_data, len);

	if(I2C_OK != ret_code)
	print("%s: i2c_write: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

kal_uint32 bq24261_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	bq24261_i2c.id = BQ24261_I2C_ID;
	bq24261_i2c.addr = (BQ24261_SLAVE_ADDR_READ >> 1);
	bq24261_i2c.mode = ST_MODE;
	bq24261_i2c.speed = 100;
	len = 1;

	ret_code = i2c_write_read(&bq24261_i2c, dataBuffer, len, len);

	if(I2C_OK != ret_code)
	print("%s: i2c_read: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
int bq24261_read_interface (unsigned char RegNum, unsigned char *val, unsigned char MASK, unsigned char SHIFT)
{
    unsigned char bq24261_reg = 0;
    int ret = 0;

    printf("--------------------------------------------------PL\n");

    ret = bq24261_read_byte(RegNum, &bq24261_reg);

     printf("[bq24261_read_interface] Reg[%x]=0x%x\n", RegNum, bq24261_reg);
	
    bq24261_reg &= (MASK << SHIFT);
    *val = (bq24261_reg >> SHIFT);
	
     printf("[bq24261_read_interface] val=0x%x\n", *val);
	
    return ret;
}

int bq24261_config_interface (unsigned char RegNum, unsigned char val, unsigned char MASK, unsigned char SHIFT)
{
    unsigned char bq24261_reg = 0;
    int ret = 0;

     printf("--------------------------------------------------\n");

    ret = bq24261_read_byte(RegNum, &bq24261_reg);
    printf("[bq24261_config_interface] Reg[%x]=0x%x\n", RegNum, bq24261_reg);
    
    bq24261_reg &= ~(MASK << SHIFT);
    bq24261_reg |= (val << SHIFT);

    if(RegNum == bq24261_CON1 && val == 1 && MASK ==CON1_RESET_MASK && SHIFT == CON1_RESET_SHIFT)
    {
        // RESET bit
    }
    else if(RegNum == bq24261_CON1)
    {
        bq24261_reg &= ~0x80;	//RESET bit read returs 1, so clear it
    }
	 

    ret = bq24261_write_byte(RegNum, bq24261_reg);
     printf("[bq24261_config_interface] write Reg[%x]=0x%x\n", RegNum, bq24261_reg);

    // Check
    //bq24261_read_byte(RegNum, &bq24261_reg);
    //printk("[bq24261_config_interface] Check Reg[%x]=0x%x\n", RegNum, bq24261_reg);

    return ret;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/

/* CON1---------------------------------------------------- */

//CON0----------------------------------------------------

void bq24261_set_tmr_rst(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24261_config_interface(   (kal_uint8)(bq24261_CON0), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON0_TMR_RST_MASK),
                                    (kal_uint8)(CON0_TMR_RST_SHIFT)
                                    );
}

void bq24261_set_en_boost(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24261_config_interface(   (kal_uint8)(bq24261_CON0), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON0_EN_BOOST_MASK),
                                    (kal_uint8)(CON0_EN_BOOST_SHIFT)
                                    );
}

kal_uint32 bq24261_get_stat(void)
{
    kal_uint32 ret=0;
    kal_uint8 val=0;

    ret=bq24261_read_interface(     (kal_uint8)(bq24261_CON0), 
                                    (&val),
                                    (kal_uint8)(CON0_STAT_MASK),
                                    (kal_uint8)(CON0_STAT_SHIFT)
                                    );
    return val;
}


