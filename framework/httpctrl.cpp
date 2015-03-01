#include "../include/httpctrl.h"
static int status = 0;
int httpctrl()
{
	if (status == 1)
		pause();
	status = 1;
	//pause();
	//waitpid()
	//把共享内存从当前进程中分离  
	//if (shmdt(shared) == -1)
	//{
	//	fprintf(stderr, "shmdt failed\n");
	//	exit(EXIT_FAILURE);
	//}

	//把共享内存从当前进程中分离  
	//if (shmdt(shm) == -1)
	//{
	//	fprintf(stderr, "shmdt failed\n");
	//	exit(EXIT_FAILURE);
	//}
	////删除共享内存  
	//if (shmctl(shmid, IPC_RMID, 0) == -1)
	//{
	//	fprintf(stderr, "shmctl(IPC_RMID) failed\n");
	//	exit(EXIT_FAILURE);
	//}
	return HTTP_RUN;
}