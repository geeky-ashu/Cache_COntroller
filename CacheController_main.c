#include<math.h>
#include<conio.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
#include<stdlib.h>
#include<time.h>

#pragma warning( disable : 4996 )
#pragma align(16)
#define max_lines  262144 
#define max_ways 16
// WS = 1 : WriteBack
// WS = 2 : WriteThroughAllocate
// WS = 3 : WriteThroughNonAllocate
int N, BL, WS = 0;
int valid[max_lines][max_ways];
int dirty[max_lines][max_ways];
int lru[max_lines][max_ways];
uint32_t tag[max_lines][max_ways];

int No_passes = 0;
int read_memory_count = 0;
int read_cache_count = 0;
int total_bytes_written = 0;
int total_bytes_read = 0;
int read_hit_count = 0;
int read_writeBack_count = 0;
int read_miss_count = 0;
int write_miss_count = 0;
int write_hit_count = 0;
int read_replace_count = 0;
int read_Cache2CPU_count = 0;
int read_Mem2Cache_count = 0;
int write_back_count = 0;
int write_memory_count = 0;
int write_cache_count;
int write_writeBack_count = 0;
int write_CPU2Cache_count = 0;
int write_CPU2Mem_count = 0;
int write_Cache2Mem_count = 0;
int write_replace_count = 0;
int write_dirty_count = 0;
int total_access_time = 0;
int read_time = 0;
int write_time = 0;
int flush_time = 0;


int flush_count = 0;
int width = 24;
int B, L;
uint32_t addr_global;

bool hit;

void write_back(uint32_t, int);
void read_cache(uint32_t, int, int);
void read_Mem2Cache(uint32_t, int);
void read_Cache2CPU(uint32_t, int);
void write_Cache2Mem(uint32_t, int);
void write_CPU2Mem(uint32_t, int);
void write_CPU2Cache(uint32_t, int);
void write_cache(uint32_t, int, int);
void updateLRU(uint32_t, int);
void invalidate(uint32_t, int);
void validate(uint32_t, int);
void read_memory(void *, uint32_t);
void write_memory(void *, uint32_t);
void cholsl(double **, int, double[], double[], double[]);
void setTag(uint32_t, int, int);
int find_tag(uint32_t, int, int);
uint32_t get_line(uint32_t, int, int);

void file_init() {// reference from -->https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
	FILE *file1;
	file1 = fopen("data.csv", "w");
	if (file1 == NULL)
	{
		fprintf(stderr, "file not opened\n");
		exit(1);
	}
	fprintf(file1, "\t");
	fprintf(file1, "List of all the variables and counter values\n");
	fclose(file1);
}




void file_display() {
	FILE *file1;
	file1 = fopen("data.csv", "w");;// reference from -->https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
	if (file1 == NULL)
	{
		fprintf(stderr, "file not opened\n");
		exit(1);
	}
	fprintf(file1, "\n");
	fprintf(file1, "\t");
	fprintf(file1, "List of all the variables and counter values\n");
	fprintf(file1, "Write Strategy is:  %d\t\n", WS);
	fprintf(file1, "Number of ways:  %d\n", N);
	fprintf(file1, "Burst Length is:  %d\n", BL);
	fprintf(file1, "Read Memory Count is:  %d\t\n", read_memory_count);
	fprintf(file1, "Read Hit count is:  %d\t\n", read_hit_count);
	fprintf(file1, "Write Hit count is:  %d\t\n", write_hit_count);
	fprintf(file1, "Read miss count is:  %d\t\n", read_miss_count);
	fprintf(file1, "Write miss count is:  %d\t\n", write_miss_count);
	fprintf(file1, "Write replace count is:  %d\t\n", write_replace_count);
	fprintf(file1, "Read cache count is:  %d\t\n", read_cache_count);
	fprintf(file1, "Write memory count is:  %d\t\n", write_memory_count);
	fprintf(file1, "Write Write Back Count is:  %d\t\n", write_writeBack_count);
	fprintf(file1, "Read Write Back Count is:  %d\t\n", read_writeBack_count);
	fprintf(file1, "Read replace count is:  %d\t", read_replace_count);//because write through memory has 4 byte writes only
	fprintf(file1, "Write cache count is:  %d\t\n", write_cache_count);
	fprintf(file1, "Write through memory count is:  %d\t\n", write_CPU2Mem_count);
	fprintf(file1, "Write CPU to Cache count is:  %d\t\n", write_CPU2Cache_count);
	fprintf(file1, "Read Cache to CPU count is:  %d\t\n", read_Cache2CPU_count);
	fprintf(file1, "Read Memory to Cache count is:  %d\t\n", read_Mem2Cache_count);
	fprintf(file1, "Write Dirty count is:  %d\t\n", write_dirty_count);
	fprintf(file1, "Total number of bytes written is:  %d\t\n", total_bytes_written);
	fprintf(file1, "Total number of bytes read is:  %d\t\n", total_bytes_read);
	fprintf(file1, "Flush count is:  %d\t\n", flush_count);
	fprintf(file1, "Total access time is:  %d\t\n", total_access_time);

	fprintf(file1, "\n");
	fprintf(file1, "\n");

	fclose(file1);
}
void read_memory(void *pmem, uint32_t size)
{
	read_memory_count++;
	int last_line = -1;
	addr_global = (uint32_t)pmem;
	for (int i = 0; i < size; i++)
	{
		int line = get_line(addr_global, N, BL);
		if (line != last_line)
		{
			read_cache(addr_global, N, BL);
			last_line = line;
		}// might need ws too
		addr_global++;
	}
	total_bytes_read += size;          //Global Variable
}




void write_memory(void *pmem, uint32_t size)
{
	write_memory_count++;

	uint32_t last_line = -1;
	uint32_t addr_global = (uint32_t)pmem;
	for (int i = 0; i < size; i++)
	{
		int line = get_line(addr_global, N, BL);
		if (line != last_line)
		{
			write_cache(addr_global, N, BL);
			last_line = line;
		}
		addr_global++;
	}
	total_bytes_written += size;          //Global Variable
}

void write_cache(uint32_t address, int N, int BL)
{
	hit = 0;
	uint32_t tag;
	int line = get_line(address, N, BL);
	tag = get_tag(address, N, BL);      //[tag = upper 24 bits of address - B bits - L bits]
	int way = find_tag(address, line, N); //[Address shift to the right by B + L bits & WAT <0 (MISS) way >= 0 (HIT)]	
	if (way != -1)
		hit = 1;         // Its a hit

	if (!hit && WS != 3) //
	{
		write_miss_count++;
		write_cache_count++;
		way = find_oldest(line, N);
		if (isDirty(line, way)) // && WS=1
		{
			write_writeBack_count++;
			//write_back();   //size will be sent
			write_Cache2Mem(line, way);
			invalidate(line, way);
			dirty[line][way] = 0;
			write_replace_count++;
		}
		write_CPU2Cache(line, way);
		validate(line, way);
		setTag(address, line, way);
		if (WS == 1)
			dirty[line][way] = 1;
	}
	if (!hit && WS == 3)
		write_miss_count++;
	if (hit)
	{
		write_hit_count++;
		write_cache_count++;
		write_CPU2Cache(line, way);
		if (WS == 2)
		{
			write_replace_count++;
		}
		if (WS == 1)
		{
			dirty[line][way] = 1;
			write_dirty_count++;
		}
	}
	if (WS != 1)
		write_CPU2Mem(line, way);
}


void read_cache(uint32_t address, int N, int BL)
{
	read_cache_count++;
	uint32_t tag;
	int line = get_line(address, N, BL);
	tag = get_tag(address, N, BL);      //[tag = upper 24 bits of address - B bits - L bits]
	int way = find_tag(address, line, N); //[Address shift to the right by B + L bits & WAT <0 (MISS) way >= 0 (HIT)]
	if (way != -1)
		hit = 1;         // Its a hit

	if (hit)
	{
		read_hit_count++;
	}
	else
	{
		read_miss_count++;
		way = find_oldest(line, N); //Find oldest LRU
		if (isDirty(line, way) && WS == 1)
		{
			read_writeBack_count++;
			//write_back(line, way);
			invalidate(line, way);
			dirty[line][way] = 0;
			read_replace_count++;
		}
		setTag(address, line, way);
		read_Mem2Cache(line, way);
		validate(line, way);
	}

	read_Cache2CPU(line, way);
	updateLRU(line, way);
}

void write_back(int line_1, int current_way)
{
	write_back_count++;
}


void read_Mem2Cache(int line_1, int current_way)
{
	read_Mem2Cache_count++;
}

void read_Cache2CPU(int line_1, int current_way)
{
	read_Cache2CPU_count++;
}

void write_CPU2Cache(int line_1, int current_way)
{
	write_CPU2Cache_count++;
}

void write_CPU2Mem(int line_1, int current_way)
{
	write_CPU2Mem_count++;
}
void write_Cache2Mem(int line_1, int current_way)
{
	write_Cache2Mem_count;
}
void setTag(uint32_t address, int line_1, int way)
{
	tag[line_1][way] = get_tag(address);
}

void updateLRU(int line_1, int current_way)
{
	int i = 0;
	for (i = 0; i < N; i++)
		lru[line_1][N] += 1;
	lru[line_1][current_way] = 0;
}

int find_tag(uint32_t addr, int line_1, int N)
{
	int flag = 0, i;
	for (i = 0; i < N; i++)
	{
		if (tag[line_1][i] == get_tag(addr))
		{
			if (valid[line_1][i] == 1)
			{
				flag = 1;
				return i;
			}
		}
	}
	if (flag == 0)//tag not found in cache
	{
		return -1;
	}
}



uint32_t get_tag(uint32_t addr1)
{
	uint32_t num, tag;
	tag = addr1 >> (logt(B) + logt(L));
	return tag;
}

int logt(int x)
{
	return log2(x);
}


uint32_t get_line(uint32_t addr, int N, int BL)
{
	uint32_t line;
	int l;
	int t = width - logt(L) - logt(B);
	l = logt(L);
	line = (addr >> logt(B));
	line = line & (uint32_t)(pow(2, l) - 1);
	return line;
}
int find_oldest(int line_1)
{
	int i, j, lru_value = -1;
	if (N != 1)
	{
		for (i = 0; i < N; i++)
		{

			if (lru[line_1][i] > lru_value)
			{
				lru_value = lru[line_1][i];
				j = i;

			}
		}
		return j;
	}
	else
		return 0;

}

uint32_t isDirty(int line_1, int current_way)
{
	if (dirty[line_1][current_way] == 1)
		return 1;
	else
		return 0;
}

void invalidate(uint32_t line_1, int current_way)
{
	valid[line_1][current_way] = 0;
}

void validate(uint32_t line_1, int current_way)
{
	valid[line_1][current_way] = 1;
}

void record_total()
{
	printf("No of Passes: %d\n", No_passes);
	printf("No of ways is: %d\tBurst Length is: %d\tWrite Strategy is : %d\n", N, BL, WS);
	printf("Flush count is: %d\n", flush_count);
	printf("Total number of byes written is: %d\n", total_bytes_written);
	printf("Total number of byes read is: %d\n", total_bytes_read);
	printf("read_memory_count: %d\n", read_memory_count);
	printf("read_cache_count: %d\n", read_cache_count);
	printf("read_hit_count : %d\n", read_hit_count);
	printf("read_miss_count : %d\n", read_miss_count);
	printf("read_replace_count : %d\n", read_replace_count);
	printf("write_memory_count : %d\n", write_memory_count);
	printf("write_cache_count : %d\n", write_cache_count);
	printf("write_hit_count : %d\n", write_hit_count);
	printf("write_miss_count : %d\n", write_miss_count);
	printf("write_replace_count : %d\n", write_replace_count);
	printf("write_dirty_count : %d\n", write_dirty_count);
	printf("write_writeBack_count : %d\n", write_writeBack_count);
	printf("write_CPU2Cache_count : %d\n", write_CPU2Cache_count);
	printf("write_CPU2Mem_count : %d\n", write_CPU2Mem_count);
	printf("write_Cache2Mem_count : %d\n", write_Cache2Mem_count);
	printf("total_access_time: %d\n", total_access_time);
	printf("\n\n");
	//printf("No of ways is: %d\n", N);
}


void zero_cache()
{
	int r;
	int c = 0;
	for (r = 0; r < max_lines; r++)
	{
		for (c = 0; c < max_ways; c++)
		{
			valid[r][c] = 0;
			dirty[r][c] = 0;
			lru[r][c] = c;
			tag[r][c] = 0;
		}
	}
	total_bytes_written = 0;
	total_bytes_read = 0;
	read_hit_count = 0;
	read_writeBack_count = 0;
	read_miss_count = 0;
	read_replace_count = 0;
	read_Cache2CPU_count = 0;
	read_Mem2Cache_count = 0;
	read_memory_count = 0;
	write_memory_count = 0;
	write_miss_count = 0;
	write_hit_count = 0;
	read_cache_count = 0;
	write_CPU2Cache_count = 0;
	write_cache_count = 0;
	write_CPU2Mem_count = 0;
	write_writeBack_count = 0;
	write_dirty_count = 0;
	write_replace_count = 0;
	flush_count = 0;
	write_Cache2Mem_count = 0;
	total_access_time = 0;
}


void flush_cache()
{
	uint64_t i;
	int j;
	for (i = 0; i < max_lines; i++)
	{
		for (j = 0; j < max_ways; j++)
		{
			if (dirty[i][j] == 1)
			{
				dirty[i][j] = 0;
				flush_count++;
			}
		}
	}
}


void nrerror(char error_text[])
{
	int i = 0;
	for (i = 0; i < sizeof(error_text); i++)
		printf("%c", error_text);
}
void choldc(double **a, int n, double p[3])
{
	printf("\n");
	int i, j, k;
	k = 0;
	double sum = 0.0;
	read_memory(&n, sizeof(int));
	for (i = 0; i < n; i++)
	{
		write_memory(&i, sizeof(int));
		//printf("i = %d\n ", i);
		read_memory(&j, sizeof(int));
		read_memory(&n, sizeof(int));
		for (j = 0; j < n; j++)
		{
			write_memory(&j, sizeof(int));
			read_memory(&sum, sizeof(double));
			read_memory(&a[i][j], sizeof(double));
			sum = a[i][j];
			write_memory(&sum, sizeof(double));
			read_memory(&k, sizeof(int));
			for (k = i - 1; k >= 0; k--)
			{
				write_memory(&j, sizeof(int));
				read_memory(&sum, sizeof(double));
				read_memory(&a[i][k], sizeof(double));
				read_memory(&a[j][k], sizeof(double));
				sum -= a[i][k] * a[j][k];
				write_memory(&sum, sizeof(double));
			}
			read_memory(&j, sizeof(int));
			read_memory(&i, sizeof(int));
			if (i == j)
			{
				read_memory(&sum, sizeof(double));
				if (sum <= 0.0)
					nrerror("choldc failed");
				p[i] = sqrt(sum);
				write_memory(&p[i], sizeof(double));
				read_memory(&p[i], sizeof(double));
				a[i][j] = p[i];
				write_memory(&a[i][j], sizeof(double));
			}
			else
			{
				read_memory(&a[j][i], sizeof(double));
				read_memory(&sum, sizeof(double));
				read_memory(&p[i], sizeof(double));
				a[j][i] = sum / p[i];
				write_memory(&a[j][i], sizeof(double));
			}
			//printf("%lf\t", a[i][j]);
		}
		//printf("\n");
	}
}
void cholsl(double **a, int n, double p[], double b[], double x[])
{
	int i, k;
	double sum;
	read_memory(&n, sizeof(int));
	read_memory(&i, sizeof(int));
	for (i = 0; i <= n; i++)
	{
		write_memory(&i, sizeof(int));
		read_memory(&b[i], sizeof(double));
		sum = b[i];
		write_memory(&sum, sizeof(double));
		read_memory(&k, sizeof(int));
		for (k = i - 1; k >= 1; k--)
		{
			read_memory(&i, sizeof(int));
			read_memory(&k, sizeof(int));
			read_memory(&sum, sizeof(double));
			read_memory(&a[i][k], sizeof(double));
			read_memory(&x[k], sizeof(double));
			sum -= a[i][k] * x[k];
			write_memory(&sum, sizeof(double));
		}
		read_memory(&sum, sizeof(double));
		read_memory(&p[i], sizeof(double));
		x[i] = sum / p[i];
		write_memory(&x[i], sizeof(double));
	}
	read_memory(&n, sizeof(int));
	read_memory(&i, sizeof(int));
	for (i = n; i >= 1; i--)
	{
		write_memory(&i, sizeof(int));
		read_memory(&x[i], sizeof(double));
		sum = x[i];
		write_memory(&sum, sizeof(double));
		read_memory(&i, sizeof(int));
		read_memory(&n, sizeof(int));
		read_memory(&k, sizeof(int));
		for (k = i + 1; k <= n; k++)
		{
			write_memory(&k, sizeof(int));
			read_memory(&x[k], sizeof(double));
			read_memory(&a[k][i], sizeof(double));
			read_memory(&sum, sizeof(double));
			sum -= a[k][i] * x[k];
			write_memory(&sum, sizeof(double));
		}
		read_memory(&sum, sizeof(double));
		read_memory(&p[i], sizeof(double));
		x[i] = sum / p[i];
		write_memory(&x[i], sizeof(double));
	}
}

int main()
{
	//printf("Hello World\n");
	int i, j, k;
	k = 0;
	bool local_flag = 0;
	static double p[256] = { 0 };
	static double a[256][256];
	static double *D[256];
	static double b[256];
	static double x[256];
	//printf("Enter the elements of matrix!\n");
	for (i = 0; i < 256; i++)
	{
		D[i] = a[i];
		for (j = 0; j < 256; j++)
		{
			a[i][j] = 2 * (j + 1);
			a[j][i] = 2 * (j + 1);
			//scanf("%lf", &a[i][j]);
		}
		k++;
	}

	/*for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			printf("%lf\t", a[i][j]);
		printf("\n");
	}*/
	//file_init();

	uint8_t data[524288] = { 0 };

	for (N = 1; N <= 16; N = N * 2)
	{
		for (BL = 1; BL <= 8; BL = BL * 2)
		{
			for (WS = 1; WS <= 3; WS++)
			{
				No_passes++;
				B = 2 * BL;
				L = max_lines / (BL*B*N);
				zero_cache();
				choldc(&D, 256, p);
				cholsl(&D, 256, p, b, x);

				/*for (uint32_t i = 0; i < 524288; i++)
				{
					data[i]=0;
					write_memory(&data[i], sizeof(uint8_t));
				}*/
				read_time = (read_hit_count + read_replace_count * (60 + (BL - 1) * 17));//in ns
				if (WS == 1)
					write_time = (write_hit_count + write_miss_count * (60 + (BL - 1) * 17));
				if (WS == 2)
					write_time = write_hit_count + write_miss_count * (60 + (BL - 1) * 17) + (write_CPU2Mem_count * 60);
				if (WS == 3)
					write_time = write_hit_count + write_CPU2Mem_count * 60;
				flush_time = flush_count;
				total_access_time = read_time + write_time + flush_time;
				//file_display();
				record_total();
				flush_cache();
			}
		}
	}
	return 0;
}
