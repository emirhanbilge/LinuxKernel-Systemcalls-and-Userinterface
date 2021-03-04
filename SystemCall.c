#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#define PARAM_LEN 10

struct task_struct *task;
//struct task_struct *task_child; // eğer killer için childrenlarını da kontrol edersem diye  //in case I check their children for killer dunction
struct file *f;
char buf[512];
mm_segment_t fs;
int i;

void tostring(char str[], int num) // task_structureden aldığım pidleri proc/pid/status şeklinde birleştirmek için int -> char * yapıyorum  
//I do int -> char * to combine the pids I got from task_structur into proc / pid / status
{
	int i, rem, len = 0, n;
	n = num;
	while (n != 0)
	{
		len++;
		n /= 10;
	}
	for (i = 0; i < len; i++)
	{
		rem = num % 10;
		num = num / 10;
		str[len - (i + 1)] = rem + '0';  // '0' ile toplamak ascii table den bakılırsa intigeri chara çevirmenin en kolay yolu //Summing with '0' is the easiest way to convert intiger to char, from ascii table
	}
	str[len] = '\0';   //Bunu yapmazsak sonunda int ten kaynaklı xff fff gibi fazla bitler gelmekte çinki int 8 bit char 6 bit olmasından kaynaklanıyormuş en sonuna null
		// koyarsam işim bitince okuma sırasında fonksyon nullle kadar git diye tanımlandığı için bu şekilde önüne geçiliyor
		//// If we do not do this, there will be more bits such as xff fff originating from int at the end, because int 8 bit char is caused by
		// 6 bits.If I put null at the end, when I finish reading, the function is defined as go up to null.
}

int getLenght(char *idk)  // hazır fonksyon kullanmaktan kaçınarak kendim yapmak istedim araya gelecek id nin char arrayinin size dinamik olarak hesaplanıyor
//// I wanted to do it myself, avoiding using ready-made functions, the char array of the id that will come together is dynamically calculated
{
	int lenght = 0;
	while (idk[lenght] != '\0')
	{
		lenght++;
	}
	return lenght;
}

void onepid(char *idK)

{

	//Burada birleştirme için gerekli adımlar ve array operasyonlarım  // Here are the steps required for concatenation and array operations
	int lenght = getLenght(idK);
	char message[lenght];
	strcpy(message, idK);
	char url[20] = {'/', 'p', 'r', 'o', 'c', '/'};
	int brc;
	for (brc = 6; (brc - 6) < lenght; brc++)
	{
		url[brc] = message[brc - 6];
	}
	char devam[8] = {'/', 'c', 'm', 'd', 'l', 'i', 'n', 'e'};
	for (brc = 6 + lenght; brc < 6 + lenght + 8; brc++)
	{
		url[brc] = devam[brc - 6 - lenght];
	}
	printk(KERN_INFO "Only one process cmdline pid %s cmdline : %s", idK,
		   url);
	for (i = 0; i < 128; i++)
		buf[i] = 0;

	// proc/pi/cmdline açma durumum // my status to open proc / pi / cmdline
	f = filp_open(url, O_RDONLY, 0);
	if (f == NULL)
		printk(KERN_ALERT "filp_open error!!.\n");
	else

	{
		fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf, 512, &f->f_pos);  ///
		printk(KERN_INFO "CMDLINE  : %s\n", buf);
		set_fs(fs);
	}
	filp_close(f, NULL);
}

int pow(int num, int p) // burada chardan inte çevirirken basamaklarını onluk tabanda sırayla 1 10 100 diye çarpacağımdan bir fonksyon yarattım 
//I created a function here because I will multiply the digits in decimal base by 1 10 100 while converting from char to inte.
{
	int c;
	int base = 1;
	for (c = 0; c < p; c++)
	{
		base = base * 10;
	}
	return num * base;
}

int char_to_int(char *charArray)  // chardan inte çevirme durumu ise kill etme işlemimi de task_structureden yapmam onu da hazır kullanmadım
// burada yaptığım kullanıcıdan pid yi char arrayi olarak aldığım için task_structuremde pid int olduğundan eşitliği kontrol etmek için char to int yapıyorum.
//If it is the case of converting from char to inte, I do not use it from task_structur. I did not use it ready, because I got the pid as char arrayi from the
// user I made here, so I do char to int in order to check the equality because pid is int in the task_structurem.

{
	int lenght = getLenght(charArray);
	int c;
	int result = 0;
	for (c = 0; c < lenght; c++)
	{
		result = pow((charArray[lenght - c - 1] - '0'), c) + result;
	}
	return result;
}

void kill(char *pidNumber)

{
	int idK = char_to_int(pidNumber);
	printk(KERN_INFO "Kill in %d", idK);
	for_each_process(task)
	{
		if (task->pid == idK)  /// Kullanıcının girdiği id ile eşleşen processe bakıyorum aslında childları da olabilir fakat buna gerek duymadım
		// içinde bir for each ile childları üzerinde gezinerek orda da if koşulu ile bakabilirdim.
		/// I am looking at the process that matches the id that the user enters, actually it may have children, but I did not need it, I could browse over its children with a for each and look there with if condition.
		{
			task->exit_state = EXIT_DEAD;  // Task Structta tanımlı olan ve bir sürü durumu için kullanılan tanımlama  EXIT_DEAD yapılınca signali kullanarak
				//o process kill çalışmaz duruma getiriliyor
				// signal de kill_pid veya exit fonksyonları da kullanılabilir ama o yapılar hezır olduğu için tercih etmedim.
				// The definition that is defined in Task Struct and used for a lot of situations, when EXIT_DEAD is done, that process kill is disabled by using the signal.
				// Kill_pid or exit functions can also be used in the signal, but I did not choose it because those structures are defeated.
			printk("KERN_INFO Killed ");
			printk(KERN_INFO "\n PID: %d PROCESS: %s STATE: %ld ",
				   task->pid, task->comm, task->state);
		}
	}
}

void readstatus(char *url)
{
	for (i = 0; i < 128; i++)
	{
		buf[i] = 0;
	}
	f = filp_open(url, O_RDONLY, 0);
	if (f == NULL)
	{
		printk(KERN_ALERT "filp_open error!!.\n");
	}
	else
	{
		fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf, 128, &f->f_pos);
		char tempBuf[50];
		int ebb = 0;
		int tt = 0;
		int index = 0;
		while (buf[tt] != '\0')
		{
			// Burada bufferı 128 tutmak ideal buffer geniş olursa statusun içini komple almaktayım ama bizde istenen bilgileri kapsaması için 128 yeterli
			// Keeping the buffer at 128 here, if the ideal buffer is large, I take the status completely, but 128 is enough for us to cover the requested information.
			// Burada ebb == x diye kontrollerim benim okuduğum status klasöründeki almam ya da bastırmam gereken değerler yani alınacak satır indexleri
			//// Here, I check ebb == x, the values that I need to get or print in the status folder, that is, the line indexes to be taken
			if ((ebb == 0 || ebb == 2 || ebb == 5 || ebb == 6 ||
				 ebb == 8) &&
				buf[tt] != '\n')
			{
				tempBuf[index] = buf[tt];
				index++;
			}
			if (buf[tt] == '\n')
			{
				ebb++;
				printk(KERN_INFO "%s ", tempBuf);
				memset(tempBuf, 0, 50); // Buffer boşaltma  Buffer cleaning
				index = 0;
			}
			tt++;
		}

		set_fs(fs);
	}
	printk(KERN_INFO
		   "-----------------------------------------------------------------");
	filp_close(f, NULL);
}

void readAll(void)

{
	for_each_process(task)
	{
		int task_id = task->pid;
		int eb = task->pid;
		int lenght = 0;
		while (1)
		{
			if (eb == 0)
			{
				break;
			}
			eb = eb / 10;
			lenght++;
		}
		char url[20] = {'/', 'p', 'r', 'o', 'c', '/'};
		char message[lenght];
		tostring(message, task_id);
		int brc;
		for (brc = 6; (brc - 6) < lenght; brc++)
		{
			url[brc] = message[brc - 6];
		}
		char devam[7] = {'/', 's', 't', 'a', 't', 'u', 's'};
		for (brc = 6 + lenght; brc < 6 + lenght + 7; brc++)
		{
			url[brc] = devam[brc - 6 - lenght];
		}
		// printk(KERN_INFO "Process address is : %s \n", url);
		// Taskın pidsini en başta çekip sonra char arrayine dönüştürüp gerekli uzantılar eklenerek /proc/pid/status haline getirdim şuan read status fonksyonuna 
		//yolluyorum
		// I pulled the task pids first and then converted it to char array and added the necessary extensions to / proc / pid / status. Now I'm sending it to the read status function.
		readstatus(url);
	}
}

void mainfunction(char *src, char *dest)
{

	// Burada user spacede zaten 
	// 1 - no argumant
	// 2 - all
	// 3 - p
	// 4 - k şeklinde handle ettiğim için gelen değerleri bu şekilde kontrol ettim

	if (dest[0] == '1')
	{
		printk(KERN_INFO
			   " -all all process information \n -p pid progrecc id and its cmdline \n -k pid kill process by id");
	}
	else if (dest[0] == '2')
	{
		readAll();
	}

	else if (dest[0] == '3')
	{
		onepid(src);
	}

	else if (dest[0] == '4')
	{
		kill(src);
	}
	else
	{
		printk(KERN_INFO " THIS IS ERROR MESSAGE ");
	}
}

SYSCALL_DEFINE2(processinfo, char __user *, usr_param_src, char __user *, usr_param_src_two)
{
	char param_0[PARAM_LEN] = {'\0'};
	char param_1[PARAM_LEN] = {'\0'};
	copy_from_user(param_0, usr_param_src, PARAM_LEN);
	copy_from_user(param_1, usr_param_src_two, PARAM_LEN);

	// Yukarıda parametreler alınarak ana fonksyona yönlendirdim 
	// I took the parameters above and directed them to the mainfunction
	mainfunction(param_0, param_1);
	return 0;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(" Emirhan Bilge Bulut (Ebb) ");
MODULE_AUTHOR("Ebb");
