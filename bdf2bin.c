#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//align(8) chars = amount of characters
//align(8) encoding table - 2 byte array [chars]
//align(8) bitmap table - 16 byte array [chars]

#define ROUND2_UP(val,bit) (( (val>>bit) 	\
	+ ( (val&((1<<bit)-1)) ? 1 : 0 ) ) << bit)

typedef struct {
	uint8_t bmap[16];
} bmchar_t;

int main(int argc,char** argv)
{
	FILE* bdf,*bin;
	char buf[512];
	
	uint64_t chars = 1;
	uint16_t* encodings;
	bmchar_t* bitmaps;
	
	int enc = 0;
	
	int parse_chr = 0;
	int parse_bm = 0;
	
	int cur_chr = 0;
	int cur_bm = 0;
	
	bdf = fopen(argv[1],"rb");
	
	while(fgets(buf,sizeof(buf),bdf))
	{
		if(!memcmp(buf,"CHARS ",6))
		{
			sscanf(buf,"CHARS %u\n",&chars);
			printf("chars %u\n",chars);
			
			encodings = (uint16_t*)calloc(chars,sizeof(uint16_t));
			bitmaps = (bmchar_t*)calloc(chars,sizeof(bmchar_t));
		}
		else if(!memcmp(buf,"ENDFONT",7)) break;
		else if(!memcmp(buf,"STARTCHAR",9))
			parse_chr = 1;
		if(parse_chr)
		{
			if(parse_bm)
			{
				if(!memcmp(buf,"ENDCHAR",7))
				{
					parse_chr = 0;
					parse_bm = 0;
					cur_bm = 0;
					printf("cur_chr %u\n",cur_chr);
					if(++cur_chr == chars) break;
				}
				else
				{
					int a;
					sscanf(buf,"%02X\n",&a);
					bitmaps[cur_chr].bmap[cur_bm] = a&0xFF;
					printf("bm %x\n",a);
					cur_bm++;
				}
			}
			else
			{
				if(!memcmp(buf,"ENCODING ",9))
				{
					sscanf(buf,"ENCODING %u\n",&enc);
					encodings[cur_chr] = enc;
					printf("char %u\n",enc);
				}
				else if(!memcmp(buf,"BITMAP",6))
					parse_bm = 1;
			}
		}
	}
	fclose(bdf);
	
	
	bin = fopen(argv[2],"wb");
	fwrite(&chars,sizeof(uint64_t),1,bin);
	fwrite(encodings,chars<<1,1,bin);
	fwrite(bitmaps,chars<<4,1,bin);
	fclose(bin);
	
	free(encodings);
	free(bitmaps);
	return 0;
}
