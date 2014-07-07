/*
 * key-value file system
 * Author  Shanjin Yang
 */
#include<stdio.h>
#include<stdlib.h>
#include"fs_list.h"
#include"fs_lib.h"
#include"sim.h"

#define SECTOR_SIZE    10 //4 * 1024       
#define WRITE          0xffffffff
#define READ           0x55
#define NO_USE         0x00
#define cd_another     0x2000

int sector;
int all_len;

typedef struct fs{
    char          fs_list;      // flag for WRITE? 
    char          flag;         // flag for read or no use
    char          len;          // strlen(buf),for caclulate the next fs block address
    char          id;           // key
    int          start_address;  
}fs;

fs *fsp;

int fs_write_and_init(char id, int start_address, char *buf);
int fs_write(char id, char *buf);
int fs_read(char id, char *buf );
int flash_write(fs *fsp, char *buf);
int flash_read(fs *fsp, char *buf );
void copy_all(int sector_address, fs *fsp, fs *fsp_tmp, char *buf);


int flash_write(fs *fsp, char *buf)
{
    int fs_len = sizeof(fs);
    char *p    = (char *)fsp; 
    int start = fsp->start_address;
    int i;
    
    for (  i = 0; i < fs_len; i++ )
    {
        sim_write_byte(start,*p);
        p++;
        start++;
    }

    /*calculate the data begin address: start*/
    char *tmp = buf;
    int len       = fs_strlen(buf);
        
    for( i = 0; i < len; i++ )
    { 
        sim_write_byte(start,*tmp);
        start++;
        tmp++;    
    }
    buf = tmp - fsp->len;
}

int fs_write_and_init(char id, int start_address, char *buf)
{
    int i;
    fs *fs_tmp  = fsp;

    int start = start_address;
   
    for( i = 0; i < sizeof(fs); i++ )
    {
        sim_read_byte( start, (char *)fs_tmp);
        start++;
        (char *)fs_tmp ++;
    }
    fs_tmp =  (fs *)( (char *)fs_tmp - sizeof(fs));

    if(fs_tmp->fs_list != WRITE)
    {
        printf("11111 %d\n",fs_tmp->id);
        //while (1);
        return 0;
    }

    fsp->fs_list        = 1;
    fsp->flag           = READ;
    fsp->start_address  = start_address;
    fsp->len            = fs_strlen(buf);
    fsp->id             = id;

    flash_write( fsp, buf);

    all_len  = fsp->len + sizeof(fs);
}


int fs_read( char id, char *buf )
{
    fs *fs_tmp = fsp;
    
    int i;
    char *p = (char *)fsp;
    
    /* read first node from flash */
    int start = sector;
    for( i = 0; i < sizeof(fs); i++ )
    {
        sim_read_byte( start, p);
        start++;
        p++;
    }
    fs_tmp =  (fs *)( p - sizeof(fs));
    p      =  (char*)( p - sizeof(fs));

    /*judge last one*/
    while ( fs_tmp->fs_list != 1 )
    {
        
        if ( id == fs_tmp->id )
        {
            if ( fs_tmp->flag == READ )
            {
                flash_read( fs_tmp, buf);
                return 0;
            }
        }

        //read_fs_next(fs_tmp);
        start = fs_tmp->start_address + sizeof(fs) + fs_tmp->len;
        for( i = 0; i < sizeof(fs); i++ )
        {
            sim_read_byte( start, p);
            start++;
            p++;
        }
        fs_tmp =  (fs *)( p - sizeof(fs));
        p      =  (char*)( p - sizeof(fs));
    }

    if ( fs_tmp->id == id )
    {
        if (fs_tmp->flag == READ)
        {
            flash_read( fs_tmp, buf);
            return 0;
        }
    }

    printf("id = %d no found\n",id);
    printf("fs_tmp->start_address = %d, fs_tmp->id = %d\n", fs_tmp->start_address, fs_tmp->id );
    printf("read err\n");
    return 1; //error
}


int flash_read( fs *fsp, char *buf )
{
    int i;
    char *buff = buf;
    int start = fsp->start_address + sizeof(fs); 
    for( i = 0; i < fsp->len; i++ )
    {
       // printf("mem[%d] = %c\n",start, *buf );
        sim_read_byte( start, buff);
        
        start++;
        buff++;
    } 

    buf = buff - fsp->len;
}

int fs_write(char id, char *buf)
{
    all_len += fs_strlen(buf) + sizeof(fs);

    fs *fsp_tmp = fsp;
    fs *element = fsp;//(fs *)malloc(sizeof(fs));
    int i;
    char *p = (char *)fsp;
    int start;
   
    /*read first node*/
    start = sector;
    for( i = 0; i < sizeof(fs); i++ )
    {
        sim_read_byte( start, p);
        start++;
        p++;
    }
    fsp_tmp =  (fs *)( p - sizeof(fs));
    p       =  (char *)(p - sizeof(fs));

    /* first node */
    if ( fsp_tmp->fs_list == 1 )
    {
        if ( fsp_tmp->id == id )
        {
            if ( fsp_tmp->flag == READ)
            {
                printf("1\n");
                fsp_tmp->flag = NO_USE;
            }
        }
        
        if( all_len > 100 )// FLASH_SECTOR_SIZE )
        {       
            flash_write(fsp_tmp, NULL); 
            /* firse node write in anther sector,it's number fs_list should be 1,*/
            printf("Now,sector copy to another\n\n\n");
            sector       ^= cd_another;
            fsp_tmp->id   = id;
            fsp_tmp->flag = READ;
            copy_all( sector, fsp, fsp_tmp, buf);
            sim_erase_sector(sector ^ cd_another);
            
            return 0;
        
         }
        fsp_tmp->fs_list = 0;
        flash_write(fsp_tmp, NULL);  

        element->start_address = fsp_tmp->start_address + sizeof(fs) + fsp_tmp->len;
        element->flag          = READ;
        element->len           = fs_strlen(buf);
        element->id            = id;
        element->fs_list = 1;
        flash_write(element, buf);

        //free(element);
        return 0;
    }


    while ( fsp_tmp->fs_list != 1 ) 
    {
        if(fsp_tmp->fs_list != 0 )
        {
            printf("error\n\n\n");
            printf("fs_write: fsp_tmp->start_address = %c, fsp_tmp->id = %d, fsp_tmp->fs_list = %d\n", fsp_tmp->start_address, fsp_tmp->id, fsp_tmp->fs_list );
            exit(0);
        }

        if ( fsp_tmp->id == id )
        {
            if ( fsp_tmp->flag == READ)
            {
                fsp_tmp->flag = NO_USE;
                //fsp_tmp->fs_list = 0;
                flash_write(fsp_tmp, NULL);              
            }
        }
        
        //next fs block
        start = fsp_tmp->start_address + sizeof(fs) + fsp_tmp->len;
        for( i = 0; i < sizeof(fs); i++ )
        {
            sim_read_byte( start, p);
            start++;
            p++;
        }
        fsp_tmp =  (fs *)( p - sizeof(fs));
        p       =  (char *)(p - sizeof(fs));
    }

    /* judge last node id and flag */
    if ( fsp_tmp->id == id )
    {
        if ( fsp_tmp->flag == READ)
        {
            fsp_tmp->flag = NO_USE;
            //fsp_tmp->fs_list = 0;
            //flash_write(fsp_tmp, NULL);              
        }
    }
  
    if( all_len > 100)// FLASH_SECTOR_SIZE )
    {  
        flash_write(fsp_tmp, NULL);

        /* firse node write in anther sector,it's number fs_list should be 1,*/
        printf("Now, sector copy to another\n");
        sector       ^= cd_another;
        fsp_tmp->id   = id;
        fsp_tmp->flag = READ;
        copy_all( sector, fsp, fsp_tmp, buf);
        sim_erase_sector(sector ^ cd_another);

        return 0;

    }

    fsp_tmp->fs_list = 0;
    flash_write(fsp_tmp, NULL);    
    
    element->start_address = fsp_tmp->start_address + sizeof(fs) + fsp_tmp->len;
    element->flag          = READ;
    element->len           = fs_strlen(buf);
    element->id            = id;
    element->fs_list = 1;
    flash_write(element, buf);    

    //free(element);
    return 0;
}

int main()
{
    fsp = (fs *)malloc( sizeof(fs) );
    char *buf = (char *)malloc(50);
    int id1 = 2;
    int id2 = 0;
    int id3 = 1;
    sim_init();

 //while(1){          

    fs_write_and_init(0, sector, "123");
 
    fs_write(1, "455");
    fs_write(2, "788");
    fs_write(3, "abb");  
    fs_write(1, "456");
    fs_write(2, "789");
    fs_write(3, "acc");//////  
   

    fs_write(1, "455");
    fs_write(2, "788");
    fs_write(3, "abb");  
    fs_write(1, "456");
    fs_write(2, "789");
    fs_write(3, "acc");//////  
//
    fs_write(1, "466");
    fs_write(2, "1234567890788");
    fs_write(3, "abc");  
    fs_write(1, "456");/////
//   
    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  

    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  
    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  

    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  

    fs_write(2, "1234567890789");
    fs_write(3, "abcd"); 

   
    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  
    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  

    fs_write(2, "1234567890789");
    fs_write(3, "abcd");  

    fs_write(2, "12345689");
    fs_write(3, "abcdsdffd");  
    fs_write(2, "1234567890789");
    fs_write(1, "abcd");  

    fs_write(2, "12345689");
    fs_write(0, "abcdsdffd");  

    memset(buf, 0, 50);

    fs_read(id1, buf);
    printf("\n\nbuf id %d = %s\n\n\n",id1,buf);
    
    memset(buf, 0, 50);
    fs_read(id2, buf);
    printf("buf id %d = %s\n\n\n",id2,buf);
    
    memset(buf, 0, 50);
    fs_read(id3, buf);
    printf("buf id %d = %s\n\n\n", id3, buf);

    memset(buf,0,50);
    fs_read(3, buf);
    printf("buf id %d = %s\n\n\n", 3, buf);
//}
    return 0;
}


void copy_all(int sector_address, fs *fsp, fs *fsp_tmp, char *buf)
{
    char *buff = (char *)malloc(100);
    int i;
    char *p = (char *)fsp_tmp;
    fs *fsp_copy = (fs *)malloc(sizeof(fs)); // keep previous fs struct;
    char tmp_fs_list;
    int  tmp_address;
    
    fsp_tmp->len = fs_strlen(buf);

    /*write the first fs in new sector*/
    fsp_tmp->start_address = sector_address;
    fsp_tmp->fs_list       = 1;
    flash_write(fsp_tmp, buf);

    fs_memcpy(fsp_copy,fsp_tmp,sizeof(fs));

    /*read first node */
    int start = sector ^ cd_another;
    for( i = 0; i < sizeof(fs); i++ )
    {
        sim_read_byte( start, p);
        start++;
        p++;

    }
    fsp_tmp =  (fs *)(p - sizeof(fs));
    p       =  (char *)(p - sizeof(fs));

    /* for read old sector data */
    sector = sector ^ cd_another;

    if (fsp_tmp->flag == READ)
    { 
        /*write head fs_list,mean to no last one*/
        fsp_copy->fs_list = 0;
        flash_write(fsp_copy,NULL);
    
        fs_read(fsp_tmp->id, buff );

        tmp_address = fsp_tmp->start_address;
        fsp_tmp->start_address = fsp_copy->start_address + fsp_copy->len + sizeof(fs);
        
        tmp_fs_list =fsp_tmp->fs_list;
        fsp_tmp->fs_list = 1;

        flash_write( fsp_tmp, buff);

        fs_memcpy(fsp_copy,fsp_tmp,sizeof(fs));

        fsp_tmp->start_address = tmp_address;
        fsp_tmp->fs_list = tmp_fs_list;
    }
    /* from first one begin*/
    while ( fsp_tmp->fs_list != 1 )
    {   
        if( fsp_tmp->fs_list != 0 )
        {
            printf("read error\n");
            printf("fs_write: fsp_tmp->start_address = %c, fsp_tmp->id = %d, fsp_tmp->fs_list = %d\n", fsp_tmp->start_address, fsp_tmp->id, fsp_tmp->fs_list );
            exit(0);
        }

        start   = fsp_tmp->start_address + fsp_tmp->len + sizeof(fs);
        for( i = 0; i < sizeof(fs); i++ )
        {
            sim_read_byte( start, p );
            start++;
            p++;
        }
        fsp_tmp =  (fs *)( p - sizeof(fs));
        p       = p - sizeof(fs);
        
        /* put node in another sector*/
        if ( fsp_tmp->flag == READ )
        {
            memset(buff,0,100);
            /*write head fs_list,mean to no last one*/
            fsp_copy->fs_list = 0;
            flash_write(fsp_copy,NULL);
    
            fs_read( fsp_tmp->id, buff );

            tmp_address = fsp_tmp->start_address;
            fsp_tmp->start_address = fsp_copy->start_address + fsp_copy->len + sizeof(fs);
            
            tmp_fs_list = fsp_tmp->fs_list;
            fsp_tmp->fs_list = 1;

            flash_write( fsp_tmp, buff);

            all_len = fsp_tmp->start_address + fsp_tmp->len + sizeof(fs) - (sector ^ cd_another);    
            
            fs_memcpy(fsp_copy,fsp_tmp,sizeof(fs));

            fsp_tmp->start_address = tmp_address;
            fsp_tmp->fs_list = tmp_fs_list;
            
        }
    }

    printf("copy end!!!\nall_len = %d\n\n",all_len);
    sector = sector ^ cd_another;
    free(buff);
    free(fsp_copy);
}

