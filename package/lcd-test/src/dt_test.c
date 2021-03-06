

#include "dt.h"


#define FB_WIDTH 800
#define FB_HEIGHT 480
#define DOUBLE_BUFFER 0 //0/1
#define SCREEN_A 0

int main(void)
{
	unsigned long arg[4];
	int dispfh,memfh,fbfh0;
	unsigned long layer_hdl0;
	void *mem_addr0;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	__disp_color_t bk_color;
	__disp_fb_create_para_t fb_para;
	__disp_rect_t scn_win;

	
	if((dispfh = open("/dev/disp",O_RDWR)) == -1)
	{
		printf("open file /dev/disp fail. \n");
		return 0;
	}

	if((memfh= open("./memin_0.bin",O_RDONLY)) == -1)
	{
		printf("open file /drv/memin.bin fail. \n");
		return 0;
	}

    arg[0] = SCREEN_A;
	ioctl(dispfh,DISP_CMD_LCD_ON,(unsigned long)arg);

	bk_color.red = 0xff;
	bk_color.green = 0x00;
	bk_color.blue = 0x00;
	arg[0] = SCREEN_A;
	arg[1] = (unsigned long)&bk_color;
	ioctl(dispfh,DISP_CMD_SET_BKCOLOR,(unsigned long)arg);

//fb0
	fb_para.mode = DISP_LAYER_WORK_MODE_SCALER;
	fb_para.smem_len = 736 * FB_HEIGHT * 2/*32bpp*/ * (DOUBLE_BUFFER+1);
	fb_para.ch1_offset = 736 * FB_HEIGHT;
	fb_para.ch2_offset = 0;
	arg[0] = SCREEN_A;
	arg[1] = (unsigned long)&fb_para;
	layer_hdl0 = ioctl(dispfh,DISP_CMD_FB_REQUEST,(unsigned long)arg);
	if(layer_hdl0 <= 0)
	{
		printf("request fb fail\n");
	}

	
	printf("open frame buffer device.");
	if((fbfh0 = open("/dev/fb0",O_RDWR)) > 0)
	{
		printf("open fb0 ok\n");
	}
	else
	{
		printf("open fb0 fail!!!\n");	
		close(dispfh);
		return 0;
	}

	
	ioctl(fbfh0,FBIOGET_FSCREENINFO,&fix);


    mem_addr0 = mmap(NULL, fix.smem_len,PROT_READ | PROT_WRITE, MAP_SHARED, fbfh0, 0);
    memset(mem_addr0,0x80,fix.smem_len);
    read(memfh,mem_addr0,fix.smem_len/(DOUBLE_BUFFER+1),0);
	
	ioctl(fbfh0,FBIOGET_VSCREENINFO,&var);
	var.xoffset= 0;
	var.yoffset= 0;
	var.xres = FB_WIDTH;
	var.yres = FB_HEIGHT;
	var.xres_virtual = FB_WIDTH;
	var.yres_virtual = FB_HEIGHT*(DOUBLE_BUFFER+1);
	var.nonstd = 0;
	var.bits_per_pixel = 32;
	var.transp.length = 8;
	var.red.length = 8;
	var.green.length = 8;
	var.blue.length = 8;
	var.reserved[0] = DISP_MOD_MB_UV_COMBINED;
	var.reserved[1] = DISP_FORMAT_YUV422;
	var.reserved[2] = DISP_SEQ_UVUV;
	var.reserved[3] = 0;
	var.activate = FB_ACTIVATE_FORCE;
	ioctl(fbfh0,FBIOPUT_VSCREENINFO,&var);

    scn_win.x = 0;
    scn_win.y = 0;
    scn_win.width = FB_WIDTH;
    scn_win.height = FB_HEIGHT;
	arg[0] = SCREEN_A;
	arg[1] = layer_hdl0;
	arg[2] = (unsigned long)&scn_win;
    ioctl(dispfh,DISP_CMD_LAYER_SET_SCN_WINDOW,(unsigned long)arg);

#if 0
    printf("press any key to VESA_POWERDOWN\n");
    getchar();
    ioctl(fbfh0, FBIOBLANK, FB_BLANK_POWERDOWN); 

    printf("press any key to FB_BLANK_UNBLANK\n");
    getchar();
    ioctl(fbfh0, FBIOBLANK, FB_BLANK_UNBLANK); 
#endif

#if 0
    printf("press any key to close lcd\n");
    getchar();
	arg[0] = SCREEN_A;
    ioctl(dispfh,DISP_CMD_LCD_OFF,(unsigned long)arg);

    printf("press any key to open lcd\n");
    getchar();
	arg[0] = SCREEN_A;
    ioctl(dispfh,DISP_CMD_LCD_ON,(unsigned long)arg);
#endif

    printf("press any key to exit\n");
    getchar();

	arg[0] = SCREEN_A;
	arg[1] = layer_hdl0;
    ioctl(dispfh,DISP_CMD_FB_RELEASE,(unsigned long)arg);

	arg[0] = SCREEN_A;
    ioctl(dispfh,DISP_CMD_LCD_OFF,(unsigned long)arg);

	close(memfh);
	close(fbfh0);
	close(dispfh);
	return 0;
}
