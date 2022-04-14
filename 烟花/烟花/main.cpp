#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<time.h>
#include<math.h>
#include<conio.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")

#define PI 3.14
#define NUM 13

DWORD *pm;

struct Jet
{
	int x, y;			//烟花弹当前坐标
	int hx, hy;			//极限位置
	int height;			//高度
	bool isshoot;		//是否发射
	DWORD t1, t2, dt;	//发射速度
	IMAGE Jetimg[2];	//一明一暗两张图片
	unsigned char n : 1;

}jet[NUM];

struct Fire
{
	int x, y;//烟花在窗口位置
	int cx, cy;//烟花在图片中的位置

	int r;//爆炸半径
	int maxr;//最大半径
	int width, height;//图片的宽高
	DWORD t1, t2, dt;//绽放速度

	bool isboom;//是否爆炸
	bool isdraw;//是否绘制
	
	int pix[240][240];//图片像素
}fire[NUM];

void welcome()
{
	setcolor(YELLOW);//设置颜色

	//模拟轨迹
	int R = 120;
	int xoff = 600, yoff = 400;//圆心坐标
	
	for (int i = 90; i >= -210; i -= 6)
	{
		int x = xoff + R*cos(i / 180.0 * PI);
		int y = yoff + R*sin(i / 180.0 * PI);

		cleardevice();

		settextstyle((90 - i) / 300.0 * 50, 0, "楷体");

		// outtextxy在指定的位置，输入文字
		outtextxy(x - 80, y, "浪漫表白日");
		outtextxy(x - 10, y + 100, "献给挚爱某某某");

		Sleep(10);
	}
	getchar();
	cleardevice();

	settextstyle(40, 0, "楷体");
	outtextxy(500, 150, "我爱你");
	outtextxy(500, 200, "往后余生是你");
	outtextxy(500, 250, "风雪是你");
	outtextxy(500, 300, "平淡是你");
	outtextxy(500, 350, "清贫是你");
	outtextxy(500, 400, "荣华是你");
	outtextxy(500, 450, "目光所致也是你");
	
	getchar();
}

void Initfire(int i)
{
	//初始化烟花弹
	jet[i].t1 = timeGetTime();
	jet[i].dt = rand() % 10;
	jet[i].n = 0;
	jet[i].isshoot = false;

	// 分别为：烟花中心到图片边缘的最远距离、烟花中心到图片左上角的距离 (x、y) 两个分量
	int r[13] = { 120, 120, 155, 123, 130, 147, 138, 138, 130, 135, 140, 132, 155 };
	int x[13] = { 120, 120, 110, 117, 110, 93, 102, 102, 110, 105, 100, 108, 110 };
	int y[13] = { 120, 120, 85, 118, 120, 103, 105, 110, 110, 120, 120, 104, 85 };

	//初始化烟花
	fire[i].x = 0;
	fire[i].y = 0;
	fire[i].height = 240;
	fire[i].width = 240;
	fire[i].cx = x[i];
	fire[i].cy = y[i];
	fire[i].r = 0;
	fire[i].maxr = r[i];
	fire[i].isboom = false;
	fire[i].isdraw = false;
	fire[i].dt = 5;
	fire[i].t1 = timeGetTime();

}

//加载图片
void loadImages()
{
	//储存烟花的像素点
	IMAGE fm, gm;
	loadimage(&fm, "fire/flower.jpg");

	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&fm);
		getimage(&gm, i * 240, 0, 240, 240);
		SetWorkingImage(&gm);

		for (int j = 0; j < 240; j++)
			for (int k = 0; k < 240; k++)
				fire[i].pix[j][k] = getpixel(j, k);

	}

	//加载烟花弹
	IMAGE jm;
	loadimage(&jm, "fire/shoot.jpg");

	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&jm);
		int n = rand() % 5;

		getimage(&jet[i].Jetimg[0], n * 20, 0, 20, 50);
		getimage(&jet[i].Jetimg[1], (n + 5) * 20, 0, 20, 50);
	}
	SetWorkingImage();//还原当前窗口
}

//烟花弹升空
void shoot()
{
	for (int i = 0; i < NUM; i++)
	{
		jet[i].t2 = timeGetTime();

		if (jet[i].t2 - jet[i].t1>jet[i].dt && jet[i].isshoot == true)
		{
			//擦除
			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			//更新位置
			if (jet[i].y > jet[i].hy)
			{
				jet[i].n++;
				jet[i].y -= 5;
			}

			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			//上升到1/4处减速
			if ((jet[i].y - jet[i].hy) * 4 < jet[i].height)
				jet[i].dt = rand() % 4 + 10;

			//上升到最大高度
			if (jet[i].y <= jet[i].hy)
			{
				//擦除烟花弹
				putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

				//渲染烟花
				fire[i].x = jet[i].hx + 10;
				fire[i].y = jet[i].hy;
				fire[i].isboom = true;

				jet[i].isshoot = false;

				// 关闭点烟花的音效，并播放爆炸的音效, 并重新打开点烟花的音效
				char c1[64], c2[64];
				sprintf(c1, "close s%d", i);
				sprintf(c2, "play f%d", i);
				mciSendString(c1, 0, 0, 0);
				mciSendString(c2, 0, 0, 0);

				sprintf_s(c1, sizeof(c1), "open fire/shoot.mp3 alias s%d", i);
				mciSendString(c1, 0, 0, 0);

			}
			//更新烟花弹时间
			jet[i].t1 = jet[i].t2;
		}
	}
}

//绘制烟花
void drawFire(int i)
{
	if (!fire[i].isdraw)
		return;

	for (double a = 0; a <= 6.8; a += 0.1)
	{
		int x1 = (int)(fire[i].cx + fire[i].r*cos(a));
		int y1 = (int)(fire[i].cy - fire[i].r*sin(a));

		if (x1 > 0 && x1 < fire[i].width && y1>0 && y1 < fire[i].height)
		{
			int b = fire[i].pix[x1][y1] & 0xff;
			int g = (fire[i].pix[x1][y1] >> 8) & 0xff;
			int r = fire[i].pix[x1][y1] >> 16;

			//烟花点在窗口上的坐标
			int xx = (int)(fire[i].x + fire[i].r*cos(a));
			int yy = (int)(fire[i].y - fire[i].r*sin(a));

			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800)
				pm[yy * 1200 + xx] = BGR(fire[i].pix[x1][y1]);	// 显存操作绘制烟花
		}
	}

	fire[i].isdraw = false;
}

//逐渐消失的烟花
void clearimage()
{
	for (int i = 0; i < 2000; i++)
	{
		int px1 = rand() % 1200; // 0..1199
		int py1 = rand() % 800;  // 0.799

		pm[py1 * 1200 + px1] = BLACK;
		pm[py1 * 1200 + px1 + 1] = BLACK;	// 对显存赋值擦出像素点		
	}
}

//随机点燃烟花
void chose(DWORD t1)
{
	DWORD t2 = timeGetTime();

	if (t2 - t1 > 100)
	{
		int n = rand() % 20;
		
		if (n < 13 && jet[n].isshoot == false && fire[n].isboom == false)
		{
			//重置烟花弹，准备发射
			jet[n].x = rand() % 1200;
			jet[n].y = rand() % 100 + 600;
			jet[n].hx = jet[n].x;
			jet[n].hy = rand() % 400;

			jet[n].height = jet[n].y - jet[n].hy;
			jet[n].isshoot = true;

			//绘制烟花初始状态
			putimage(jet[n].x, jet[n].y, &jet[n].Jetimg[jet[n].n], SRCINVERT);

			/**** 播放每个烟花弹的声音 *****/
			char cmd[50];
			sprintf(cmd, "play s%d", n);
			mciSendString(cmd, 0, 0, 0);
		}
		t1 = t2;
	}
}

//绽放烟花
void showfire()
{
		// 烟花个阶段绽放时间间隔，制作变速绽放效果
		// 为什么数组大小定义为16？
		// 目前烟花的最大半径是155，准备以半径/10可刻度，不同的半径，绽放速度不同
		// 半径越大，绽放越慢
		//              10 20 30 40 50 
		int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

		for (int i = 0; i < NUM; i++) {
			fire[i].t2 = timeGetTime();

			// 增加爆炸半径，绽放烟花，增加时间间隔做变速效果
			if (fire[i].t2 - fire[i].t1 > fire[i].dt && fire[i].isboom == true) 
			{
				// 更新烟花半径
				if (fire[i].r < fire[i].maxr) 
				{
					fire[i].r++;
					fire[i].dt = drt[fire[i].r / 10];
					fire[i].isdraw = true;
				}

				// 销毁烟花，并重新初始化该序号的飞弹和烟花
				if (fire[i].r >= fire[i].maxr) 
				{
					fire[i].isdraw = false;
					Initfire(i);

					// 关闭爆炸音效，并重新打开爆炸音效
					char cmd[64];
					sprintf(cmd, "close f%d", i);
					mciSendString(cmd, 0, 0, 0);

					sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
					mciSendString(cmd, 0, 0, 0);
				}

				// 更新烟花的时间
				fire[i].t1 = fire[i].t2;
			}

			// 绘制指定的烟花
			drawFire(i);
	}
}

void heartFire(DWORD& st1)
{
	DWORD st2 = timeGetTime();

	if (st2 - st1 > 20000)		// 20秒
	{
		// 先擦除正在发送的烟花弹
		for (int i = 0; i < 13; i++) {
			if (jet[i].isshoot)
				putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);
		}


		// 心形坐标
		int x[13] = { 600, 750, 910, 1000, 950, 750, 600, 450, 250, 150, 250, 410, 600 };
		int y[13] = { 650, 530, 400, 220, 50, 40, 200, 40, 50, 220, 400, 530, 650 };
		for (int i = 0; i < NUM; i++)
		{
			jet[i].x = x[i];
			jet[i].y = y[i] + 750;  //每个烟花弹的发射距离都是750，确保同时爆炸
			jet[i].hx = x[i];
			jet[i].hy = y[i];


			jet[i].height = jet[i].y - jet[i].hy;
			jet[i].isshoot = true;
			jet[i].dt = 7;
			// 显示烟花弹
			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			/**** 设置烟花参数 ***/
			fire[i].x = jet[i].x + 10;
			fire[i].y = jet[i].hy;
			fire[i].isboom = false;
			fire[i].r = 0;
		}
		st1 = st2;
	}
}

//初始化
void Init()
{
	initgraph(1200, 800);

	// 播放背景音乐
	mciSendString("play fire/bg.mp3 repeat", 0, 0, 0);

	for (int i = 0; i < NUM; i++)//初始化烟花和烟花弹
		Initfire(i);

	loadImages();

	pm = GetImageBuffer();

	// 打开音效并设置别名
	char cmd[128];
	for (int i = 0; i < 13; i++) 
	{
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.mp3 alias s%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次
	}
}

int main()
{
	Init();
	welcome();

	DWORD t1 = timeGetTime();//筛选烟花
	DWORD ht1 = timeGetTime();//播放花样计时

	BeginBatchDraw();

	while (!kbhit())
	{
		Sleep(10);
		clearimage();

		chose(t1);
		shoot();
		showfire();

		heartFire(ht1);

		FlushBatchDraw();//显示前面所有操作
	}
	

	system("pause");
	return 0;
}

