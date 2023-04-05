#include<iostream>
#include<vector>
#include<stack>
#include<limits.h>
#include <stdlib.h>
#include<windows.h>
#include"Matrix.h"

using namespace std;
using namespace Numeric_lib;


struct task {
	int num; // the number of the task
	bool ct;  // judge whether the task is a cloud task
	double pri; //
	int FTl;  // the finish time of the task in a core
	int FTWS; // the finish time of the task in sending
	int FTC;   // the finish time of the task in cloud
	int FTWR;   // the finish time of the task in receiving
	int RTl; // the earlist  time that the task can start in local core
	int RTWS; // the earlist  time that the task can start in sending
	int RTC;  // the earlist  time that the task can start in cloud
	int RTWR; //the earlist  time that the task can start in receiving
	int ST; // the task's actual start time
	int chan; // illustrate which channel the task operate (local core = 0,1,2, cloud=3)
	bool exit;  //whether it is an exit task
	bool entry;  // whether it is an entry task
	int r1;
	int r2;
};

int c_t_i_re(int t_s,int t_c, int t_r)
{
	int t_i_re = t_s + t_c + t_r;
	return t_i_re;
}

int c_min_t_i_l(int (*local_core)[3],int i)
{
	int min = INT_MAX;
	for(int j=0;j<3;j++)
	{
		if(local_core[i][j] < min)
			min = local_core[i][j];
	}
	return min;
}

int l_min(task &t_i,Matrix<int, 2>&l_c)
{
	int min = INT_MAX;
	for(int j=0;j< l_c.dim2();j++)
	{
		if(l_c(t_i.num - 1,j) < min)
			min = l_c(t_i.num - 1,j);
	}
	return min;
}

float c_avg_t_i_l(int (*local_core)[3],int i)
{
	float avg = 0;
	for(int j=0;j<3;j++)
	{
		avg += local_core[i][j];
	}
	avg = avg/3;
	return avg;
}


int *primary_assignment(int (*local_core)[3], int t_s, int t_c, int t_r)
{
	int* c_p = new int[20];
	//c_p = (int *)malloc(sizeof(int) * 10);
	int t_i_re[20];
	int min_t_i_l;
	for(int i=0;i<20;i++)
	{
		t_i_re[i] = c_t_i_re(t_s,t_c,t_r);
		min_t_i_l = c_min_t_i_l(local_core,i);
		if(min_t_i_l <= t_i_re[i])//1 calculate in core,2 calculate in cloud
			c_p[i] = 1;
		else
            c_p[i] = 2;
	}
	return c_p;
}


float *c_w(int *c_p, int (*local_core)[3], int t_s, int t_c, int t_r)
{
	float *self_w = new float[20];
	for(int i=0;i<20;i++)
	{
		if(c_p[i] == 1)
			self_w[i] = c_avg_t_i_l(local_core,i);
		else
			self_w[i] = c_t_i_re(t_s,t_c,t_r);
	}
	/*cout<<endl;
	for(int i=0;i<10;i++)
		cout<<self_w[i]<<" ";*/
	return self_w;
}

float *task_priority(int (*graph)[20], int *c_p, int (*local_core)[3], int t_s, int t_c, int t_r)
{
	float **w =NULL;
	w = (float **)malloc(sizeof(float *) * 19);
	for(int r=0;r<20;r++)
		w[r] = (float *)malloc(sizeof(float) * 20);
	for(int i=0;i<19;i++)
		for(int j=0;j<20;j++)
			w[i][j] = 0;
	float *priority = new float[20];
	float *self_w = c_w(c_p,local_core,t_s,t_c,t_r);
	priority[19] = self_w[19];
    //priority[18] = self_w[18];
    /*for(int i=0;i<20;i++)
        priority[i] = self_w[i];*/
	for(int i = 18;i>=0;i--)
	{
		for(int j=0;j<20;j++)
		{
			if(graph[i][j] == 1)
			{
				w[i][j] = self_w[i] + priority[j];
			}
		}
		float max = 0;
		for(int j=0;j<20;j++)
		{
			if(w[i][j] > max)
				max = w[i][j];
		}
		priority[i] = max;
	}
	/*cout<<endl<<"priority of each task:"<<endl;
	for(int i=0;i<10;i++)
		cout<<priority[i]<<" ";
	cout<<endl;*/
	return priority;
}

float **merge(float *priority, int (*local_core)[3], int *c_p)
{
	float **merge = NULL;
	merge = (float **)malloc(sizeof(float *) * 20);
	for(int r=0;r<20;r++)
		merge[r] = (float *)malloc(sizeof(float) * 5);
	cout<<endl<<"priority of each task:"<<endl;
	for(int i=0;i<20;i++)
		cout<<priority[i]<<" ";
	cout<<endl;
	for(int i=0;i<20;i++)
	{
		for(int j=0;j<6;j++)
		{
			if(j==0)
				merge[i][j] = priority[i];
			else if(j<4)
				merge[i][j] = local_core[i][j-1];
			else if(j==4)
				merge[i][j] = 5;
			else
				merge[i][j] = c_p[i];

		}
	}
	cout<<endl;
	for(int i=0;i<20;i++)
	{
		for(int j=0;j<6;j++)
			cout<<merge[i][j]<<" ";
		cout<<endl;
	}
	return merge;
}

void transform(vector<task>&S, float **merge, Matrix<int, 2>&G)
{
	for(int i=0; i<20;i++)
	{
		S[i].num = i+1;
		S[i].pri = merge[i][0];
		if(merge[i][5] == 1)
			S[i].ct = 0;
		if(merge[i][5] == 2)
			S[i].ct = 1;
	}
	int k = 0;
	int m = S.size() - 1;
	for (int i = 0; i < 20; i++)
	{
			for (int j = 0; j < G.dim2(); j++)
			if (G(m - i, j) == 1)
				k = k + 1;
		if (k == 0)
			S[m - i].exit = 1;
		k = 0;
		for (int j = 0; j < G.dim2(); j++)
			if (G(j, m - i) == 1)
				k = k + 1;
		if(k==0)
			S[m - i].entry = 1;
	}

}

void insertsort(vector<task>&S,Matrix<int, 2>&l_c)//task2 and task3
{
	int i,j,k;
	for(i = 1;i < 20;i++)
	{
		float temp = S[i].pri;
		int s = S[i].ct;
		int t = S[i].num;
		int q = S[i].entry;
		int p = S[i].exit;
		for(j = 0;j < i;j++)
		{
			if(S[i].pri > S[j].pri)
			{
				for(k = i;k > j;k--){
					S[k].pri = S[k-1].pri;
					S[k].ct = S[k-1].ct;
					S[k].num = S[k-1].num;
					S[k].entry = S[k-1].entry;
					S[k].exit = S[k-1].exit;
				}
				S[j].pri = temp;
				S[j].num = t;
				S[j].ct = s;
				S[j].entry = q;
				S[j].exit = p;
			}
		}
	}
}

int max_2(int &m, int &n)
{
	if(m<n)
		return n;
	else
		return m;
}

int d_rtl(task &t_i, vector<task>&S, Matrix<int, 2>&G) // if local schedule, return RTl
{
	int max=0;
	if (S.size()!=0) // s.size=10
	{
		for (int i = 0; i < G.dim2(); i++)  // i : 0-9
			if (G(i, t_i.num - 1) == 1)
				for (int j = 0; j < S.size(); j++)
					if ((S[j].num == i + 1)&&(max < max_2(S[j].FTl, S[j].FTWR)))
						max = max_2(S[j].FTl, S[j].FTWR);
	}
	return max;
}

// if cloud schedule, return RTWS
int d_rtws(task &t_i, vector<task>&S, Matrix<int, 2>&G)
{
	int max=0;
	if (S.size()!=0)
	{
		for (int i = 0; i < G.dim2(); i++)
			if (G(i, t_i.num - 1) == 1)
				for (int j = 0; j < S.size(); j++)
					if ((S[j].num == i + 1)&&(max < max_2(S[j].FTl, S[j].FTWS)))
						max = max_2(S[j].FTl, S[j].FTWS);
	}
	return max;
}

int d_rtc(task &t_i, vector<task>&S, Matrix<int, 2>&G)// if cloud schedule, return RTC
{
	int max=t_i.FTWS;
	if (S.size()!=0)
	{
		for (int i = 0; i < G.dim2(); i++)
			if (G(i, t_i.num - 1) == 1)
				for (int j = 0; j < S.size(); j++)
					if ((S[j].num == i + 1)&&(max < max_2(t_i.FTWS, S[j].FTC)))
						max = max_2(t_i.FTWS, S[j].FTC);
	}
	return max;
}

int d_rtwr(task &t_i)// if cloud schedule, return RTWR
{
	return t_i.FTC;
}

// if local schedule, return the smallest finish time
int locals(task &t_i, vector<task>&S, Matrix<int, 2>&G, Matrix<int, 2>&l_c,int r)
{
	t_i.RTl = d_rtl(t_i, S, G);
	int mint=INT_MAX;
	int ft;
	int max = 0; // find a local core's biggest finish time
	if (r == 0)
	{
		for (int i = 0; i < l_c.dim2(); i++)
		{
			ft = l_c(t_i.num - 1, i);
			if (mint > ft)
			{
				mint = ft;
				t_i.chan = i;
			}
		}
		return mint;
	}
	for (int i = 0; i < l_c.dim2(); i++)
	{
		ft = t_i.RTl + l_c(t_i.num - 1, i);
		max = 0;
		for (int j = 0; j < r; j++)
			if ((S[j].chan == i) && (max < S[j].FTl))
				max = S[j].FTl;
		if(max>t_i.RTl)
			ft=max+ l_c(t_i.num - 1, i);
		if (mint > ft)
		{
			mint = ft;
			t_i.chan = i;
		}
	}
	return mint;
}

// if cloud schedule, return the finish time
int clouds(task &t_i, vector<task>&S, Matrix<int, 2>&G, int t_s, int t_c, int t_r,int r)
{
	t_i.RTWS = d_rtws(t_i, S, G);
	int maxs = 0;
	int t;
	int maxc = 0;
	int maxr = 0;
	int ft;
	t = t_s + t_c + t_r;
	if (r == 0)
	{
		t_i.FTWS = t_s;
		t_i.RTC = t_s;
		t_i.FTC = t_s + t_c;
		t_i.RTWR = t_s + t_c;
		return t;
	}
	for(int i=0; i<r ;i++)
		if (S[i].chan == 3)
			if (maxs < S[i].FTWS)
				maxs = S[i].FTWS;
	if (maxs > t_i.RTWS)
		t_i.FTWS = maxs + t_s;
	else
		t_i.FTWS = t_i.RTWS + t_s;
	t_i.RTC = d_rtc(t_i, S, G);
	for (int i = 0; i < r ; i++)
		if (S[i].chan == 3)
			if (maxc < S[i].FTC)
				maxc = S[i].FTC;
	if (maxc > t_i.RTC)
		t_i.FTC = maxc + t_c;
	else
		t_i.FTC = t_i.RTC + t_c;
	t_i.RTWR = d_rtwr(t_i);
	for (int i = 0; i < r ; i++)
		if (S[i].chan == 3)
			if (maxr < S[i].FTWR)
				maxr = S[i].FTWR;
	if (maxr > t_i.RTWR)
		ft = maxr + t_r;
	else
		ft = t_i.RTWR + t_r;
	return ft;
}

void initial(vector<task>&S, float **merge, Matrix<int, 2>&l_c, Matrix<int, 2>&G, int t_s, int t_c, int t_r)
{

	int t;
	int mint_l;
	int t_inc;
	t = t_s + t_c + t_r;
	for(int i=0;i<20;i++)
	{
		if (!S[i].ct) //0 in core
		{
			mint_l = locals(S[i], S, G, l_c,i);
			t_inc = clouds(S[i], S, G, t_s, t_c, t_r,i);
			if (t_inc < mint_l)
			{
				S[i].RTl = 0;
				S[i].FTl = 0;
				S[i].chan = 3;
				S[i].FTWR = t_inc;
				S[i].ST = t_inc - t;
			}
			else
			{
				S[i].FTC = 0;
				S[i].FTWS = 0;
				S[i].RTWS = 0;
				S[i].RTC = 0;
				S[i].RTWR = 0;
				S[i].FTWR = 0;
				S[i].FTl = mint_l;
				S[i].ST = mint_l - l_c(S[i].num - 1, S[i].chan);
			}
		}
		else
		{
			S[i].FTl = 0;
			S[i].RTl = 0;
			S[i].chan = 3;
			S[i].FTWR= clouds(S[i], S, G, t_s, t_c, t_r,i);
			S[i].ST = S[i].FTWR - t;
		}
	}
}

// return a task's finish time
int find_ft(task&t_i)
{
	int max;
	max = max_2(t_i.FTl, t_i.FTWR);
	return max;
 }

void sequence(vector<task>&S,vector<task>&S_0,vector<task>&S_1,vector<task>&S_2,vector<task>&S_3)
{
	for(int i = 0; i<S.size();i++)
	{
        if(S[i].chan == 0)
        {
            int q = 0;
            for(q=0;q<S_0.size();q++)
            {
                if(S_0[q].ST > S[i].ST)
                {
                    S_0.insert(S_0.begin()+q,S[i]);
                    break;
                }

            }
            if(q == S_0.size())
                S_0.push_back(S[i]);
        }
        else if(S[i].chan == 1)
        {
            int q=0;
            for(q=0;q<S_1.size();q++)
            {
                if(S_1[q].ST > S[i].ST)
                {
                    S_1.insert(S_1.begin()+q,S[i]);
                    break;
                }
            }
            if(q == S_1.size())
                S_1.push_back(S[i]);
        }
        else if(S[i].chan == 2)
        {
            int q = 0;
            for(q=0;q<S_2.size();q++)
            {
                if(S_2[q].ST > S[i].ST)
                {
                    S_2.insert(S_2.begin()+q,S[i]);
                    break;
                }

            }
            if(q == S_2.size())
                S_2.push_back(S[i]);
        }
        else if(S[i].chan == 3) {
            int q = 0;
            for (q = 0; q < S_3.size(); q++) {
                if (S_3[q].ST > S[i].ST) {
                    S_3.insert(S_3.begin() + q, S[i]);
                    break;
                }

            }
            if (q == S_3.size())
                S_3.push_back(S[i]);
        }
        if(S[i].chan == 3)
        {
            S[i].FTl = 0;
            S[i].RTl = 0;
        }
    }

}
// print the sequence S
void prints(vector<task>&S)
{
	int k,m;
	for (int i = 0; i < S.size(); i++)
	{
		k = 1 + S[i].chan;
		m = find_ft(S[i]);
		cout << "Task" << S[i].num << ": ";
		switch (S[i].chan)
		{
		case 0:
			cout << "local core " << k << ", ";
			break;
		case 1:
			cout << "local core " << k << ", ";
			break;
		case 2:
			cout << "local core " << k << ", ";
			break;
		case 3:
			cout << "cloud" << ", ";
			break;
		default:
			break;
		}
		cout << "start time is: " << S[i].ST << "  finish time is: "<<m << "  priority: " << S[i].pri<<endl;;
	}
}

// return the completion time of sequence S
int t_com(vector<task>&S)
{
	int max=0;
	for (int i = 0; i <S.size() ; i++)
		//if ((S[i].exit) && (max < find_ft(S[i])))
        if (max < find_ft(S[i]))
			max = find_ft(S[i]);
	return max;
}

float e_com(vector<task>&S, int p_l_1, int p_l_2, int p_l_3, float p_c)
{
	float e = 0;
	for(int i=0;i<S.size();i++)
	{
		switch(S[i].chan)
		{
			case 0:
				e = e + p_l_1 * (find_ft(S[i]) - S[i].ST);
				break;
			case 1:
				e = e + p_l_2 * (find_ft(S[i]) - S[i].ST);
				break;
			case 2:
				e = e + p_l_3 * (find_ft(S[i]) - S[i].ST);
				break;
			case 3:
				e = e + p_c * (S[i].FTWS - S[i].ST);
				break;
			default:
				break;
		}
	}
	return e;
 }

void ready1(vector<task>&S, Matrix<int,2>&g)
{
	for(int i=0;i<S.size();i++)
	{
		int m=0;
		for(int j=0;j<S.size();j++)
		{
			if(g(S[j].num-1,S[i].num -1) == 1)
				m += 1;//maybe some questions
		}
		S[i].r1 = m;
	}
}

void ready2(vector<task>&S_new,vector<task>S_0,vector<task>S_1,vector<task>S_2,vector<task>S_3)
{
	for(int i=0;i<S_new.size();i++)
	{
		int m=0;
		if(S_new[i].chan == 0)
		{
			int j = 0;
			while(S_0[j].num != S_new[i].num)
			{
				m += 1;
				j++;
			}
		}
		else if(S_new[i].chan == 1)
		{
			int j = 0;
			while(S_1[j].num != S_new[i].num)
			{
				m += 1;
				j++;
			}
		}
		else if(S_new[i].chan == 2)
		{
			int j = 0;
			while(S_2[j].num != S_new[i].num)
			{
				m += 1;
				j++;
			}
		}
		else if(S_new[i].chan == 3)
		{
			int j = 0;
			while(S_3[j].num != S_new[i].num)
			{
				m += 1;
				j++;
			}
		}
		S_new[i].r2 = m;
	}
}

int re_local(vector<task>&S_new, task&vi, Matrix<int,2>&g, Matrix<int,2>&l_c)
{
	vi.RTl = d_rtl(vi, S_new, g);
	int FT;
	int max = 0;
	if(S_new.size() == 0)
		FT = vi.RTl + l_c(vi.num - 1, vi.chan);
	else
	{
		for(int i=0;i < S_new.size();i++)
		{
			if((S_new[i].chan == vi.chan)&& (max < S_new[i].FTl))
				max = S_new[i].FTl;
			if(max > vi.RTl)
			{
				FT = max + l_c(vi.num - 1,vi.chan);
			}
			else
			{
				FT = vi.RTl + l_c(vi.num - 1,vi.chan);
			}

		}
	}
	return FT;
}

int re_clouds(task &t_i, vector<task>&S, Matrix<int, 2>&G, int t_s, int t_c, int t_r)
{
	t_i.RTWS = d_rtws(t_i, S, G);
	int maxs = 0;
	int t;
	int maxc = 0;
	int maxr = 0;
	int ft;
	t = t_s + t_c + t_r;
	if (S.size() == 0)
	{
		t_i.FTWS = t_s;
		t_i.RTC = t_s;
		t_i.FTC = t_s + t_c;
		t_i.RTWR = t_s + t_c;
		return t;
	}
	for(int i=0; i<S.size() ;i++)
		if (S[i].chan == 3)
			if (maxs < S[i].FTWS)
				maxs = S[i].FTWS;
	if (maxs > t_i.RTWS)
		t_i.FTWS = maxs + t_s;
	else
		t_i.FTWS = t_i.RTWS + t_s;
	t_i.RTC = d_rtc(t_i, S, G);
	for (int i = 0; i < S.size() ; i++)
		if (S[i].chan == 3)
			if (maxc < S[i].FTC)
				maxc = S[i].FTC;
	if (maxc > t_i.RTC)
		t_i.FTC = maxc + t_c;
	else
		t_i.FTC = t_i.RTC + t_c;
	t_i.RTWR = d_rtwr(t_i);
	for (int i = 0; i < S.size() ; i++)
		if (S[i].chan == 3)
			if (maxr < S[i].FTWR)
				maxr = S[i].FTWR;
	if (maxr > t_i.RTWR)
		ft = maxr + t_r;
	else
		ft = t_i.RTWR + t_r;
	return ft;
}
// get new sequence
void new_sequence(task&v_tar, int k_tar, Matrix<int,2>&g,vector<task>&S_new,vector<task>&S_0,vector<task>&S_1,vector<task>&S_2,vector<task>&S_3)
{
	for(int i=0;i<S_new.size();i++)
	{
		if(v_tar.num == S_new[i].num && S_new[i].chan != 3)
		{
			if(S_new[i].chan == 0)
			{
				for(int iter=0;iter<S_0.size();iter++)
				{
					if(S_0[iter].num == S_new[i].num)
						S_0.erase(S_0.begin()+iter);
				}
			}
			else if(S_new[i].chan == 1)
			{
				for(int iter=0;iter<S_1.size();iter++)
				{
					if(S_1[iter].num == S_new[i].num)
						S_1.erase(S_1.begin()+iter);
				}
			}
			else if(S_new[i].chan == 2)
			{
				for(int iter=0;iter<S_2.size();iter++)
				{
					if(S_2[iter].num == S_new[i].num)
						S_2.erase(S_2.begin() + iter);
				}
			}
			else if(S_new[i].chan == 3)
			{
				for(int iter=0;iter<S_3.size();iter++)
				{
					if(S_3[iter].num == S_new[i].num)
						S_3.erase(S_3.begin()+iter);
				}
			}
			S_new[i].chan = k_tar; //change channel
			if(k_tar == 0)
			{
				int q = 0;
				for(q=0;q<S_0.size();q++)
				{
					if(S_0[q].ST > S_new[i].RTl)
					{
						S_0.insert(S_0.begin()+q,S_new[i]);
						break;
					}

				}
				if(q == S_0.size())
					S_0.push_back(S_new[i]);
			}
			else if(k_tar == 1)
			{
				int q=0;
				for(q=0;q<S_1.size();q++)
				{
					if(S_1[q].ST > S_new[i].RTl)
					{
						S_1.insert(S_1.begin()+q,S_new[i]);
						break;
					}
				}
				if(q == S_1.size())
					S_1.push_back(S_new[i]);
			}
			else if(k_tar == 2)
			{
				int q = 0;
				for(q=0;q<S_2.size();q++)
				{
					if(S_2[q].ST > S_new[i].RTl)
					{
						S_2.insert(S_2.begin()+q,S_new[i]);
						break;
					}

				}
				if(q == S_2.size())
					S_2.push_back(S_new[i]);
			}
			else if(k_tar == 3)
			{
				int q = 0;
				for(q=0;q<S_3.size();q++)
				{
					if(S_3[q].ST > S_new[i].RTl)
					{
						S_3.insert(S_3.begin()+q,S_new[i]);
						break;
					}

				}
				if(q == S_3.size())
					S_3.push_back(S_new[i]);
			}
			if(k_tar == 3)
			{
				S_new[i].FTl = 0;
				S_new[i].RTl = 0;
			}
		}
	}
}

void kernel_algorithm(task&v_tar, int k_tar,vector<task>&S_reschedule,vector<task>&S_new,vector<task>&S_0,vector<task>&S_1,vector<task>&S_2,vector<task>&S_3,Matrix<int,2>&g, Matrix<int,2>&l_c, int t_s, int t_c, int t_r)
{
	vector<task>S_d;
	vector<task>LIFO;
	int t = t_c + t_s + t_r;
	new_sequence(v_tar, k_tar,g,S_new,S_0,S_1,S_2,S_3);
	ready1(S_new,g);
	ready2(S_new,S_0,S_1,S_2,S_3);
    S_d = S_new;
    /*for(int i=0;i<S_d.size();i++)
        cout<<S_d[i].num<<" ";
    cout<<endl;
    cout<<"core 1: ";
    for(int i =0; i<S_0.size();i++)
        cout<<S_0[i].num<<" ";
    cout<<endl;
    cout<<"core 2: ";
    for(int i =0; i<S_1.size();i++)
        cout<<S_1[i].num<<" ";
    cout<<endl;
    cout<<"core 3: ";
    for(int i =0; i<S_2.size();i++)
        cout<<S_2[i].num<<" ";
    cout<<endl;
    cout<<"cloud: ";
    for(int i =0; i<S_3.size();i++)
        cout<<S_3[i].num<<" ";
    cout<<endl;
    for(int q=0;q<LIFO.size();q++)
        cout<<LIFO[q].num<<" ";
    cout<<endl;
    for(int q=0;q<S_d.size();q++)
        cout<<S_d[q].num<<":"<<S_d[q].r1<<" ";
    cout<<endl;
    for(int q=0;q<S_d.size();q++)
        cout<<S_d[q].num<<":"<<S_d[q].r2<<" ";
    cout<<endl;*/
	for(int m=0; m<S_new.size();m++)
	{
		if(S_new[m].r1 == 0 && S_new[m].r2 == 0)
		{
			LIFO.push_back(S_new[m]);
            //cout<<1<<endl;
            /*for(int z=0;z<S_d.size();z++)
            {
                if (S_new[m].num == S_d[z].num)
                    S_d.erase(S_d.begin() + z);
            }*/
			/*if(S_new[m].chan == 0)
			{
				for(int j=0;j<S_0.size();j++)
				{
					if(S_0[j].num == S_new[m].num)
					{
						S_0.erase(S_0.begin()+j);
						break;
					}
				}
			}
			else if(S_new[m].chan == 1)
			{
				for(int j=0;j<S_1.size();j++)
				{
					if(S_1[j].num == S_new[m].num)
					{
						S_1.erase(S_1.begin()+j);
						break;
					}

				}
			}
			else if(S_new[m].chan == 2)
			{
				for(int j=0;j<S_2.size();j++)
				{
					if(S_2[j].num == S_new[m].num)
					{
						S_2.erase(S_2.begin()+j);
						break;
					}

				}
			}
			else if(S_new[m].chan == 3)
            {
                for (int j = 0; j < S_3.size(); j++) {
                    if (S_3[j].num == S_new[m].num) {
                        S_3.erase(S_3.begin() + j);
                        break;
                    }
                }
            }*/
		}
	}
    /*for(int i=0;i<S_d.size();i++)
        cout<<S_d[i].num<<" ";
    cout<<endl;
    cout<<"core 1: ";
    for(int i =0; i<S_0.size();i++)
        cout<<S_0[i].num<<" ";
    cout<<endl;
    cout<<"core 2: ";
    for(int i =0; i<S_1.size();i++)
        cout<<S_1[i].num<<" ";
    cout<<endl;
    cout<<"core 3: ";
    for(int i =0; i<S_2.size();i++)
        cout<<S_2[i].num<<" ";
    cout<<endl;
    cout<<"cloud: ";
    for(int i =0; i<S_3.size();i++)
        cout<<S_3[i].num<<" ";
    cout<<endl;
    for(int q=0;q<LIFO.size();q++)
        cout<<LIFO[q].num<<" ";
    cout<<endl;
    for(int q=0;q<S_d.size();q++)
        cout<<S_d[q].num<<":"<<S_d[q].r1<<" ";
    cout<<endl;
    for(int q=0;q<S_d.size();q++)
        cout<<S_d[q].num<<":"<<S_d[q].r2<<" ";
    cout<<endl;*/
	//cout<<1<<endl;
    //cout<<2<<endl;
	while(LIFO.size() != 0)
	{
		//<<2<<endl;
        //cout<<3<<endl;
		if(LIFO.back().chan ==3)
		{
			for(int i=0;i<S_new.size();i++)
			{
				if(LIFO.back().num == S_new[i].num)
				{
					//LIFO.back().FTWR = clouds(LIFO.back(),S_new,g,t_s,t_c,t_r,i);
					LIFO.back().FTWR = re_clouds(LIFO.back(),S_reschedule,g,t_s,t_c,t_r);
					LIFO.back().ST = LIFO.back().FTWR - t;
					break;
				}
			}
		}
		else
		{
			for(int i=0;i<S_new.size();i++)
			{
				if(LIFO.back().num == S_new[i].num)
				{
					//LIFO.back().FTWR = re_local(S_new,LIFO.back(),g,l_c,i);
					LIFO.back().FTl = re_local(S_reschedule,LIFO.back(),g,l_c);
					LIFO.back().ST = LIFO.back().FTl - l_c(LIFO.back().num - 1,LIFO.back().chan);
					break;
				}
			}
		}
		S_reschedule.push_back(LIFO.back());//right?
		for(int i=0;i<S_d.size();i++)
		{
			if(LIFO.back().num == S_d[i].num)
			{
				S_d.erase(S_d.begin()+i);
				break;
			}
		}
		if(LIFO.back().chan == 0)
		{
			for(int j=0;j<S_0.size();j++)
			{
				if(S_0[j].num == LIFO.back().num)
				{
					S_0.erase(S_0.begin()+j);
					break;
				}
			}
		}
		else if(LIFO.back().chan == 1)
		{
			for(int j=0;j<S_1.size();j++)
			{
				if(S_1[j].num == LIFO.back().num)
				{
					S_1.erase(S_1.begin()+j);
					break;
				}

			}
		}
		else if(LIFO.back().chan == 2)
		{
			for(int j=0;j<S_2.size();j++)
			{
				if(S_2[j].num == LIFO.back().num)
				{
					S_2.erase(S_2.begin()+j);
					break;
				}

			}
		}
		else if(LIFO.back().chan == 3)
		{
			for(int j=0;j<S_3.size();j++)
			{
				if(S_3[j].num == LIFO.back().num)
				{
					S_3.erase(S_3.begin()+j);
					break;
				}
			}
		}
        //cout<<4<<endl;
		LIFO.pop_back();
        /*for(int i=0;i<S_d.size();i++)
            cout<< S_d[i].num<<" ";
        cout<<endl;*/
		ready1(S_d,g);
        /*cout<<5<<endl;
        for(int i =0; i<S_0.size();i++)
            cout<<S_0[i].num<<" ";
        cout<<endl;
        cout<<"core 2: ";
        for(int i =0; i<S_1.size();i++)
            cout<<S_1[i].num<<" ";
        cout<<endl;
        cout<<"core 3: ";
        for(int i =0; i<S_2.size();i++)
            cout<<S_2[i].num<<" ";
        cout<<endl;
        cout<<"cloud: ";
        for(int i =0; i<S_3.size();i++)
            cout<<S_3[i].num<<" ";
        cout<<endl;*/
		ready2(S_d,S_0,S_1,S_2,S_3);
        /*cout<<6<<endl;
		cout<<"LIFO.size:"<<LIFO.size()<<endl;
        cout<<"core 1: ";
        for(int i =0; i<S_0.size();i++)
            cout<<S_0[i].num<<" ";
        cout<<endl;
        cout<<"core 2: ";
        for(int i =0; i<S_1.size();i++)
            cout<<S_1[i].num<<" ";
        cout<<endl;
        cout<<"core 3: ";
        for(int i =0; i<S_2.size();i++)
            cout<<S_2[i].num<<" ";
        cout<<endl;
        cout<<"cloud: ";
        for(int i =0; i<S_3.size();i++)
            cout<<S_3[i].num<<" ";
        cout<<endl;
		for(int q=0;q<LIFO.size();q++)
			cout<<LIFO[q].num<<" ";1
		cout<<endl;
		for(int q=0;q<S_d.size();q++)
			cout<<S_d[q].num<<":"<<S_d[q].r1<<" ";
		cout<<endl;
		for(int q=0;q<S_d.size();q++)
			cout<<S_d[q].num<<":"<<S_d[q].r2<<" ";
		cout<<endl;*/
		//ready1(S_d,g);
		//ready2(S_d,S_0,S_1,S_2,S_3);
		for(int i=0;i<S_d.size();i++)
		{
			if(S_d[i].r1 == 0 && S_d[i].r2 == 0)
			{
				if(LIFO.size() == 0)
					LIFO.push_back(S_d[i]);
				else
				{
					int j=0;
					for(j=0;j<LIFO.size();j++)
					{
						if(LIFO[j].num == S_d[i].num)
							break;
					}
					if(j==LIFO.size())
						LIFO.push_back(S_d[i]);//how to reduce
				}
			}
		}
	}
}

void outer_loop(float t_max, vector<task>&S, vector<task>&S_0,vector<task>&S_1,vector<task>&S_2,vector<task>&S_3, Matrix<int,2>&g,Matrix<int,2>&l_c,int t_s, int t_c, int t_r,int p_l_1,int p_l_2, int p_l_3, int p_c)
{
	vector<task>S_reschedule;
	vector<task>S_new;
	S_new = S;
	int t_com_ori,t_com_2,t_com_best;
	t_com_ori = t_com(S);
	int status;
	float en_ori = e_com(S,p_l_1,p_l_2,p_l_3,p_c);
	float en_min = en_ori;
	t_com_best = t_com_ori;
	float en;
	float ratio1,ratio2;
	ratio1 = 0;
	status = 0;
	for(int i=0;i<S.size();i++)
	{
		int c = S[i].chan;
		if( c != 3)
		{
			for(int j=0;j<l_c.dim2()+1;j++)
			{
				if( j != c)
				{
					S_new = S;
					sequence(S_new,S_0,S_1,S_2,S_3);
					S_reschedule.erase(S_reschedule.begin(),S_reschedule.end());
					new_sequence(S[i],j,g,S_new,S_0,S_1,S_2,S_3);
					kernel_algorithm(S[i],j,S_reschedule,S_new,S_0,S_1,S_2,S_3,g,l_c,t_s,t_c,t_r);
					t_com_2 = t_com(S_reschedule);
					en = e_com(S_reschedule,p_l_1,p_l_2,p_l_3,p_c);
					if(t_com_2<= t_com_ori && en < en_ori)
					{
						status = 1;
						t_com_best = t_com_2;
						en_min = en;
						S = S_reschedule;
					}
					if(status == 1)
					{
						if((t_com_2 <= t_com_best) && (en <= en_min ))
						{
							t_com_best = t_com_2;
							en_min = en;
							S = S_reschedule;
						}
					}
					if(status == 0)
					{
						if(en < en_min && t_com_2 <= t_max)
                        {
                            ratio2 = (en_min - en) / (t_com_2 - t_com_best);
                            if (ratio2 >= ratio1) {
                                ratio1 = ratio2;
                                t_com_best = t_com_2;
                                en_min = en;
                                S = S_reschedule;
                            }
                        }
					}
				}
			}
		}
	}
}

void outer_iteration(float t_max, vector<task>&S, vector<task>&S_0,vector<task>&S_1,vector<task>&S_2,vector<task>&S_3, Matrix<int,2>&g,Matrix<int,2>&l_c,int t_s, int t_c, int t_r,int p_l_1,int p_l_2, int p_l_3, int p_c)
{
	float en;
	float en1=0;
	cout<<"S:"<<endl;
	prints(S);
	en = e_com(S,p_l_1,p_l_2,p_l_3,p_c);
	//int i=0;
	while (en1<en)
	{
		en = e_com(S,p_l_1,p_l_2,p_l_3,p_c);
		outer_loop(t_max,S,S_0,S_1,S_2,S_3,g,l_c, t_s, t_c, t_r, p_l_1, p_l_2, p_l_3, p_c);
		en1= e_com(S,p_l_1,p_l_2,p_l_3,p_c);
	}
}

int main()
{
		int N=20;
		int graph[20][20]={{0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	               		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
		cout<<"graph path:"<<endl;
		int local_core[20][3]={{9,7,5},
		                       {8,6,5},
							   {6,5,4},
							   {7,5,3},
							   {5,4,2},
							   {7,6,4},
							   {8,5,3},
							   {6,4,2},
							   {5,3,2},
							   {7,4,2},
							   {8,5,3},
							   {9,8,6},
							   {7,6,4},
							   {9,7,2},
							   {8,5,4},
							   {6,4,3},
							   {8,7,3},
							   {9,4,2},
							   {7,5,3},
							   {8,6,4}};
		int t_s = 3;
		int t_c = 1;
		int t_r = 1;
		int p_l_1 = 1;
		int p_l_2 = 2;
		int p_l_3 =4;
		float p_c = 0.5;
		for(int i=0;i<N;i++)
		{
			for(int j=0;j<N;j++)
			{
				cout<<graph[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<"local_core:"<<endl;
		for(int i=0;i<N;i++)
		{
			for(int j=0;j<3;j++)
			{
				cout<<local_core[i][j]<<" ";
			}
			cout<<endl;
		}
		int *c_p = primary_assignment(local_core,t_s,t_c,t_r);
		c_w(c_p,local_core,t_s,t_c,t_r);
		float *priority = task_priority(graph, c_p, local_core, t_s, t_c, t_r);

		float **merge_m = merge(priority,local_core, c_p);
		vector<task>S(N);
		vector<task>S_0;
		vector<task>S_1;
		vector<task>S_2;
		vector<task>S_3;
		Matrix<int, 2>g(N,N);
		Matrix<int, 2>l_c(N,3);
		g(0, 2) = 1;
		g(0, 3) = 1;
		g(1, 2) = 1;
		g(1, 3) = 1;
		g(1, 4) = 1;
		g(2, 5) = 1;
		g(2, 6) = 1;
		g(3, 6) = 1;
		g(3, 7) = 1;
		g(4, 7) = 1;
		g(4, 8) = 1;
		g(5, 9) = 1;
		g(6, 9) = 1;
		g(7, 9) = 1;
		g(8, 9) = 1;
		g(9, 10) = 1;
		g(9, 11) = 1;
		g(9,12) = 1;
		g(10,13) = 1;
		g(10,14) = 1;
		g(11,14) = 1;
		g(11,15) = 1;
		g(11,16) = 1;
		g(12,15) = 1;
		g(12,16) = 1;
		g(13,18) = 1;
		g(14,17) = 1;
		g(15,17) = 1;
		g(15,18) = 1;
		g(16,18) = 1;
		g(17,19) = 1;
		g(18,19) = 1;
		l_c(0, 0) = 9;
		l_c(0, 1) = 7;
		l_c(0, 2) = 5;
		l_c(1, 0) = 8;
		l_c(1, 1) = 6;
		l_c(1, 2) = 5;
		l_c(2, 0) = 6;
		l_c(2, 1) = 5;
		l_c(2, 2) = 4;
		l_c(3, 0) = 7;
		l_c(3, 1) = 5;
		l_c(3, 2) = 3;
		l_c(4, 0) = 5;
		l_c(4, 1) = 4;
		l_c(4, 2) = 2;
		l_c(5, 0) = 7;
		l_c(5, 1) = 6;
		l_c(5, 2) = 4;
		l_c(6, 0) = 8;
		l_c(6, 1) = 5;
		l_c(6, 2) = 3;
		l_c(7, 0) = 6;
		l_c(7, 1) = 4;
		l_c(7, 2) = 2;
		l_c(8, 0) = 5;
		l_c(8, 1) = 3;
		l_c(8, 2) = 2;
		l_c(9, 0) = 7;
		l_c(9, 1) = 4;
		l_c(9, 2) = 2;
		l_c(10,0) = 8;
		l_c(10,1) = 5;
		l_c(10,2) = 3;
		l_c(11,0) = 9;
		l_c(11,1) = 8;
		l_c(11,2) = 6;
		l_c(12,0) = 7;
		l_c(12,1) = 6;
		l_c(12,2) = 4;
		l_c(13,0) = 9;
		l_c(13,1) = 7;
		l_c(13,2) = 2;
		l_c(14,0) = 8;
		l_c(14,1) = 5;
		l_c(14,2) = 4;
		l_c(15,0) = 6;
		l_c(15,1) = 4;
		l_c(15,2) = 3;
		l_c(16,0) = 8;
		l_c(16,1) = 7;
		l_c(16,2) = 3;
		l_c(17,0) = 9;
		l_c(17,1) = 4;
		l_c(17,2) = 2;
		l_c(18,0) = 7;
		l_c(18,1) = 5;
		l_c(18,2) = 3;
		l_c(19,0) = 8;
		l_c(19,1) = 6;
		l_c(19,2) = 4;
		transform(S,merge_m,g);
		cout<<endl;
		insertsort(S,l_c);
		cout<<endl;
		initial(S, merge_m,l_c, g, t_s, t_c, t_r);
		cout<<S[0].entry<<" "<<S[1].entry<<endl;
		cout << "Initial Scheduling:" << endl;
		prints(S);
		cout << "the completion time is: " << t_com(S) << endl;
		cout << "the energy cost is: " << e_com(S,p_l_1,p_l_2,p_l_3,p_c) << endl;
		float t_max = 1.5*t_com(S);
		cout<<"Task Migration Algorithm:"<<endl;
		outer_iteration(t_max,S,S_0,S_1,S_2,S_3,g,l_c, t_s, t_c, t_r, p_l_1, p_l_2, p_l_3, p_c);
		cout<<"S:"<<endl;
		prints(S);
		cout << "the completion time is: " << t_com(S) << endl;
		cout << "the energy cost is: " << e_com(S,p_l_1,p_l_2,p_l_3,p_c)<< endl;
        sequence(S,S_0,S_1,S_2,S_3);
        cout<<"core 1: ";
        for(int i =0; i<S_0.size();i++)
            cout<<S_0[i].num<<" ";
        cout<<endl;
        cout<<"core 2: ";
        for(int i =0; i<S_1.size();i++)
            cout<<S_1[i].num<<" ";
        cout<<endl;
        cout<<"core 3: ";
        for(int i =0; i<S_2.size();i++)
            cout<<S_2[i].num<<" ";
        cout<<endl;
        cout<<"cloud: ";
        for(int i =0; i<S_3.size();i++)
            cout<<S_3[i].num<<" ";
        cout<<endl;


		//cout << "the energy cost is: " << e_com(S,p_l_1,p_l_2,p_l_3,p_c) << endl;
		/*vector<task>S_new;
		S_new = S;
		sequence(S,S_0,S_1,S_2,S_3);
		new_sequence(S[1],3,g,S_new,S_0,S_1,S_2,S_3);

		vector<task>S_reschedule;
		kernel_algorithm(S[1], 3, S_reschedule, S_new,S_0,S_1,S_2,S_3,g,l_c,t_s,t_c,t_r);

		prints(S_reschedule);
		cout << "the energy cost is: " << e_com(S_reschedule,p_l_1,p_l_2,p_l_3,p_c) << endl;*/
		system("pause");
		return 0;

}

