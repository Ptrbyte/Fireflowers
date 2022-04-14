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
	int x, y;			//�̻�����ǰ����
	int hx, hy;			//����λ��
	int height;			//�߶�
	bool isshoot;		//�Ƿ���
	DWORD t1, t2, dt;	//�����ٶ�
	IMAGE Jetimg[2];	//һ��һ������ͼƬ
	unsigned char n : 1;

}jet[NUM];

struct Fire
{
	int x, y;//�̻��ڴ���λ��
	int cx, cy;//�̻���ͼƬ�е�λ��

	int r;//��ը�뾶
	int maxr;//���뾶
	int width, height;//ͼƬ�Ŀ��
	DWORD t1, t2, dt;//�����ٶ�

	bool isboom;//�Ƿ�ը
	bool isdraw;//�Ƿ����
	
	int pix[240][240];//ͼƬ����
}fire[NUM];

void welcome()
{
	setcolor(YELLOW);//������ɫ

	//ģ��켣
	int R = 120;
	int xoff = 600, yoff = 400;//Բ������
	
	for (int i = 90; i >= -210; i -= 6)
	{
		int x = xoff + R*cos(i / 180.0 * PI);
		int y = yoff + R*sin(i / 180.0 * PI);

		cleardevice();

		settextstyle((90 - i) / 300.0 * 50, 0, "����");

		// outtextxy��ָ����λ�ã���������
		outtextxy(x - 80, y, "���������");
		outtextxy(x - 10, y + 100, "�׸�ֿ��ĳĳĳ");

		Sleep(10);
	}
	getchar();
	cleardevice();

	settextstyle(40, 0, "����");
	outtextxy(500, 150, "�Ұ���");
	outtextxy(500, 200, "������������");
	outtextxy(500, 250, "��ѩ����");
	outtextxy(500, 300, "ƽ������");
	outtextxy(500, 350, "��ƶ����");
	outtextxy(500, 400, "�ٻ�����");
	outtextxy(500, 450, "Ŀ������Ҳ����");
	
	getchar();
}

void Initfire(int i)
{
	//��ʼ���̻���
	jet[i].t1 = timeGetTime();
	jet[i].dt = rand() % 10;
	jet[i].n = 0;
	jet[i].isshoot = false;

	// �ֱ�Ϊ���̻����ĵ�ͼƬ��Ե����Զ���롢�̻����ĵ�ͼƬ���Ͻǵľ��� (x��y) ��������
	int r[13] = { 120, 120, 155, 123, 130, 147, 138, 138, 130, 135, 140, 132, 155 };
	int x[13] = { 120, 120, 110, 117, 110, 93, 102, 102, 110, 105, 100, 108, 110 };
	int y[13] = { 120, 120, 85, 118, 120, 103, 105, 110, 110, 120, 120, 104, 85 };

	//��ʼ���̻�
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

//����ͼƬ
void loadImages()
{
	//�����̻������ص�
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

	//�����̻���
	IMAGE jm;
	loadimage(&jm, "fire/shoot.jpg");

	for (int i = 0; i < NUM; i++)
	{
		SetWorkingImage(&jm);
		int n = rand() % 5;

		getimage(&jet[i].Jetimg[0], n * 20, 0, 20, 50);
		getimage(&jet[i].Jetimg[1], (n + 5) * 20, 0, 20, 50);
	}
	SetWorkingImage();//��ԭ��ǰ����
}

//�̻�������
void shoot()
{
	for (int i = 0; i < NUM; i++)
	{
		jet[i].t2 = timeGetTime();

		if (jet[i].t2 - jet[i].t1>jet[i].dt && jet[i].isshoot == true)
		{
			//����
			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			//����λ��
			if (jet[i].y > jet[i].hy)
			{
				jet[i].n++;
				jet[i].y -= 5;
			}

			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			//������1/4������
			if ((jet[i].y - jet[i].hy) * 4 < jet[i].height)
				jet[i].dt = rand() % 4 + 10;

			//���������߶�
			if (jet[i].y <= jet[i].hy)
			{
				//�����̻���
				putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

				//��Ⱦ�̻�
				fire[i].x = jet[i].hx + 10;
				fire[i].y = jet[i].hy;
				fire[i].isboom = true;

				jet[i].isshoot = false;

				// �رյ��̻�����Ч�������ű�ը����Ч, �����´򿪵��̻�����Ч
				char c1[64], c2[64];
				sprintf(c1, "close s%d", i);
				sprintf(c2, "play f%d", i);
				mciSendString(c1, 0, 0, 0);
				mciSendString(c2, 0, 0, 0);

				sprintf_s(c1, sizeof(c1), "open fire/shoot.mp3 alias s%d", i);
				mciSendString(c1, 0, 0, 0);

			}
			//�����̻���ʱ��
			jet[i].t1 = jet[i].t2;
		}
	}
}

//�����̻�
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

			//�̻����ڴ����ϵ�����
			int xx = (int)(fire[i].x + fire[i].r*cos(a));
			int yy = (int)(fire[i].y - fire[i].r*sin(a));

			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800)
				pm[yy * 1200 + xx] = BGR(fire[i].pix[x1][y1]);	// �Դ���������̻�
		}
	}

	fire[i].isdraw = false;
}

//����ʧ���̻�
void clearimage()
{
	for (int i = 0; i < 2000; i++)
	{
		int px1 = rand() % 1200; // 0..1199
		int py1 = rand() % 800;  // 0.799

		pm[py1 * 1200 + px1] = BLACK;
		pm[py1 * 1200 + px1 + 1] = BLACK;	// ���Դ渳ֵ�������ص�		
	}
}

//�����ȼ�̻�
void chose(DWORD t1)
{
	DWORD t2 = timeGetTime();

	if (t2 - t1 > 100)
	{
		int n = rand() % 20;
		
		if (n < 13 && jet[n].isshoot == false && fire[n].isboom == false)
		{
			//�����̻�����׼������
			jet[n].x = rand() % 1200;
			jet[n].y = rand() % 100 + 600;
			jet[n].hx = jet[n].x;
			jet[n].hy = rand() % 400;

			jet[n].height = jet[n].y - jet[n].hy;
			jet[n].isshoot = true;

			//�����̻���ʼ״̬
			putimage(jet[n].x, jet[n].y, &jet[n].Jetimg[jet[n].n], SRCINVERT);

			/**** ����ÿ���̻��������� *****/
			char cmd[50];
			sprintf(cmd, "play s%d", n);
			mciSendString(cmd, 0, 0, 0);
		}
		t1 = t2;
	}
}

//�����̻�
void showfire()
{
		// �̻����׶�����ʱ������������������Ч��
		// Ϊʲô�����С����Ϊ16��
		// Ŀǰ�̻������뾶��155��׼���԰뾶/10�ɿ̶ȣ���ͬ�İ뾶�������ٶȲ�ͬ
		// �뾶Խ������Խ��
		//              10 20 30 40 50 
		int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

		for (int i = 0; i < NUM; i++) {
			fire[i].t2 = timeGetTime();

			// ���ӱ�ը�뾶�������̻�������ʱ����������Ч��
			if (fire[i].t2 - fire[i].t1 > fire[i].dt && fire[i].isboom == true) 
			{
				// �����̻��뾶
				if (fire[i].r < fire[i].maxr) 
				{
					fire[i].r++;
					fire[i].dt = drt[fire[i].r / 10];
					fire[i].isdraw = true;
				}

				// �����̻��������³�ʼ������ŵķɵ����̻�
				if (fire[i].r >= fire[i].maxr) 
				{
					fire[i].isdraw = false;
					Initfire(i);

					// �رձ�ը��Ч�������´򿪱�ը��Ч
					char cmd[64];
					sprintf(cmd, "close f%d", i);
					mciSendString(cmd, 0, 0, 0);

					sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
					mciSendString(cmd, 0, 0, 0);
				}

				// �����̻���ʱ��
				fire[i].t1 = fire[i].t2;
			}

			// ����ָ�����̻�
			drawFire(i);
	}
}

void heartFire(DWORD& st1)
{
	DWORD st2 = timeGetTime();

	if (st2 - st1 > 20000)		// 20��
	{
		// �Ȳ������ڷ��͵��̻���
		for (int i = 0; i < 13; i++) {
			if (jet[i].isshoot)
				putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);
		}


		// ��������
		int x[13] = { 600, 750, 910, 1000, 950, 750, 600, 450, 250, 150, 250, 410, 600 };
		int y[13] = { 650, 530, 400, 220, 50, 40, 200, 40, 50, 220, 400, 530, 650 };
		for (int i = 0; i < NUM; i++)
		{
			jet[i].x = x[i];
			jet[i].y = y[i] + 750;  //ÿ���̻����ķ�����붼��750��ȷ��ͬʱ��ը
			jet[i].hx = x[i];
			jet[i].hy = y[i];


			jet[i].height = jet[i].y - jet[i].hy;
			jet[i].isshoot = true;
			jet[i].dt = 7;
			// ��ʾ�̻���
			putimage(jet[i].x, jet[i].y, &jet[i].Jetimg[jet[i].n], SRCINVERT);

			/**** �����̻����� ***/
			fire[i].x = jet[i].x + 10;
			fire[i].y = jet[i].hy;
			fire[i].isboom = false;
			fire[i].r = 0;
		}
		st1 = st2;
	}
}

//��ʼ��
void Init()
{
	initgraph(1200, 800);

	// ���ű�������
	mciSendString("play fire/bg.mp3 repeat", 0, 0, 0);

	for (int i = 0; i < NUM; i++)//��ʼ���̻����̻���
		Initfire(i);

	loadImages();

	pm = GetImageBuffer();

	// ����Ч�����ñ���
	char cmd[128];
	for (int i = 0; i < 13; i++) 
	{
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.mp3 alias s%d", i);
		mciSendString(cmd, 0, 0, 0); // ��13��

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
		mciSendString(cmd, 0, 0, 0); // ��13��
	}
}

int main()
{
	Init();
	welcome();

	DWORD t1 = timeGetTime();//ɸѡ�̻�
	DWORD ht1 = timeGetTime();//���Ż�����ʱ

	BeginBatchDraw();

	while (!kbhit())
	{
		Sleep(10);
		clearimage();

		chose(t1);
		shoot();
		showfire();

		heartFire(ht1);

		FlushBatchDraw();//��ʾǰ�����в���
	}
	

	system("pause");
	return 0;
}

