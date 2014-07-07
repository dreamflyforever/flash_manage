/**
 * SPI NOR Flash Simulation Library
 */
#include <string.h>
#include <stdlib.h>
#include"type.h"
// Reference Device : W25Q128BV
#define FLASH_SECTOR_SIZE (4 * 1024) // 4K
#define FLASH_SECOTR_NUM (256 * 16) // 256 Groups * 16 sectors
#define FLASH_SIZE (FLASH_SECTOR_SIZE * FLASH_SECOTR_NUM) // 128Mbit
static u8 flash_mem[FLASH_SIZE]; // flash memory

int sim_init() 
{
    memset(flash_mem, 0xff, FLASH_SIZE);
    return 0;
}

int sim_read_byte(u32 addr, u8* data) 
{
    if (addr > FLASH_SIZE)
        return -1;
    if ( addr < 0 )
        printf("addr < 0\n\n");

    *data = flash_mem[addr];
    return 0;
}

int sim_write_byte(u32 addr, u8 data)
{
    if (addr > FLASH_SIZE)
        return -1;
    
    if ( addr < 0 )
    printf("addr < 0\n\n");

    flash_mem[addr] &= data;
    return 0;
}

int sim_erase_sector(u32 sector)
{
    if (sector > FLASH_SECOTR_NUM) {
        //return -1;
    }
    memset(&flash_mem[sector /* FLASH_SECTOR_SIZE*/], 0xff,100 /*FLASH_SECTOR_SIZE*/);
    return 0;
}

/*
    fs_write(fsp,3,"456");
printf("111111\n");
    fs_write(fsp,2,"1234545647");
printf("111112\n");

    fs_write(fsp,3,"123454564");
printf("111113\n");
    fs_write(fsp,0,"123454564");
printf("1111114\n");
    fs_write(fsp,0,"123454564");
printf("1111115\n");
    fs_write(fsp,0,"123454564");
printf("1111116\n");
    fs_write(fsp,0,"123454564");
printf("1111117\n");
    fs_write(fsp,0,"123454564");
printf("1111118\n");
    fs_write(fsp,0,"123454564");
printf("1111119\n");
    fs_write(fsp, 1,"123454564");
printf("11111110\n");
    fs_write(fsp, 2,"123454564");
printf("111111\n");
    fs_write(fsp,0,"123454564");
printf("111111\n");
    fs_write(fsp,0,"123454564");
printf("111111\n");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
    fs_write(fsp,0,"123454564");
  
    char *buf = (char *)malloc(10);
    fs_read(fsp, id1, buf);
    printf("buf id %d = %s\n",id1,buf);
    
    printf("\n\n");
    memset(buf,0,10);
    fs_read(fsp, id2, buf);
    printf("buf id %d = %s\n",id2,buf);

    printf("\n\n");
    memset(buf,0,10);
    fs_read(fsp, id3, buf);
    printf("buf id %d = %s\n",id3,buf);

        memset(buf,0,10);
    fs_read(fsp, id3, buf);
    printf("buf id %d = %s\n",id3,buf);

    memset(buf,0,10);
    fs_read(fsp, id3, buf);
    printf("buf id %d = %s\n",id3,buf);    
    
    memset(buf,0,10);
    fs_read(fsp, id3, buf);
    printf("buf id %d = %s\n",id3,buf);    
    
    memset(buf,0,10);
    fs_read(fsp, id2, buf);
    printf("buf id %d = %s\n",id2,buf);    
    
    memset(buf,0,10);
    fs_read(fsp, id1, buf);
    printf("buf id %d = %s\n\n\n\n",id1,buf);
*/
/*
 * fs_read() function,passed!
 *
    sim_init();
    char *buf = (char *)malloc(10);
    fs *fsp = (fs *)malloc( sizeof(fs) );
    fs_write_and_init(fsp,0,0,"123");

    fs_read(fsp,0,buf);
    printf("buf = %s\n",buf);
*/


/*
 * flash_read() function test,passed!
 *
    sim_init();
    char *buf = (char *)malloc(10);


    fs *fsp = (fs *)malloc( sizeof(fs) );

    fs_write_and_init(fsp,0,0,"123");

    //fs_read(fsp, 0, buf);
    flash_read( fsp,buf );
    printf("buf = %s\n",buf);
*/



/*
 * fs_write_and_init() function test,passed!
 *
 *
 * sim_init();

    fs *fsp = (fs *)malloc(sizeof(fs));

    fs_write_and_init(fsp,0,0,"123");

    int i;
    char *p = (char *)fsp;
    char start = fsp->start_address;
    for( i = 0; i < sizeof(fs); i++ )
    {
        sim_read_byte( start, p);
        start++;
        p++;
    }
    
    fs *fs_tmp =  (fs *)( p - sizeof(fs));
    printf("%d\n",fs_tmp->flag);
*/

