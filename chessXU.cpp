
#include<stdio.h>
#include<iostream>
#include<conio.h>
#include<easyx.h>
#include<random>
#include<vector>
#include<Windows.h>
#include"tools.h"
#include "mmsystem.h"			//音乐播放头文件
#pragma comment(lib,"Winmm.lib")
IMAGE bgp;
IMAGE chessBlackImg;
IMAGE chessWhiteImg;
int if_end ;//是否结束，结束为0，没结束为1
class each_point
{
public:
	int m_score=0;
	bool if_placed=1;
	int bORw = 0;//黑为-1,白为1
};
int BW[2] = { -1,1 };
each_point chessboard[16][16];
int num = 0;
//两个位移数组遍历8方向
const int dx[2][4] = { -1,-1,-1,0,
						1,1,1,0};
const int dy[2][4] = {-1,0,1,-1,
						1,0,-1,1};

void menu();
void draw_line();
void draw_point();
int draw_piece(int m, int n,int if_end);
void mouse();
void mouse_pve();
void start_pvp();
void start_pve();
void close(each_point p);
void judge(each_point p,int u,int v);
void restart_or_end();//游戏结束后的小菜单界面
bool aiThink(int i);
void draw_right_side();

void button(int x, int y, int w, int h, const char a[20])
{
	setbkmode(TRANSPARENT);
	
	settextstyle(50, 16, _T("华文新魏"));
	

	int tx = x + (w - textwidth(_T(a))) / 2;
	int ty = y + (h - textheight(_T(a))) / 2;
	outtextxy(tx, ty, _T(a));
	
}
void judge_button()
{

	int tx = 437 + (171 - textwidth(_T("玩家对战")))/2;
	int ty = 141 + (86 - textheight(_T("李磊")))/2;
	MOUSEMSG mouse;
	while (true)
	{
		settextstyle(50, 16, _T("华文新魏"));
		mouse = GetMouseMsg();
		int color = 0;
		if (mouse.x > 437 && mouse.x < 608 && mouse.y>141 && mouse.y < 227)
		{
			color = 1;
		}
		else if (mouse.x > 437 && mouse.x < 608 && mouse.y>265 && mouse.y < 345)
		{
			color = 2;
		}
		switch (color)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(tx, ty, _T("玩家对战"));
			outtextxy(tx, ty+124, _T("人机对弈"));
			goto flag;
		case 1:
			settextcolor(WHITE);
			outtextxy(tx, ty, _T("玩家对战"));
			goto flag;
		case 2:
			settextcolor(WHITE);
			outtextxy(tx, ty+124, _T("人机对弈"));
			goto flag;
		default:
			goto flag;
		}
	flag:
		if (mouse.uMsg == WM_LBUTTONDOWN && mouse.x > 437 && mouse.x < 608 && mouse.y>141 && mouse.y < 227)
		{
			start_pvp();
			break;
		}
		else if (mouse.uMsg == WM_LBUTTONDOWN && mouse.x > 437 && mouse.x < 608 && mouse.y>265 && mouse.y < 345)
		{
			start_pve();
			break;
		}
	}
}
void menu()
{
	num = 0;
	for (int a = 0; a < 16; a++)
	{
		for (int b = 0; b < 16; b++)
		{
			chessboard[a][b].bORw = 0;
			chessboard[a][b].if_placed = 1;
			chessboard[a][b].m_score = 0;
		}
	}
	if_end = 1;

	IMAGE img;
	loadimage(&img, _T("start.png"), 640, 480, true);
	putimage(0, 0, &img);
	settextcolor(BLACK);
	button(437, 141, 171, 86, "玩家对战");
	button(437, 265, 171, 86, "人机对弈");
	judge_button();
	
}
void draw_right_side()
{
	IMAGE white_bowl;
	loadimage(&white_bowl, _T("bowly.png"), 100, 80);
	drawPNG( &white_bowl,512, 80);
	IMAGE black_bowl;
	loadimage(&black_bowl, _T("bowly2.png"), 100, 80);
	drawPNG(&black_bowl, 515, 300);
	settextcolor(BLACK);
	settextstyle(30, 10, _T("华文行楷"));
	outtextxy(532, 210, _T("对阵"));
	IMAGE hand;
	loadimage(&hand, _T("hand.png"), 50, 50);
	//
	//
	drawPNG(&hand, 538, 328);

}
void draw_line()
{
	setlinecolor(BLACK);
	line(5,5,5,475);
	line(5, 5, 475, 5);
	line(5, 475, 475, 475);
	line(475, 5, 475, 475);
	setlinecolor(BROWN);
	for (int x = 15; x < 480; x += 30)
	{
		line(x, 15, x, 465);
	}
	for (int y = 15; y < 480; y += 30)
	{
		line(15, y, 465, y);
	}
}

void draw_point()
{
	setfillcolor(BLACK);
	fillcircle(4 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(4 * 30 - 15, 12 * 30 - 15, 3);
	fillcircle(8 * 30 - 15, 8 * 30 - 15, 3);
	fillcircle(12 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(12 * 30 - 15, 12 * 30 - 15, 3);
}

int draw_piece(int m, int n,int if_end)
{
	IMAGE white_bowl;
	loadimage(&white_bowl, _T("bowly.png"), 100, 80);
	IMAGE black_bowl;
	loadimage(&black_bowl, _T("bowly2.png"), 100, 80);
	IMAGE hand;
	loadimage(&hand, _T("hand.png"), 50, 50);
	if (if_end == 0) return 0;
	loadimage(&chessBlackImg, _T("black.png"), 30, 30, true);
	loadimage(&chessWhiteImg, _T("white.png"), 30, 30, true);
	int x = m - (m % 30)+15 ;
	int y = n - (n % 30) +15;
	int u = (x - 15) / 30;
	int v = (y - 15) / 30;
	if (chessboard[v][u].if_placed == 0)//已经下过的地方不能再下
		return false;
	else {
		if (num % 2 == 0)
		{
			drawPNG(&chessBlackImg, x - 15, y - 15);
			mciSendString(_T("play down.wav"), 0, 0, 0);//落子音效	
			drawPNG(&black_bowl, 515, 300);
			drawPNG(&hand, 535, 110);
			chessboard[v][u].bORw = -1;
		}
		
		else if (num % 2 == 1)
		{
			drawPNG(&chessWhiteImg, x - 15, y - 15);
			mciSendString(_T("play down2.wav"), 0, 0, 0);
					drawPNG(&white_bowl, 512, 80);
					drawPNG(&hand, 538, 328);
				chessboard[v][u].bORw = 1;
		}
		chessboard[v][u].if_placed = 0;
		num += 1;
	}
	judge(chessboard[v][u],u,v);
	return 1;
}

void mouse()
{
	
	MOUSEMSG m;
	while (1)
	{
		
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN&&m.x<=480) //若鼠标左击
		{
			draw_piece(m.x, m.y,if_end);//m.x,m.y为点击处坐标
			Sleep(200);
		}
	}
}

void mouse_pve()
{
	MOUSEMSG m;
	int i = 0;//0人执黑，1人执白
	int a=0;

	while (1)
	{		
	there:
		if (a % 2 == 0) {
			m = GetMouseMsg();
		
			switch (m.uMsg)
			{
				
			case  WM_LBUTTONDOWN:
				if (chessboard[(m.y - (m.y % 30)) / 30][(m.x - (m.x % 30)) / 30].if_placed==1&&m.x<=480)
				{
					draw_piece(m.x, m.y,if_end);
					a++;
					break;
				}
				else
				{
					goto there;
				}
			default:
				continue;
			}
		}
		else if (a % 2 == 1)
		{
			head:
			bool IF=aiThink(i);
			if (IF==true)
			{
				goto behind;
			}
			else
				goto head;

			behind:
			a++;
		}
		
	}
	
}

bool aiThink(int o)
{
	for (int left_row = 0; left_row < 16; left_row++) chessboard[left_row][0].m_score = -1;
	for (int up_col = 0; up_col < 16; up_col++) chessboard[0][up_col].m_score = -1;
	for (int right_row = 0; right_row < 16; right_row++) chessboard[right_row][15].m_score = -1;
	for (int down_col = 0; down_col < 16; down_col++) chessboard[15][down_col].m_score = -1;
	/*   主要思想
	电脑白子  自己黑子
	白子连成五子	+10000	阻碍黑子连成五子	+1000
	白子连成活四	+500	阻碍黑子连成活四	+100
	白子连成死四	+50	    阻碍黑子连成死四	+20
	白子连成活三	+30	    阻碍黑子连成活三	+10
	白子连成死三	+8	    阻碍黑子连成死三	+5
	白子连成活二	+2	    阻碍黑子连成活二	+1
	白子连成死二	+2	    阻碍黑子连成死二	+1
	白子连成活一	+1	    阻碍黑子连成活一	+0
	白子连成死一	+1	    阻碍黑子连成死一	+0
*/
	int two_sides = 0;//判断两端是否被阻塞，0为无阻塞，1为一端阻塞，2为两端阻塞
	for (int a = 0; a < 16; a++)
	{
		for (int b = 0; b < 16; b++)
		{
			if (chessboard[a][b].if_placed == 0)//如果该位置被下了，跳过到下一个位置
				continue;
			//ai防守
			//观察人类落子情况
			else
			{

				//开始给每点打分			
				for (register int i = 0; i < 4; i++)
				{
					int score = 0;
					int two_sides = 0;
					for (register int k = 0; k < 2; k++)
					{
						int copy_u = b;
						int copy_v = a;
						while (copy_u >= 0 && copy_u <= 15 && copy_v >= 0 && copy_v <= 15)
						{
							copy_u += dx[k][i];
							copy_v += dy[k][i];
							if (copy_u < 0 || copy_u >15 || copy_v < 0 || copy_v > 15)//如果该方向超出范围
							{
								two_sides++;//阻塞数加一
								copy_u -= dx[k][i];
								copy_v -= dy[k][i];
								break;//判断另一方向
							}
							else if (chessboard[copy_v][copy_u].bORw == BW[o])//位移方向有人类落子
								score++;
							else if (chessboard[copy_v][copy_u].bORw == BW[1 - o])//位移方向是ai落子
							{
								two_sides++;
								break;
							}
							else //位移方向是未落子
								break;
						}
						
					}
					//赋权重
					if (score == 1 && two_sides == 1)//阻碍黑子连成死二 + 1
						chessboard[a][b].m_score += 1;
					else if (score == 1 && two_sides == 0) //阻碍黑子连成活二 + 1
						chessboard[a][b].m_score += 1;
					else if (score == 2 && two_sides == 1) //阻碍黑子连成死三 + 5
						chessboard[a][b].m_score += 5;
					else if (score == 2 && two_sides == 0) //阻碍黑子连成活三 + 10
						chessboard[a][b].m_score += 10;
					else if (score == 3 && two_sides == 1) //阻碍黑子连成死四 + 20
						chessboard[a][b].m_score += 10;
					else if (score == 3 && two_sides == 0) //阻碍黑子连成活四 + 100
						chessboard[a][b].m_score += 100;
					else if (score == 4 ) //阻碍黑子连成五子 + 1000
						chessboard[a][b].m_score += 1000;
				
			    }
				//ai进攻
				//观察ai落子情况

						//开始给每点打分			
				for (register int i = 0; i < 4; i++)
				{
					int score = 0;
					int two_sides = 0;
					for (register int k = 0; k < 2; k++)
					{
						int copy_u = b;
						int copy_v = a;
						while (copy_u >= 0 && copy_u <= 15 && copy_v >= 0 && copy_v <= 15)
						{
							copy_u += dx[k][i];
							copy_v += dy[k][i];
							if (copy_u < 0 || copy_u >15 || copy_v < 0 || copy_v > 15)//如果该方向超出范围
							{
								two_sides++;//阻塞数加一
								copy_u -= dx[k][i];
								copy_v -= dy[k][i];
								break;//判断另一方向
							}
							else if (chessboard[copy_v][copy_u].bORw == BW[1 - o])//位移方向有ai落子
								score++;
							else if (chessboard[copy_v][copy_u].bORw == BW[o])//位移方向是人类落子
							{
								two_sides++;
								break;
							}
							else //位移方向是未落子
								break;
						}

					}
					//赋权重
					if (score == 0&&a>3&&a<12&&b>3&&b<12)//白子连成活、死1   + 1
						chessboard[a][b].m_score += 0;
					else if (score == 1)//白子连成活、死2  +2
						chessboard[a][b].m_score += 2;
					else if (score == 2&&two_sides==1)//白子连成死3  +8
						chessboard[a][b].m_score += 8;
					else if (score ==2&&two_sides==0)//白子连成活三	 +30	
						chessboard[a][b].m_score += 30;
					else if (score == 3 && two_sides == 1)//白子连成死四  +50	
						chessboard[a][b].m_score += 50;
					else if (score == 3 && two_sides == 0)//白子连成活四  +500	
						chessboard[a][b].m_score += 500;
					else if (score == 4  )//白子连成五子  +10000	
						chessboard[a][b].m_score += 10000;

				}
			}
		}
	}

	int max_x = 0;
	int max_y = 0;
	//每个点的分数都打完了，找分数最高的点
	srand(unsigned(time(NULL)));
	for (int i=0;i<16;i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (chessboard[i][j].if_placed == 0)
			{
				chessboard[i][j].m_score = 0;
				continue;
			}
			else if (chessboard[i][j].m_score > chessboard[max_y][max_x].m_score)
				max_x = j, max_y = i;
			else if (chessboard[i][j].m_score == chessboard[max_y][max_x].m_score)
			{
				int rand_piece = rand() % 100 ;
				std::cout << std::endl << rand_piece << std::endl;
				if (rand_piece >= 70+abs(j-8)*2+abs(i-8)*2)max_x = j, max_y = i; else max_x = max_x, max_y = max_y;
			}
		}
	}
	Sleep(200);
	draw_piece(max_x*30+15, max_y*30+15,1);
	//每一次ai下完子后，清空所有点分数
	//system("cls");
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{		
			std::cout << chessboard[i][j].m_score << "\t";
			chessboard[i][j].m_score = 0;
		}
		std::cout << std::endl;
	}
	return true;
}

void start_pvp()
{
	setbkcolor RGB(247, 184, 129);	
	cleardevice();
	draw_line();
	draw_point();
	draw_right_side();
	mouse();
}
void start_pve()
{
	setbkcolor RGB(247, 184, 129);
	cleardevice();
	draw_line();
	draw_point();
	draw_right_side();
	mouse_pve();
}
void judge(each_point p,int u,int v)
{
	IMAGE win_white;
	IMAGE win_black;
	loadimage(&win_black, _T("black2.png"), 30, 30, true);
	loadimage(&win_white, _T("white2.png"), 30, 30, true);
	int score;
	for (register int i = 0; i < 4; i++)
  	{
		score = 0;
		int two_side_times[2] = { 0,0 };
		for (register int k = 0; k < 2; k++)
		{
			int copy_u = u;
			int copy_v = v;
			while ( copy_u >= 0 && copy_u <= 15 && copy_v >= 0 && copy_v <= 15)
			{
				copy_u += dx[k][i];
				copy_v += dy[k][i];
				if (chessboard[copy_v][copy_u].bORw == chessboard[v][u].bORw)
					score++, two_side_times[k]++;
				else
					break;
			}			
		}
		if (score >= 4)
		{
			if_end = 0;
			int win_pieces[5][2];//五个子的x、y
			int which_piece = 0;
			int uu = u, vv = v;
			for(int abc=0;abc<two_side_times[0];abc++)
			{
				uu += dx[0][i], vv += dy[0][i];
			}
			
			for (int yui = 0; yui < 5; yui++)
			{
				win_pieces[yui][0] = uu, win_pieces[yui][1] = vv;
				uu -= dx[0][i], vv -= dy[0][i];
			}

			//在五子上标出黄点
			for (int mark = 0; mark < 5; mark++)
			{
				if (p.bORw == -1)//若黑赢
				{
					drawPNG(&win_black, win_pieces[mark][0] * 30 , win_pieces[mark][1] * 30 );
					Sleep(200);
				}
				else//若白赢
				{
					drawPNG(&win_white, win_pieces[mark][0] * 30 , win_pieces[mark][1] * 30 );
					Sleep(200);
				}
			}
			
			close(p);
			
			return;
		}
	}
}

void restart_or_end()
{
	settextstyle(30, 10, _T("华文新魏"));
	button(255, 250, 105, 50, _T("主菜单"));
	button(255, 320, 105, 50, _T("退出游戏"));
	int tx1 = 255 + (105 - textwidth(_T("主菜单"))) / 2;
	int ty1 = 250 + (50 - textheight(_T("徐祺尚"))) / 2;
	int tx2 = 255 + (105 - textwidth(_T("退出游戏"))) / 2;
	int ty2 = 320 + (50 - textheight(_T("李冀仁"))) / 2;
	MOUSEMSG mouse2;
	while (1)
	{
		mouse2 = GetMouseMsg();
		int color = 0;
		if (mouse2.x > 255 && mouse2.x < 360 && mouse2.y>250 && mouse2.y < 300)
		{
			color = 1;
		}
		else if (mouse2.x > 255 && mouse2.x < 360 && mouse2.y>320 && mouse2.y < 370)
		{
			color = 2;
		}
		switch (color)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(tx1, ty1, _T("主菜单"));
			outtextxy(tx2, ty2, _T("退出游戏"));
			goto flag;
		case 1:
			settextcolor(WHITE);
			outtextxy(tx1, ty1, _T("主菜单"));
			goto flag;
		case 2:
			settextcolor(WHITE);
			outtextxy(tx2, ty1 + 70, _T("退出游戏"));
			goto flag;
		default:
			goto flag;
		}
	flag:
		if (mouse2.uMsg == WM_LBUTTONDOWN && mouse2.x > 255 && mouse2.x < 360 && mouse2.y>250 && mouse2.y < 300)
		{
			cleardevice();
			EndBatchDraw();
			menu();
			break;
		}
		else if (mouse2.uMsg == WM_LBUTTONDOWN && mouse2.x > 255 && mouse2.x < 360 && mouse2.y>320 && mouse2.y < 370)
		{
		   closegraph();
			break;
		}
	}
}
void close(each_point p)
{
	
	IMAGE close;
	settextcolor(BLACK);
	if (p.bORw == -1)
		//outtextxy(170, 100, _T("游戏结束，黑子胜"));
	{
		Sleep(1000);		
		loadimage(&close, _T("blackwin.png"), 515, 312);
		
		putimage(63, 84, &close);

		EndBatchDraw();
		restart_or_end();
	}
	else if (p.bORw == 1)
		//outtextxy(170, 100, _T("游戏结束，白子胜"));
	{
		Sleep(1000);
		loadimage(&close, _T("whitewin.png"), 515, 312);
		
		putimage(63, 84, &close);
		EndBatchDraw();
		restart_or_end();
	}
		
}

int main()
{
	initgraph(640, 480);
	menu();
	return 0;
}
